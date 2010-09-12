<?php

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
  drupal_set_message(print_r($user_info, 1));
  $result = db_query("UPDATE {flickrusers} SET flickrusername='%s', flickrispro=%d, flickrphotosurl='%s', flickrprofileurl='%s', flickravatar='%s' WHERE uid=%d AND flickrid='%d'",
    $user_info['username'],
    $user_info['ispro'],
    $user_info['photosurl'],
    $user_info['profileurl'],
    $avatar,
    $flickruser->uid,
    $flickruser->flickrid
  );
//         'flickrusername' => array('type' => 'text', 'not null' => TRUE),
//       'flickrispro' => array('type' => 'int', 'size' => 'tiny', 'default' => 0),
//       'flickrphotosurl' => array('type' => 'text'),
//       'flickrprofileurl' => array('type' => 'text'),
//       'flickravatar' => array('type' => 'text'),
}

