<?php

/**
 * @file
 * Contains the basic 'node' title field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a node.
 * Definition terms:
 * - link_to_node default: Should this field have the checkbox "link to node" enabled by default.
 *
 * @ingroup views_field_handlers
 */
class alchemist_handler_field_node_title extends views_handler_field_node {

  /**
   * Render whatever the data is as a link to the node.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    $data = parent::render_link($data, $values);
    if(($node = alchemist_get_node()) && $node->nid == $this->get_value($values, 'nid')){
      $plugin = alchemist_get_field_plugin($node, 'title', t('Title'));
      $element = $plugin->preprocess(array());
      $element['#markup'] = $data;
      // $element['#attached']['js'][] = drupal_get_path('module','alchemist') . '/js/alchemistField_title.js';
      return drupal_render($element);
    }
    return $data;
  }

}
