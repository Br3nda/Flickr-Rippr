<?php
// $Id: flickrrippr.user.inc,v 1.14.4.7 2010/09/30 01:06:31 taniwha Exp $
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
  db_update('flickrusers')
    ->fields(
      array(
        'username' => $user_info['username'],
        'flickrispro' => $user_info['ispro'],
        'flickrphotosurl' => $user_info['photosurl'],
        'flickrprofileurl' => $user_info['profileurl'],
        'flickravatar' => $avatar,
      )
    )
    ->condition('uid', $flickruser->uid)
    ->condition('flickrid', $flickuser->flickrid)
    ->execute();

//   drupal_set_message(print_r($user_info, 1));
  $result = db_query("UPDATE {flickrusers} SET flickrusername=:username, flickrispro=:ispro, flickrphotosurl=:photosurl, flickrprofileurl=:profileurl, flickravatar=:avatar
    WHERE uid=:uid AND flickrid=:id",
    array(

    )
  );
}


/** 
 * For editing one account
 */
function flickrrippr_user_edit_account($uid, $flickrid) {
  $fuser = flickrrippr_get_flickruser($uid, $flickrid);
  error_log(print_r($fuser, 1));
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
function flickrrippr_user_settings_form($form, &$form_state, $flickruser) {
 
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
      '#value' => flickrrippr_user_avatar($flickruser) . '<p><strong>' . t('Flickr Username:') . ' </strong><br/>' . check_plain($flickruser->flickrusername) . '</p>',
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
    '#weight' => 9,
    );

  if (isset($flickruser->flickrid)) {
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 9,
    );
  }
  return $form;
}




function flickrrippr_user_settings_form_validate($form, &$form_state) {

  if (!isset($form_state['values']['flickrid'])) {
    $existing = db_select('flickrusers')   
      ->fields('flickrusers', array('uid'))
      ->condition('flickrusername', $form_state['values']['flickr_username'])
      ->execute()
      ->fetchField();
    if ($existing) {
      form_set_error('flickr_username', t('Cannot add this flickr account, it is already configured on this website.' . print_r($existing, 1)));
      return;
    }
  }
  
  if (isset($form_state['values']['flickr_username'])) {
    $flickr_details = flickrrippr_user_fetch_user_from_flickr($form_state['values']['flickr_username']);
    if (!$flickr_details) {
      form_set_error('flickr_username', t('Account not found on flickr.com'));
    }
  }
}
/**
 * Save submission of a user editting their own settings
 */
function flickrrippr_user_settings_form_submit($form, &$form_state) {

  if (!empty($form_state['values']['uid'])) {
    $user = user_load(intval($form_state['values']['uid']));
  }
  else {
    GLOBAL $user;
  }

  //delete button 
  if ($form_state['values']['op'] == 'Delete') {
    drupal_goto('user/' . $user->uid . '/flickrrippr/delete/' . check_plain($form_state['values']['flickrid']));
  }


  //is editing existing
  if (isset($form_state['values']['flickrid'])) {
    $flickr_account = flickrrippr_get_flickruser($form_state['values']['uid'], $form_state['values']['flickrid']);
  }

  //this fetches from flickr.com



  if (isset($flickr_account->flickrusername)) {
    unset($record);
    $record = (object)array(
      'tag' => $$form_state['values']['flickr_tag'],
      'uid' => $user->uid,
      'flickrid' => $form_state['values']['flickrid']
    );
    drupal_write_record('flickrusers', $record, array('uid', 'flickrid')); //
    drupal_set_message(t("Flickr account updated"));
  }
  else {
    $flickr_details = flickrrippr_user_fetch_user_from_flickr($form_state['values']['flickr_username']);
    $flickrid = $flickr_details['id'];

    unset($record);
    $record->flickrusername = $form_state['values']['flickr_username'];
    $record->flickrid = $flickrid;
    $record->tag = $form_state['values']['flickr_tag'];
    $record->uid = $user->uid;
    drupal_write_record('flickrusers', $record);
    drupal_set_message(t("Flickr username set"));

    $flickr_account->flickrid = $flickrid;
    $flickr_account->uid = $user->uid;
  }
    
  flickrrippr_fetch_user_info_and_save($flickr_account);


  drupal_goto('user/' . $user->uid . '/flickrrippr');
}

function flickrrippr_user_fetch_user_from_flickr($flickrusername) {
    $flickr = flickrapi_phpFlickr();
    return  $flickr->people_findByUsername($flickrusername);
}

//TODO Move this all the themes
function flickrrippr_user_view_photos($uid, $flickruserid) {
  $output = '';

  $result = pager_query("SELECT * FROM {flickrphotos} WHERE owner_nsid='%s' AND failed <> 1 ORDER BY flickrphoto_id DESC", 60, 0, NULL, $flickruserid);
  
  $rows = array();
  while ($photo = db_fetch_object($result)) {
    //   drupal_set_message(print_r($photo, 1));
    $jpeg_path = flickrrippr_path((array)$photo, 't');
    $col[] = l(theme('image', $jpeg_path, $photo->title, $photo->title, NULL, FALSE), 'node/' . $photo->nid, array('html' => TRUE));
    if (count($col) > 6) {
      $rows[] = $col;
      $col = array();
    }
  }
  $output .= theme('pager');
  $output .= theme('table', array(), $rows);
  return $output;
}