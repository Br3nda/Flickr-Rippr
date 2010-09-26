<?php
// $Id: flickrrippr.admin.inc,v 1.36 2010/09/26 21:54:49 taniwha Exp $
/**
 * @file Admin of Flickr Rippr
 */

function flickrrippr_admin_edit($uid, $flickrid) {

  $fuser = flickrrippr_get_flickr_user($uid, $flickrid);
  include_once(drupal_get_path('module', 'flickrrippr') .'/flickrrippr.user.inc');
  return drupal_get_form('flickrrippr_user_settings_form', $fuser);
}



//TODO CHANGE TO FORM!
function flickrrippr_admin_fetch($uid, $flickrid) {
  $fuser = flickrrippr_get_flickr_user($uid, $flickrid);
  $new = flickrrippr_fetch_latest($fuser);
  drupal_goto('admin/content/flickrrippr');
}

/**
 * This is called from admin menu, and will refresh the 20 oldest photos
 * TODO check this work in drupal6
 */
function flickrrippr_admin() {
  $output = '';

  $headers = array(
    array('data' => t('Name'), 'field' => 'name', 'sort' => 'asc'),
    array('data' => t('Flickr Username'), 'field' => 'flickrusername'),
    array('data' => t('Flickr NSID'), 'field' => 'flickrid'),
    array('data' => t('Tag'), 'field' => 'tag'),
  );
  $result = pager_query("SELECT u.uid, u.name, f.tag, f.flickrusername, f.flickrid FROM {users} u 
      LEFT OUTER JOIN {flickrusers} f  
      ON u.uid = f.uid AND u.uid > 0 WHERE u.name <> '' ". tablesort_sql($headers), 50);


  while ($u = db_fetch_object($result)) {
    $rows[] = array(
      theme('username', $u),
      check_plain($u->flickrusername),
      check_plain($u->flickrid),
      check_plain($u->tag),
      drupal_get_form('flickrrippr_user_fetch_now', $u->uid, $u->flickrid, 'admin/content/flickrrippr'),
      l(t('Edit Settings'), 'admin/content/flickrrippr/edit/'. $u->uid .'/'. check_plain($u->flickrid)),
    );
  }
  $output .= theme('table', $headers, $rows);
  $output .= theme('pager');
  return $output;

}

function flickrrippr_path_options() {
  $options = array(t('None'));  
  $options += module_invoke_all(__FUNCTION__);

  return $options;
}
function flickrrippr_node_handler_options() {
  $options = array('flickrrippr' => t('Flickr Rippr'));  
  $options += module_invoke_all(__FUNCTION__);

  return $options;
}

function flickrrippr_admin_settings_form() {

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('Flickr Rippr Settings'),
    '#collapsible' => TRUE,
    '#weight' => -5,
  );

  $path_options = flickrrippr_path_options();
  $form['general']['path_handler'] = array(
    '#type' => 'radios', 
    '#title' => t('Flickr Path Handler'), 
    '#options' => $path_options,
    '#default_value' => variable_get('flickrrippr_path_handler', 0),
    '#required' => TRUE,
    '#description' => t('Note: Changing the path handler does not modify existing photos previously fetch from flickr.com.'),
  );

  $node_handlers = flickrrippr_node_handler_options();
  $form['general']['node_handler'] = array(
    '#type' => 'radios', 
    '#title' => t('Flickr Node Handler'), 
    '#options' => $node_handlers,
    '#default_value' => variable_get('flickrrippr_node_handler', 'flickrrippr'),
    '#required' => TRUE,
    '#description' => t('Note: Changing the Node handler does not modify existing nodes.'),
  );

  $form['general']['markup_trust'] = array(
    '#type' => 'checkbox',
    '#title' => t('Trust HTML markup from flickr.com'),
    '#description' => t('When checked, Flickr Rippr will not escape markup from flickr.com photo descriptions. Use with care.'),
    '#default_value' => variable_get('flickrrippr_markup_trust', FALSE),
  );

  $form['general']['num_per_fetch'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of photos to fetch'),
    '#description' => t('Per account, on manual fetch or hook_cron'),
    '#default_value' => variable_get('flickrrippr_num_per_fetch', 10),
    '#required' => TRUE,
  );



  $form['image_sizes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image Sizes'),
    '#collapsible' => TRUE,
  );
  $form['image_sizes']['teaser_size'] = array(
    '#title' => t('Teaser image size'),
    '#description' => t('What size should jpeg images be within teasers?'),
    '#type' => 'radios',
    '#options' => _flickr_rippr_jpeg_sizes(),
    '#default_value' => variable_get('flickrrippr_teaser_size', 't'),
  );
  $form['image_sizes']['page_size'] = array(
    '#title' => t('Page image size'),
    '#description' => t('What size should jpeg images be when viewing a flickr photo page?'),
    '#type' => 'radios',
    '#options' => _flickr_rippr_jpeg_sizes(),
    '#default_value' => variable_get('flickrrippr_page_size', 'm'),
  );

  $form['failed'] = array(
    '#title' => 'Failed Imports',
    '#type' => 'fieldset',
    'failed_action' => array(
      '#type' => 'radios',
      '#title' => t('When photos cannot be refetched from flickr.com:'),
      '#options' => array(
        'delete' => t('Delete completely from this website.'),
        'unpublish' => t('Unpublish the photo.'),
        'nothiing' => t('Do nothing. Leave the content on this website'),
      ),
      '#required' => TRUE,
      '#description' => t('Photos by be imported and then later they deleted from flickr, or made private. This module needs to know what to do with these photos.'),
    ),
  );


  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 9,
  );
  
  return $form;
}

function _flickr_rippr_jpeg_sizes() {
  return array(
//     'sq' => t('Square (75 x 75)'), 
    't'  => t('Thumbnail (100 x 75)'),      
    's'  => t('Small (240 x 180)'),
    0  => t('Medium 500 (500 x 375)'), 
    'z'  => t('Medium 640 (640 x 480)'),
    'b'  => t('Large (1024 x 768)'),
    'o'  => t('Original (2592 x 1944)')
  );
}

function flickrrippr_admin_settings_form_validate($form_id, $form_values) {
  $num_per_fetch = $form_values['values']['num_per_fetch'];
  if (!is_numeric($num_per_fetch)) {
    form_set_error('num_per_fetch', t('Num to fetch must be number'));
  }
  
}

function flickrrippr_admin_settings_form_submit($form_id, $form_values) {

  $path_handler = $form_values['values']['path_handler'];
  variable_set('flickrrippr_path_handler', $path_handler);

  $node_handler = $form_values['values']['node_handler'];
  variable_set('flickrrippr_node_handler', $node_handler);

  $num_per_fetch = $form_values['values']['num_per_fetch'];
  variable_set('flickrrippr_num_per_fetch', $num_per_fetch); 

  $markup_trust = $form_values['values']['markup_trust'];
  variable_set('flickrrippr_markup_trust', $markup_trust);


  $page_size = $form_values['values']['page_size'];
  $teaser_size = $form_values['values']['teaser_size'];
  variable_set('flickrrippr_page_size', $page_size);
  variable_set('flickrrippr_teaser_size', $teaser_size);

  drupal_set_message('Flickr Rippr settings saved');  
}
