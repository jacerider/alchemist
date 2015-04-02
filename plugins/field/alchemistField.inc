<?php

/**
 * @file
 * Abstraction of the selection logic of an asset.
 *
 * Implementations that wish to provide an implementation of this should
 * register it using CTools' plugin system.
 */
interface alchemistFieldInterface {
  public function __construct($alchemist_id, $node, $field, $instance, $viewMode, $langcode);

  public function valid();
}

/**
 * An abstract implementation of alchemistFieldInterface.
 */
abstract class alchemistFieldAbstract implements alchemistFieldInterface {

  protected $alchemistId;
  protected $id;
  protected $node;
  protected $field;
  protected $instance;
  protected $viewMode;
  protected $langcode;
  protected $wrapper;

  /**
   * Constructor for the links.
   *
   * @param $type
   * The name of the links plugin.
   */
  public function __construct($alchemistId, $node, $field, $instance, $viewMode, $langcode) {
    $this->alchemistId = $alchemistId;
    $this->node = $node;
    $this->field = $field;
    $this->instance = $instance;
    $this->viewMode = $viewMode;
    $this->langcode = !empty($langcode) ? $langcode : 'und';
    $this->id = drupal_html_id('alc-field-' . $this->getFieldName());
    $this->wrapper = entity_metadata_wrapper('node', $node);
  }

  /**
   * Make sure this field will work with alchemist. All fields are enabled by
   * default.
   */
  public function valid(){
    return TRUE;
  }

  /**
   * Use unified form. This setting controls if field needs to be saved in
   * outside form or if it can be saved inline.
   */
  public function unified(){
    return FALSE;
  }

  /**
   * The default value added to the field when no value is defined.
   */
  public function defaultValue(){
    return array();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Setters
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Sets the value of the field.
   */
  protected function setCurrentValue($value){
    $this->getWrapper()->{$this->getFieldName()}->set($value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Getters
  //////////////////////////////////////////////////////////////////////////////

  protected function getAlchemistId(){
    return $this->alchemistId;
  }

  public function getId(){
    return $this->id;
  }

  public function getNode(){
    return $this->node;
  }

  public function getField(){
    return $this->field;
  }

  public function getInstance(){
    return $this->instance;
  }

  public function getViewMode(){
    return $this->viewMode;
  }

  public function getLang(){
    return $this->langcode;
  }

  public function getFieldName(){
    $field = $this->field;
    return $field['field_name'];
  }

  public function getFieldType(){
    $field = $this->field;
    return $field['type'];
  }

  public function getWrapper(){
    return $this->wrapper;
  }

  public function getAlchemistSettings(){
    $instance = $this->getInstance();
    return !empty($instance['settings']['alchemist']['settings']) ? $instance['settings']['alchemist']['settings'] : array();
  }

  public function getAlchemistSetting($key){
    $settings = $this->getAlchemistSettings();
    return !empty($settings[$key]) ? $settings[$key] : NULL;
  }

  /**
   * Get javascript selector for element.
   */
  public function getFieldSelector(){
    return '#' . drupal_html_class('edit-' . $this->getFieldName());
  }

  /**
   * Get the class defined value of the field.
   */
  protected function getDefaultValue(){
    return $this->defaultValue();
  }

  /**
   * Get the current value of the field.
   */
  protected function getCurrentValue(){
    return $this->getWrapper()->{$this->getFieldName()}->value();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Settings
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Add additional settings to confurations form.
   */
  public function settingsForm($defaults){
    $form = array();
    $form['dark'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use dark edit link'),
      '#default_value' => isset($defaults['dark']) ? $defaults['dark'] : 0,
    );
    return $form;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Alterations
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Allows the ability to add a default value to a field when field is empty.
   * This is for display only and does not affect the node object being saved.
   */
  public function alterValue(){
    $value = array();
    $current = $this->getWrapper()->{$this->getFieldName()}->value();
    $default = $this->getDefaultValue();
    $current = $this->getCurrentValue();
    if(!empty($default) && empty($current)){
      $this->setCurrentValue($default);
    }
  }

  /**
   * Each element added to the hidden node form will be passed here for any
   * necessary alterations.
   */
  public function alterFormField(&$element){
    $element['#prefix'] = '<div class="element-hidden">';
    $element['#suffix'] = '</div>';
    // Make sure ckeditor module never hits this field.
    $element[$element['#language']][0]['#wysiwyg'] = FALSE;
  }

  /**
   * Allow altering of the alchemist settings form for this field.
   */
  public function alterSettingsFormField(&$element, &$form_state){
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Render field.
   */
  public function render($element){
    $instance = $this->getInstance();
    $class = 'afe-'.drupal_html_class($this->getFieldName());
    alchemist_create_field_editable('.' . $class, array('field_name'=>$this->getFieldName(),'label'=>$instance['label'], 'dark'=>$this->getAlchemistSetting('dark')));
    $element['#prefix'] = '<div class="'.$class.'">';
    $element['#suffix'] = '</div>';
    return $element;
  }

  /**
   * Render the inline edit link.
   */
  protected function editLink(){
    $output = '<div class="alc-fe">';
    $output .= '  <div class="alc-fe-outline">';
    $output .= '  </div>';
    $output .= '  <div class="alc-fe-toolbar tool-right">';
    $output .= '    <div class="arrow"></div>';
    $output .= '    <a class="alc-fe-link" href="#" /><i class="fa fa-edit"></i></a>';
    $output .= '    ';
    $output .= '  </div>';
    $output .= '</div>';
    return $output;
  }

  /**
   * Wrap field inside container.
   */
  public function wrap($element){
    $prefix = !empty($element['#prefix']) ? $element['#prefix'] : '';
    $suffix = !empty($element['#suffix']) ? $element['#suffix'] : '';
    $element['#prefix'] = '<div id="'.$this->getId().'" class="alc-field '.drupal_html_class($this->getFieldType()).'">' . $prefix . '<div class="'.$this->getId().'-content">';
    $element['#suffix'] = '</div>' . $suffix . '</div>';
    return $element;
  }
}

/**
 * A null implementation of alchemistField.
 */
class alchemistField extends alchemistFieldAbstract {
}
