<?php

/**
 * Implements hook_views_plugins().
 */
function alchemist_views_plugins() {
  return array(
    'argument default' => array(
      'node_alchemist' => array(
        'title' => t('Content ID from URL include Alchemist'),
        'handler' => 'alchemist_plugin_argument_default_node'
      ),
    ),
  );
}


/**
 * Implements hook_views_data_alter().
 */
function alchemist_views_data_alter(&$data) {
  $data['node']['title']['field']['handler'] = 'alchemist_handler_field_node_title';
}

/**
 * Implements hook_views_pre_execute().
 */
function alchemist_views_pre_execute(&$view) {
  if($view->base_table === 'node' && alchemist_valid_path()){
    foreach($view->field as $field_id => &$field){
      if(is_a($field, 'views_handler_field_field')){
        $settings = array(
          'fallback_format' => $field->options['type'],
          'fallback_settings' => $field->options['settings'],
        );
        $field->options['type'] = 'alchemist';
        $field->options['settings'] = $settings;
      }
    }
  }
}
