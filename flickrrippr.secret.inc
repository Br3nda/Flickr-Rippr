<?php

function flickrrippr_secret_form() {
  $form = array();
  
  $form['jpeg_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Url to jpeg hosted on flickr.com'),
  );
  
//   $form['flickrphoto_id'] = array(
//     '#type' => 'textfield',
//     '#title' => 'Photo ID',
//   );
// 
//   $form['secret'] = array(
//     '#type' => 'textfield',
//     '#title' => 'secret',
//   );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

function flickrrippr_secret_form_submit($form, $form_state) {

/*  
  
  if (empty($form_state['values']['jpeg_url'])) {
    $flickrphoto_id = $form_state['values']['flickrphoto_id'];
    $secret = $form_state['values']['secret'];
  }
  else {*/
    $jpeg_url = $form_state['values']['jpeg_url'];
    $bits = preg_split('!(_|/|\.)!', $jpeg_url);
    $flickrphoto_id = $bits[7];
    $secret = $bits[8];
    drupal_set_message(t('photoid = @id, secret=@secret', array('@id' => $flickrphoto_id, '@secret' => $secret)));
//   }
  
  $node = flickrrippr_makenode($flickrphoto_id, $secret);
  
  if (!$node) {
     form_set_error('', 'That didn\'t work');
     return;
  }
  drupal_goto('node/'. $node->nid);
}