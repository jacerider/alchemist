<?php

class alchemistField_text_long extends alchemistField {

  public function defaultValue(){
    $default = parent::defaultValue();
    if(!empty($this->instance['settings']['text_processing'])){
      $default = array(
        'summary' => NULL,
        'value' => NULL,
        'format' => NULL,
      );
    }
    return $default;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Settings
  //////////////////////////////////////////////////////////////////////////////

  public function settingsForm($defaults){
    $form = array();
    $form['inline'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Inline Editor'),
      '#description' => t('Will allow for directly editing the content of this text field. <strong>Multi-value fields are not supported.</strong>'),
      '#default_value' => isset($defaults['inline']) ? $defaults['inline'] : 0
    );
    return $form;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    if(function_exists('exo_prevent')) exo_prevent();
    $field = parent::editForm($form, $form_state);
    if($this->getSetting('inline')){
      $element = &$field[$this->field['field_name']];
      // Prevent eXo from doing anything if it is enabled on this field.
      $element[$element['#language']][0]['#exo_prevent'] = TRUE;
      // Hide textarea
      $element['#attributes']['class'][] = 'element-hidden';

      $render = field_view_field('node', $this->node, $this->field['field_name'], 'default', $this->langcode);
      foreach (element_children($render) as $key) {
        $item = &$render[$key];
        $item['#markup'] = '<div id="'.$this->id.'-inline" class="alc-inline" contenteditable="true">'.$item['#markup'].'</div>';
      }
      if(!module_exists('ckeditor')){
        $render['#attached']['library'][] = array('alchemist', 'ckeditor');
      }
      else{
        $render['#attached']['library'][] = array('ckeditor', 'ckeditor');
      }
      $field['inline'] = $render;
    }
    return $field;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output
  //////////////////////////////////////////////////////////////////////////////

  public function preprocess($element, $attributes = array()){
    $element = parent::preprocess($element, $attributes);
    // Prepare for inline editing
    if($this->getSetting('inline')){
      $element['#attached']['js'][] = drupal_get_path('module','alchemist') . '/js/alchemistField_text_long.js';
    }
    return $element;
  }

}
