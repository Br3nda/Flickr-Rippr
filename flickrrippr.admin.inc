<?php

function flickrrippr_admin_edit($uid, $flickrid) {

  $fuser = flickrrippr_get_flickr_user($uid, $flickrid);
  return drupal_get_form('flickrrippr_user_settings_form', $fuser);
}



//TODO CHANGE TO FORM!
function flickrrippr_admin_fetch($uid, $flickrid) {
  $fuser = flickrrippr_get_flickr_user($uid, $flickrid);
  $new = flickrrippr_fetch_latest($fuser);
  drupal_goto('admin/content/flickrrippr');
}



/**
 * Validates the admin form
 */
function flickrrippr_admin_form_validate() {
}

/**
 * This runs after admin form is submitted, and saves our module's settings
 */
function flickrrippr_admin_form_submit($form, &$form_state) {
  $form_values = &$form_state['values'];
  variable_set('flickrapikey', $form_state['values']['api_key']);
//   variable_set('flickrjpegcache', $form_state['values']['jpegcache']);
  drupal_set_message(t('Settings saved'));
}

/**
 * This is called from admin menu, and will refresh the 20 oldest photos
 * TODO check this work in drupal6
 */
function flickrrippr_admin() {
  $output = '';

  $headers = array(
    array('data' => t('Name'), 'field' => 'name', 'sort'=>'asc'),
    array('data' => t('Flickr Username'), 'field' => 'flickrusername'),
    array('data' => t('Flickr NSID'), 'field' => 'flickrid'),
    array('data' => t('Tag'), 'field' => 'tag'),
  );
  $result = pager_query("SELECT u.uid, u.name, f.tag, f.flickrusername, f.flickrid FROM {users} u LEFT OUTER JOIN {flickrusers} f  ON u.uid = f.uid AND u.uid > 0". tablesort_sql($headers), 50);


  while ($u = db_fetch_object($result)) {
    $rows[] = array(
      $u->name,
      $u->flickrusername,
      $u->flickrid,
      $u->tag,
      drupal_get_form('flickrrippr_user_fetch_now', $u->uid, $u->flickrid, 'admin/content/flickrrippr'),
      l(t('Edit Settings'), 'admin/content/flickrrippr/edit/'. $u->uid .'/'. $u->flickrid),
    );
  }
  $output .= theme('table', $headers, $rows);
  $output .= theme('pager');
  return $output;

}

function flickrrippr_admin_settings_form() {

  $form['tags'] = array('#type' => 'fieldset', '#title' => t('Tags'));
  $form['tags']['import_tags'] = array(
    '#title' => t('Import tags?'), 
    '#type' => 'checkbox',
    '#default_value' => variable_get('flickrrippr_import_tags', FALSE),
  );

  $options = array();
  foreach(taxonomy_get_vocabularies() as $v) {
    $options[$v->vid] = $v->name; 
  }
  $form['tags']['vocab'] = array(
    '#title' => t('Vocabulary for tags'), 
    '#type' => 'select', 
    '#options' => $options,
    '#default_value' => variable_get('flickrrippr_tags_vocab', 0),
  );

  $form['jpegcache'] = array('#type' => 'fieldset', '#title' => t('Jpeg Cache'));
  $form['jpegcache']['cache_jpegs'] = array(
    '#title' => t('Cache Jpegs Locally?'), 
    '#type' => 'checkbox',
    '#default_value' => variable_get('flickrrippr_cache_jpegs', FALSE),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  return $form;
}

function flickrrippr_admin_settings_form_submit($form_id, $form_values) {
  $import_tags = $form_values['values']['import_tags'];
  
  variable_set('flickrrippr_import_tags', $import_tags);
  if ($import_tags) {
    $vocab = $form_values['values']['vocab'];
    variable_set('flickrrippr_tags_vocab', $vocab);  
  }

  $cache_jpegs = $form_values['values']['cache_jpegs'];
  variable_set('flickrrippr_cache_jpegs', $cache_jpegs);

  drupal_set_message('Flickr Rippr settings saved');
}