<?php

/**
 * Base class for a Alchemist fields.
 */
class alchemistField {

  public $alchemistId;
  public $id;
  public $node;
  public $nodeType;
  public $field;
  public $instance;
  public $fieldName;
  public $langcode;
  public $jsOptions;

  /**
   * Constructor for the links.
   *
   * @param $type
   *   The name of the links plugin.
   * @param $args
   *   An array of arguments.
   */
  public function __construct($type, $args = array()) {
    $this->node = $args[0];
    $this->nodeType = node_type_get_type($this->node->type);
    $this->field = $args[1];
    $this->instance = $args[2];
    $this->fieldName = $this->field['field_name'];
    $this->alchemistId = $this->node->alchemist_id;
    $this->id = drupal_html_id('alc-field-' . $this->fieldName);
    $this->langcode = entity_language('node', $this->node);
    $this->jsOptions = array();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    return array();
  }

  public function editValidate($form, &$form_state){
  }

  public function editSubmit($form, &$form_state){
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Preprocess a content field.
   */
  public function preprocess($element, $attributes = array()){
    $attributes['id'] = $this->id;
    $attributes['class'][] = 'alc-data';
    $attributes['class'][] = 'alc-'.drupal_html_class($this->fieldName);

    // $attributes['data-alc-label'] = $this->instance['label'];

    $prefix = isset($element['#prefix']) ? $element['#prefix'] : '';
    $element['#prefix'] = '<span'.drupal_attributes($attributes).'></span>' . $prefix;

    // $attributes['id'] = $this->id;
    // $attributes['class'][] = 'alc-field';
    // $attributes['class'][] = 'alc-'.drupal_html_class($this->fieldName);

    // $attributes['data-alc-label'] = $this->instance['label'];

    // $prefix = isset($element['#prefix']) ? $element['#prefix'] : '';
    // $element['#prefix'] = '<div'.drupal_attributes($attributes).'>' . $prefix;
    // $suffix = isset($element['#suffix']) ? $element['#suffix'] : '';
    // $element['#suffix'] = $suffix . '</div>';

    alchemist_js_editable($this->fieldName, $this->id, array(
      'type' => $this->field['type'],
      'label' => $this->instance['label'],
      'alchemistId' => $this->alchemistId,
    ) + $this->jsOptions);

    return $element;
  }

  /**
   * Render a content field.
   */
  public function render(){
    return array();
  }

}
