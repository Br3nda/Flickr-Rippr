<?php


function flickrrippr_views_data() {
  $data['flickrphotos']['table']['group'] = t('Flickr Rippr');
  $data['flickrphotos']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Photo'),
  );
  $data['flickrphotos']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid'
    ),
    'help' => t('Photos imported from Flickr.com'),
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
    'help' => t('Description on photo from flickr.com'),
    'field' => array(
      'views_handler_field',
      'click sortable' => false,
    ),
//     'sort' => array(
//       'handler' => 'views_handler_sort',
//     ),
//     'filter' => array(
//       'handler' => 'views_handler_filter_string',
//     ),
  );


  $data['flickrphotos']['lastfetched'] = array(
     'title' => t('Last Fetched from Flickr.com'),
     'help' => t('Updated everytime we refetch a photo'),
     'field' => array(
       'handler' => 'views_handler_field_date',
       'click sortable' => TRUE,
     ),
     'sort' => array(
       'handler' => 'views_handler_sort_date',
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_date',
     ),
  );

  $data['flickrphotos']['flickrphoto_id'] = array(
     'title' => t('Flickr Photo Id'),
     'help' => t('The ID of photo on flickr'),
     'field' => array(
       'handler' => 'views_handler_field_numeric',
       'click sortable' => TRUE,
      ),
     'filter' => array(
       'handler' => 'views_handler_filter_numeric',
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
  );

  $data['flickrphotos']['failed'] = array(
     'title' => t('Failed to refetch'),
     'help' => t('Has fetching from flickr.com failed?'),
     'field' => array(
       'handler' => 'views_handler_field_boolean',
       'click sortable' => TRUE,
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_boolean_operator',
       'label' => t('Failed to refetch'),
       'type' => 'yes-no',
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
   );

  $data['flickrphotos']['secret'] = array(
    'title' => t('Photo Secret'),
    'help' => t('Secret string for access to photo from flickr.com'),
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
  $data['flickrphotos']['dateuploaded'] = array(
    'title' => t('Date Uploaded'),
    'help' => t('when uploaded to flickr.com'),
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

  $data['flickrphotos']['license'] = array(
    'title' => t('Licence'),
    'help' => t('What licence is this photo under?'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );


  $data['flickrphotos']['rotation'] = array(
    'title' => t('Rotation'),
    'help' => t('Has this jpeg been rotated?'),
     'field' => array(
       'handler' => 'views_handler_field_boolean',
       'click sortable' => TRUE,
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_boolean_operator',
       'label' => t('Failed to refetch'),
       'type' => 'yes-no',
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
  );

  $data['flickrphotos']['date_posted'] = array(
    'title' => t('Date Posted'),
    'help' => t('when posted to flickr.com'),
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


  $data['flickrphotos']['date_taken'] = array(
    'title' => t('Date Taken'),
    'help' => t('Time and date the photo was taken, according to jpeg exif info and/or flickr.com'),
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

  $data['flickrphotos']['date_lastupdate'] = array(
     'title' => t('Last Update'),
     'help' => t('Time and date the photo on flickr.com was last modified/updated'),
     'field' => array(
       'handler' => 'views_handler_field_date',
       'click sortable' => TRUE,
     ),
     'sort' => array(
       'handler' => 'views_handler_sort_date',
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_date',
     ),
  );

  $data['flickrphotos']['views'] = array(
     'title' => t('Views'),
     'help' => t('Number of views of the photo on flickr.com'),
     'field' => array(
       'handler' => 'views_handler_field_numeric',
       'click sortable' => TRUE,
      ),
     'filter' => array(
       'handler' => 'views_handler_filter_numeric',
     ),
     'sort' => array(
       'handler' => 'views_handler_sort',
     ),
  );


  return $data;
}


function flickrrippir_views_handlers(){
  return array(
    // The 'info' index is optional; use it to describe to Views where your
    // handler class definitions are, using the 'path' sub-index. Views will
    // search for the handlers in a file called CLASSNAME.inc (for example,
    // example_handler_filter_custom.inc).
    //
    // If this value is not present, Views will search in your module's root
    // directory for a file called MODULE_views_TYPE_handlers.inc (for example,
    // example_views_field_handlers.inc).
    'info' => array(
      'path' => drupal_get_path('module', 'flickrrippr') . '/views/handlers',
    ),

    // Next, the required array of handlers, keyed by their class names. Each
    // handler class must specify its parent class and, optionally, the file
    // in which it resides.

    // @todo: Expand this documentation, copy/pasting from Advanced Help
    // probably.
    // @todo: Show example of the 'file' key as well.
    'handlers' => array(
      // @todo: These "when to use X" descriptions need help.


      // Field handlers provide options to impact how Views displays fields.
      // Most often, field handlers inherit from views_handler_field.
      'example_handler_field_custom' => array(
        'parent' => 'views_handler_field',
      ),
      'example_handler_field_custom_link' => array(
        'parent' => 'views_handler_field',
      ),
      'example_handler_field_other_custom_link' => array(
        'parent' => 'views_handler_field_custom_link',
      ),

      // Argument handlers provide options for dynamically filtering a View.
      // You may need to define an argument handler if a field exposed by your
      // Views integration requires more than basic data type handling.
      // @todo: Example.
//       '' => array(
//       ),

      // @todo: Examples of sorts, filters, and relationships.

    ),

  );
}