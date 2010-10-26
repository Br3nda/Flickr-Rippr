<?php
// $Id: flickrgroups.admin.inc,v 1.1.2.8 2010/10/26 00:48:54 taniwha Exp $ 

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
  $result = pager_query("SELECT * FROM {flickrgroups}". tablesort_sql($headers));
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

function flickrgroups_admin_form($a, $name=null) {

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#default_value' => "Kid's Eats",
  );
 
  $form['find'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  
  if (!empty($name)) {
    drupal_set_message($name);
    $flickr = flickrapi_phpFlickr();
    if (!$flickr) {
      drupal_set_message(t("Unable to query flickr.com"), 'error');
      watchdog('flickrrippr', 'Unable to query flickr.com');
    }
    
    $results = $flickr->groups_search($name);
    foreach($results['group'] as $g) {
      $groups[$g['nsid']] = $g['name'];
    }
    $form['flickrgroup_id'] = array(
      '#type' => 'radios',
      '#options' => $groups
    );  
    
    $form['add'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
    );
  }
  
  
  return $form;
}

function flickrgroups_admin_form_submit($form, &$form_state) {
  $group->name = $form_state['values']['name'];
  
  if(isset($form_state['values']['flickrgroup_id'])) {
    $group->flickrgroup_id = $form_state['values']['flickrgroup_id'];
    drupal_write_record('flickrgroups', $group);
    drupal_goto('admin/flickr/flickrgroups');
  }
  else {
    drupal_goto('admin/flickr/flickrgroups/add/'. $group->name);
  }
}



