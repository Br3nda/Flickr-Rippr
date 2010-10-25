<?php
// $Id: flickrgroups.admin.inc,v 1.1.2.6 2010/10/25 23:13:11 taniwha Exp $ 

/**
 * @file administering the automatic import of flickr groups
 */

/**
 * Menu callback - shows currently configured groups and lets you add more
 */
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