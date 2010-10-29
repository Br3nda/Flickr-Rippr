<?php

function flickrrippr_secret_form() {
  $form = array();
  $form['flickrphoto_id'] = array(
    '#type' => 'textfield',
    '#title' => 'Photo ID',
    '#required' => TRUE,
  );

  $form['secret'] = array(
    '#type' => 'textfield',
    '#title' => 'secret',
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

function flickrrippr_secret_form_submit($form, $form_state) {
  
  $node = flickrrippr_makenode(
    $form_state['values']['flickrphoto_id'],
    $form_state['values']['secret']
  );
  if (!$node) {
    form_set_error('', 'That didn\'t work');
    return;
  }
  drupal_goto('node/'. $node->nid);
}