Flickr Rippr
http://drupal.org/project/flickrrippr
=====================================


DESCRIPTION
------------
Turns flickr photos into drupal nodes - imported from flickr via the xml api.
Nodes are updated by cron run, or manually on the node's edit page.

Creates a block, of thumbnails, for each user that has defined her/his flickr username.

Just publish photos + descriptions to flickr, and see these automagically appear on your blog.


REQUIREMENTS
------------
Drupal 6.x
PhpFlickr (LGPL license) http://phpflickr.com or http://code.google.com/p/phpflickr/downloads/list


INSTALLING
----------
1. Copy the 'flickrrippr' folder to your sites/all/modules directory.

2. Go to Administer > Site building > Modules. Enable the module.
Read more about installing modules at http://drupal.org/node/70151


CONFIGURING AND USING
---------------------
1. Go to Administer > User management > Permissions. Under section 'flickrrippr module' set appropriate permissions.

2. If not already done go to http://phpflickr.com download the file 'phpFlickr-2.x.x.x.zip'. 
Note: 'x' means any numbers.

3. Extract 'phpFlickr-2.x.x.x.zip' file to the following folder.
sites/all/modules/flickrrippr/phpFlickr
Note: The 'phpFlickr.php' file must be located at sites/all/modules/flickrrippr/phpFlickr/phpFlickr.php

4. Go to admin/settings/flickrrippr. Type in your Flickr Api Key. For example '4aac5309645301718877x3c6ce426ars'. Click on SAVE button.

5. Go to user/flickrippr. Type in your Flickr Screen Name/User name. Check appropriate option. Click on save.
Note: Your Flickr Screen Name/User name can be found at http://www.flickr.com/account. Under the section 'Your screen name'.

5. Creates a block, of thumbnails, for each user that has defined her/his Flickr Username.

6. Publish photos + descriptions to flickr, and see these automagically appear on your Drupal site.

7. Nodes are updated by CRON run, or manually on the node's edit page. To run cron go to admin/reports/status. Click on 'run cron manually' link. If any configuration error are display under 'Status report' title you must fix them. Then run CRON again.

8. If needed re-fetch oldest flickr photos go to admin/content/flickrrippr-fetch


REPORTING ISSUE. REQUESTING SUPPORT. REQUESTING NEW FEATURE
-----------------------------------------------------------
1. Go to the module issue queue at http://drupal.org/project/issues/flickrrippr?status=All&categories=All
2. Click on CREATE A NEW ISSUE link.
3. Fill the form.
4. To get a status report on your request go to http://drupal.org/project/issues/user


UPGRADING
---------
Read more at http://drupal.org/node/250790
