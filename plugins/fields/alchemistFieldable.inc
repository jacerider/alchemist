<?php

/**
 * Base class for a Alchemist fields.
 */
class alchemistFieldable extends alchemistField {

  public $display;

  /**
   * Constructor for the links.
   *
   * @param $type
   *   The name of the links plugin.
   * @param $args
   *   An array of arguments.
   */
  public function __construct($type, $args = array()) {
    parent::__construct($type, $args);
    $this->langcode = $args[3];
    $this->display = $args[4];
  }

  /**
   * Define the default placeholder for a field.
   *
   * @return
   *   Return a value ready to be set using entity_metadata_wrapper->set().
   */
  protected function defaultPlaceholder(){
    return array(
      '#theme' => 'field',
      '#title' => $this->instance['label'],
      '#access' => TRUE,
      '#label_display' => $this->display['label'],
      '#view_mode' => ' _custom_display',
      '#language' => $this->langcode,
      '#field_name' => $this->instance['field_name'],
      '#field_type' => $this->field['type'],
      '#entity_type' => 'node',
      '#bundle' => $this->node->type,
      '#object' => $this->node,
      '#formatter' => $this->display['settings']['fallback_format'],
      '#items' => array(array('#value' => 'Hello Old Frield')),
      0 => array('#markup' => '<div class="alc-placeholder">Edit '. $this->instance['label'] .'</div>'),
    );
  }

  //////////////////////////////////////////////////////////////////////////////
  // Setters
  //////////////////////////////////////////////////////////////////////////////

  // public function setPlaceholder(){
  //   // if($placeholder = $this->getPlaceholder()){
  //   //   if($this->field['cardinality'] > 1){
  //   //     for($delta = 0; $delta < $this->field['cardinality']; $delta++){
  //   //       if(!$this->getValue($delta)){
  //   //         $this->setValue($placeholder, $delta);
  //   //       }
  //   //     }
  //   //   }
  //   //   // Unlimited field.
  //   //   // @todo handle more than first occurance.
  //   //   else if($this->field['cardinality'] == -1 && !$this->getValue(0)){
  //   //     $this->setValue($placeholder, 0);
  //   //   }
  //   //   else if(!$this->getValue()){
  //   //     $this->setValue($placeholder);
  //   //   }
  //   // }
  // }

  // /**
  //  * Sets the value of the field.
  //  */
  // protected function setValue($value, $delta = NULL){
  //   $wrapper = entity_metadata_wrapper('node', $this->node);
  //   if(!is_null($delta) && is_numeric($delta)){
  //     $wrapper->{$this->fieldName}[$delta]->set($value);
  //   }
  //   else{
  //     $wrapper->{$this->fieldName}->set($value);
  //   }
  // }

  //////////////////////////////////////////////////////////////////////////////
  // Getters
  //////////////////////////////////////////////////////////////////////////////

  // /**
  //  * Get placeholder. Will return placeholder from node if exists else the
  //  * placeholder defined in code.
  //  */
  // protected function getPlaceholder(){
  //   // $wrapper = entity_metadata_wrapper('node', $this->placeholder);
  //   // if($placeholder = $wrapper->{$this->fieldName}->value()){
  //   //   return $placeholder;
  //   // }
  //   // elseif($placeholder = $this->defaultPlaceholder()){
  //   //   return $placeholder;
  //   // }
  //   return FALSE;
  // }

  // /**
  //  * Get the current value of the field.
  //  */
  // protected function getValue($delta = NULL){
  //   $wrapper = entity_metadata_wrapper('node', $this->node);
  //   if(is_numeric($delta)){
  //     return $wrapper->{$this->fieldName}[$delta]->value();
  //   }
  //   return $wrapper->{$this->fieldName}->value();
  // }

  //////////////////////////////////////////////////////////////////////////////
  // Settings
  //////////////////////////////////////////////////////////////////////////////

  public function settingsForm($form, &$form_state, $defaults){
    return $form;
  }

  public function settingsValidate($form, &$form_state){
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    field_attach_form('node', $this->node, $form, $form_state, $this->langcode, array('field_name' => $this->fieldName));
    return isset($form[$this->fieldName]) ? array($this->fieldName => $form[$this->fieldName]) : array();
  }

  public function editValidate($form, &$form_state){
    field_attach_form_validate('node', $this->node, $form, $form_state, array('field_name' => $this->fieldName));
  }

  public function editSubmit($form, &$form_state){
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Render a content field.
   */
  public function render(){
    $wrapper = entity_metadata_wrapper('node', $this->node);
    $value = $wrapper->{$this->fieldName}->value();
    if(empty($value)){
      return $this->defaultPlaceholder();
    }
    else{
      $display = $this->display;
      $display['type'] = $display['settings']['fallback_format'];
      $display['settings'] = $display['settings']['fallback_settings'];
      $formatter_type = field_info_formatter_types($display['type']);
      $display['module'] = $formatter_type['module'];

      // Clone the entity to avoid messing with it.
      $cloned_node = clone $this->node;

      return field_view_field('node', $cloned_node, $this->fieldName, $display, $this->langcode);
    }
  }
}
