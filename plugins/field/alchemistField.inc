<?php

interface alchemistFieldBase {

  public function __construct($type, $args = NULL);

}

/**
 * @file
 * Abstraction of an alchemist field.
 *
 * Implementations that wish to provide an implementation of this should
 * register it using CTools' plugin system.
 */
interface alchemistFieldFieldable {
  public function setDisplay($display);
  public function getField();
  public function getInstance();
  public function getFieldName();
  public function getLang();
  public function getId();
  public function settingsForm($defaults);
  public function editForm($form, &$form_state);
  public function preprocess($element, $attributes = array());
}

/**
 * An abstract implementation of the the basic requirements for an Alchemist
 * field.
 */
abstract class alchemistFieldBaseAbstract implements alchemistFieldBase {

  /**
   * Constructor for the links.
   *
   * @param $type
   *   The name of the links plugin.
   */
  public function __construct($type, $args = array()) {
    $this->type = $type;
    ctools_include('plugins');
    $plugin = ctools_get_plugins('asset', 'widget', $type);
    $this->plugin = $plugin;

    $this->node = $args[0];
    $this->alchemistId = $this->node->alchemist_id;
    $this->id = drupal_html_id('alc-field-CHANGEME');
  }

  /**
   * Define the generic structure of a field that should be used
   * when an empty field is displayed on the alchemist edit page.
   */
  protected function defaultValue(){
    return '';
  }

  //////////////////////////////////////////////////////////////////////////////
  // Settings
  //////////////////////////////////////////////////////////////////////////////

  public function settingsForm($defaults){
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    return array();
  }

}

/**
 * An abstract implementation of alchemistFieldInterface.
 */
abstract class alchemistFieldFieldableAbstract extends alchemistFieldBaseAbstract implements alchemistFieldFieldable {

  /**
   * The name of the button plugin.
   */
  protected $type;

  /**
   * The plugin definition.
   */
  protected $plugin;

  /**
   * Utility variables.
   */
  protected $alchemistId;
  protected $id;
  protected $node;
  protected $field;
  protected $instance;
  protected $display;
  protected $langcode;
  protected $jsOptions;

