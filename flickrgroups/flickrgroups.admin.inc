<?php
// $Id: flickrgroups.admin.inc,v 1.1.2.7 2010/10/26 00:26:53 taniwha Exp $ 

/**
 * @file administering the automatic import of flickr groups
 */

/**
 * Menu callback - shows currently configured groups and lets you add more
 */
function flickrgroup_admin_page() {
  $output = '';
  $headers = array('name');
  $output .= '<p>'. l('Add group', 'admin/flickr/flickrgroups/add') .'</p>';
  $result = pager_query("SELECT * FROM {flickr_groups}". tablesort_sql($headers));
  while ($g = db_fetch_object($result)) {
    $rows[] = array($g->name);
  }
  if (!count($rows)) {
    $output .= '<p>'. t('No groups') .'</p>';
  }
  $output .= theme('table', $headers, $rows);
  $output .= theme('pager');
  return $output;
}

function flickrgroups_admin_form() {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
  );
  return $form;
}