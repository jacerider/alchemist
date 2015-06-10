<?php

class alchemistField_image extends alchemistField {

  public function defaultValue(){
    $default = array();
    $settings = variable_get('alchemist', array());
    if(isset($settings['default_image']) && $file = file_load($settings['default_image'])){
      $default = (array) $file;
    }
    return $default;
  }

  /**
   * Get the current value of the field.
   */
  protected function getValue($delta = NULL){
    $value = parent::getValue($delta);
    // Newely uploaded images don't have all the information we need to
    // render them... so we need to reload them.
    if(!empty($value['fid']) && empty($value['uri'])){
      $value = (array) file_load($value['fid']);
      $this->setValue($value, $delta);
    }
    return $value;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    // Clear out value if it is the default.
    if($this->getValue() == $this->defaultValue()){
      $this->setValue(NULL);
    }
    return parent::editForm($form, $form_state);
  }

}
