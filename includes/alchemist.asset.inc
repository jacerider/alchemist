<?php

function alchemist_asset_browser(AssetType $asset_type, $type = 'ajax'){
  $output = array();
  $output['#attached']['js'][] = drupal_get_path('module', 'alchemist') . '/js/alchemist.asset.js';
  $output['browser']['view'] = asset_type_browser($asset_type, 'asset_preview', TRUE);
  $output['browser']['#prefix'] = '<div class="alchemist-asset-browser">';
  $output['browser']['#suffix'] = '</div>';

  $classes = array('button','expand');
  if($type == 'ajax') $classes[] = 'use-ajax';
  $output['new']['#markup'] = l(t('Create New !icon %type', array('!icon'=>'<i class="fa fa-'.$asset_type->data['icon'].'"></i>','%type'=>$asset_type->name)), 'alchemist/asset/' . $asset_type->type, array('html'=>TRUE, 'query' => array('r' => current_path()), 'attributes' => array('class' => $classes)));

  if($type == 'ajax'){
    $commands = array();
    $commands[] = ajax_command_alchemist_panel_top(drupal_render($output), t('Browse').' <i class="fa fa-'.$asset_type->data['icon'].'"></i> <span>' . $asset_type->plural . '</span>');
    ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
    return;
  }

  return $output;

  drupal_set_message(t('You must have javascript enabled to use this feature.'));
  drupal_goto();
}

/**
 * Asset creation and editing.
 */
function alchemist_asset($asset, $type = 'ajax'){
  if(!is_a($asset, 'Asset')){
    $asset = asset_create(array('type' => $asset));
  }
  $title = 'Create';
  if($asset->id){
    $title = 'Edit';
  }
  $asset_type = asset_type_load($asset->type);
  drupal_set_title(t($title).' ' . $asset_type->name);
  $output = array();
  $output['#attached']['js'][] = drupal_get_path('module', 'alchemist') . '/js/alchemist.asset.js';

  $output['form'] = drupal_get_form('alchemist_asset_form', $asset, $type);

  if($type == 'ajax'){
    $commands = array();
    $commands[] = ajax_command_alchemist_panel_top(drupal_render($output), t($title).' <i class="fa fa-'.$asset_type->data['icon'].'"></i> <span>' . $asset_type->name . '</span>');
    ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
    return;
  }

  return $output;

  drupal_set_message(t('You must have javascript enabled to use this feature.'));
  drupal_goto();
}

function alchemist_asset_form($form, &$form_state, $asset, $type = 'ajax'){
  form_load_include($form_state, 'inc', 'asset', 'includes/asset.admin');
  $form += asset_edit_form($form, $form_state, $asset);
  $asset_type = $form_state['asset_type'];
  $form['actions']['submit']['#type'] = 'alchemist_submit';
  $form['actions']['submit']['#value'] = t('Save') . " <i class='fa fa-{$asset_type->data['icon']}'></i> " . $asset_type->name;
  $form['actions']['submit']['#attributes']['class'][] = 'button';
  $form['actions']['submit']['#attributes']['class'][] = 'expand';
  $wrapper = 'alchemist-asset-form';
  $form['#id'] = $wrapper;
  unset($form['actions']['delete']);
  $url = '';
  if(isset($_GET['r'])){
    $form['actions']['cancel']['#markup'] = l('Return', $_GET['r'], array('html'=>TRUE, 'attributes' => array('class' => array('button','muted','expand','use-ajax'))));
  }
  else{
    $form['actions']['cancel']['#markup'] = l('Cancel', $url, array('html'=>TRUE, 'attributes' => array('class' => array('button','muted','expand','alc-panel-close'))));
  }
  // $form['#attributes']['class'][] = 'form-wrapper';
  if($asset->id && empty($asset_type->data['require_instance'])){
    unset($form['instance_form']);
  }
  if($type == 'ajax'){
    $form['actions']['submit']['#ajax'] = array(
      'wrapper' => $wrapper,
      'callback' => 'alchemist_asset_form_ajax_submit',
    );
  }
  return $form;
}

