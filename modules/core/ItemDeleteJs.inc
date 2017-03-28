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
 * Generate JavaScript for deleting item 
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Felix Rabinvoich <felix@rabinovich.org> thanks to mindless for guidance
 * @version $Revision: 18144 $
 */
class ItemDeleteJsView extends GalleryView {
    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * @see GalleryView::shouldSaveSession
     */
    function shouldSaveSession() {
	return false;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	$template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	$template->setVariable('l10Domain', 'modules_core');

	if (function_exists('getallheaders')) {
	    $headers = getallheaders();
	    if (isset($headers['If-modified-since'])) {
		$headers['If-Modified-Since'] = $headers['If-modified-since'];
	    }
	    if (isset($headers['If-Modified-Since']) &&
		    ($when = strtotime($headers['If-Modified-Since'])) > 0) {
		/* Check date on language files to determine if we should resend the strings. */
		/** 
		 * TODO: Create an API to check whether the translation file has changed
		 *       and refactor the hardcoded .mo file locations.
		 */
		global $gallery;
		$platform =& $gallery->getPlatform();
		list ($ret, $lang) = $gallery->getActiveLanguageCode();
		if ($ret) {
		    return $ret;
		}
		$inc = $gallery->getConfig('data.gallery.base') 
		    . "locale/$lang/LC_MESSAGES/modules_core.mo";
		if ($platform->file_exists($inc)) {
		    if (($stat = $platform->stat($inc)) && $stat[9] > $when) {
			$new = true;
		    }
		} else {
		    $lang = substr($lang, 0, 2);
		    $inc = $gallery->getConfig('data.gallery.base') 
			. "locale/$lang/LC_MESSAGES/modules_core.mo";
		    if ($platform->file_exists($inc) && ($stat = $platform->stat($inc)) 
			&& $stat[9] > $when) {
			$new = true;
		    }
		}
		if (!isset($new)) {
		    header('HTTP/1.0 304 Not Modified');
		    return null;
		}
	    }
	}

	header('Content-type: text/javascript; charset=UTF-8');
	header('Last-Modified: ' . GalleryUtilities::getHttpDate());
	header('Expires: ' . GalleryUtilities::getHttpDate(time() + 2592000));

	$ret = $template->display('modules/core/templates/ItemDeleteJs.tpl');
	return $ret;
    }
}
?>
