<?php

class alchemistField_text_long extends alchemistField {

  /**
   * Make sure this field will work with alchemist.
   */
  public function valid(){
    $field = $this->getField();
    $instance = $this->getInstance();
    if($field['cardinality'] == 1 && !empty($instance['settings']['text_processing'])){
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Use unified form. This setting controls if field needs to be saved in
   * outside form or if it can be saved inline.
   */
  public function unified(){
    return TRUE;
  }

  /**
   * The default value added to the field when no value is defined.
   */
  public function defaultValue(){
    $default = array();
    $default['value'] = '';
    $default['summary'] = '';
    $default['format'] = filter_default_format();
    $default['safe_value'] = '';
    $default['safe_summary'] = '';
    return $default;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Settings
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Add additional settings to confurations form.
   */
  public function settingsForm($defaults){
    return array();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Render field.
   */
  public function render($element){
    // Set asset view mode which should be on asset_preview when in edit mode.
    $element['#asset_view_mode'] = 'asset_preview';

    $instance = $this->getInstance();
    $element['#attached']['library'][] = array('alchemist', 'alchemist_textarea');
    if(!module_exists('ckeditor')){
      $element['#attached']['library'][] = array('alchemist', 'ckeditor');
    }
    else{
      $editor_url_path = ckeditor_path('url');
      drupal_add_js($editor_url_path . '/ckeditor.js', array('type' => 'file', 'scope' => 'footer', 'preprocess' => FALSE));
    }
    drupal_add_js(array('alchemist' => array(
      'textarea' => array(
        $this->getId() . '-editable' => array(
          'entity_type' => 'node',
          'field_name' => $this->getFieldName(),
          'field' => $this->getFieldSelector(),
    )))), 'setting');
    $placeholder = 'Enter '.strtolower($instance['label']).' content here...';

    // Because textareas can have text filters applied to them, we want to use
    // the unmodified data of the field when editing.
    $field = $this->getField();
    $value = $this->getWrapper()->{$field['field_name']}->value();

    $element[0]['#markup'] = '<div id="'.$this->getId().'-editable" class="alc-editable '.drupal_html_class($this->getFieldType()).'" data-ph="'.$placeholder.'" contenteditable="true">' . $value['value'] . '</div>';
    return $element;
  }

  /**
   * Get javascript selector for element.
   */
  public function getFieldSelector(){
    $selector = parent::getFieldSelector();
    return $selector .= ' textarea';
  }

}
