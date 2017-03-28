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
 * This controller locates (and creates if needed) a user album and jumps to its view.
 * @package UserAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class UserAlbumController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$activeUserId = $gallery->getActiveUserId();

	$userId = GalleryUtilities::getRequestVariables('userId');
	if (empty($userId)) {
	    $userId = $activeUserId;
	}

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser($userId);
	if ($ret) {
	    return array($ret, null);
	}
	if ($isAnonymous) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	list ($ret, $albumId) =
	    GalleryCoreApi::getPluginParameter('module', 'useralbum', 'albumId', $userId);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $createMode) =
	    GalleryCoreApi::getPluginParameter('module', 'useralbum', 'create');
	if ($ret) {
	    return array($ret, null);
	}

	if (!empty($albumId)) {
	    list ($ret, $album) = GalleryCoreApi::loadEntitiesById($albumId, 'GalleryItem');
	    if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		return array($ret, null);
	    }
	}

	if (!isset($album) && $createMode == 'access') {
	    /* If userId matches active user then create the user album */
	    if ($userId != $activeUserId) {
		return
		    array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	    }
	    GalleryCoreApi::requireOnce('modules/useralbum/classes/UserAlbumHelper.class');

	    $ret = UserAlbumHelper::createUserAlbum($gallery->getActiveUser());
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $albumId) =
		GalleryCoreApi::getPluginParameter('module', 'useralbum', 'albumId', $userId);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$results = array('delegate' => array('view' => 'core.ShowItem',
					     'itemId' => $albumId),
			 'status' => array(),
			 'error' => array());

	return array(null, $results);
    }
}
?>
