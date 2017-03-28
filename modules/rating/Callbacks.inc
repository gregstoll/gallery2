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
 * @package Rating
 * @author  Don Seiler <don@seiler.us>
 * @version $Revision: 17580 $
 */
class RatingCallbacks {

    function callback($params, &$smarty, $callback, $userId) {
	global $gallery;

	switch ($callback) {
	case 'LoadRating':
	    $itemId = $params['itemId'];
	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $hasPermission) =
		GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
	    if ($ret) {
		return $ret;
	    }
	    if (!$hasPermission) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	    }

	    if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		$albumId = $itemId;
		list ($ret, $allowAlbumRating) =
		    GalleryCoreApi::getPluginParameter('module', 'rating', 'allowAlbumRating');
		if ($ret) {
		    return $ret;
		}
		if (!$allowAlbumRating) {
		    return null;
		}
	    } else {
		$albumId = $item->getParentId();
	    }

	    list ($ret, $enabled) =
		GalleryCoreApi::getPluginParameter('module', 'rating', 'enabled', $albumId);
	    if ($ret) {
		return $ret;
	    }
	    if (!$enabled) {
		return null;
	    }

	    list ($ret, $permission) =
		GalleryCoreApi::hasItemPermission($itemId, 'rating.view', $userId);
	    if ($ret) {
		return $ret;
	    }
	    if (!$permission) {
		return null;
	    }

	    GalleryCoreApi::requireOnce('modules/rating/classes/RatingHelper.class');
	    list ($ret, $RatingData) = RatingHelper::fetchRatings(array($itemId), $userId);
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $permission) =
		GalleryCoreApi::hasItemPermission($itemId, 'rating.add', $userId);
	    if ($ret) {
		return $ret;
	    }
	    $RatingData[$itemId]['canRate'] = ($permission ? true : false);

	    $session =& $gallery->getSession();
	    $authToken = $session->isPersistent() ? $session->getAuthToken() : '';
	    $RatingSummary = array('ratingValues' => array(1, 2, 3, 4, 5),
				   'firstCall' => true, 'authToken' => $authToken);

	    $block =& $smarty->_tpl_vars['block'];
	    $block['rating']['RatingData'] = $RatingData[$itemId];
	    $block['rating']['RatingSummary'] = $RatingSummary;

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