function alchemist_asset_form_submit($form, &$form_state){
}

function alchemist_asset_form_ajax_submit($form, &$form_state){
  if(form_get_errors()) return $form;

  $asset = $form_state['asset'];

  // Clear the asset cache so assets get loaded with updated information.
  entity_get_controller('asset')->resetCache();
  $asset = asset_load_js($asset->id, NULL, 'asset', TRUE);

  $commands = array();
  $commands[] = ajax_command_alchemist_panel_close();
  $commands[] = ajax_command_alchemist_asset($asset->id);
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Instance creation and editing.
 */
function alchemist_asset_instance(Asset $asset, $instance, $type = 'ajax'){
  // Make sure this asset type has public instance settings.
  $asset_type = asset_type_load($asset->type);
  if(empty($asset_type->public)){
    return alchemist_asset($asset, $type);
  }

  if(!is_a($instance, 'AssetInstance')){
    $instance = asset_instance_create($asset);
  }

  // When instance matches default instance of an asset, we want to open it up
  // for the creation of a new instance.
  if($instance->id == $asset->instance_id){
    $instance->id = NULL;
    $instance->is_new = TRUE;
  }

  $output = array();
  $output['#attached']['js'][] = drupal_get_path('module', 'alchemist') . '/js/alchemist.asset.js';

  $output['form'] = drupal_get_form('alchemist_asset_instance_form', $asset, $instance, $type);

  if($type == 'ajax'){
    $commands = array();
    $commands[] = ajax_command_alchemist_panel_top(drupal_render($output), $asset_type->name . ' <i class="fa fa-'.$asset_type->data['icon'].'"></i> <span> Settings</span>');
    ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
    return;
  }

  return $output;

  drupal_set_message(t('You must have javascript enabled to use this feature.'));
  drupal_goto();
}

function alchemist_asset_instance_form($form, &$form_state, $asset, $instance, $type = 'ajax'){
  form_load_include($form_state, 'inc', 'asset', 'includes/assetInstance.admin');
  $form += asset_instance_edit_form($form, $form_state, $asset, $instance);
  if($type == 'ajax'){
    $form['actions']['submit']['#ajax'] = array(
      'callback' => 'alchemist_asset_instance_form_ajax_submit',
    );
  }

  $asset_type = asset_type_load($asset->type);
  $form['actions']['submit']['#type'] = 'alchemist_submit';
  $form['actions']['submit']['#value'] = t('Save') . ' <i class="fa fa-'.$asset_type->data['icon'].'"></i> <span> Settings</span>';
  $form['actions']['submit']['#attributes']['class'][] = 'button';
  $form['actions']['submit']['#attributes']['class'][] = 'expand';
  unset($form['actions']['delete']);

  $options = array();
  $options['attributes']['class'] = array('button','expand','secondary','tiny');
  $options['query'] = array('r' => current_path());
  $options['html'] = TRUE;
  if($type == 'ajax') $options['attributes']['class'][] = 'use-ajax';

  $form['actions']['edit']['#markup'] = l(t('Edit') . ' <i class="fa fa-edit"></i> <span>' . $asset_type->name . '</span>', 'alchemist/asset/' . $asset->id, $options);
  $form['actions']['cancel']['#markup'] = l('Cancel', $url, array('html'=>TRUE, 'attributes' => array('class' => array('button','muted','expand','alc-panel-close'))));

  // unset($form['actions']['submit']['#submit']);
  return $form;
}

function alchemist_asset_instance_form_submit($form, &$form_state){
}

function alchemist_asset_instance_form_ajax_submit($form, &$form_state){
  $asset = $form_state['asset'];
  $instance = $form_state['asset_instance'];
  asset_load_js($asset->id, $instance->id, 'asset', TRUE);

  $commands = array();
  $commands[] = ajax_command_alchemist_panel_close();
  $commands[] = ajax_command_alchemist_asset($asset->id, $instance->id);
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
