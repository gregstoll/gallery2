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
 * Switch the detail mode in the session and return to the previous page.
 * This controller is used if the browser is not using javascript.
 * @package Exif
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class SwitchDetailModeController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest() {
	global $gallery;

	$mode = GalleryUtilities::getRequestVariables('mode');
	if ($mode != 'summary' && $mode != 'detailed') {
	    $mode = 'summary';
	}

	/* Store the new mode in the session */
	$session =& $gallery->getSession();
	$session->put('exif.module.LoadExifInfo.mode', $mode);

	$results['return'] = 1;
	$results['redirect']['view'] = 'core.ShowItem';
	$results['status'] = array();
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * This view provides raw html for the exif block, used in ajax calls to switch mode.
 * @author Alan Harder <alan.harder@sun.com>
 */
class SwitchDetailModeView extends GalleryView {

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
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$session =& $gallery->getSession();

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return $ret;
	}

	$mode = GalleryUtilities::getRequestVariables('mode');
	if ($mode != 'summary' && $mode != 'detailed') {
	    $mode = 'summary';
	}

	/* Store the new mode in the session */
	$session->put('exif.module.LoadExifInfo.mode', $mode);

	if (!headers_sent()) {
	    header("Content-type: text/plain; charset=UTF-8");
	}

	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	$template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	$template->setVariable('l10Domain', 'modules_exif');
	$template->setVariable('ajax', true);
	$template->setVariable('item', (array)$item);
	$template->setVariable('theme', array('actingUserId' => $gallery->getActiveUserId()));

	$ret = $template->display('gallery:modules/exif/templates/blocks/ExifInfo.tpl');
	if ($ret) {
	    return $ret;
	}

	return null;
    }
}
?>