  /**
   * Constructor for the links.
   *
   * @param $type
   *   The name of the links plugin.
   */
  public function __construct($type, $args = array()) {
    parent::__construct($type, $args);

    $this->field = $args[1];
    $this->instance = $args[2];
    $this->langcode = entity_language('node', $this->node);
    $this->id = drupal_html_id('alc-field-' . $this->field['field_name']);

    // Add alchemist settings
    if(!empty($this->instance['settings']['alchemist']['settings'])){
      $this->jsOptions['settings'] = $this->instance['settings']['alchemist']['settings'];
    }
    // Add instance settings
    if(!empty($this->instance['settings']['alchemist']['instance'])){
      $this->jsOptions['instance'] = $this->instance['settings']['alchemist']['instance'];
    }

    // Make sure fields show even if they don't have content.
    if(!empty($this->instance['settings']['alchemist']['status'])){
      $this->setPlaceholders();
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Edit
  //////////////////////////////////////////////////////////////////////////////

  public function editForm($form, &$form_state){
    field_attach_form('node', $this->node, $form, $form_state, $this->langcode, array('field_name' => $this->field['field_name']));
    return isset($form[$this->field['field_name']]) ? array($this->field['field_name'] => $form[$this->field['field_name']]) : array();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Placeholders
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Setting placeholders will set default values for all enabled alchemist
   * fields so that they show up in the edit interface. Otherwise Drupal
   * will not show the fields because they are empty.
   */
  protected function setPlaceholders(){
    $placeholder = $this->defaultValue();
    if($this->field['cardinality'] > 1){
      for($delta = 0; $delta < $this->field['cardinality']; $delta++){
        if(!$this->getValue($delta)){
          $this->setValue($placeholder, $delta);
        }
      }
    }
    // Unlimited field.
    // @todo handle more than first occurance.
    else if($this->field['cardinality'] == -1 && !$this->getValue(0)){
      $this->setValue($placeholder, 0);
    }
    else if(!$this->getValue()){
      $this->setValue($placeholder);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Setters
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Allows setting of the field display after initialization.
   */
  public function setDisplay($display){
    $this->display = $display;
  }

  /**
   * Sets the value of the field.
   */
  protected function setValue($value, $delta = NULL){
    $wrapper = entity_metadata_wrapper('node', $this->node);
    if(!is_null($delta) && is_numeric($delta)){
      $wrapper->{$this->field['field_name']}[$delta]->set($value);
    }
    else{
      $wrapper->{$this->field['field_name']}->set($value);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Getters
  //////////////////////////////////////////////////////////////////////////////

  public function getField(){
    return $this->field;
  }

  public function getFieldName(){
    return $this->field['field_name'];
  }

  public function getInstance(){
    return $this->instance;
  }

  public function getLang(){
    return $this->langcode;
  }

  public function getId(){
    return $this->id;
  }

  public function getDisplay(){
    if(is_array($this->display)){
      return $this->display;
    }
    return isset($this->instance['display'][$this->display]) ? $this->instance['display'][$this->display] : $this->instance['display']['default'];
  }

  /**
   * Get an instance settings.
   *
   * @param $name
   *   The key of the setting to return.
   * @param $parent
   *   The parent key of the setting. Likely "instance" for field specific
   *   settings or "settings" for alchemist base settings.
   */
  public function getSetting($name, $parent = 'instance'){
    return isset($this->instance['settings']['alchemist'][$parent][$name]) ? $this->instance['settings']['alchemist'][$parent][$name] : NULL;
  }

  /**
   * Get the current value of the field.
   */
  protected function getValue($delta = NULL){
    $wrapper = entity_metadata_wrapper('node', $this->node);
    if(is_numeric($delta)){
      return $wrapper->{$this->field['field_name']}[$delta]->value();
    }
    return $wrapper->{$this->field['field_name']}->value();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output
  //////////////////////////////////////////////////////////////////////////////

  /**
   * Preprocess a content field.
   */
  public function preprocess($element, $attributes = array()){
    $attributes['id'] = $this->id;
    $attributes['class'][] = 'alc-field';
    $attributes['class'][] = 'alc-'.drupal_html_class($this->field['field_name']);

    $attributes['data-alc-label'] = $this->instance['label'];

    $prefix = isset($element['#prefix']) ? $element['#prefix'] : '';
    $element['#prefix'] = '<div'.drupal_attributes($attributes).'>' . $prefix;
    $suffix = isset($element['#suffix']) ? $element['#suffix'] : '';
    $element['#suffix'] = $suffix . '</div>';

    alchemist_js_editable($this->field['field_name'], $this->id, array(
      'type' => $this->field['type'],
      'alchemistId' => $this->alchemistId,
    ) + $this->jsOptions);

    return $element;
  }

  /**
   * Render a content field.
   */
  public function render(){
    $display = $this->getDisplay();
    $this->setPlaceholders();
    return field_view_field('node', $this->node, $this->field['field_name'], $this->display, $this->langcode);
  }

  // /**
  //  * Render a view field.
  //  */
  // public function renderView(){
  //   $display = $this->getDisplay();
  //   $view = views_get_view($display['view']);
  //   $view->live_preview = TRUE;
  //   $view->alchemist_node = $this->node;
  //   $view->set_display($display['display']);
  //   $view->set_arguments($display['args']);
  //   $view->execute();

  //   // Limit to requested field
  //   $view->field = array($display['field'] => $view->field[$display['field']]);

  //   $view->style_plugin->render_fields($view->result);
  //   $view->style_plugin->view->row_index = 0;
  //   $view->result[$display['row']]->_field_data['alchemist_processed'] = TRUE;
  //   $output = $view->style_plugin->row_plugin->render($view->result[$display['row']]);
  //   $view->destroy();
  //   return array('#markup' => $output);
  // }

}

/**
 * A null implementation of alchemistField.
 */
class alchemistField extends alchemistFieldFieldableAbstract {
}
