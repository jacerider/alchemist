<?php

class alchemistField_link_field extends alchemistField {

  /**
   * The default value added to the field when no value is defined.
   */
  public function defaultValue(){
    $default = array();
    $default['url'] = '';
    $default['title'] = '';
    $default['attributes'] = array();
    $default['display_url'] = '';
    $default['html'] = '';
    return $default;
  }

}
