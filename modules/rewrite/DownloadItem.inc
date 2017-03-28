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
 * Send a binary item as HTML instead.
 * This allows viewing via direct URL (browser will see it is HTML), but not using images directly
 * in a <img> tag on another site.
 *
 * @package Rewrite
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class DownloadItemView extends GalleryView {

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
     * @see GalleryView::shouldSaveSession
     */
    function shouldSaveSession() {
	return false;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	$itemId = GalleryUtilities::getRequestVariables('itemId');
	if (empty($itemId)) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}
	list ($ret, $image) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryEntity');
	if ($ret) {
	    return $ret;
	}

    	if (!method_exists($image, 'fetchPath') || !method_exists($image, 'getMimeType')) {
	    /* Avoid information disclosure for bogus entities by acting as if it didn't exist */
	    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	}

	$derivativeType = null;
	if (GalleryUtilities::isA($image, 'GalleryDerivative')) {
	    $derivativeType = $image->getDerivativeType();
	}

	$ret = $this->_assertPermissions($image, $derivativeType);
	if ($ret) {
	    return $ret;
	}

	if (GalleryUtilities::isA($image, 'GalleryDerivative')) {
	    list ($ret, $item) =
		GalleryCoreApi::loadEntitiesById($image->getParentId(), 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }
	} else {
	    $item = $image;
	}

	header('Content-type: text/html; charset=UTF-8');
	header('Expires: ' . GalleryUtilities::getHttpDate(time() - 100));
	header('Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0');
	header('Pragma: no-cache');

	list ($ret, $albumId) = GalleryCoreApi::getDefaultAlbumId();
	if ($ret) {
	    return $ret;
	}
	list ($ret, $album) = GalleryCoreApi::loadEntitiesById($albumId, 'GalleryItem');
	if ($ret) {
	    return $ret;
	}
	list ($ret, $canViewRoot) = GalleryCoreApi::hasItemPermission($albumId, 'core.view');
	if ($ret) {
	    return $ret;
	}
	$galleryTitle = '';
	if ($canViewRoot) {
	    $galleryTitle = $album->getTitle();
	}

	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	$template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	$template->setVariable('l10Domain', 'modules_rewrite');
	$template->setVariable('item', (array)$item);
	$template->setVariable('image', (array)$image);
	$template->setVariable('galleryTitle', $galleryTitle);

	$ret = $template->display('gallery:modules/rewrite/templates/DownloadItem.tpl');
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * Assert the required permissions for the given item.
     * @param GalleryChildEntity $item GalleryDataItem or GalleryChildEntity with a data item as
     *        parent. Throws ERROR_MISSING_OBJECT if a non-item has no item as parent.
     * @param mixed $derivativeType
     * @return GalleryStatus
     */
    function _assertPermissions($item, $derivativeType) {
    	global $gallery;
    	$session =& $gallery->getSession();

    	$itemIdForPermission = $item->getId();
	if (!empty($derivativeType)) {
	    $itemIdForPermission = $item->getParentId();
	}

	/* Make sure we have permission */
	if (($ids = $session->get('core.isPrintService')) && in_array($item->getId(), $ids)) {
	    /* Print services only need core.view to get access to full size version of photos */
	    $permission = 'core.view';
	} else {
	    $permission = 'core.viewSource';
	    switch ($derivativeType) {
	    case DERIVATIVE_TYPE_IMAGE_THUMBNAIL:
		$permission = 'core.view';
		break;

	    case DERIVATIVE_TYPE_IMAGE_RESIZE:
		$permission = 'core.viewResizes';
		break;

		/* DERIVATIVE_TYPE_IMAGE_PREFERRED uses core.viewSource */
	    }
	}
	$ret = GalleryCoreApi::assertHasItemPermission($itemIdForPermission, $permission);
	if ($ret) {
	    /* Avoid information disclosure */
	    if ($ret->getErrorCode() & ERROR_PERMISSION_DENIED) {
	    	if ($permission != 'core.view') {
		    list ($ret2, $hasPermission) =
			GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
		    if ($ret2) {
			return $ret2;
		    }
	    	}
	    	if ($permission == 'core.view' || empty($hasPermission)) {
		    $ret->addErrorCode(ERROR_MISSING_OBJECT);
		    return $ret;
	    	}
	    }
	    return $ret;
	}

	return null;
    }
}
?>
