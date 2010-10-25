<?php

function flickrgroup_admin_page() {
  $output = '';
  
  $output .= '<p>'. l('Add group', 'admin/flickr/flickrgroups/add') .'</p>';
  $result = pager_query("SELECT * FROM {flickr_groups}". tablesort_sql($headers);
  while ($g = db_fetch_object($result)) {
    $rows[] = array($g->name);
  }
  $output .= theme('table', $headers, $rows);
  return $output;
}