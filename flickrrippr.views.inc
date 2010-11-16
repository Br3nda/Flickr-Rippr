<?php
// $Id: flickrrippr.views.inc,v 1.1.6.4 2010/11/16 01:24:18 taniwha Exp $

/**
 * @file Views stuff
 */

function flickrrippr_views_data() {
  $data['flickrphotos']['table']['group'] = t('Flickr Rippr');
  $data['flickrphotos']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Flickr Photo'),
  );
  $data['flickrphotos']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid'
    ),
  );
  $data['flickrphotos']['nid'] = array(
    'title' => t('Flickr Photo'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Flickr Photo'),
    )
  );
  $data['flickrphotos']['description'] = array(
    'title' => t('Photo description'),
    'field' => array(
      'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  return $data;
}
