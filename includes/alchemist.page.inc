<?php

/**
 * Primary edit page for alchemist enabled nodes.
 */
function alchemist_page_edit($node, $view_mode = 'full'){
  $langcode = entity_language('node', $node);

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
  drupal_set_title($vars['node']->title);
  $plugin = alchemist_get_field_plugin($vars['node'], 'title', t('Title'));
  $element = $plugin->preprocess(array());
  $element['#markup'] = '<div class="node-title">' . drupal_get_title() . '</div>';
  // $element['#attached']['js'][] = drupal_get_path('module','alchemist') . '/js/alchemistField_title.js';
  return drupal_render($element);
}
