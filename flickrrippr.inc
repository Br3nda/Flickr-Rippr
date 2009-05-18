<?php
//$Id ;

/**
 * @file flickrriprr.inc 
 * Functions that are likely to be of use to other modules, are in this file.
 * PHP Flickr: http://phpflickr.com/docs/
 * Flickr API docs http://www.flickr.com/services/api/
 */

/**
 * returns the phpFlickr object.
 * If we need to change anything, such as making the cache configuration,
 * then it'll all be done here.
 */
function flickrrippr_phpFlickr() {
  //Find the phpFlickr library
  $include_file = drupal_get_path('module', 'flickrrippr') .'/phpFlickr/phpFlickr.php';
  if (!file_exists($include_file)) {
    drupal_set_message(t("Missing phpFlickr - expected to find it at !file", array('!file' => $include_file)), 'error');
    return false;
  }
//  require_once($include_file);
//
//  //Get the api key, we need this to make queries.
//  $api_key = variable_get('flickrapikey', '');
//  if (!$api_key) {
//    drupal_set_message(t("Flickr API key not set"), 'error');
//    if (user_access('Administer global flickr api settings')) {
//      drupal_set_message(t("Goto !link to configure FlickrRipper", array('!link' => l('admin/settings/flickrrippr', 'admin/settings/flickrrippr'))));
//    }
//  }
//
//  //call the phpFlickr library
//  $flickr = new phpFlickr($api_key);
//
//  //TODO make caching configurable.. but really you'd want to have this turned on.
//  $flickr->enableCache("fs", variable_get('flickrcachepath', '/tmp'));
//
//  return $flickr;
}
//
//
///**
// * Works out img tags to display a photo - optionally wrapped in a link to the photo's page.
// * NOTE: flickr requires links to flickr.com if you're not caching the file on your own server
// */
//function flickrrippr_show_photo($photo, $link = true, $owner = "", $cache = true) {
//  $src = flickrrippr_path($photo, $cache, 's');
//  if ($cache) {
//    file_create_url($src);
//  }
//  $alt = $photo['title'];
//  if (!$photo['owner']) {
//    $photo['owner'] = $owner;
//  }
//
//  return '<img src="'. htmlentities($src) .'" title="'. htmlentities($alt) .'" alt="'. htmlentities($alt) .'" height="75" width="75">';
//  
//}
//
///**
// * Make a html chunk of photos ready for display from a 
// * photo list as returned by phpFlickr methods
// */
//function flickrrippr_thumbnail_table($photos, &$photo_list, $cache = true) {
//	
//  $row = 0;
//  if (!is_array($photos['photo'])) {
//    return t('No photos found');
//  }
//  $s = '';
//  
//  foreach ($photos['photo'] as $p) {
//    if ($cache) {
//      $nid = flickrrippr_photo_get_nid($p['id']);
//      if (!$nid) {
//        flickrrippr_makenode($p['id'], $p);
//        $nid = flickrrippr_photo_get_nid($p['id']);
//      }
//    }
//    
//    $img = flickrrippr_show_photo($p, false, null, $cache);
//    if ($cache) {
//      $s .= '<a href="'. url('node/'. $nid)  .'">';
//    }
//    else {
//      $s .= '<a href="http://flickr.com/photos/'. htmlentities($p['owner']) .'/'. $p['id'] .'">';
//    }
//    $s .= $img;
//    $s .= '</a>';
//    $photo_list[] = $p['id'];
//    $row++;
//  }
//  return $s;
//}
//
    
/**
* Helper function, to get drupal user details for a flickr user (string of their username of flickr)
*/
function flickrrippr_get_user_by_flickrusername($fuser) {
  if (!$fuser) {
    drupal_set_message(t("Refusing to search for blank username"));
    return;
  }
  
  $flickruser = db_fetch_object(db_query("SELECT * FROM {flickrusers} WHERE flickrusername = '%s'", $fuser));
  if (!$flickruser) {
    drupal_set_message(t("Flickruser %flickrusername is not a registered user of this site", array('%flickrusername' => $fuser)));
    return;
  }
  
  $user = user_load(array('uid' => $flickruser->uid));
  return $user;
}

