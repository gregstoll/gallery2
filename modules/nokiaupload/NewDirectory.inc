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

GalleryCoreApi::requireOnce('modules/nokiaupload/classes/ImageUploadHelper.class');

/**
 * This controller implements the Nokia Image Uploader API's 'New Directory' command.
 * @package NokiaUpload
 * @author Jerome Rannikko <jeromer@hotpop.com>
 * @version $Revision: 17580 $
 */
class NewDirectoryController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	ImageUploadHelper::logRequest('CreateAlbumController');

	/* Request forgery protection */
	$httpUserAgent = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
	if (strstr($httpUserAgent, 'Nokia') === false) {
	    return array(GalleryCoreApi::error(ERROR_REQUEST_FORGED), null);
	}

	$results['delegate'] = array('view' => 'nokiaupload.NewDirectory');
	$results['status'] = ImageUploadHelper::createNewDirectory();
	$results['error'] = array();

	return array(null, $results);
    }

    /**
     * @see GalleryController::omitAuthTokenCheck
     */
    function omitAuthTokenCheck() {
	return true;
    }
}

/**
 * This view implements the Nokia Image Uploader API's 'New Directory' command.
 */
class NewDirectoryView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * API defines the output very specifically so we can't allow any global templating to be used.
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$gallery->debug("##### NewDirectoryView #####\n\$status = ");
	$gallery->debug_r($status);

	if ($status['success'] == NOKIAUPLOAD_ERR_NOERR && !isset($status['id'])) {
	    $status['success'] = NOKIAUPLOAD_ERR_UNKNOWN;
	}

	$reply = $status['success'] . "\r\n";

	if ($status['success'] == NOKIAUPLOAD_ERR_NOERR) {
	    $reply .= 'Did=' . $status['id'] . "\r\n";
	} else {
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'nokiaupload');
	    if ($ret) {
		$reply .= "Album creation failed.\r\n";
	    } else {
		$reply .= $module->translate('Album creation failed.') . "\r\n";
	    }
	}

	$gallery->debug("Sending:\n$reply");

	header("Content-type: text/plain; charset=UTF-8");
	header('Content-Length: '.strlen($reply));
	print $reply;

	return null;
    }
}
?>
