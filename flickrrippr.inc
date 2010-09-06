<?php
/**
 * @file flickrriprr.inc 
 * Functions that are likely to be of use to other modules, are in this file.
 * PHP Flickr: http://phpflickr.com/docs/
 * Flickr API docs http://www.flickr.com/services/api/
 */
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
  
  $user = user_load(array('uid' => $flickruser->uid));
  return $user;
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
    drupal_set_message(t("NSID %nsid is not a registered user of this site.", array('%nsid' => $nsid)));
    return;
  }
  
  $user = user_load(array('uid' => $flickruser->uid));
  return $user;
}



/**
* Work out the path to a photo - also handles the cache on the local filesystem
*/
function flickrrippr_path($flickr_photo, $size = FALSE, $node=null) {
  $path_handler = variable_get('flickrrippr_path_handler', 0);
  if (!$path_handler) {
    //none chose, use default
    return flickrrippr_path_remote();
  }
  return module_invoke($path_handler, 'get_path', $flickr_photo, $size, $node);
}

/**
 * Calculates path to photo on flickr
 */
function flickrrippr_path_remote($flickr_photo, $size) {
 //http://farm{farm-id}.static.flickr.com/{server-id}/{id}_{secret}.jpg
  return 'http://farm'. $flickr_photo['farm'] .'.static.flickr.com/'. $flickr_photo['server'] .'/'. $flickr_photo['id'] .'_'. $flickr_photo['secret'] . ($size ? '_'. $size : '') .'.jpg';
}

/**
 * Makes a node for a flickr photo - or updates if it already exists
 */
function flickrrippr_makenode($photo_id) {


  if (!$photo_id) {
    drupal_set_message(t("Photo_id required"), 'error');
    return;
  }

//   drupal_set_message(t("Fetching data from flickr .com for photoid=%photo_id", array('%photo_id' => $photo_id)));
// 
  $nid = flickrrippr_photo_get_nid($photo_id) ;
  if (!empty($nid)) {
//     //already exists
    $node = node_load(array('nid' => $nid));
//     drupal_set_message(t('Existing node id = !nid. !link.', array('!nid' => $nid, '!link' => l(check_plain($node->title), 'node/'. $nid))));
  }
  else {
  
  }

  $flickr = flickrapi_phpFlickr();
  if (!$flickr) {
    drupal_set_message(t("Unable to query flickr.com, library missing"));
    return $output;
  }

  //Info to put into the node
  $flickr_info = $flickr->photos_getInfo($photo_id);
  //we can't get info from flickr -- mark this node as bad
  if (!$flickr_info) {
    drupal_set_message(t('No info found on photo %photoid.', array('%photoid' => $photo_id)), 'error');
    db_query("UPDATE {flickrphotos} SET failed = 1 WHERE flickrphoto_id = %d", $photo_id);
    node_save($node);
    return;
  }
  
  $flickr_context = $flickr->photos_getContext($photo_id);
  

  //flickr username
  $flickrusername = $flickr_info['owner']['username'];
  $flickrid = $flickr_info['owner']['nsid'];
  
//   drupal_set_message(t("Making a node for photo by %flickrusername", array('%flickrusername' => $flickrusername)), 'error');

  //get user details
//   $user = flickrrippr_get_user_by_flickrusername($flickrusername);
  $user = flickrrippr_get_user_by_nsid($flickrid);
  if (!$user) {
    drupal_set_message(t("User %flickrusername doesn't exist, unable to save photo %photo_id", array('%flickrusername' => $flickrusername, '%photo_id' => $photo_id)), 'error');
    return FALSE;
  }

  //Full (normal) sized image
  $photo_path =  flickrrippr_path($flickr_info, variable_get('flickrrippr_page_size', 'm'), $node);
  $thumbnail_path =  flickrrippr_path($flickr_info, variable_get('flickrrippr_page_size', 'm'), $node);
  $description = $flickr_info['description'];
  $title = $flickr_info['title'];
  $page_url = $flickr_info['urls']['url'][0]['_content'];

  
  $node->title = htmlentities($title);
  $node->created = strtotime($flickr_info['dates']['taken']);
  if (!$node->name) {
    $node->name = $user->name;
  }
  $node->uid = $user->uid;
  $node->type = 'flickrrippr_photo';

  node_save($node);

  if ($node->is_new) {
//     drupal_set_message("New node, nid = $node->nid");
    $result = db_query("INSERT INTO {flickrphotos} (flickrphoto_id, nid, lastfetched, photo_path, thumbnail_path, description, page_url)
      VALUES ('%s', %d, %d, '%s', '%s', '%s', '%s')",
      $photo_id, $node->nid, strtotime('now'), $photo_path, $thumbnail_path, $description, $page_url);
      drupal_set_message(t("Saved new photo %photo_id, ". l($node->title, 'node/' . $node->nid), array('%photo_id' => $photo_id)));
  }
  else {
    $result = db_query("UPDATE {flickrphotos} SET
      lastfetched=%d,
      photo_path='%s',
      thumbnail_path='%s',
      description = '%s',
      page_url = '%s'
      WHERE flickrphoto_id=%d AND nid=%d",
      strtotime('now'),
      $photo_path,
      $thumbnail_path,
      $description,
      $page_url,
      $photo_id,
      $node->nid);
    drupal_set_message(t("Updated photo %photo_id, ". l($node->title, 'node/' . $node->nid), array('%photo_id' => $photo_id)));
  }

  if (variable_get('flickrrippr_import_tags', FALSE)) {
    include_once(drupal_get_path('module', 'flickrrippr') .'/flickrrippr.tags.inc');
    $vocab = variable_get('flickrrippr_tags_vocab', 0);
    if (!$vocab) {
      return;
    }

    flickrrippr_photo_add_tags($nid, $flickr_info);
  }

}



/**
 * get the node id for a flickr photo id
 */
function flickrrippr_photo_get_nid($photo_id) {
  $rec = db_fetch_object(db_query("SELECT * FROM {flickrphotos} WHERE flickrphoto_id = '%s'", $photo_id));
  return (!empty($rec->nid) ? $rec->nid : NULL);
}