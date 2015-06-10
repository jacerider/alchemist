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
    if(!$result){
      $result = arg(0) == 'alchemist' && arg(1) == 'add';
    }
    return $result;
  }
}
