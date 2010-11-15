<?php
// $Id: flickrgroups.content.inc,v 1.1.4.7 2010/11/15 03:25:22 taniwha Exp $ 

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
  $output .= '<p>'. l('Add group', 'flickr/groups/add') .'</p>';
  $result = pager_query("SELECT * FROM {flickrgroups}". tablesort_sql($headers));
  $rows = array();
  while ($g = db_fetch_object($result)) {
    $rows[] = array(
      l($g->name, 'flickr/groups/'. $g->flickrgroup_id),
      drupal_get_form('flickrgroups_fetch_form', $g->flickrgroup_id),
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

function flickrgroups_fetch_form($a, $flickrgroup_id) {
  $form['flickrgroup_id'] = array(
    '#type' => 'hidden',
    '#value' => $flickrgroup_id,
  );
  $form['fetch'] = array(
    '#type' => 'submit',
    '#value' => t('Fetch'),
  );  
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  return $form;
}

function flickrgroups_fetch_form_submit($a, $form_state) {
  $flickrgroup_id = $_POST['flickrgroup_id'];
  if(t('Delete') == $form_state['values']['op']) {
    db_query("DELETE FROM {flickrgroup_photos} WHERE flickrgroup_id='%s'", $flickrgroup_id);
    db_query("DELETE FROM {flickrgroups} WHERE flickrgroup_id='%s'", $flickrgroup_id);
  }
  if(t('Fetch') == $form_state['values']['op']) {
    flickrgroups_fetch_group($flickrgroup_id, TRUE);
  }
}

function flickrgroups_admin_form($a, $name=null) {
  if (empty($name)) {
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
  }
  else {
     drupal_set_message($name);
    $flickr = flickrapi_phpFlickr();
    if (!$flickr) {
      drupal_set_message(t("Unable to query flickr.com"), 'error');
      watchdog('flickrrippr', 'Unable to query flickr.com');
    }
    
    $results = $flickr->groups_search($name);
//     drupal_set_message(print_r($results, 1));
    foreach($results['group'] as $g) {
      $groups[$g['nsid']] = $g['name'];
    }
    if (count($groups)) {
      $form['flickrgroup_id'] = array(
        '#type' => 'radios',
        '#options' => $groups
      );

      $form['add'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
      );
    }
    else {
      drupal_set_message("No groups found");
      drupal_goto('flickr/groups/add');
    }
  }
  
  
  return $form;
}

function flickrgroups_admin_form_submit($form, &$form_state) {

//     drupal_set_message(__FUNCTION__);
  if(isset($form_state['values']['flickrgroup_id'])) {
// 
    $flickr = flickrapi_phpFlickr();
    if (!$flickr) {
      drupal_set_message(t("Unable to query flickr.com"), 'error');
      watchdog('flickrrippr', 'Unable to query flickr.com');
    }
    $results = $flickr->groups_getInfo($form_state['values']['flickrgroup_id']);
    $group->name = $results['name'];
    $group->flickrgroup_id = $form_state['values']['flickrgroup_id'];
    drupal_write_record('flickrgroups', $group);
    drupal_goto('flickr/groups');
  }
  else {
    $group->name = $form_state['values']['name'];
    drupal_goto('flickr/groups/add/'. $group->name);
  }
}



