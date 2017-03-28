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
 * Handle the rendering of an album or item.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ShowItemController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	/*
	 * Note that this always changes user preview mode; if we add management of other variables
	 * to this controller then we should guard this properly.  (Maybe delete this comment after
	 * writing the unit test that verifies it)
	 */
	$guestPreviewMode = GalleryUtilities::getRequestVariables('guestPreviewMode');
	if ($guestPreviewMode != null) {
	    $session =& $gallery->getSession();
	    $session->put('theme.guestPreviewMode', $guestPreviewMode ? 1 : 0);
	}

	return array(null, array('return' => 1, 'status' => array(), 'error' => array()));
    }
}

/**
 * Handle the rendering of an album or item.
 */
class ShowItemView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	list ($ret, $item, $wasSpecified) = $this->getItem(true);
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($_GET[GalleryUtilities::prefixFormVariable('path')]) && !$wasSpecified) {
	    /*
	     * Bug #1468797
	     * Detect use of rewritten URL but rewrite module inactive; walk up to find main.php
	     */
	    $urlGenerator =& $gallery->getUrlGenerator();
	    $redirect = dirname($urlGenerator->getCurrentUrlDir()) . '/' . GALLERY_MAIN_PHP;
	    return array(null, array('redirectUrl' => $redirect));
	}

	/*
	 * Don't increment the view count for anything but the first page of the album so that we
	 * don't count each individual page views as an album view.  This only applies to
	 * non-persistent sessions because GalleryCoreApi::incrementItemViewCount does the right
	 * thing for persistent sessions.
	 */
	if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    $session =& $gallery->getSession();
	    if (!$session->isPersistent()) {
		list ($page, $highlightId) =
		    GalleryUtilities::getRequestVariables('page', 'highlightId');
		if ($highlightId || ($page && $page != 1)) {
		    return array(null, null);
		}
	    }
	}

	$ret = GalleryCoreApi::incrementItemViewCount($item->getId());
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, array());
    }

    /**
     * @see GalleryView::getItem
     */
    function getItem($getOriginalSpecified=false) {
	static $originalSpecified;
	list ($ret, $item, $wasSpecified) = parent::getItem();
	if ($ret) {
	    return array($ret, null, null);
	}
	if (!isset($originalSpecified)) {
	    /* Save value for first call; $wasSpecified always true later as itemId is set below */
	    $originalSpecified = $wasSpecified;
	}
	if (!$wasSpecified) {
	    GalleryUtilities::putRequestVariable('itemId', $item->getId());
	}
	/* Default wasSpecified=true so without itemId we still use root album theme/params */
	return array(null, $item, $getOriginalSpecified ? $originalSpecified : true);
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	$typeName = $item->itemTypeName(true);
	return array(null, $typeName[1]);
    }

    /**
     * @see GalleryView::getViewType
     */
    function getViewType() {
	return VIEW_TYPE_SHOW_ITEM;
    }
}
?>
