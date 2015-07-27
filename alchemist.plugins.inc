<?php

/**
 * Field plugins.
 */
function _alchemist_alchemist_fields() {
  $path = drupal_get_path('module', 'alchemist');
  $plugins = array();
  $plugins['alchemistField'] = array(
    'handler' => array(
      'path' => $path . '/plugins/fields',
      'file' => 'alchemistField.inc',
      'class' => 'alchemistField',
    ),
  );
  $plugins['alchemistField_title'] = array(
    'handler' => array(
      'path' => $path . '/plugins/fields',
      'file' => 'alchemistField_title.inc',
      'class' => 'alchemistField_title',
      'parent' => 'alchemistField',
    ),
  );
  $plugins['alchemistFieldable'] = array(
    'handler' => array(
      'path' => $path . '/plugins/fields',
      'file' => 'alchemistFieldable.inc',
      'class' => 'alchemistFieldable',
      'parent' => 'alchemistField',
    ),
  );
  $plugins['alchemistFieldable_image'] = array(
    'handler' => array(
      'path' => $path . '/plugins/fields',
      'file' => 'alchemistFieldable_image.inc',
      'class' => 'alchemistFieldable_image',
      'parent' => 'alchemistFieldable',
    ),
  );
  $plugins['alchemistFieldable_text_long'] = array(
    'handler' => array(
      'path' => $path . '/plugins/fields',
      'file' => 'alchemistFieldable_text_long.inc',
      'class' => 'alchemistFieldable_text_long',
      'parent' => 'alchemistFieldable',
    ),
  );
  $plugins['alchemistFieldable_text_with_summary'] = array(
    'handler' => array(
      'path' => $path . '/plugins/fields',
      'file' => 'alchemistFieldable_text_long.inc',
      'class' => 'alchemistFieldable_text_with_summary',
      'parent' => 'alchemistFieldable_text_long',
    ),
  );
  return $plugins;
}
