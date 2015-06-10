<?php

/**
 * Redirect to a unique node creation page.
 */
function alchemist_page_add_redirect($node_type){
  $alchemist_id = alchemist_get_id();
  drupal_goto('node/add/alchemist/' . $node_type . '/' . $alchemist_id);
}

/**
 * Primary edit page for alchemist enabled nodes.
 */
function alchemist_page_edit($node, $view_mode = 'full'){
  $langcode = entity_language('node', $node);

  if($cache = alchemist_get_cache($node)){
    $node = $cache;
    $node->alchemist_draft = TRUE;
  }

  $node->alchemist_page = TRUE;

  // Get valid Alchemist fields.
  $fields = alchemist_build_fields($node);

  node_build_content($node, $view_mode, $langcode);
  $build = $node->content;
  // We don't need duplicate rendering info in node->content.
  unset($node->content);

  // Add necessary data for themeing.
  $build += array(
    '#theme' => 'node',
    '#node' => $node,
    '#view_mode' => $view_mode,
    '#language' => $langcode,
    '#prefix' => '<div id="alchemist">',
    '#suffix' => '</div>',
  );

  $output = array();
  $output['#attached']['library'][] = array('alchemist', 'alchemist');
  $settings = array(
    'alchemist' => array(
      'path' => base_path() . drupal_get_path('module', 'alchemist'),
      'alchemist_id' => $node->alchemist_id,
    ),
  );
  $output['#attached']['js'][] = array('data' => $settings, 'type' => 'setting');

  $output['alchemist_message'] = array(
    '#markup' => '<div id="alchemist-messages"></div>',
    '#weight' => -100,
  );

  $output['nodes'][$node->nid] = $build;

  // Let hook_page_alter know we want to cache this page.
  $output['#alchemist_cache'] = TRUE;

  return $output;
}

function alchemist_preprocess_page_title(&$vars){
  $field_name = 'title';
  $id = drupal_html_id('alc-field-' . $field_name);
  $attributes = array();
  $attributes['id'] = $id;
  $attributes['class'][] = 'alc-field';
  $attributes['class'][] = 'alc-' . $field_name;
  $attributes['data-alc-label'] = t('Title');

  drupal_set_title($vars['node']->title);

  alchemist_js_editable('title', $id, array(
    'type' => $field_name,
    'alchemistId' => $vars['node']->alchemist_id,
  ));

  $vars['page']['content']['system_main']['#attached']['js'][] = drupal_get_path('module','alchemist') . '/js/alchemistField_title.js';

  return '<div'.drupal_attributes($attributes).'>' . drupal_get_title() . '</div>';
}
