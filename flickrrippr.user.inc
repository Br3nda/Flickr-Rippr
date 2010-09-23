<?php
// $Id: flickrrippr.user.inc,v 1.3.2.7 2010/09/23 23:11:40 taniwha Exp $
/**
 * @file functions related to users of flickr rippr
 */


function flickrrippr_fetch_user_info_and_save($flickruser) {

 //search flickr for latest photos from that user.
  $flickr = flickrapi_phpFlickr();
  
  if (!$flickr) {
    drupal_set_message(t("Unable to query flickr.com"), 'error');
    watchdog('flickrrippr', 'Unable to query flickr.com');
    return FALSE;
  }

  //check their username is the same
  $user_info = $flickr->people_getInfo($flickruser->flickrid);
  //e.g. Array ( [id] => 95198959@N00 [nsid] => 95198959@N00 [ispro] => 1 [iconserver] => 5 [iconfarm] => 1 [path_alias] => alan50 [username] => Alan50 [realname] => Alan [location] => Hamilton, New Zealand [photosurl] => http://www.flickr.com/photos/alan50/ [profileurl] => http://www.flickr.com/people/alan50/ [mobileurl] => http://m.flickr.com/photostream.gne?id=274055 [photos] => Array ( [firstdatetaken] => 2002-11-23 12:11:53 [firstdate] => 1111902587 [count] => 941 ) )
  
  $avatar = flickrrippr_get_avatar($user_info);
//   drupal_set_message(print_r($user_info, 1));
  $result = db_query("UPDATE {flickrusers} SET flickrusername='%s', flickrispro=%d, flickrphotosurl='%s', flickrprofileurl='%s', flickravatar='%s'
    WHERE uid=%d AND flickrid='%s'",
    $user_info['username'],
    $user_info['ispro'],
    $user_info['photosurl'],
    $user_info['profileurl'],
    $avatar,
    $flickruser->uid,
    $flickruser->flickrid
  );
}


/** 
 * For editing one account
 */
function flickrrippr_user_edit_account($uid, $flickrid) {
  $fuser = flickrrippr_get_flickr_user($uid, $flickrid);
  $output = drupal_get_form('flickrrippr_user_settings_form', $fuser);
  return $output;
}




/** 
 * Add a new account
 */
function flickrrippr_user_add_account($uid) {
  $flickruser->uid = $uid;
  $output = drupal_get_form('flickrrippr_user_settings_form', $flickruser);
  return $output;
}


/**
 * Form for a user editing their own settings
 */
function flickrrippr_user_settings_form($a, $flickruser) {
  
  $form = array();
  if (isset($flickruser->flickrusername)) {
    drupal_set_title(t('Flickr Rippr settings for %username', array('%username' => $flickruser->flickrusername)));
  }

  if (isset($flickruser->flickrid)) {
    $form['flickrid'] = array('#type' => 'hidden', '#value' => $flickruser->flickrid);
  }
  $form['uid'] = array('#type' => 'hidden', '#value' => $flickruser->uid);

  if (isset($flickruser->flickrid)) {
    $form['flickr_username'] = array(
      '#type' => 'markup', 
      '#value' => '<p><strong>'. t('Flickr Username: ') .'</strong><br/>'. check_plain($flickruser->flickrusername) .'</p>',
    );
  }
  else {
    $form['flickr_username'] = array(
      '#type' => 'textfield',
      '#title' => t('Flickr Username'),
      '#default_value' => isset($flickruser->flickrusername) ? $flickruser->flickrusername : '',
      '#required' => TRUE
    );
  }

  $form['flickr_tag'] = array(
      '#type' => 'textfield',
      '#title' => t('Filter by Tag'),
      '#description' => t('If entered, only photos that have this tag will be imported. Be aware that your friends and family on flickr may also be allowed to tag your photos.'),
      '#default_value' => isset($flickruser->tag) ? $flickruser->tag : '',
    );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    );

  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    );
  return $form;
}




function flickrrippr_user_settings_form_validate($form, &$form_state) {
  //TODO, check the submitted username exists on flickr?
  //query api
  //and store the 3432@N00 uid then
}
/**
 * Save submission of a user editting their own settings
 */
function flickrrippr_user_settings_form_submit($form, &$form_state) {

  if (!empty($form_state['values']['uid'])) {
    $user = user_load(array('uid' => intval($form_state['values']['uid'])));
  }
  else {
    GLOBAL $user;
  }

  //delete button 
  if ($form_state['values']['op'] == 'Delete') {
    drupal_goto('user/'. $user->uid .'/flickrrippr/delete/'. $form_state['values']['flickrid']);
  }


  //is editing existing
  if (isset($form_state['values']['flickrid'])) {
  $flickr_account = flickrrippr_get_flickr_user($form_state['values']['uid'], $form_state['values']['flickrid']);
  }

  if (isset($flickr_account->flickrusername)) {
    $flickr = flickrapi_phpFlickr();
    $flickr_response = $flickr->people_getInfo($form_state['values']['flickrid']);
    $result = db_query("UPDATE {flickrusers} SET tag='%s', flickrusername='%s' WHERE uid = %d AND flickrid='%s'",
      $form_state['values']['flickr_tag'],
      $flickr_response['username'],
      $user->uid,
      $form_state['values']['flickrid']);
    drupal_set_message(t("Flickr account updated"));
  }
  else {
    $flickr = flickrapi_phpFlickr();
    $flickr_response = $flickr->people_findByUsername($form_state['values']['flickr_username']);
    $flickrid = $flickr_response['id'];

    $result = db_query("INSERT INTO {flickrusers} (flickrusername, flickrid, tag, uid) VALUES ('%s', '%s', '%s', %d)",
      $form_state['values']['flickr_username'],
      $flickrid,
      $form_state['values']['flickr_tag'],
      $user->uid);
    drupal_set_message(t("Flickr username set"));
  }
  drupal_goto('user/'. $user->uid .'/flickrrippr');
}
