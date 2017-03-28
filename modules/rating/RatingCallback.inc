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
 * This view handles calls from the javascript (AJAX) side and returns data by printing results.
 * @package Rating
 * @author  Don Seiler <don@seiler.us>
 * @version $Revision: 17580 $
 */
class RatingCallbackView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isControllerLike
     */
    function isControllerLike() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/rating/classes/RatingHelper.class');

	list ($command, $rating) = GalleryUtilities::getRequestVariables('command', 'rating');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return $ret;
	}
	$itemId = $item->getId();

	if (!headers_sent()) {
	    header("Content-type: text/plain; charset=UTF-8");
	}

	switch ($command) {
	case 'rate':
	    list ($ret, $permission) = GalleryCoreApi::hasItemPermission($itemId, 'rating.add');
	    if ($ret) {
		return $ret;
	    }
	    if (!$permission) {
		return null;
	    }

	    list ($ret, $results) =
		RatingHelper::rateItem($itemId, (int)$rating, $gallery->getActiveUserId());
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'rating');
	    if ($ret) {
		return $ret;
	    }
	    $results['votesString'] = 
		$module->translate(array('one' => '%d vote', 
					 'many' => '%d votes', 
					 'count' => $results['votes'], 
					 'arg1' => $results['votes']));

	    $session =& $gallery->getSession();
	    print $results['itemId'] . "\n"
		. $results['rating'] . "\n"
		. $results['votesString'] . "\n"
		. $results['userRating'] . "\n"
		. $session->getAuthToken();
	    /*
	     * If this was the first rating from an anonymous user then their session just became
	     * persistent (see RatingHelper::rateItem); return the authToken so it can be used
	     * in future rating requests.
	     */

	    break;
	}

	return null;
    }
}
?>
