<?php

/**
 * Redirect to a unique node creation page.
 */
function alchemist_page_add_redirect($node_type){
  $node = alchemist_create_node($node_type);
  $alchemist_id = alchemist_get_id($node);
  drupal_goto('alchemist/add/' . $node_type . '/' . $alchemist_id);
}

/**
 * Primary edit page for alchemist enabled nodes.
 */
function alchemist_page_edit($node, $alchemist_id = NULL, $view_mode = 'full', $langcode = NULL){
  if(is_string($node)){
    $node = alchemist_create_node($node);
  }
  if(empty($langcode)){
    $langcode = entity_language('node', $node);
  }

  // Get the unique alchemist id for this node if one has not been provided.
  $alchemist_id = alchemist_get_id($node, $alchemist_id);

  // Tell asset module we want to use preview mode.
  $node->asset_preview = TRUE;

  // Check to see if we have a current draft and load in working node.
  $draft = FALSE;
  if($vars = alchemist_get_cache($alchemist_id)){
    $draft = TRUE;
    $node = $vars['node'];
    $node->alchemist_draft = TRUE;
  }

  // Get alchemist-enabled fields.
  $fields = alchemist_get_fields($alchemist_id, $node, $view_mode, $langcode);

  $output = array();

  // Attach base js/css.
  $output['#attached']['library'][] = array('alchemist', 'alchemist');
  // Send variables for use in javascript.
  drupal_add_js(array('alchemist' => array(
    'path' => base_path() . drupal_get_path('module', 'alchemist'),
    'alchemist_id' => $alchemist_id,
  )), 'setting');

  node_build_content($node, $view_mode, $langcode);
  $build = $node->content;
  // We don't need duplicate rendering info in node->content.
  unset($node->content);

  // Custom addition to render each field.
  foreach($fields as $field_id => $field){
    $build[$field_id]['#alchemist'] = TRUE;
    $build[$field_id] = $field->render($build[$field_id]);
    $build[$field_id] = $field->wrap($build[$field_id]);
  }

  // Cache page information.
  $vars = array(
    'node' => $node,
    'fields' => $fields,
  );
  alchemist_set_cache($alchemist_id, $vars);

  // Add necessary data for themeing.
  $build += array(
    '#theme' => 'node',
    '#node' => $node,
    '#view_mode' => $view_mode,
    '#language' => $langcode,
  );

  $output['nodes'][$node->nid] = $build;
  $output['alc_form'] = drupal_get_form('alchemist_page_form', $alchemist_id, $langcode, $vars);

  return $output;
}



////////////////////////////////////////////////////////////////////////////////
// Master Form
////////////////////////////////////////////////////////////////////////////////

/**
 * Global save form with any inline fields.
 */
