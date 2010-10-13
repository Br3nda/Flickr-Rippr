<?php

// $Id: flickrrippr.inc,v 1.32.2.26 2010/10/13 22:54:54 taniwha Exp $
/**
 * @file flickrriprr.inc 
 * Functions that are likely to be of use to other modules, are in this file.
 * PHP Flickr: http://phpflickr.com/docs/
 * Flickr API docs http://www.flickr.com/services/api/
 */


function flickrrippr_get_avatar($user_info) {
  return 'http://farm'. $user_info['iconfarm'] .'.static.flickr.com/'. $user_info['iconserver'] .'/buddyicons/'. $user_info['nsid'] .'.jpg';
}
/**
* Helper function, to get drupal user details for a flickr user (string of their username of flickr)
*/
function flickrrippr_get_user_by_flickrusername($fuser) {
  if (!$fuser) {
    drupal_set_message(t("Refusing to search for blank username"));
    return;
  }
  
  $flickruser = db_fetch_object(db_query("SELECT * FROM {flickrusers} WHERE LOWER(flickrusername) = '%s'", strtolower($fuser)));
  if (!$flickruser) {
    drupal_set_message(t("Flickruser %flickrusername is not a registered user of this site", array('%flickrusername' => $fuser)));
    return;
  }
  
  return user_load(array('uid' => $flickruser->uid));
}

/**
 * Fetch info from our database about a user, by nsid
 * 
 * @param ndid string primary key flickr users
 * @return User
 */
function flickrrippr_get_user_by_nsid($nsid) {
    if (!$nsid) {
    drupal_set_message(t("Refusing to search for blank nsid"));
    return;
  }
  
  $flickruser = db_fetch_object(db_query("SELECT * FROM {flickrusers} WHERE flickrid = '%s'", $nsid));
  if (!$flickruser) {
    drupal_set_message(t("NSID %nsid is not a registered user of this site.", array('%nsid' => $nsid)), 'error');
    return;
  }
 
  return user_load(array('uid' => $flickruser->uid));
}



/**
 * Work out the path to a photo - also handles the cache on the local filesystem
 */
function flickrrippr_path($node, $size = FALSE) {
  
  $flickr_photo = $node->flickr_photo;

  if (empty($flickr_photo['secret'])) {
    drupal_set_message(t("Cannot calculate path without the 'secret'. Something is wrong with node %nid.", array('%nid' => $node->nid)), 'error');
    
    return '';
  }
  $path_handler = variable_get('flickrrippr_path_handler', 0);
  if (!$path_handler) {
    //none chosen, use default
    return flickrrippr_path_remote($node, $size);
  }

  return module_invoke($path_handler, 'get_path', $node, $size);
}

/**
 * Calculates path to photo on flickr
 */
function flickrrippr_path_remote($node, $size) {
  $flickr_photo = $node->flickr_photo;
 //http://farm{farm-id}.static.flickr.com/{server-id}/{id}_{secret}.jpg
  return 'http://farm'. $flickr_photo['farm'] .'.static.flickr.com/'. $flickr_photo['server'] .'/'. $flickr_photo['flickrphoto_id'] .'_'. $flickr_photo['secret'] . ($size ? '_'. $size : '') .'.jpg';
}

/**
 * Makes a node for a flickr photo - or updates if it already exists
 */
