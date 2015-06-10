<?php

function alchemist_field_edit($node, $field_id){
  $commands = array();

  if($handler = alchemist_get_handler_by_id($node, $field_id)){
    $output = drupal_get_form('alchemist_field_edit_form', $node, $handler);
    // return $output;
    $commands[] = ajax_command_html('#' . $handler->getId(), drupal_render($output));
    $commands[] = ajax_command_alchemist_editing_start($field_id);
  }
  else{
    $function = 'alchemist_field_edit_form_' . strtr($field_id, array('alc-field-' => '', '-' => '_'));
    if(function_exists($function)){
      $output = drupal_get_form($function, $node);
      $commands[] = ajax_command_html('#' . $field_id, drupal_render($output));
      $commands[] = ajax_command_alchemist_editing_start($field_id);
    }
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
function alchemist_field_edit_form($form, &$form_state, $node, $handler){
  $entity_type = 'node';
  $field_name = $handler->getFieldName();
  $field = $handler->getField();
  $instance = $handler->getInstance();
  $langcode = $handler->getLang();

  $form_state['node'] = $node;
  $form_state['handler'] = $handler;

  // Set the base properties of the form.
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $node);
  $form['#entity_type'] = $entity_type;
  $form['#node'] = $node;
  $form['#bundle'] = $bundle;
  $form['#langcode'] = $langcode;

  $form['#prefix'] = '<div id="' . $handler->getId() . '-form" class="alchemist-field-edit-form">';
  $form['#suffix'] = '</div>';
  // Set #parents to 'top-level' by default.
  $form += array('#parents' => array());

  $field_form = array();
  $form += $handler->editForm($field_form, $form_state);

  $form['#validate'][] = 'alchemist_field_edit_form_validate';
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#name' => 'submit-' . implode('-', $form['#parents']),
    '#type' => 'submit',
    '#value' => t('Update') . ' ' . $instance['label'],
    '#attributes' => array('class'=>array('button','expand')),
    '#submit' => array('alchemist_field_edit_form_submit'),
    '#ajax' => array(
      'callback' => 'alchemist_field_edit_form_ajax',
      'wrapper' => '#' . $handler->getId() . '-form',
      'event' => 'click',
    ),
    '#limit_validation_errors' => array($form['#parents']),
  );

  return $form;
}

/**
 * Form validation callback.
 */
function alchemist_field_edit_form_validate($form, &$form_state){
  $node = $form_state['node'];
  $handler = $form_state['handler'];

  // Bypass node_validate last changed check.
  $form_state['values']['changed'] = time();

  entity_form_submit_build_entity('node', $node, $form, $form_state);
  node_validate($node, $form, $form_state);
  field_attach_form_validate('node', $node, $form, $form_state, array('field_name' => $handler->getFieldName()));
}

/**
 * Form submit callback: save the field modifications.
 */
function alchemist_field_edit_form_submit($form, &$form_state) {
  $node = $form_state['node'];
  $handler = $form_state['handler'];
  entity_form_submit_build_entity('node', $node, $form, $form_state);
  alchemist_set_cache($node);
}

/**
 * Form submit ajax callback: update the display with the changed content.
 */
function alchemist_field_edit_form_ajax($form, $form_state) {
  $node = $form_state['node'];
  $handler = $form_state['handler'];
  $field_name = $handler->getFieldName();
  $commands = array();
  $commands[] = ajax_command_alchemist_editing_stop();

  $fields = $node->alchemist_fields[$field_name];
  foreach($fields as $field_handler){
    $output = $field_handler->render();
    $commands[] = ajax_command_html('#' . $field_handler->getId(), drupal_render($output));
  }

  if($messages = theme('status_messages')){
    $commands[] = ajax_command_html('#alchemist-messages', $messages);
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

////////////////////////////////////////////////////////////////////////////////
/// Title Edit Form
////////////////////////////////////////////////////////////////////////////////

/**
 * Title edit form.
 */
function alchemist_field_edit_form_title($form, &$form_state, $node){
  $form_state['node'] = $node;
  $form += array('#parents' => array());

  $form['title'] = array(
    '#type' => 'textfield',
    '#default_value' => $node->title,
    '#required' => TRUE,
    '#id' => 'alc-field-title-textfield',
    '#prefix' => '<div class="element-hidden">',
    '#suffix' => '</div>',
  );

  $form['editable']['#markup'] = '<div id="alc-field-title-editable" contenteditable="true">'.$node->title.'</div>';

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#name' => 'submit-' . implode('-', $form['#parents']),
    '#type' => 'submit',
    '#value' => t('Update Title'),
    '#attributes' => array('class'=>array('button','expand')),
    '#submit' => array('alchemist_field_edit_form_title_submit'),
    '#ajax' => array(
      'callback' => 'alchemist_field_edit_form_title_ajax',
      'wrapper' => '#alc-title-form',
      'event' => 'click',
    ),
    '#limit_validation_errors' => array($form['#parents']),
  );

  return $form;

}

/**
 * Form submit callback: save the title modifications.
 */
function alchemist_field_edit_form_title_submit($form, &$form_state) {
  $node = $form_state['node'];
  $title = trim($form_state['values']['title']);
  $wrapper = entity_metadata_wrapper('node', $node);
  $wrapper->title->set($title);
  alchemist_set_cache($node);
}

/**
 * Form submit callback: save the title modifications.
 */
function alchemist_field_edit_form_title_ajax($form, $form_state) {
  $node = $form_state['node'];

  $commands = array();
  $commands[] = ajax_command_alchemist_editing_stop();

  if($messages = theme('status_messages')){
    $commands[] = ajax_command_html('#alchemist-messages', $messages);
  }

  $commands[] = ajax_command_html('#alc-field-title', $node->title);

  return array('#type' => 'ajax', '#commands' => $commands);
}
