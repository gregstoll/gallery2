<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */
/**
 * Bootstrap code that we're going to need no matter what we do
 * @package Gallery
 */

/**
 * The prefix for all HTTP GET/POST arguments.  Define this as early as possible
 * so that we can use it in the FastDownload code in main.php.
 */
if (!defined('GALLERY_FORM_VARIABLE_PREFIX')) {
    define('GALLERY_FORM_VARIABLE_PREFIX', 'g2_');
}

require_once(dirname(__FILE__) . '/modules/core/classes/Gallery.class');
require_once(dirname(__FILE__) . '/modules/core/classes/GalleryDataCache.class');

/*
 * Set up our Gallery global.  This global should be the only one that Gallery requires.
 * Everything else should be inside it so that we do not pollute the namespace
 * (especially important when we're embedded inside another application).
 */
$gallery = new Gallery();
$gallery->init();
$GLOBALS['gallery'] =& $gallery;

if (!defined('GALLERY_CONFIG_DIR')) {
    define('GALLERY_CONFIG_DIR', dirname(__FILE__));
}
@include(GALLERY_CONFIG_DIR . '/config.php');

/*
 * Disable session.use_trans_sid to mitigate performance-penalty
 * (do it before any output is started)
 */
if (!defined('SID')) {
    @ini_set('session.use_trans_sid', 0);
}
?>
