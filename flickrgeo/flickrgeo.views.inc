<?php
function flickrgeo_views_data() {
  $data['flickrgeo']['table']['group'] = t('Flickr Geo');
  
  $data['flickrgeo']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Geo data'),
  );

  $data['flickrgeo']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid'
    ),
    'help' => t('Flickr Geo data'),
  );

    $data['flickrgeo']['nid'] = array(
    'title' => t('Geo data'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Flickr Geo data'),
    )
  );
  $fields = array(
    'latitude',
    'longitude',
    'accuracy',
    'context',
     'locality_content',
     'locality_place_id',
     'locality_woeid',
     'county_content',
     'county_place_id',
     'county_woeid',
     'region_content',
     'region_place_id',
     'region_woeid',
     'country_content',
     'country_place_id',
     'country_woeid',
     'place_id',
     'woeid',
  );

  foreach($fields as $f) {
    $data['flickrgeo'][$f] = array(
      'title' => t($f),
      'field' => array(
        'views_handler_field',
        'click sortable' => false,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
  }
  
  return $data;
}