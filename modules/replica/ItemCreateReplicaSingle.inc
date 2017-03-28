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
 * This controller will handle replicating one item from one album to another.
 * @package Replica
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemCreateReplicaSingleController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	$status = $error = array();
	if (isset($form['action']['link'])) {
	    if (empty($form['destination'])) {
		$error[] = 'form[error][destination][empty]';
	    }

	    if (empty($error)) {
		$destinationId = $form['destination'];

		/* Make sure we can write to the destination */
		list ($ret, $permissions) =
		    GalleryCoreApi::fetchPermissionsForItems(array($itemId, $destinationId));
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($permissions[$destinationId])) {
		    /* Avoid information disclosure, act as if the item didn't exist. */
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}

		if (!isset($permissions[$destinationId]['core.addDataItem'])) {
		    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
		}

		if (!isset($permissions[$itemId]['core.viewSource'])) {
		    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
		}

		/* Look out for monkey business */
		if (!$item->getIsLinkable()) {
		    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
		}

		/*
		 * Ok we've got a linkable item and a legal destination album.
		 * Lock everything up and start linkin'.  We need to read lock
		 * the source ids, source hierarchy and destination hierarchy.
		 */
		list ($ret, $locks[]) =
		    GalleryCoreApi::acquireReadLock(array($itemId, $destinationId));
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $locks[]) = GalleryCoreApi::acquireReadLockParents($itemId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($locks);
		    return array($ret, null);
		}

		list ($ret, $locks[]) = GalleryCoreApi::acquireReadLockParents($destinationId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($locks);
		    return array($ret, null);
		}

		/* Create all our links */
		$classType = get_class($item);
		$linkedItem = new $classType;

		/*
		 * If we're linking to an item that's already a link,
		 * then link to its source instead.
		 */
		if ($item->isLinked()) {
		    $linkedEntity = $item->getLinkedEntity();
		    $ret = $linkedItem->createLink($linkedEntity, $destinationId);
		} else {
		    $ret = $linkedItem->createLink($item, $destinationId);
		}
		if ($ret) {
		    GalleryCoreApi::releaseLocks($locks);
		    return array($ret, null);
		}

		$ret = $linkedItem->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($locks);
		    return array($ret, null);
		}

		$ret = GalleryCoreApi::addExistingItemToAlbum($linkedItem, $destinationId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($locks);
		    return array($ret, null);
		}

		$status['linked'] = 1;

		/* Release the locks */
		$ret = GalleryCoreApi::releaseLocks($locks);
		if ($ret) {
		    return array($ret, null);
		}

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'replica.ItemCreateReplicaSingle';
		$redirect['itemId'] = $itemId;
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'replica.ItemCreateReplicaSingle';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view lets you choose where you want to put the new replica
 */
class ItemCreateReplicaSingleView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ItemCreateReplicaSingle') {
	    /* First time around, load the form with item data */
	    $form['destination'] = '';
	    $form['formName'] = 'ItemCreateReplicaSingle';
	}

	/* Find all the possible locations where this item can be linked. */
	list ($ret, $albumIds) = GalleryCoreApi::fetchAllItemIds(
	    'GalleryAlbumItem', array('core.addDataItem', 'core.view'));
	if ($ret) {
	    return array($ret, null);
	}

	/* Load all the album entities */
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	$ItemCreateReplicaSingle = array();
	$ItemCreateReplicaSingle['albumTree'] = GalleryUtilities::createAlbumTree($albums);
	$ItemCreateReplicaSingle['itemTypeNames'] = $item->itemTypeName();

	$template->setVariable('ItemCreateReplicaSingle', $ItemCreateReplicaSingle);
	$template->setVariable('controller', 'replica.ItemCreateReplicaSingle');
	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('lib/yui/treeview-min.js');
	$template->style('modules/core/data/tree.css');
	return array(null,
		     array('body' => 'modules/replica/templates/ItemCreateReplicaSingle.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'replica');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('replicate an item'));
    }
}
?>
