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
 * This controller implements the Nokia Image Uploader API's 'Login' command.
 * @package NokiaUpload
 * @author Jerome Rannikko <jeromer@hotpop.com>
 * @version $Revision: 17580 $
 */
class LoginController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	ImageUploadHelper::logRequest('LoginController');

	/* Request forgery protection */
	$httpUserAgent = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
	if (strstr($httpUserAgent, 'Nokia') === false) {
	    return array(GalleryCoreApi::error(ERROR_REQUEST_FORGED), null);
	}

	$results['delegate'] = array('view' => 'nokiaupload.Login');
	$results['status'] = ImageUploadHelper::doLogin();
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
 * This view implements the Nokia Image Uploader API's 'Login' command.
 */
class LoginView extends GalleryView {

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
	$gallery->debug("##### LoginView #####\n\$status = ");
	$gallery->debug_r($status);

	$reply = array($status['success'] . "\r\n");
	$reply[] = 'Version=' . NOKIAUPLOAD_PROTOCOLVERSION . "\r\n";

	if ($status['success'] == NOKIAUPLOAD_ERR_NOERR) {
	    $session =& $gallery->getSession();
	    $reply[] = 'SId=' . GalleryUtilities::prefixFormVariable($session->getKey()) .
		'=' . $session->getId() . "\r\n";
	    $reply[] = 'RSURL=' . $status['rsurl'] . "\r\n";

	    /* Send RS Capabilities as well if we have them. */
	    if (isset($status['capabilities'])) {
		foreach ($status['capabilities'] as $capability) {
		    $reply[] = $capability;
		}
	    }
	} else {
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'nokiaupload');
	    if ($ret) {
		$reply[] = "Login failed.\r\n";
	    } else {
		$reply[] = $module->translate('Login failed.') . "\r\n";
	    }
	}

	$reply = implode('', $reply);
	$gallery->debug("Sending:\n$reply");

	header("Content-type: text/plain; charset=UTF-8");
	header('Content-Length: ' . strlen($reply));
	print $reply;

	return null;
    }
}
?>
