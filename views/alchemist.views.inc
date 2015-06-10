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
 * Get alchemist node from a view.
 */
function alchemist_views_get_node($view){
  $node = isset($view->alchemist_node) ? $view->alchemist_node : NULL;
  if(!$node){
    $node = alchemist_get_node();
  }
  return $node;
}

/**
 * Implements hook_views_pre_execute().
 */
function alchemist_views_pre_execute(&$view) {
  if(alchemist_valid_path() && $node = alchemist_views_get_node($view)){
    foreach($view->field as $field_id => &$field){
      if(is_a($field, 'views_handler_field_field')){
        $settings = array(
          'fallback_format' => $field->options['type'],
          'fallback_settings' => $field->options['settings'],
          'alchemist_field' => alchemist_build_field($field->field_info['field_name'], $node, '_alchemist_custom', $node->language),
        );
        $field->options['type'] = 'alchemist';
        $field->options['settings'] = $settings;
      }
    }
  }
}

/**
 * Implements hook_views_post_execute();
 */
function alchemist_views_post_execute(&$view) {
  if($view->base_table === 'node' && (alchemist_valid_path() || isset($view->alchemist_node))){
    $node = isset($view->alchemist_node) ? $view->alchemist_node : NULL;
    if(!$node){
      $node = alchemist_get_node();
    }
    if($node){
      if(empty($view->result)){
        // Populate node views with values so they will show on alchemist create
        // and edit pages.
        $result = new stdClass();
        $result->_field_data['nid'] = array(
          'entity_type' => 'node',
          'entity' => $node
        );
        $view->result = array($result);
        $view->_post_execute();
      }
      else{
        foreach($view->result as $row => &$data){
          if(isset($data->_field_data['nid']['entity']->nid) && $data->_field_data['nid']['entity']->nid == $node->nid){
            $data->_field_data['nid']['entity'] = $node;
          }
        }
        $view->_post_execute();
      }
    }
  }
}
