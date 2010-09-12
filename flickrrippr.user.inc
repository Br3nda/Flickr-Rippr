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
  $avatar = 'http://farm1.static.flickr.com/2/buddyicons/36251685@N00.jpg';
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

