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
  //e.g. Array ( [id] => 95198959@N00 [nsid] => 95198959@N00 [ispro] => 1 [iconserver] => 5 [iconfarm] => 1 [path_alias] => alan50 [username] => Alan50 [realname] => Alan [location] => Hamilton, New Zealand [photosurl] => http://www.flickr.com/photos/alan50/ [profileurl] => http://www.flickr.com/people/alan50/ [mobileurl] => http://m.flickr.com/photostream.gne?id=274055 [photos] => Array ( [firstdatetaken] => 2002-11-23 12:11:53 [firstdate] => 1111902587 [count] => 941 ) )
  
  $avatar = 'http://farm'. $user_info['iconfarm'] .'.static.flickr.com/'. $user_info['iconserver'] .'/buddyicons/'. $user_info['nsid']. '.jpg';
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
