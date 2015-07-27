<?php

/**
 * Menu callback for settings form.
 */
function alchemist_settings($form, $form_state) {
  $defaults = variable_get('alchemist');
  $form['alchemist'] = alchemist_configuration_form($defaults);
  $form['alchemist']['#title'] = 'Alchemist default settings';
  $form['alchemist']['#tree'] = TRUE;
  $form['alchemist']['default_image'] = array(
    '#type' => 'value',
    '#value' => $defaults['default_image'],
  );
  return system_settings_form($form);
}

/**
 * Settings form for Alchemist. Currently just a placeholder.
 *
 * @param $defaults
 *   The current settings.
 * @param $limit_by_type
 *   Specify the field type to limit the form by.
 */
function alchemist_configuration_form($defaults = array(), $limit_by_type = array()){
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['info']['#markup'] = '<div class="description">' . t('Temporary placeholder for future global settings.') . '</div>';

  $types = array();
  $fields = field_info_field_map();
  foreach($fields as $field_name => $info){
    if(isset($info['bundles']['node'])){
      $types[$info['type']] = $field_name;
    }
  }
  if(!empty($limit_by_type)){
    $types = array_intersect_key($types, array_flip($limit_by_type));
  }

  if(!empty($types)){
    $fields_form = array();
    foreach($types as $type => $field_name){
      $field = field_info_field($field_name);
      $instance = array();
      $field_defaults = isset($defaults['types'][$type]) ? $defaults['types'][$type] : (isset($defaults['type']) ? $defaults['type'] : array());
      $fields_form[$type] = alchemist_configuration_field_form($field, $field_defaults);
    }
    if(!empty($fields_form)){
      if(empty($limit_by_type) || count($limit_by_type) > 1){
        $form['types'] = array(
          '#type' => 'fieldset',
          '#title' => t('Field Type Settings'),
          '#collapsible' => FALSE,
          '#tree' => TRUE,
          '#element_validate' => array('alchemist_configuration_form_cleanup'),
        ) + $fields_form;
      }
      else{
        $form['type'] = array(
          '#title' => t('Field Settings'),
        ) + $fields_form[$limit_by_type[0]];
      }
    }
  }

  drupal_alter('alchemist_configuration_form', $form, $default_values);
  return $form;
}

/**
 * Settings form for Alchemist field types.
 *
 * @param $field
 *   The field structure for the operation.
 * @param $defaults
 *   The current settings.
 */
function alchemist_configuration_field_form($field, $defaults = array()){
  $node = new stdClass();
  $node->type = 'alchemist';
  $node->alchemist_id = 'alchemist';
  $instance = array();
  $langcode = LANGUAGE_NONE;
  $plugin = alchemist_get_fieldable_plugin($node, $field, $instance, $langcode);
  $field_form = $plugin->settingsForm(array(), $form_state, $defaults);
  $form = array();
  if(!empty($field_form)){
    $type = $field['type'];
    $form += array(
      '#type' => 'fieldset',
      '#title' => t('Settings for %label fields !type', array('%label' => ucwords(str_replace('_', ' ', $type)), '!type' => '<small>[' . $type . ']</small>')),
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#element_validate' => array('alchemist_configuration_form_cleanup'),
    );
    $form += $field_form;
  }
  return $form;
}

/**
 * Validation callback that will clean values before being saved as variables.
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 */
function alchemist_configuration_form_cleanup($form, &$form_state){
  $values = drupal_array_get_nested_value($form_state['values'], $form['#parents']);
  $callback = function($item) use (&$callback) {
    if (is_array($item)) {
      return array_filter($item, $callback);
    }
    if (!empty($item)) {
      return true;
    }
  };
  $values = array_filter($values, $callback);
  drupal_array_set_nested_value($form_state['values'], $form['#parents'], $values);
}
