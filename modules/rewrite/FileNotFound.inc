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
 * View for 404 Not Found
 * @package Rewrite
 * @subpackage UserInterface
 * @author Douglas Cau <douglas@cau.se>
 * @version $Revision: 17580 $
 */
class FileNotFoundView extends GalleryView {

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'rewrite');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('404 File Not Found'));
    }

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$phpVm = $gallery->getPhpVm();
	$urlGenerator =& $gallery->getUrlGenerator();

	/* Don't use getCurrentUrl, it migh be staged by the url generator */
	$path = GalleryUtilities::getServerVar('REQUEST_URI');
	$path = $urlGenerator->makeUrl($path);

	$phpVm->header('HTTP/1.0 404 Not Found');
	$template->setVariable('FileNotFound', $path);

	return array(null, array('body' => 'modules/rewrite/templates/FileNotFound.tpl'));
    }
}
?>
