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
 * This controller will handle a user logging out of Gallery
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class LogoutController extends GalleryController {

    /**
     * @see GalleryController::isAllowedInMaintenance
     */
    function isAllowedInMaintenance() {
	return true;
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$event = GalleryCoreApi::newEvent('Gallery::Logout');
	$activeUser = $gallery->getActiveUser();
	$event->setEntity($activeUser);
	list ($ret, $eventResults) = GalleryCoreApi::postEvent($event);
	if ($ret) {
	    return array($ret, null);
	}

	$results = array();
	foreach ($eventResults as $key => $value) {
	    if (!empty($value['delegate'])) {
		$results['delegate'] = $value['delegate'];
	    }
	}

	$session =& $gallery->getSession();
	$ret = $session->reset();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $anonymousId) = GalleryCoreApi::getAnonymousUserId();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $guestUser) = GalleryCoreApi::loadEntitiesById($anonymousId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	$gallery->setActiveUser($guestUser);

	if (!isset($results['status'])) {
	    $results['status'] = array();
	}
	if (!isset($results['error'])) {
	    $results['error'] = array();
	}

	/*
	 * Force return to core.ShowItem, as we don't know if the guest user has necessary
	 * permissions for the return page
	 */
	if (!isset($results['return'])
		&& !isset($results['redirect'])
		&& !isset($results['delegate'])) {
	    $results['redirect']['view'] = GALLERY_DEFAULT_VIEW;
	}

	return array(null, $results);
    }
}
?>
