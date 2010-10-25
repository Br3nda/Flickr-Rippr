<?php

function flickrgroup_admin_page() {
  $result = pager_query("SELECT * FROM {flickr_groups}". tablesort_sql($headers);
  while ($g = db_fetch_object($result)) {
    $rows[] = array($g->name);
  }
  $output .= theme('table', $headers, $rows);
  return $output;
}