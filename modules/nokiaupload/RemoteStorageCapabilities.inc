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
 * This view implements the Nokia Image Uploader API's 'Remote Storage Capabilities' query.
 * @package NokiaUpload
 * @author Jerome Rannikko <jeromer@hotpop.com>
 * @version $Revision: 17580 $
 */
class RemoteStorageCapabilitiesView extends GalleryView {

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
	ImageUploadHelper::logRequest('RemoteStorageCapabilitiesView');

	$capabilities = ImageUploadHelper::getRSCapabilities();

	if (empty($capabilities)) {
	    $reply = NOKIAUPLOAD_ERR_UNKNOWN . "\r\n";
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'nokiaupload');
	    if ($ret) {
		$reply .= "Couldn't retrieve settings.\r\n";
	    } else {
		$reply .= $module->translate("Couldn't retrieve settings.") . "\r\n";
	    }
	} else {
	    $reply = NOKIAUPLOAD_ERR_NOERR . "\r\n" . implode('', $capabilities);
	}

	$gallery->debug("Sending:\n$reply");

	header("Content-type: text/plain; charset=UTF-8");
	header('Content-Length: ' . strlen($reply));
	print $reply;

	return null;
    }
}
?>
