<?php
// $Id: flickrgroups.admin.inc,v 1.1.2.14 2010/10/26 02:15:12 taniwha Exp $ 

/**
 * @file administering the automatic import of flickr groups
 */

/**
 * Menu callback - shows currently configured groups and lets you add more
 */
function flickrgroup_admin_page() {
  $output = '';
  $headers = array(
    array('field' => 'name', 'data' => 'Name', 'sort' => 'asc'),
  );
  $output .= '<p>'. l('Add group', 'admin/flickr/flickrgroups/add') .'</p>';
  $result = pager_query("SELECT * FROM {flickrgroups}". tablesort_sql($headers));
  $rows = array();
  while ($g = db_fetch_object($result)) {
    $rows[] = array(
      check_plain($g->name),
      drupal_get_form('flickrgroups_fetch_form', $g->group_id),
    );
  }
  if (!count($rows)) {
    $output .= '<p>'. t('No groups') .'</p>';
  }
  else {
    $output .= theme('table', $headers, $rows);
    $output .= theme('pager');
  }
  return $output;
}

function flickrgroups_fetch_form($a, $group_id) {
  $form['group_id'] = array(
    '#type' => 'hidden',
    '#value' => $group_id,
  );
  $form['fetch'] = array(
    '#type' => 'submit',
    '#value' => t('Fetch'),
  );
  return $form;
}

function flickrgroups_fetch_form_submit($a, $form_state) {
  $group_id = intval($_POST['group_id']);
  _flickrgroups_fetch_group($group_id, TRUE);
}

function flickrgroups_admin_form($a, $name=null) {

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#default_value' => isset($name) ? $name : '',
  );
 
  $form['find'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  
  if (!empty($name)) {
//     drupal_set_message($name);
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

  
  if(isset($form_state['values']['flickrgroup_id'])) {

    $flickr = flickrapi_phpFlickr();
    if (!$flickr) {
      drupal_set_message(t("Unable to query flickr.com"), 'error');
      watchdog('flickrrippr', 'Unable to query flickr.com');
    }
    $results = $flickr->groups_getInfo($form_state['values']['flickrgroup_id']);
    drupal_set_message(print_r($results, 1));
    $group->name = $results['name'];
    $group->flickrgroup_id = $form_state['values']['flickrgroup_id'];
    drupal_write_record('flickrgroups', $group);
    drupal_goto('admin/flickr/flickrgroups');
  }
  else {
    $group->name = $form_state['values']['name'];
    drupal_goto('admin/flickr/flickrgroups/add/'. $group->name);
  }
}