///**
//* Work out the path to a photo - also handles the cache on the local filesystem
//*/
//function flickrrippr_path($flickr_photo, $cache = true, $size = false) {
//  
//  
//  //http://farm{farm-id}.static.flickr.com/{server-id}/{id}_{secret}.jpg
//      $src =  'http://farm'. $flickr_photo['farm'] .'.static.flickr.com/' . $flickr_photo['server'] . '/' . $flickr_photo['id'] . '_' . $flickr_photo['secret'] . ($size ? '_'.$size : '') .'.jpg';
//  if (!$cache) return $src;
//    
//  $folder =  file_directory_path() . '/flickr';
//  if (!file_create_path($folder)) {
//    drupal_set_message(t('Files directory not set, unable to cache flickr photos'), 'error');
//    return ;
//  }  
//  
//  $filename =  $folder . '/' . $flickr_photo['id'] . '_' . $flickr_photo['secret'] . ($size ? '_'.$size : '') .'.jpg';
//
//  //If we've already go it, then we dont' need to get it
//      if (file_exists($filename)) {
//    //if (filesize($filename) > 0) {
//      //TODO, check mtime against photo info -- if possible
//        return $filename;
//    //}
//      }
//
//      require_once "HTTP/Request.php";
//      $req =& new HTTP_Request($src);
//    //Something went wrong?
//        if (PEAR::isError($req->sendRequest())) {
//      drupal_set_message(t("Failed to retrieve jpeg file from flick.com, url = %url", array('%url' => $src)));
//    //use flickr hosted jpeg
//        return $src;
//        }
//  
//    //file handle to save the jpeg into
//        $contents = $req->getResponseBody();
//    if (file_save_data($contents, $filename, FILE_EXISTS_REPLACE)) {
//      return $filename;
//    }
//    else {
//      drupal_set_message(t("Failed to save image to %filename", array('%filename' => $filename)));
//      return $src;
//    }
//  
//}
//
///**
//* Makes a node for a flickr photo - or updates if it already exists
//*/
//function flickrrippr_makenode($photo_id) {
//  $output = '';
//  if (!$photo_id) {
//    drupal_set_message(t("Photo_id required"));
//    return;
//  }
//
//  $node->nid = flickrrippr_photo_get_nid($photo_id) ;
//
//  $flickr = @flickrrippr_phpFlickr();
//  if (!$flickr) {
//    drupal_set_message(t("Unable to query flickr.com, library missing"));
//    return $output;
//  }
//  
//  //Info to put into the node
//      $flickr_info = $flickr->photos_getInfo($photo_id);
//  $flickr_context = $flickr->photos_getContext($photo_id);
//  
//  //flickr username
//      $flickrusername = $flickr_info['owner']['username'];
//  drupal_set_message(t("Making a node for photo by %flickrusername", array('%flickrusername' => $flickrusername)));
//  
//  //get user details
//      $user = flickrrippr_get_user_by_flickrusername($flickrusername);
//  if (!$user) {
//    drupal_set_message(t("User doesn't exist, unable to save photo %photo_id", array('%photo_id' => $photo_id)));
//    return false;
//  }
//  
//  //Full (normal) sized image
//      $path =  flickrrippr_path($flickr_info);
//  list($width, $height, $type, $attr) = getimagesize($path);
//  $img = '<img src="' . file_create_url($path)  . '" alt="' . t($info['title']) . '" width="' . $width . '" height="' . $height . '" />';
//  
//  
//  //thumbnail version
//      $path =  flickrrippr_path($flickr_info, true, 'm');
//  
//  list($width, $height, $type, $attr) = getimagesize($path);
//  $thumb = '<img src="' . file_create_url($path) . '" width="' . $width . '" height="' . $height . '" />';
//  
//  
//  $node->title = htmlentities($flickr_info['title']);
//  
//  $node->created = strtotime($flickr_info['dates']['taken']);
//  
//  $node->teaser = '<p>'  . $thumb . '</p>';
//  
//  $node->teaser .= '<p>' . htmlentities(html_entity_decode($flickr_info['description'])) . '</p>';
//  $node->format = 3;
//  
//  $content = '';
//  if ($flickr_info['description']) {
//    $content .= '<p>' . htmlentities(html_entity_decode($flickr_info['description'])) . '</p>';
//  }
//  $content .= $img;
//  
//  //$content .= '<p>' . t('photo by %name', array('%name' => $realname)) ."</p>\n";
//  //$content .= '<!--break-->';
//  
//  $content .= '<p>' . t('This is a ' . l(t('Flickr Photo') , 'http://flickr.com/photos/' . $flickr_info['owner']['nsid'] . '/' . $photo_id .'/') . ' imported via the flickr api') . '</p>';
//  
//  $node->body = $content;
//  
//  $node->name = $user->name;
//  $node->uid = $user->uid;
//  $node->type = 'flickrrippr';
//  $node->published = 1;
//  $node->status =1;
//  $node->comment = 2;
//  $node->promote = 1;
//  
//  node_save($node);
//  
//  if ($node->is_new) {
//    $result = db_query("INSERT INTO {flickrphotos} (flickrphoto_id, nid) VALUES (%d, %d)", $photo_id, $node->nid);
//    drupal_set_message(t("New photo found, %photo_id", array('%photo_id' =>  $photo_id)));
//  }
//  else {
//    drupal_set_message(t("Updated photo %photo_id, ". l($node->title, 'node/' . $node->nid), array('%photo_id' => $photo_id)));
//  }
//
//  flickrrippr_addcomments($photo_id, $node->nid);
//  
//  return $node->is_new;
//}
//
