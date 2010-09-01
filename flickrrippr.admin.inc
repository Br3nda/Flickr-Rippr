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
 * administer global settings
 */
function flickrrippr_admin_form() {
  $form = array();
  $form['instructions'] = array('#type' => 'markup', '#value' => 'Get your own API key from '. l('http://www.flickr.com/services/api/', 'http://www.flickr.com/services/api/'));

  $form['api_key'] = array(
        '#title' => 'Flickr Api Key',
        '#required' => TRUE,
        '#type' => 'textfield',
        '#default_value' => variable_get('flickrapikey', '')
      );
//   $form['jpegcache'] = array(
//       '#title' => 'Allow Local Cache',
//       '#type' => 'checkbox',
//       '#default_value' => variable_get('flickrjpegcache', TRUE),
//       '#description' => t('Allow users to cache flickr photo jpeg files locally'),
//       );

  $form['save'] = array(
     '#type' => 'submit',
     '#value' => t('Save'),
      );
  return $form;
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