function flickrrippr_makenode($photo_id) {

  if (!$photo_id) {
    drupal_set_message(t("Photo_id required."), 'error');
    return FALSE;
  }

  //TODO, this code is in two place -- make a function
  $local_cached_details = flickrrippr_get_photo_details_by_photoid($photo_id);
  $age = $local_cached_details ? strtotime('now') - $local_cached_details->lastfetched : 0;
  //
  watchdog('flickrrippr', 'Photo was fetched %age seconds ago', array('%age' => $age));
  //if it's already been fetched in the last hour....
//   if ($age < 60*60) {
//     return FALSE;
//   }


  $flickr = flickrapi_phpFlickr();
  if (!$flickr) {
    drupal_set_message(t("Unable to query flickr.com, library is missing."), 'error');
    return FALSE;
  }

  //Info to put into the node
  $flickr_info = $flickr->photos_getInfo($photo_id);
  //we can't get info from flickr -- mark this node as bad
  if (!$flickr_info) {
    drupal_set_message(t('No info found on photo %photoid.', array('%photoid' => $photo_id)), 'error');
    db_query("UPDATE {flickrphotos} SET failed = 1 WHERE flickrphoto_id = %d", $photo_id);
    //TODO unpublish the node?
    
    return FALSE;
  }
  

  $flickr_context = $flickr->photos_getContext($photo_id);
  

  //flickr username
  $flickrusername = $flickr_info['owner']['username'];
  $flickrid = $flickr_info['owner']['nsid'];
  
  //get user details

  $user = flickrrippr_get_user_by_nsid($flickrid);
  if (!$user) {
    drupal_set_message(t("User %flickrusername doesn't exist, unable to save photo %photo_id", array('%flickrusername' => $flickrusername, '%photo_id' => $photo_id)), 'error');
    return FALSE;
  }

  //assemble everything to pass to hook
  $details = array(
    'flickrid' => $flickrid,
    'photo_id' => $flickr_info['id'],
    'flickrusername' => $flickrusername,
    'context' => $flickr_context,
//     'info' => $flickr_info,
    'user' => $user,
  );

  $details += $flickr_info;
  $details += $flickr_context;
  foreach($flickr_info['dates'] as $key => $d) {
    $details['date_'. $key] = $d;
  }
  foreach($flickr_info['owner'] as $key => $o) {
    $details['owner_'. $key] = $o;
  }
  $details['flickrphoto_id'] = $details['id'];
  $details['photopage'] = $flickr_info['urls']['url'][0]['_content'];
//   drupal_set_message(print_r($flickr_info, 1));

  $fields = array('farm', 'server', 'secret', 'dateuploaded', 'license', 'rotation', 'title', 'date_posted', 'date_taken', 'date_lastupdate', 'views', 'photopage', 'media', 'description', 'owner_nsid');
  foreach ($fields as $f) {
    $values[$f] = $details[$f];
  }

  
  //call the hook
  $node_handler = variable_get('flickrrippr_node_handler', 'flickrrippr');
//   drupal_set_message("Node handler = $node_handler");
  $node = module_invoke($node_handler, 'flickrrippr_node_save_handler', $details);

  if (!$node) {
    drupal_set_message(t("Make node failed, giving up"), 'error');
    return FALSE;
  }
 
  if (isset($node->is_new) && $node->is_new) {
//     drupal_set_message("New node, nid = $node->nid");
  
    $values[] = $details['id'];
    $values[] = $node->nid;
    $values[] = strtotime('now');

    $result = db_query("INSERT INTO {flickrphotos} (". implode($fields, ', ') . ", flickrphoto_id, nid, lastfetched)
      VALUES (". str_repeat("'%s', ",  count($fields) ). " %d, %d, %d )",
      $values);
      drupal_set_message(t("Saved new photo %photo_id, ". l($node->title, 'node/' . $node->nid), array('%photo_id' => $photo_id)));
  }
  else {

    $sql = "UPDATE {flickrphotos} SET ";
    foreach($fields as $v) {
      $sql .= "$v = '%s', ";
    }
    $sql .= " lastfetched=%d, failed=0 WHERE flickrphoto_id=%d AND nid=%d ";

    $values[] = strtotime('now');
    $values[] = $details['id'];
    $values[] = $node->nid;

    $result = db_query($sql, $values);

    drupal_set_message(t("Updated photo '%photo_id', !link", array('!link' => l($node->title, 'node/' . $node->nid), '%photo_id' => $photo_id)));
  }
  //the extra hooks
  module_invoke_all('flickrrippr_node_alter', $node, $details);

  return $node;

}

function flickrrippr_flickrrippr_node_save_handler($details) {
  $photo_id = $details['id'];
  if (!$photo_id) {
    drupal_set_message(t("Refusing to update/create node for null photo_id"), 'error');
    return FALSE;
  }
  $nid = flickrrippr_photo_get_nid($photo_id) ;

  if (!empty($nid) && $nid) {
    //already exists
    $node = node_load(array('nid' => $nid));
  }
  $node->type = 'flickrrippr_photo';

  $node->title = htmlentities($details['title']);
  $node->created = strtotime($details['date_taken']);
  $node->changed = strtotime($details['date_posted']);

  //if owner not set, then set it.
  if (!isset($node->uid)) {
    $node->uid = $details['user']->uid;
    $node->name = $details['user']->name;
  }
  node_save($node);
  return $node;
}

/**
 * get the photo id for a nid
 */
function flickrrippr_node_get_photoid($nid) {
  $rec = db_fetch_object(db_query("SELECT flickrphoto_id FROM {flickrphotos} WHERE nid = %d", $nid));
  return (!empty($rec->flickrphoto_id) ? $rec->flickrphoto_id : NULL);
}

/**
 * get the node id for a flickr photo id
 */
function flickrrippr_photo_get_nid($photo_id) {
  $rec = db_fetch_object(db_query("SELECT nid FROM {flickrphotos} WHERE flickrphoto_id = '%s'", $photo_id));
  return (!empty($rec->nid) ? $rec->nid : NULL);
}

/**
 * get the photo id for a nid
 */
function flickrrippr_get_photo_details_by_photoid($pid) {
  return db_fetch_object(db_query("SELECT * FROM {flickrphotos} WHERE flickrphoto_id = %d", $pid));
}

/**
 * Get a user's flickr settings from our database
 */
function flickrrippr_get_flickr_accounts($uid) {
  $result = db_query("SELECT * FROM {flickrusers} WHERE uid = %d ORDER BY lower(flickrusername)", $uid);
  $flickr_users = array();
  while ($fuser = db_fetch_object($result)) {
    $flickr_users[] = $fuser;
  }
  return $flickr_users;
  
}