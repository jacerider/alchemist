<?php

/**
 * @file
 * Contains the node from URL argument default plugin.
 */

/**
 * Default argument plugin to extract a node via menu_get_object
 *
 * This plugin actually has no options so it odes not need to do a great deal.
 */
class alchemist_plugin_argument_default_node extends views_plugin_argument_default_node {
  function get_argument() {
    $result = parent::get_argument();
    if(!$result && arg(0) == 'node' && arg(1) == 'add' && arg(2) == 'alchemist' && arg(3)){
      $settings = variable_get('alchemist_' . arg(3), array());
      $result = isset($settings['nid']) ? (int) $settings['nid'] : NULL;
    }
    return $result;
  }
}
