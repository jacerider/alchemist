<?php

/**
 * Image field handler for Alchemist.
 */
class alchemistFieldable_image extends alchemistFieldable {

  protected function defaultPlaceholder(){
    $default = array();
    $settings = variable_get('alchemist', array());
    if(isset($settings['default_image']) && $file = file_load($settings['default_image'])){
      $default = (array) $file;
    }
    return $default;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Getters
  //////////////////////////////////////////////////////////////////////////////

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
  // Settings
  //////////////////////////////////////////////////////////////////////////////

  public function settingsForm($form, &$form_state, $defaults){
    $form = parent::settingsForm($form, $form_state, $defaults);
    $form['blak'] = array(
      '#type' => 'checkbox',
      '#title' => t('Blak?'),
      '#default_value' => !empty($defaults['blak']),
    );
    return $form;
  }

}