function alchemist_page_form($form, &$form_state, $alchemist_id, $langcode, $vars){
  // Contains node, fields
  extract($vars);

  // Prepare node
  node_object_prepare($node);

  $form_state['node'] = $node;
  $form_state['alchemist_id'] = $alchemist_id;
  $form['#id'] = 'alchemist-page-form';

  field_attach_form('node', $node, $form, $form_state, $langcode);

  form_load_include($form_state, 'inc', 'node', 'node.pages');
  $form += node_form($form, $form_state, $node);
  unset($form['additional_settings']['#type']);

  // Run alter hooks as if we were the actual node form.
  $form_id = 'form_'.$node->type.'_node_form';
  $hooks = array('form', 'form_node_form', 'form_'.$node->type.'_node_form');
  drupal_alter($hooks, $form, $form_state, $form_id);

  foreach(element_children($form) as $id){
    $element = &$form[$id];
    if(isset($fields[$id]) && $fields[$id]->unified()){
      $fields[$id]->alterFormField($element);
    }
    else{
      if(isset($fields[$id])){
        unset($form[$id]);
      }
      else{
        // Some fields need to be loaded into primary form but hidden. One case
        // for this is path alias. If a path alias is changed via the settings
        // pannel, and then saved, and then re-editing, the change will be lost
        // unless the defaults are saved in this form.

        // Remove all field API fields so default values are saved.
        if(isset($form[$id][$langcode]['#field_name'])){
          unset($form[$id]);
        }
        else{
          $form['#alchemist_hidden'][] = $id;
        }
        // dsm($form[$id]);
        // $form[$id]['#after_build_hide'] = TRUE;
      }
    }
  }

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#weight' => -5,
    '#id' => 'edit-title',
    '#alchemist_available' => TRUE,
    '#prefix' => '<div class="element-hidden">',
    '#suffix' => '</div>',
  );
  $form['actions'] = array('#type' => 'actions','#alchemist_available' => TRUE);

  $form['actions']['settings']['#markup'] = l('<i class="fa fa-cog"></i> Settings', 'alchemist/settings/' . alchemist_get_id(), array('html'=>TRUE, 'attributes'=>array('class'=>array('alc-edit', 'button', 'use-ajax'))));

  $form['actions']['publish'] = array(
    '#type' => 'alchemist_submit',
    '#value' => '<i class="fa fa-circle"></i> Publish',
    '#attributes' => array('class'=>array('button','primary')),
    '#submit' => array('alchemist_page_form_publish_submit'),
  );
  if(variable_get('alchemist_revisions_' . $form_state['node']->type, 0)){
    $form['actions']['revise'] = array(
      '#type' => 'alchemist_submit',
      '#value' => t('<i class="fa fa-history"></i> Publish with Revision'),
      '#attributes' => array('class'=>array('button','primary')),
      '#submit' => array('alchemist_page_form_revision_submit'),
    );
  }
  $form['actions']['unpublish'] = array(
    '#type' => 'alchemist_submit',
    '#value' => '<i class="fa fa-circle-o"></i> Unpublish',
    '#attributes' => array('class'=>array('button','secondary')),
    '#submit' => array('alchemist_page_form_unpublish_submit'),
  );
  if(!empty($node->alchemist_draft)){
    $form['actions']['revert'] = array(
      '#type' => 'alchemist_submit',
      '#value' => '<i class="fa fa-undo"></i> ' . t('Discard'),
      '#attributes' => array('class'=>array('button','secondary')),
      '#submit' => array('alchemist_page_form_revert_submit'),
      '#limit_validation_errors' => array(),
    );
  }
  $form['alchemist_id'] = array(
    '#type' => 'hidden',
    '#value' => $alchemist_id,
  );

  if($node->nid){
    $title = t('Delete');
    $options = array();
    $options['attributes']['class'] = array('button','alert','tiny','secondary');
    $form['actions']['delete']['#markup'] = l('<i class="fa fa-trash-o"></i> Delete', 'node/'.$node->nid.'/delete', array('html'=>TRUE, 'attributes'=>array('class'=>array('button','alert','tiny','secondary'))));
  }

  $form['#after_build'][] = 'alchemist_page_form_after_build';

  return $form;
}

function alchemist_page_form_after_build($element){
  // dsm($element['page_title']);
  foreach($element['#alchemist_hidden'] as $id){
    if(empty($element[$id]['#alchemist_available'])){
      $element[$id]['#prefix'] = '<div class="element-hidden">';
      $element[$id]['#suffix'] = '</div>';
      // $element[$id]['#access'] = FALSE;
    }
  }
  return $element;
}

function alchemist_page_form_validate($form, &$form_state){
  $node = $form_state['node'];
  $tmp_node = clone $node;
  entity_form_submit_build_entity('node', $tmp_node, $form, $form_state);
  // Notify field widgets to validate their data.
  field_attach_form_validate('node', $tmp_node, $form, $form_state);
}

function alchemist_page_form_unpublish_submit($form, &$form_state){
  $form_state['node']->status = 0;
  alchemist_page_form_submit($form, $form_state);
}

