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
      $node = alchemist_node_scaffold_create($bundle);
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
    // Save variable if Alchemist is enabled.
    variable_set('alchemist_' . $bundle, $new);
  }

  // Remove to prevent Drupal from re-saving this value.
  unset($form_state['values']['alchemist']);
}

/**
 * Create a new node that will act as the scaffolding for new Alchemist-created
 * nodes.
 *
 * @param [string] $bundle
 *   The node bundle name.
 *
 * @return [object]
 *   A node object.
 */
function alchemist_node_scaffold_create($bundle){
  $user = user_load(1);
  $values = array(
    'type' => $bundle,
    'uid' => $user->uid,
    'status' => 1,
    'promote' => 0,
  );
  $entity = entity_create('node', $values);
  $wrapper = entity_metadata_wrapper('node', $entity);
  $wrapper->title->set(t('Alchemist !type scaffold', array('!type' => $bundle)));
  $wrapper->log->set('alchemist');
  $wrapper->save();
  return $entity;
}
