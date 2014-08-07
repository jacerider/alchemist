<?php

/**
 * Implements hook_form_NODE_TYPE_FORM_alter().
 */
function alchemist_form_node_type_form_alter(&$form, $form_state) {
  if (isset($form['type'])) {
    $form['alchemist'] = array(
      '#type' => 'fieldset',
      '#title' => t('Alchemist settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('alchemist-node-type-settings-form'),
      ),
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'alchemist') . '/js/alchemist.node-form.js'),
      ),
    );
    $form['alchemist']['alchemist_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#default_value' => variable_get('alchemist_enable_' . $form['#node_type']->type, 0),
      '#description' => t('Enable the alchemist content creation workflow on this node type.'),
    );

    $form['alchemist']['alchemist_settings'] = array(
      '#type' => 'fieldset',
    );
    $form['alchemist']['alchemist_settings']['#states']['visible'][':input[name="alchemist_enable"]'] = array('checked' => TRUE);

    $settings = variable_get('alchemist_settings_' . $form['#node_type']->type, array());
    $form['alchemist']['alchemist_settings']['alchemist_revisions'] = array(
      '#type' => 'checkbox',
      '#title' => t('Revisions'),
      '#default_value' => variable_get('alchemist_revisions_' . $form['#node_type']->type, 0),
      '#description' => t('Allow for revision creation.'),
    );
  }
}

/**
 * Implements hook_form_FIELD_UI_FIELD_EDIT_FORM_alter().
 */
function alchemist_form_field_ui_field_edit_form_alter(&$form, &$form_state) {
  if(isset($form['#instance']['entity_type']) && $form['#instance']['entity_type'] == 'node'){
    $entity_type = $form['#instance']['entity_type'];
    $bundle = $form['#instance']['bundle'];
    if(array_key_exists($bundle, alchemist_get_node_types())){
      alchemist_form_field_form($form, $form_state);
    }
  }

  // // Get allowed field and widget types.
  // $allowed_field_types = alchemist_allowed_field_types();
  // $allowed_field = in_array($form['#field']['type'], $allowed_field_types);
  // $allowed_widget_types = alchemist_allowed_widget_types();
  // $allowed_widget = in_array($form['instance']['widget']['type']['#value'], $allowed_widget_types);

  // // Add the alchemist settings to the field instance form.
  // if ($allowed_field  && $allowed_widget) {
    // alchemist_form_field_form($form, $form_state);
  // }
}

/**
 * Mee configuration form.
 */
function alchemist_form_field_form(&$form, &$form_state){
  $field = empty($form['#field']) ? array() : $form['#field'];
  $field_name = $field['field_name'];
  $instance = empty($form['#instance']) ? array() : $form['#instance'];
  $node = alchemist_create_node($instance['bundle']);
  $handler = alchemist_get_handler($field['type'], NULL, $node, $field, $instance);

  if(!$handler || !$handler->valid()){
    return;
  }

  // Preprocess this default widget preview field.
  if(isset($form['instance']['default_value_widget'][$field_name])){
    $handler->alterFormField($form['instance']['default_value_widget'][$field_name]);
  }

  // Fieldset for alchemist settings on this field instance.
  $form['instance']['settings']['alchemist'] = array(
    '#type' => 'fieldset',
    '#title' => t('Alchemist'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#id' => 'edit-instance-settings-alchemist',
  );

  // Enable alchemist on this field instance.
  $form['instance']['settings']['alchemist']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Alchemist Editing'),
    '#description' => t('Alchemist enhances the content creation experience.'),
    '#default_value' => isset($instance['settings']['alchemist']['status']) ? $instance['settings']['alchemist']['status'] : 0,
  );

  $handler->alterSettingsFormField($form['instance']['settings']['alchemist'], $form_state);

  // Get defaults
  $default_values = !empty($instance['settings']['alchemist']['settings']) ? $instance['settings']['alchemist']['settings'] : array();

  if($handler_form = $handler->settingsForm($default_values)){
    $form['instance']['settings']['alchemist']['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Settings'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );
    $form['instance']['settings']['alchemist']['settings']['#states']['visible'][':input[name="instance[settings][alchemist][status]"]'] = array('checked' => TRUE);

    $form['instance']['settings']['alchemist']['settings'] += $handler_form;
  }
}

// /**
//  * Return a list of allowed elements.
//  * @TODO: Make this a hook.
//  */
// function alchemist_allowed_elements() {
//   $allowed_elements = array(
//     'textarea',
//   );
//   drupal_alter('alchemist_allowed_elements', $allowed_elements);
//   return $allowed_elements;
// }

// /**
//  * Return a list of allowed field types.
//  * @TODO: Make this a hook.
//  */
// function alchemist_allowed_field_types() {
//   $allowed_field_types = array(
//     'text_long',
//     'text_with_summary',
//   );
//   drupal_alter('alchemist_allowed_field_types', $allowed_field_types);
//   return $allowed_field_types;
// }

// /**
//  * Return a list of allowed widget types.
//  * @TODO: Make this a hook.
//  */
// function alchemist_allowed_widget_types() {
//   $allowed_widget_types = array(
//     'text_textarea',
//     'text_textarea_with_summary',
//   );
//   drupal_alter('alchemist_allowed_widget_types', $allowed_widget_types);
//   return $allowed_widget_types;
// }
