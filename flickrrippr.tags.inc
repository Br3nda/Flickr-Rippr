<?php

function flickrrippr_photo_add_tags($nid, $photo_info=null) {
  foreach($photo_info['tags'] as $tag) {
    drupal_set_message('<pre>'. print_r($tag, 1) .'</pre>');
  }
}