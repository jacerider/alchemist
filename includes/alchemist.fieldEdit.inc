<?php

function alchemist_field_edit($node, $field_id){
  $commands = array();

  if($plugin = alchemist_get_field_plugin_by_id($node, $field_id)){
    $output = drupal_get_form('alchemist_field_edit_form', $plugin);
    // return $output;
    // $commands[] = ajax_command_html('#' . $plugin->id, drupal_render($output));
    $commands[] = ajax_command_html('*[data-alc-id="' . $plugin->id . '"]', drupal_render($output));
    $commands[] = ajax_command_alchemist_editing_start($field_id);
  }

  if($messages = theme('status_messages')){
    $commands[] = ajax_command_html('#alchemist-messages', $messages);
  }

  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Field edit form.
 */
function alchemist_field_edit_form($form, &$form_state, $plugin){
  $form_state['plugin'] = $plugin;

  $field_form = array();
  $form += $plugin->editForm($field_form, $form_state);

  $form['#validate'][] = 'alchemist_field_edit_form_validate';
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    // '#name' => 'submit-' . implode('-', $form['#parents']),
    '#type' => 'submit',
    '#value' => t('Update') . ' ' . $plugin->instance['label'],
    '#attributes' => array('class'=>array('button','expand')),
    '#submit' => array('alchemist_field_edit_form_submit'),
    '#ajax' => array(
      'callback' => 'alchemist_field_edit_form_ajax',
      'wrapper' => '#' . $plugin->id . '-form',
      'event' => 'click',
    ),
    // '#limit_validation_errors' => array($form['#parents']),
  );

  return $form;
}

/**
 * Form validation callback.
 */
function alchemist_field_edit_form_validate($form, &$form_state){
  $plugin = $form_state['plugin'];
  // Bypass node_validate last changed check.
  $form_state['values']['changed'] = time();
  entity_form_submit_build_entity('node', $plugin->node, $form, $form_state);
  node_validate($plugin->node, $form, $form_state);
  $plugin->editValidate($form, $form_state);
}

/**
 * Form submit callback: save the field modifications.
 */
function alchemist_field_edit_form_submit($form, &$form_state) {
  if (form_get_errors()) return;

  $plugin = $form_state['plugin'];
  entity_form_submit_build_entity('node', $plugin->node, $form, $form_state);
  $plugin->editSubmit($form, $form_state);
  alchemist_set_cache($plugin->node);
}

/**
 * Form submit ajax callback: update the display with the changed content.
 */
function alchemist_field_edit_form_ajax($form, $form_state) {
  $plugin = $form_state['plugin'];

  if (form_get_errors()){
    $commands[] = ajax_command_replace('*[data-alc-id="' . $plugin->id . '"]', theme('status_messages') . drupal_render($form));
    $commands[] = ajax_command_alchemist_editing_start($plugin->id);
  }
  else{

    $commands = array();
    $commands[] = ajax_command_alchemist_editing_stop();

    $fields = $plugin->node->alchemist_fields[$plugin->fieldName];
    foreach($fields as $plugin){
      $output = $plugin->render();
      if(!empty($output)){
        // $commands[] = ajax_command_html('#' . $plugin->id, drupal_render($output));
        $commands[] = ajax_command_replace('*[data-alc-id="' . $plugin->id . '"]', drupal_render($output));
      }
    }

    if($messages = theme('status_messages')){
      $commands[] = ajax_command_html('#alchemist-messages', $messages);
    }
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}
