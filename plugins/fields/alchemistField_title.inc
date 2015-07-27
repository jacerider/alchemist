<?php

class alchemistField_title extends alchemistField {

  /**
   * Define the generic structure of a field that should be used
   * when an empty field is displayed on the alchemist edit page.
   */
  protected function defaultPlaceholder(){
    return ucfirst(strtolower($this->nodeType->title_label . ' of this ' . $this->nodeType->name));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    $form = parent::editForm($form, $form_state);
    $form['title'] = array(
      '#type' => 'textfield',
      '#default_value' => $this->node->title,
      '#required' => TRUE,
      '#id' => 'alc-field-title-textfield',
      // '#prefix' => '<div class="element-hidden">',
      // '#suffix' => '</div>',
    );
    // $form['editable']['#markup'] = '<div id="alc-field-title-editable" contenteditable="true">'.$this->node->title.'</div>';
    return $form;
  }

  /**
   * Render a content field.
   */
  public function render(){
    return array('#markup' => $this->node->title);
  }
}
