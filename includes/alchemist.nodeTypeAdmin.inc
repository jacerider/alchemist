<?php

/**
 * Implement hook_form_FORM_ID_alter().
 */
function alchemist_form_node_type_form_alter(&$form, $form_state) {
  if (isset($form['type'])) {
    $bundle = $form['#node_type']->type;
    $form['alchemist'] = array(
      '#type' => 'fieldset',
      '#title' => t('Alchemist'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('alchemist-node-type-settings-form'),
      ),
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'alchemist') . '/js/alchemist.node-type-form.js'),
      ),
    );
    $form['alchemist']['status'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#default_value' => alchemist_variable_get('status', FALSE, $bundle),
      '#description' => t('Enable the Alchemist inline content creation workflow on all content of this type.'),
      '#attributes' => array(
        'data-enabled-description' => t('Enabled'),
        'data-disabled-description' => t('Disabled'),
      ),
    );

    $options = array();
    $enabled = array();
    foreach(field_info_instances('node', $bundle) as $field_name => $instance){
      $options[$field_name] = $instance['label'];
      if(!empty($instance['settings']['alchemist']['status'])){
        $enabled[] = $field_name;
      }
    }
    $form['alchemist']['fields'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Bulk enabled fields'),
      '#options' => $options,
      '#default_value' => $enabled,
      '#description' => t('Each field can also be enabled individually when editing its instance settings. Enabling fields here will set them up to use the default settings. More options are available for each instance on their full field edit page.'),
      '#states' => array(
        'visible' => array(
          'input[name="alchemist[status]"]' => array('checked' => TRUE),
        ),
      ),
    );

    array_unshift($form['#submit'], 'alchemist_node_type_form_submit');
  }
}

/**
 * Submit handler for the node_type_form element added in
 * alchemist_form_node_type_form_alter().
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 */
function alchemist_node_type_form_submit($form, &$form_state) {
  $bundle = $form['#node_type']->type;
  $current = variable_get('alchemist_' . $bundle, array());
  $new = $form_state['values']['alchemist'];

  // Check if there are no current settings or if the status has changed.
  if(!$current || $current['status'] != $new['status']){
    // If Alchemist has just been enabled.
    if($new['status'] == 1){
      $user = user_load(1);
      $node = alchemist_node_scaffold($bundle, $user, t('Alchemist !type scaffold', array('!type' => $bundle)));
      node_save($node);
      $new['nid'] = $node->nid;

      // Store all scaffold ids as their own variable for quick access as they
      // are used to each time a node query is made.
      $ids = variable_get('alchemist_nids', array());
      $ids[] = $node->nid;
      variable_set('alchemist_nids', $ids);
    }
    // Clean up Alchemist node scaffold
    elseif(!empty($current['nid'])){
      node_delete($current['nid']);

      // Remove from scaffold variable.
      $ids = array_flip(variable_get('alchemist_nids', array()));
      unset($ids[$current['nid']]);
      if(count($ids)){
        variable_set('alchemist_nids', array_flip($ids));
      }
      else{
        variable_del('alchemist_nids');
      }
    }
  }

  if(!$new['status']){
    // Remove variable if Alchemist is disabled.
    variable_del('alchemist_' . $bundle);
  }
  else{
    $instances = field_info_instances('node', $bundle);
    foreach($new['fields'] as $field_name => $enabled){
      $instance = $instances[$field_name];
      $instance['settings']['alchemist']['status'] = isset($instance['settings']['alchemist']['status']) ? $instance['settings']['alchemist']['status'] : 0;
      if($instance['settings']['alchemist']['status'] != !empty($enabled)){
        $instance['settings']['alchemist']['status'] = !empty($enabled);
        field_update_instance($instance);
      }
    }
    unset($new['fields']);

    // Save variable if Alchemist is enabled.
    variable_set('alchemist_' . $bundle, $new);
  }

  // Remove to prevent Drupal from re-saving this value.
  unset($form_state['values']['alchemist']);
}