/**
 * Form callback that will revert any working changes to currently published node.
 */
function alchemist_page_form_revert_submit($form, &$form_state){
  $alchemist_id = $form_state['alchemist_id'];
  $vars = alchemist_get_cache($alchemist_id);
  $node = $vars['node'];
  alchemist_delete_cache($alchemist_id);
  $form_state['redirect'] = node_access('view', $node) ? 'node/' . $node->nid : '<front>';
}

function alchemist_page_form_revision_submit($form, &$form_state){
  if(variable_get('alchemist_revisions_' . $form_state['node']->type, 0)){
    $form_state['node']->revision = 1;
  }
  alchemist_page_form_publish_submit($form, $form_state);
}

function alchemist_page_form_publish_submit($form, &$form_state){
  $form_state['node']->status = 1;
  alchemist_page_form_submit($form, $form_state);
}

function alchemist_page_form_submit($form, &$form_state){
  $alchemist_id = $form_state['alchemist_id'];

  $vars = alchemist_get_cache($alchemist_id);
  $node = $vars['node'];

  // If settings form has been utilized, we need to merge in the submitted values
  if(isset($vars['settings'])){
    $form_state['values'] = $vars['settings'] + $form_state['values'];
  }

  $node = node_form_submit_build_node($form, $form_state);

  // entity_form_submit_build_entity('node', $node, $form, $form_state);

  $insert = empty($node->nid);

  // TODO: needs a try / catch.
  entity_save('node', $node);

  // node_save($node);
  $node_link = l(t('view'), 'node/' . $node->nid);
  $watchdog_args = array(
    '@type' => $node->type,
    '%title' => $node->title,
  );
  $t_args = array(
    '@type' => node_type_get_name($node),
    '%title' => $node->title,
  );

  if ($insert) {
    watchdog('content', '@type: added %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);
    drupal_set_message(t('@type %title has been created.', $t_args));
  }
  else {
    watchdog('content', '@type: updated %title.', $watchdog_args, WATCHDOG_NOTICE, $node_link);
    drupal_set_message(t('@type %title has been updated.', $t_args));
  }
  if ($node->nid) {
    $form_state['values']['nid'] = $node->nid;
    $form_state['nid'] = $node->nid;
    $form_state['redirect'] = node_access('view', $node) ? 'node/' . $node->nid : '<front>';
  }
  else {
    // In the unlikely case something went wrong on save, the node will be
    // rebuilt and node form redisplayed the same way as in preview.
    drupal_set_message(t('The post could not be saved.'), 'error');
    $form_state['rebuild'] = TRUE;
  }
  // Clear the page and block caches.
  cache_clear_all();

  // Delete alchemist cache of this entity
  alchemist_delete_cache($alchemist_id);
}



////////////////////////////////////////////////////////////////////////////////
// Settings Form
////////////////////////////////////////////////////////////////////////////////

function alchemist_settings_edit($alchemist_id, $type = 'ajax'){
  $output = drupal_get_form('alchemist_settings_edit_form', $alchemist_id);
  if($type == 'ajax'){
    $commands = array();
    $commands[] = ajax_command_alchemist_panel_left(drupal_render($output), '<i class="fa fa-cog"></i> ' . t('Settings'));
    ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
    return;
  }

  return $output;

  drupal_set_message(t('You must have javascript enabled to use this feature.'));
  drupal_goto();
}

function alchemist_settings_edit_form($form, &$form_state, $alchemist_id){
  $vars = $form_state['alchemist_vars'] = alchemist_get_cache($alchemist_id);
  $node = $vars['node'];
  $fields = $vars['fields'];
  $form_state['alchemist_id'] = $alchemist_id;

  // Prepare node
  node_object_prepare($node);

  form_load_include($form_state, 'inc', 'node', 'node.pages');
  $form += node_form($form, $form_state, $node);

  unset($form['title']);
  foreach($fields as $field_id => $field){
    unset($form[$field_id]);
  }

  unset($form['additional_settings']['#type']);


  $form['actions'] = array('#type' => 'actions');
  $ajax = array(
    'callback' => 'alchemist_settings_edit_form_ajax_submit',
    'event' => 'click',
  );
  $form['actions'] = array('#type' => 'actions','#weight' => 5000);
  $form['actions']['submit'] = array(
    '#type' => 'alchemist_submit',
    '#value' => t('Update'),
    '#ajax' => $ajax,
    '#submit' => array('alchemist_settings_edit_form_submit'),
    '#attributes' => array('class'=>array('button','expand')),
  );
  $form['#validate'][] = 'alchemist_settings_edit_form_validate';

  // Run alter hooks as if we were the actual node form.
  $form_id = 'form_'.$node->type.'_node_form';
  $hooks = array('form', 'form_node_form', $form_id);
  drupal_alter($hooks, $form, $form_state, $form_id);

  // Temporary menu enable
  if(!empty($node->menu['enabled'])){
    $form['menu']['enabled']['#default_value'] = 1;
  }

  foreach(element_children($form) as $key){
    $element = &$form[$key];
    if($element['#type'] == 'fieldset'){
      $element['#weight'] = $element['#weight'] + 1000;
      if(!empty($element['#collapsible'])){
        $element['#collapsed'] = TRUE;
      }
    }
  }

  return $form;
}

function alchemist_settings_edit_form_ajax_submit($form, $form_state){
  $commands = array();
  $commands[] = ajax_command_alchemist_panel_close();

  // Allow commands to be altered and added to.
  drupal_alter('alchemist_settings_update', $commands, $form_state['alchemist_vars']);

  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

function alchemist_settings_edit_form_validate($form, $form_state){
  node_form_validate($form, $form_state);
}

function alchemist_settings_edit_form_submit($form, &$form_state){
  $alchemist_id = $form_state['alchemist_id'];
  $node = node_form_submit_build_node($form, $form_state);
  $form_state['alchemist_vars']['node'] = $node;
  $form_state['alchemist_vars']['settings'] = $form_state['values'];
  alchemist_set_cache($alchemist_id, $form_state['alchemist_vars']);
}



////////////////////////////////////////////////////////////////////////////////
// Field Form
////////////////////////////////////////////////////////////////////////////////

/**
 * Field editor form ajax callback.
 */
function alchemist_field_edit($alchemist_id, $field_name, $type = 'ajax'){
  $vars = alchemist_get_cache($alchemist_id);
  $instance = $vars['fields'][$field_name]->getInstance();
  $output = drupal_get_form('alchemist_field_edit_form', $alchemist_id, $field_name);
  if($type == 'ajax'){
    $commands = array();
    $commands[] = ajax_command_alchemist_panel_top(drupal_render($output), '<i class="fa fa-cog"></i> ' . t('Edit') . ' <span>' . $instance['label'] . '</span>');
    ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
    return;
  }

  return $output;

  drupal_set_message(t('You must have javascript enabled to use this feature.'));
  drupal_goto();
}

/**
 * Field edit form.
 */
function alchemist_field_edit_form($form, &$form_state, $alchemist_id, $field_name){
  $vars = alchemist_get_cache($alchemist_id);
  $node = $vars['node'];
  $fields = $vars['fields'];

  // Deny access. We do this here so we don't load cache twice.
  if(!node_access('update', $node)){
    drupal_set_message(t('You do not have permission to view this page.'), 'error');
    return $form;
  }

  $entity_type = 'node';
  $handler = $fields[$field_name];
  $field = $handler->getField();
  $instance = $handler->getInstance();
  $langcode = $handler->getLang();

  // Store data for later.
  $form_state['alchemist_id'] = $alchemist_id;
  $form_state['alchemist_vars'] = $vars;
  $form_state['alchemist_fieldname'] = $field_name;

  // Wrap the whole form into a wrapper, and prepare AJAX settings.
  $wrapper_id = drupal_html_id('alchemist-field-form-' . $field['field_name']);
  $form['#prefix'] = '<div id="' . $wrapper_id . '" class="alchemist-field-edit-form">';
  $form['#suffix'] = '</div>';

  // Set #parents to 'top-level' by default.
  $form += array('#parents' => array());

  // Set the base properties of the form.
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $node);
  $form['#entity_type'] = $entity_type;
  $form['#bundle'] = $bundle;
  $form['#langcode'] = $langcode;

  // Insert the field form.
  $items = $node->{$field['field_name']};
  $items = isset($items[$langcode]) ? $items[$langcode] : array();

  $form += field_default_form($entity_type, $node, $field, $instance, $langcode, $items, $form, $form_state);

  $ajax = array(
    'callback' => 'alchemist_field_form_update_close',
    'wrapper' => $wrapper_id,
    'event' => 'click',
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#name' => 'submit-' . implode('-', $form['#parents']),
    '#type' => 'alchemist_submit',
    '#value' => t('Update'),
    '#ajax' => $ajax,
    '#attributes' => array('class'=>array('button','expand')),
    '#submit' => array('alchemist_field_edit_form_submit'),
    '#limit_validation_errors' => array($form['#parents']),
  );

  return $form;
}

/**
 * Ajax callback: process an Ajax submission of the form.
 */
function alchemist_field_form_update_close($form, $form_state) {
  $alchemist_id = $form_state['alchemist_id'];
  $field_name = $form_state['alchemist_fieldname'];
  $vars = $form_state['alchemist_vars'];
  $node = $vars['node'];
  $fields = $vars['fields'];
  $handler = $fields[$field_name];

  $id = $handler->getId();

  $commands = array();
  $field = field_view_field('node', $node, $field_name, $handler->getViewMode(), $handler->getLang());
  $render = drupal_render($field);
  $commands[] = ajax_command_html('.' . $id . '-content', $render);
  $commands[] = ajax_command_alchemist_panel_close();

  // Allow commands to be altered and added to.
  drupal_alter('alchemist_field_update', $commands, $vars);
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Form submit callback: save the field modifications.
 */
function alchemist_field_edit_form_submit(&$form, &$form_state) {
  $form_state['alchemist_edit'] = FALSE;
  // $form_state['rebuild'] = TRUE;

  $alchemist_id = $form_state['alchemist_id'];
  $vars = $form_state['alchemist_vars'];
  $node = $vars['node'];

  entity_form_submit_build_entity('node', $node, $form, $form_state);

  // Update node data to cache.
  $vars['node'] = $form_state['alchemist_vars']['node'] = $node;
  alchemist_set_cache($alchemist_id, $vars);
}



////////////////////////////////////////////////////////////////////////////////
// Page Preparation
////////////////////////////////////////////////////////////////////////////////

/**
 * Implements hook_page_alter().
 */
function _alchemist_page_alter(&$page){
  if(!empty($page['content']['system_main']['alc_form'])){
    $page['page_top']['alc_form'] = $page['content']['system_main']['alc_form'];
    $page['page_top']['alc_form']['#weight'] = 1000;
    unset($page['content']['system_main']['alc_form']);
  }
}

/**
 * Implements hook_preprocess_page().
 */
function _alchemist_preprocess_page(&$vars){
  $node = $vars['node'];
  $vars['page']['content']['#attached']['library'][] = array('alchemist', 'alchemist_textfield');
  $vars['title'] = alchemist_build_title($node);
}

/**
 * Implements hook_preprocess_html().
 */
function _alchemist_preprocess_html(&$vars){
  drupal_add_library('alchemist', 'alchemist_scrollbar');
  $vars['page']['page_top']['alchemist'] = array(
    '#weight' => -9999,
    '#markup' => '
      <div id="alc-panel" class="alc-panel">
        <div id="alc-region-left" class="alc-region">
          <div class="alc-region-inner">
            <div class="alc-region-title"></div>
            <div class="alc-region-content">
            </div>
          </div>
        </div>
        <div id="alc-region-top" class="alc-region">
          <div class="alc-region-inner">
            <div class="row">
              <div class="columns large-12 alc-region-title"></div>
              <div class="columns large-12 alc-region-content">
              </div>
            </div>
          </div>
        </div>
        <div id="alc-pusher">
          <div id="alc-content">
            <div id="alc-content-inner">',
  );
  // $vars['page']['page_bottom']['alc-edit'] = array(
  //   '#markup' => '<div id="alc-edit">' . l('<i class="fa fa-cog"></i>', 'alchemist/edit/' . alchemist_get_id(), array('html'=>TRUE, 'attributes'=>array('class'=>array('use-ajax')))) . '</div>'
  // );
  $vars['page']['page_bottom']['alchemist'] = array(
    '#weight' => 9999,
    '#markup' => '
            </div><!-- /alc-content-inner -->
          </div><!-- /alc-content -->
        </div><!-- /alc-pusher -->
      </div><!-- /alc-panel -->
      <link href="//fonts.googleapis.com/css?family=Open+Sans:300,400,700" rel="stylesheet" type="text/css">',
  );
}

/**
 * Create editable title.
 */
function alchemist_build_title($node){
  $id = 'alc-title';
  drupal_add_js(array('alchemist' => array(
    'textfield' => array(
      $id => array(
      'field' => '#edit-title',
  )))), 'setting');
  return '<div id="'.$id.'" class="alc-editable alc-title" data-ph="'.t('Title').'" contenteditable="true">' . $node->title . '</div>';
}



////////////////////////////////////////////////////////////////////////////////
// Utilities
////////////////////////////////////////////////////////////////////////////////

/**
 * Get available fields and their handlers.
 */
function alchemist_get_fields($alchemist_id, $node, $view_mode = 'full', $langcode = NULL){
  $fields = &drupal_static(__FUNCTION__);
  if (!isset($fields) && $fields = _alchemist_get_fields($node->type)) {
    foreach($fields as $field_name => $field){
      if($handler = alchemist_get_handler($field['field']['type'], $alchemist_id, $node, $fields[$field_name]['field'], $fields[$field_name]['instance'], $view_mode, $langcode)){
        $fields[$field_name] = $handler;
        // Allow altering of current value of field. This is for display only.
        $fields[$field_name]->alterValue();
      }
      else{
        $class = alchemist_get_class_name($field['field']['type']);
        drupal_set_message(t('Field %field_name is Alchemist enabled but does not have a valid handler. Handler should be %handler.', array('%field_name' => $field_name, '%handler' => $class)), 'warning');
        unset($fields[$field_name]);
      }
    }
  }
  else{
    $fields = array();
  }
  return $fields;
}

/**
 * Get fields that are alchemist enabled.
 */
function _alchemist_get_fields($bundle){
  $fields = &drupal_static(__FUNCTION__);
  if (!isset($fields)) {
    foreach(field_info_instances('node', $bundle) as $field_name => $field){
      if(!empty($field['settings']['alchemist']['status'])){
        $fields[$field_name]['field'] = field_info_field($field_name);
        $fields[$field_name]['instance'] = $field;
      }
    }
  }
  return $fields;
}

/**
 * Get id for page.
 */
function alchemist_get_id($node = NULL, $alchemist_id = NULL){
  $id = &drupal_static(__FUNCTION__);
  if(!isset($id)){
    if($alchemist_id){
      $id = $alchemist_id;
    }
    elseif($node){
      if(empty($node->nid)){
        $id = 'alchemist-' . drupal_random_key();
      }
      else{
        $id = 'alchemist-' . $node->nid;
      }
    }
  }
  return $id;
}
