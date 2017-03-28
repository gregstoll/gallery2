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
 * This controller-like view will handle the deletion of a single item
 * Note (Dec. 2006): this functionality has been rewritten using AJAX
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemDeleteSingleController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();
	
	list ($pageItemId, $page) = 
	    GalleryUtilities::getRequestVariables('pageId', 'page');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* The framework shouldn't let us get this far if we don't have delete permission */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.delete');
	if ($ret) {
	    return array($ret, null);
	}
	
	/* Get the parent *before* we delete the item */	
	$parentId = $item->getParentId();
	
	/* Get the root album id, so we don't try to delete it */
	list ($ret, $rootId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return array($ret, null);
	}

	/* The view shouldn't let us try to delete the root album, either */
	if ($itemId == $rootId) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
					 "Can't delete the root album"), null);
	}

	$ret = GalleryCoreApi::deleteEntityById($itemId, 'GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}
	
	list ($ret, $unused) = GalleryCoreApi::guaranteeAlbumHasThumbnail($parentId);
	if ($ret) {
	    return array($ret, null);
	}

	/**
	 * @todo: once there is a common status message, use it to provide feedback on success
	 */
	$params = array('view' => 'core.ShowItem', 
	      'itemId' => ($itemId == $pageItemId ? $parentId : $pageItemId));
	if (!empty($page)) {
	    $params['page'] = $page;
	}
	$result['redirect'] = $params;
	$result['status'] = array();
	$result['error'] = array();
	return array(null, $result);
    }
}
?>
