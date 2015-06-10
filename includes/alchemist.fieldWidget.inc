<?php

/**
 * Implementation of hook_field_formatter_info().
 */
function alchemist_field_formatter_info() {
  // $all_types = array_keys(field_info_field_types());
  return array(
    'alchemist' => array(
      'label' => t('Alchemist'),
      // 'field types' => $all_types,
      // This format is only available programatically so we do not expose it
      // to any field types.
      'field types' => array(),
      'settings' => array(
        'fallback_format' => NULL,
        'fallback_settings' => array(),
        'alchemist_field' => NULL,
        // 'click_to_edit' => FALSE,
        // 'click_to_edit_style' => 'button',
        // 'empty_text' => '',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function alchemist_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = alchemist_field_formatter_default_settings($display['settings'], $field);

  // Include the field_ui admin form.
  module_load_include('inc', 'field_ui', 'field_ui.admin');

  // Get the list of formatters for this field type, and remove our own.
  $formatters = field_ui_formatter_options($field['type']);
  unset($formatters['alchemist']);

  // $element['click_to_edit'] = array(
  //   '#type' => 'checkbox',
  //   '#title' => t('Click to edit'),
  //   '#default_value' => $settings['click_to_edit'],
  // );

  // $element['click_to_edit_style'] = array(
  //   '#type' => 'select',
  //   '#title' => t('Click to edit style'),
  //   '#options' => array(
  //     'button' => t('Button'),
  //     'hover' => t('Hover'),
  //   ),
  //   '#default_value' => $settings['click_to_edit_style'],
  // );

  // $element['empty_text'] = array(
  //   '#type' => 'textarea',
  //   '#rows' => 3,
  //   '#title' => t('Empty text'),
  //   '#description' => t('Text to show when the field is empty.'),
  //   '#default_value' => $settings['empty_text'],
  // );

  $element['fallback_format'] = array(
    '#type' => 'select',
    '#title' => t('Fallback formatter'),
    '#options' => $formatters,
    '#default_value' => $settings['fallback_format'],
  );

  // Refresh the form automatically when we know which context we are in.
  if (isset($form_state['complete form']) && $form_state['complete form']['#form_id'] == 'field_ui_display_overview_form') {
    // Field UI.
    $element['fallback_format'] += array(
      '#field_name' => $field['field_name'],
      '#op' => 'edit',
      '#executes_submit_callback' => TRUE,
      '#submit' => array('alchemist_field_ui_display_overview_multistep_submit'),
      '#ajax' => array(
        'callback' => 'field_ui_display_overview_multistep_js',
        'wrapper' => 'field-display-overview-wrapper',
        'effect' => 'fade',
      ),
    );

    $field_name = $form_state['triggering_element']['#field_name'];
    // $element['click_to_edit_style'] += array(
    //   '#states' => array(
    //     'visible' => array(
    //       ':input[name="fields[' . $field_name . '][settings_edit_form][settings][click_to_edit]"]' => array('checked' => TRUE),
    //     ),
    //   ),
    // );
  }
  else if (isset($form_state['form_id']) && $form_state['form_id'] == 'views_ui_config_item_form') {
    // Views UI.
    $element['fallback_format'] += array(
      '#ajax' => array(
        'path' => views_ui_build_form_url($form_state),
      ),
      '#submit' => array('views_ui_config_item_form_submit_temporary'),
      '#executes_submit_callback' => TRUE,
    );
    // $element['click_to_edit_style'] += array(
    //   '#dependency' => array('edit-options-settings-click-to-edit' => array(1)),
    // );
  }

  $fallback_formatter = field_info_formatter_types($settings['fallback_format']);
  $fallback_settings = $settings['fallback_settings'] + field_info_formatter_settings($settings['fallback_format']);

  // Store the settings in a '_dummy' view mode.
  $instance['display']['_dummy'] = array(
    'type' => $settings['fallback_format'],
    'settings' => $fallback_settings,
  );

  // Get the settings form.
  $settings_form = array('#value' => array());
  $function = $fallback_formatter['module'] . '_field_formatter_settings_form';
  if (function_exists($function)) {
    $settings_form = $function($field, $instance, '_dummy', $form, $form_state);
  }
  $element['fallback_settings'] = $settings_form;

  return $element;
}

/**
 * Form submit handler for AJAX change of the fallback formatter.
 */
function alchemist_field_ui_display_overview_multistep_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  $op = $trigger['#op'];

  // Store the saved settings.
  $field_name = $trigger['#field_name'];
  $values = $form_state['values']['fields'][$field_name]['settings_edit_form']['settings'];
  $form_state['formatter_settings'][$field_name] = $values;

  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function alchemist_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = alchemist_field_formatter_default_settings($display['settings'], $field);

  return t('Fallback formatter: @formatter', array('@formatter' => $settings['fallback_format']));
}

/**
 * Return default settings for a field type.
 */
function alchemist_field_formatter_default_settings($settings, $field) {
  if (empty($settings['fallback_format'])) {
    $field_type = field_info_field_types($field['type']);
    $settings['fallback_format'] = $field_type['default_formatter'];
  }
  $settings += array('fallback_settings' => array());
  return $settings;
}

/**
 * Format a field using the fallback formatter of the alchemist field.
 */
function alchemist_fallback_formatter($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  // Set the fallback formatter.
  $display['type'] = $display['settings']['fallback_format'];
  $display['settings'] = $display['settings']['fallback_settings'];
  $formatter_type = field_info_formatter_types($display['type']);
  $display['module'] = $formatter_type['module'];

  // Clone the entity to avoid messing with it.
  $cloned_entity = clone $entity;

  return field_view_field($entity_type, $cloned_entity, $field['field_name'], $display, $langcode);
}

/**
 * Implements hook_field_formatter_view().
 */
function alchemist_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, &$items, $display) {
  if (!$display['type'] == 'alchemist') {
    continue;
  }

  $element = alchemist_fallback_formatter($entity_type, $entity, $field, $instance, $langcode, $items, $display);

  // Return default output when on any page other than an alchemist page.
  if(!alchemist_valid_path() || empty($display['settings']['alchemist_field'])){
    return $element;
  }

  // See if access to this form element is restricted,
  // if so, skip widget processing and just set the value.
  if (isset($entity->nid) && ($entity_type != 'node' || !entity_access('update', $entity_type, $entity) || !field_access('edit', $field, $entity_type, $entity))) {
    // Can't edit.
    return $element;
  }

  // Check that we are operating on the node itself.
  $node = alchemist_get_node();
  if($entity->nid != $node->nid){
    return $element;
  }

  $handler = $display['settings']['alchemist_field'];
  // Store final display in handler.
  $handler->setDisplay($display);
  foreach (element_children($element) as $delta) {
    $item = &$element[$delta];
    $item = $handler->preprocess($item);
  }

  return $element;
}
