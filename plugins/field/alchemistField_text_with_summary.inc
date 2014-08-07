<?php

class alchemistField_text_with_summary extends alchemistField_text_long {

  /**
   * Make sure this field will work with alchemist.
   */
  public function valid(){
    $instance = $this->getInstance();
    if(parent::valid() && empty($instance['settings']['display_summary'])){
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Allow altering of the alchemist settings form for this field.
   */
  public function alterSettingsFormField(&$element, &$form_state){
    parent::alterSettingsFormField($element, $form_state);
    $element['#states']['visible'][':input[name="instance[settings][display_summary]"]'] = array('checked' => FALSE);
  }

}
