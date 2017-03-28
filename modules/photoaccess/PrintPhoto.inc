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
 * This controller handles printing a single photo
 * @package PhotoAccess
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class PrintPhotoController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	$returnUrl = GalleryUtilities::getRequestVariables('returnUrl');
	$ret = $urlGenerator->assertIsInternalUrl($returnUrl);
	if ($ret) {
	    return array($ret, null);
	}

	GalleryCoreApi::requireOnce('modules/photoaccess/classes/PhotoAccessHelper.class');
	list ($ret, $cartUrl) =
	    PhotoAccessHelper::printPhotos(array($item->getId() => 1), $returnUrl);
	if ($ret) {
	    return array($ret, null);
	}

	/* Prepare our results */
	$results['redirect']['href'] = $cartUrl;
	$results['status'] = array();
	$results['error'] = array();

	return array(null, $results);
    }
}
?>
