<?php



function alchemist_link($js = FALSE){
  $output = array();

  $output['form'] = drupal_get_form('alchemist_link_form');

  if( $js != 'ajax'){
    return $output;
  }

  $commands = array();
  $commands[] = ajax_command_alchemist_panel_top(drupal_render($output), '<i class="fa fa-link"></i> ' . t('Link'));
  ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
  return;
}

function alchemist_link_form($form, &$form_state){

  $form['#id'] = 'alchemist-link-form';

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to display'),
    '#size' => 60,
    '#maxlength' => 128,
    '#attributes' => array('class'=>array('input-block-level')),
    '#id' => 'alchemist-link-title'
  );

  $form['web'] = array(
    '#type' => 'textfield',
    '#title' => t('URL for the link'),
    '#size' => 60,
    '#maxlength' => 128,
    '#attributes' => array('class'=>array('input-block-level'),'data-type'=>'raw'),
    '#id' => 'alchemist-link-web',
  );

  if(module_exists('linkit')){

    $plugins = linkit_profile_field_load_all();
    if(!empty($plugins)){
      // Load the profile.
      $profile = array_pop($plugins);
      // Load the insert plugin for the profile.
      $insert_plugin = linkit_insert_plugin_load($profile->data['insert_plugin']['plugin']);

      $element = &$form['web'];

      // Set the field ID.
      $field_id = $element['#id'];

      $field_js = array(
        'data' => array(
          'linkit' => array(
            'fields' => array(
              $field_id => array(
                'profile' => $profile->name,
                'insert_plugin' => $profile->data['insert_plugin']['plugin'],
                'url_method' => $profile->data['insert_plugin']['url_method'],
                // @TODO: Add autocomplete settings.
              ),
            ),
          ),
        ),
        'type' => 'setting',
      );

      // Link fields can have a title field.
      if ($element['#type'] == 'link_field') {
        if (isset($instance['settings']['title']) && in_array($instance['settings']['title'], array('optional', 'required'))) {
          $field_js['data']['linkit']['fields'][$field_id]['title_field'] = $element['#id'] . '-title';
        }
      }

      // Attach js files and settings Linkit needs.
      $element['#attached']['library'][] = array('linkit', 'base');
      $element['#attached']['library'][] = array('linkit', 'field');
      $element['#attached']['js'][] = $insert_plugin['javascript'];
      $element['#attached']['js'][] = $field_js;

      $button_text = !empty($instance['settings']['linkit']['button_text']) ? $instance['settings']['linkit']['button_text'] : t('Search Local Content');
      $element['#field_suffix'] = '<a class="button tiny linkit-field-button linkit-field-' . $field_id . '" href="#"><i class="fa fa-search"></i> '.$button_text.'</a>';
    }
  }

  $form['new'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open in a new window'),
    '#id' => 'alchemist-link-new',
  );

  $form['actions'] = array('#type' => 'actions');
  // $form['actions']['save'] = array(
  //   '#markup' => l('Insert Link', '', array('attributes'=>array('class'=>array('btn', 'btn-success', 'alchemist-link-save')))),
  //   '#weight' => 100
  // );
  $form['actions']['save'] = array(
    '#type' => 'alchemist_submit',
    '#value' => t('Insert') . ' <i class="fa fa-link"></i> <span>' . t('Link') . '</span>',
    '#attributes' => array('class'=>array('button', 'expand', 'alchemist-link-save')),
    '#weight' => 100,
  );
  $form['actions']['cancel'] = array(
    '#markup' => l('Cancel', '', array('attributes'=>array('class'=>array('button', 'secondary', 'expand', 'alc-panel-close')))),
    '#weight' => 101
  );

  return $form;
}
