<?php

/**
 * Implements hook_form_FORM_ID_alter().
 */
function alchemist_form_field_ui_field_edit_form_alter(&$form, &$form_state) {
  if($form['#instance']['entity_type'] == 'node'){
    $entity_type = $form['#instance']['entity_type'];
    $bundle = $form['#instance']['bundle'];
    if(array_key_exists($bundle, alchemist_get_node_types())){
      alchemist_form_field_form($form, $form_state);
    }
  }
}

/**
 * Alchemist configuration form.
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 */
function alchemist_form_field_form(&$form, &$form_state){
  $field = empty($form['#field']) ? array() : $form['#field'];
  $instance = empty($form['#instance']) ? array() : $form['#instance'];

  // Get defaults
  $defaults = !empty($instance['settings']['alchemist']) ? $instance['settings']['alchemist'] : array();
  $defaults += array('status' => 0, 'default' => 1, 'settings' => array(), 'placeholder' => array());

  // Fieldset for alchemist settings on this field instance.
  $alchemist = array(
    '#type' => 'fieldset',
    '#title' => t('Alchemist'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#id' => 'edit-instance-settings-alchemist',
    '#element_validate' => array('alchemist_form_field_form_validate'),
  );

  // Enable alchemist on this field instance.
  $alchemist['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Alchemist'),
    '#description' => t('Expose this field to the Alchemist editing interface.'),
    '#default_value' => isset($instance['settings']['alchemist']['status']) ? $instance['settings']['alchemist']['status'] : 0,
  );

  $alchemist['default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Default Alchemist Settings'),
    '#description' => t('Defaults are set via !url.', array('!url' => l('admin/structure/alchemists/settings','admin/structure/alchemists/settings'))),
    '#default_value' => isset($instance['settings']['alchemist']['default']) ? $instance['settings']['alchemist']['default'] : 1,
    '#states' => array(
      'visible' => array(
        'input[name="instance[settings][alchemist][status]"]' => array('checked' => TRUE),
      ),
    ),
  );

  form_load_include($form_state, 'inc', 'alchemist', 'includes/alchemist.admin');
  $default_settings = !empty($defaults['settings']) ? $defaults['settings'] : variable_get('alchemist');
  $alchemist['settings'] = alchemist_configuration_form($default_settings, array($field['type']));
  $alchemist['settings']['#states'] = array(
      'visible' => array(
        'input[name="instance[settings][alchemist][status]"]' => array('checked' => TRUE),
        'input[name="instance[settings][alchemist][default]"]' => array('checked' => FALSE),
      ),
  );

  $form['instance']['settings']['alchemist'] = $alchemist;
}

/**
 * Validation callback that will clean values before being saved as variables.
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 */
function alchemist_form_field_form_validate($form, &$form_state){
  $values = &$form_state['values']['instance']['settings']['alchemist'];
  if(empty($values['status'])){
    unset($form_state['values']['instance']['settings']['alchemist']);
  }
  if(!empty($values['default'])){
    unset($values['settings']);
  }
}
