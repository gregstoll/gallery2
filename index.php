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
 * This script will just redirect to main.php
 * The Location header requires an absolute url to conform with HTTP/1.1
 * @package Gallery
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */

/* Include bootstrap.inc in case config.php overrides GALLERY_MAIN_PHP */
require_once(dirname(__FILE__) . '/bootstrap.inc');
require_once(dirname(__FILE__) . '/modules/core/classes/GalleryUrlGenerator.class');
require_once(dirname(__FILE__) . '/modules/core/classes/GalleryCoreApi.class');

/* The REQUEST_URI can either be /path/index.php or just /path/. Get rid of index.php.* */
$path = GalleryUrlGenerator::getCurrentRequestUri();
if (preg_match('|^(/(?:[^?#/]+/)*)(.*)|', $path, $matches)) {
    $path = $matches[1] . GALLERY_MAIN_PHP;
    if (!empty($matches[2]) && ($pos = strpos($matches[2], '?')) !== false) {
	$path .= substr($matches[2], $pos);
    }
}

$configBaseUri = @$gallery->getConfig('baseUri');

$urlGenerator = new GalleryUrlGenerator();
$urlGenerator->init(!empty($configBaseUri) ? $configBaseUri : null);

$phpVm = $gallery->getPhpVm();
$phpVm->header('Location: ' . $urlGenerator->makeUrl($path));
?>
