<?php

function flickrrippr_photo_add_tags($nid, $photo_info=null) {
  if (!$nid) {
    drupal_set_message(t('Refusing to add tags to a null nid'), 'error');
    return;
  }
  $node = node_load(array('nid' => $nid));
  $vid = variable_get('flickrrippr_tags_vocab', 0);
//   $node_terms = $node->taxonomy;

  foreach($photo_info['tags']['tag'] as $tag) {
    $raw = $tag['raw'];
    $terms = taxonomy_get_term_by_name($raw);

    //if we got some terms back, check if they're in our vocab or not
    if (sizeof($terms)) {
      $term = flickrrippr_matching_vid($terms, $vid);
    }
    
    //if we still don't have a term, create it.
    if (!$term or !sizeof($terms)) {
      drupal_set_message("Creating new term $raw");
      $term = array(
        'vid' => $vid, // Voacabulary ID
        'name' => $raw, // Term Name 
        'synonyms' => $tag['_content'], // (Optional) Synonym of this term
        'relations' => array(15), // (Optional) Related Term IDs 
      );
      taxonomy_save_term($term);
      //add to those we are gonna save
      $node_terms[] = $term;
    }
    else {
      $node_terms[] = $term;
    }

    


  }
  taxonomy_node_save($node, $node_terms); 
}

function flickrrippr_matching_vid($terms, $vid) {
  foreach($terms as $t) {
    if ($t->vid == $vid) {
      return $t;
    }
  }
}