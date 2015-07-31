<?php

class alchemistField_title extends alchemistField {

  /**
   * Define the generic structure of a field that should be used
   * when an empty field is displayed on the alchemist edit page.
   */
  protected function defaultPlaceholder(){
    return alchemist_node_scaffold_title($this->node->type);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    $form = parent::editForm($form, $form_state);
    if($this->node->title == $this->defaultPlaceholder()){
      $this->node->title = '';
    }
    $form['title'] = array(
      '#title' => t('Title'),
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
