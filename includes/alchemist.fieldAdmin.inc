<?php

/**
 * Implements hook_form_NODE_TYPE_FORM_alter().
 */
function alchemist_form_node_type_form_alter(&$form, $form_state) {
  if (isset($form['type'])) {
    $form['alchemist'] = array(
      '#type' => 'fieldset',
      '#title' => t('Alchemist'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('alchemist-node-type-settings-form'),
      ),
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'alchemist') . '/js/alchemist.node-type-form.js'),
      ),
    );
    $form['alchemist']['alchemist_status'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#default_value' => variable_get('alchemist_status_' . $form['#node_type']->type, 0),
      '#description' => t('Enable the Alchemist inline content creation workflow on all content of this type.'),
      '#attributes' => array(
        'data-enabled-description' => t('Enabled'),
        'data-disabled-description' => t('Disabled'),
      ),
    );

    // $form['alchemist']['alchemist_settings'] = array(
    //   '#type' => 'fieldset',
    // );
    // $form['alchemist']['alchemist_settings']['#states']['visible'][':input[name="alchemist_status"]'] = array('checked' => TRUE);

    // $settings = variable_get('alchemist_settings_' . $form['#node_type']->type, array());
    // $form['alchemist']['alchemist_settings']['alchemist_revisions'] = array(
    //   '#type' => 'checkbox',
    //   '#title' => t('Revisions'),
    //   '#default_value' => variable_get('alchemist_revisions_' . $form['#node_type']->type, 0),
    //   '#description' => t('Allow for revision creation.'),
    // );
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
}

/**
 * Alchemist configuration form.
 */
function alchemist_form_field_form(&$form, &$form_state){
  $field = empty($form['#field']) ? array() : $form['#field'];
  $instance = empty($form['#instance']) ? array() : $form['#instance'];

  // Get defaults
  $defaults = !empty($instance['settings']['alchemist']['settings']) ? $instance['settings']['alchemist']['settings'] : array();

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
    '#title' => t('Enable Alchemist'),
    '#description' => t('Expose this field to the Alchemist editing interface.'),
    '#default_value' => isset($instance['settings']['alchemist']['status']) ? $instance['settings']['alchemist']['status'] : 0,
    '#element_validate' => array('alchemist_form_field_form_default_validate'),
  );

  $form['instance']['settings']['alchemist']['default'] = array(
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

  $form['instance']['settings']['alchemist']['settings'] = alchemist_configuration_form($defaults);

  $form['instance']['settings']['alchemist']['settings']['#states'] = array(
      'visible' => array(
        'input[name="instance[settings][alchemist][status]"]' => array('checked' => TRUE),
        'input[name="instance[settings][alchemist][default]"]' => array('checked' => FALSE),
      ),
  );

  $node = alchemist_create_entity($instance['bundle']);
  $handler = alchemist_get_field_handler($field['type'], $node, $field, $instance);
  $instance_defaults = !empty($instance['settings']['alchemist']['instance']) ? $instance['settings']['alchemist']['instance'] : array();
  $instance_form = $handler->settingsForm($instance_defaults);
  if(!empty($instance_form)){
    $form['instance']['settings']['alchemist']['instance'] = array(
      '#type' => 'fieldset',
      '#title' => t('Instance Settings'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#states' => array(
        'visible' => array(
          'input[name="instance[settings][alchemist][status]"]' => array('checked' => TRUE),
        ),
      ),
    ) + $instance_form;
  }

  // $default_value = NULL;
  // if(isset($form['instance']['default_value_widget'][$instance['field_name']])){
  //   $default_value = $form['instance']['default_value_widget'][$instance['field_name']];
  // }
  // else if(isset($form['instance']['settings']['default_image'])){
  //   $default_value = $form['instance']['settings']['default_image'];
  // }
  // if($default_value){
  //   $default_value['#access'] = FALSE;
  //   $form['instance']['settings']['alchemist']['structure'] = $default_value;
  // }
}

/**
 * Validation callback that will clean values before being saved as variables.
 */
function alchemist_form_field_form_default_validate($form, &$form_state){
  $values = &$form_state['values']['instance']['settings'];
  if(empty($values['alchemist']['status'])){
    unset($values['alchemist']);
  }
  if(!empty($values['alchemist']['default'])){
    unset($values['alchemist']['settings']);
  }
}

/**
 * Settings form for Asset.
 */
function alchemist_configuration_form($defaults = array()){

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['info']['#markup'] = '<div class="description">' . t('Temporary placeholder for future settings.') . '</div>';

  drupal_alter('alchemist_configuration_form', $form, $default_values);
  return $form;
}
