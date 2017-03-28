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
 * This view creates a form to send data to shutterfly.com
 * @package Shutterfly
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class PrintPhotosView extends GalleryView {

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
     * @see GalleryView::renderImmedate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	list ($itemId, $returnUrl) = GalleryUtilities::getRequestVariables('itemId', 'returnUrl');
	if (!empty($itemId)) {
	    $cartItemIds = array($itemId => 1);
	} else {
	    $session =& $gallery->getSession();
	    $cartItemIds = $session->get('shutterfly.cart');
	    $session->remove('shutterfly.cart');
	}
	if (empty($cartItemIds) || empty($returnUrl)) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}
	$ret = $urlGenerator->assertIsInternalUrl($returnUrl);
	if ($ret) {
	    return $ret;
	}

	/* Load the necessary item data */
	$itemIds = array_keys($cartItemIds);
	list ($ret, $items) = GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryPhotoItem');
	if ($ret) {
	    return $ret;
	}

	list ($ret, $permissions) = GalleryCoreApi::fetchPermissionsForItems($itemIds);
	if ($ret) {
	    return $ret;
	}
	foreach ($itemIds as $id) {
	    if (empty($permissions[$id]['core.view'])) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	    }
	}

	list ($ret, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds($itemIds);
	if ($ret) {
	    return $ret;
	}

	list ($ret, $resizes) = GalleryCoreApi::fetchResizesByItemIds($itemIds);
	if ($ret) {
	    return $ret;
	}

	list ($ret, $preferreds) = GalleryCoreApi::fetchPreferredsByItemIds($itemIds);
	if ($ret) {
	    return $ret;
	}

	/* We want to know which are viewable to guests */
	list ($ret, $anonymousUserId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.anonymousUser');
	if ($ret) {
	    return $ret;
	}
	list ($ret, $publicPermissions) =
	    GalleryCoreApi::fetchPermissionsForItems($itemIds, $anonymousUserId, false);
	if ($ret) {
	    return $ret;
	}

	/* Assemble all our data */
	$i = 1;
	$entries = $protectedIds = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();

	    if (!isset($permissions[$itemId]['shutterfly.print'])) {
		/* Skip any cart items for which we don't have print permission */
		continue;
	    }

	    $source = isset($preferreds[$itemId]) ? $preferreds[$itemId] : $item;
	    $needSession = !isset($publicPermissions[$itemId]['core.viewSource']);

	    if ($needSession && !isset($sfSession)) {
		/*
		 * Get G2 session for shutterfly to access non-public images.
		 * We can't use this session because hijack protection will prevent access
		 * plus the current user could logout before shutterfly retrieves the images.
		 * Create a new session with the rights of current user for shutterfly to use.
		 */
		$sfSession = new GallerySession();
		$ret = $sfSession->initEmpty(true, $gallery->getActiveUserId());
		if ($ret) {
		    return $ret;
		}
	    }
	    if ($needSession) {
		$sessionParam = array($sfSession->getKey() => $sfSession->getId());
		$protectedIds[] = $source->getId();
	    } else {
		$sessionParam = array();
	    }

	    $entry = array('item' => (array)$item);
	    $entry['imageUrl'] = $urlGenerator->generateUrl(
		array_merge(array('view' => 'core.DownloadItem', 'itemId' => $source->getId()),
			    $sessionParam),
		array('forceSessionId' => false, 'forceFullUrl' => true));
	    $entry['imageWidth'] = $source->getWidth();
	    $entry['imageHeight'] = $source->getHeight();

	    if (!isset($thumbnails[$itemId]) || $thumbnails[$itemId]->getPostFilterOperations()) {
		/* Use the source if the thumbnail has a postfilter (like a watermark) */
		$thumbSource = $source;
	    } else {
		$thumbSource = $thumbnails[$itemId];
	    }

	    $entry['thumbUrl'] = $urlGenerator->generateUrl(
		array('view' => 'core.DownloadItem', 'itemId' => $thumbSource->getId()),
		array('forceSessionId' => $needSession, 'forceFullUrl' => true));
	    $entry['thumbWidth'] = $thumbSource->getWidth();
	    $entry['thumbHeight'] = $thumbSource->getHeight();

	    /*
	     * Ugh, the Shutterfly api doesn't have a parameter for quantity.
	     * Repeat the same entry multiple times to get desired quantity!
	     */
	    for ($j = 0; $j < $cartItemIds[$itemId]; $j++) {
		$entries[$i++] = $entry;
	    }
	}

	if (isset($sfSession)) {
	    /* Mark this session so that it can be treated specially */
	    $sfSession->put('core.isPrintService', $protectedIds);

	    /*
	     * TODO: Would like to enforce a particular session timeout to ensure this session
	     * lasts long enough for shutterfly to retrieve the images.  Maybe also store the
	     * sessionid in this session so we can reuse it for multiple print requests (and
	     * just bump timeout each time).
	     */
	    $ret = $sfSession->save();
	    if ($ret) {
		return $ret;
	    }
	}

	/*
	 * Ugh, the Shutterfly api can only track its session data via some cookies
	 * (redirecting to a url with embedded session id won't work) so we must
	 * render a form and submit it.. here we set our own cookie that will be
	 * checked to ensure we submit our form only once.
	 */
	if (!headers_sent() /* Should only skip cookie in unit test */) {
	    setcookie('G2_shutterfly', '1');
	    header("Content-type: text/html; charset=UTF-8");
	}

	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	$template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	$template->setVariable('l10Domain', 'modules_shutterfly');
	$template->setVariable('PrintPhotos', array('returnUrl' => $returnUrl,
						    'count' => count($entries),
						    'entries' => $entries));
	$template->display('gallery:modules/shutterfly/templates/PrintPhotos.tpl');

	return null;
    }
}
?>
