<?php

/**
 * Implementation of hook_field_formatter_info().
 */
function alchemist_field_formatter_info() {
  return array(
    'alchemist' => array(
      'label' => t('Alchemist'),
      // This format is only available programatically so we do not expose it
      // to any field types.
      'field types' => array(),
      'settings' => array(
        'fallback_format' => NULL,
        'fallback_settings' => array(),
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function alchemist_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, &$items, $display) {
  if (!$display['type'] == 'alchemist') {
    continue;
  }

  // Grab the Alchemist node and overwrite the entity if their IDs match.
  $node = alchemist_get_node();
  if(isset($node->nid, $entity->nid) && $node->nid == $entity->nid){
    $entity = $node;
  }

  $element = alchemist_fallback_formatter($entity_type, $entity, $field, $instance, $langcode, $items, $display);

  $wrapper = entity_metadata_wrapper($entity_type, $entity);

  if(!$wrapper->{$field['field_name']}->value()){
    $element = array(
      '#theme' => 'field',
      '#access' => TRUE,
      '#object' => $entity,
      '#items' => array(array('#value' => 'hi')),
      0 => array('#markup' => '<div>Edit '. $instance['label'] .'</div>'),
    );
  }
  else{
    // dsm($element);
  }

  // Return default output when on any page other than an alchemist page.
  if(!alchemist_valid_path() || $node->nid != $entity->nid || !alchemist_field_enabled($instance['entity_type'], $instance['bundle'], $instance['field_name'])){
    return $element;
  }


  // See if access to this form element is restricted,
  // if so, skip widget processing and just set the value.
  if (isset($entity->nid) && ($entity_type != 'node' || !entity_access('update', $entity_type, $entity) || !field_access('edit', $field, $entity_type, $entity))) {
    // Can't edit.
    return $element;
  }


  // Register field handler and preprocess field output.
  $plugin = alchemist_get_fieldable_plugin($node, $field, $instance, $langcode, $display);
  if(isset($display['views_field'])){
    foreach (element_children($element) as $delta) {
      $element[$delta] = $plugin->preprocess($element[$delta]);
    }
  }
  else{
    $element = $plugin->preprocess($element);
  }

  return $element;
}

/**
 * Format a field using the fallback formatter of the alchemist field.
 */
function alchemist_fallback_formatter($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  // Set the fallback formatter.
  $display['type'] = $display['settings']['fallback_format'];
  $display['settings'] = $display['settings']['fallback_settings'];
  $formatter_type = field_info_formatter_types($display['type']);
  $display['module'] = $formatter_type['module'];

  // Clone the entity to avoid messing with it.
  $cloned_entity = clone $entity;

  return field_view_field($entity_type, $cloned_entity, $field['field_name'], $display, $langcode);
}
