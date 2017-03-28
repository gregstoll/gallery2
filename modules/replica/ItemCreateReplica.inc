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
 * This controller will handle replicating one or more items from one album to another.
 * @package Replica
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 18138 $
 */
class ItemCreateReplicaController extends GalleryController {

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
	    if (empty($form['selectedIds'])) {
		$error[] = 'form[error][sources][empty]';
	    }

	    if (empty($error)) {
		$destinationId = $form['destination'];
		$selectedIds = array_keys($form['selectedIds']);

		/* Make sure we can write to the destination */
		list ($ret, $permissions) = GalleryCoreApi::getPermissions($destinationId);
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($permissions['core.view'])) {
		    /* Avoid information disclosure, act as if the item didn't exist. */
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}

		if (!isset($permissions['core.addDataItem'])) {
		    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED),
				 null);
		}

		/* Load the selected items */
		list ($ret, $selectedItems) =
		    GalleryCoreApi::loadEntitiesById($selectedIds, 'GalleryItem');
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $permissions) = GalleryCoreApi::fetchPermissionsForItems($selectedIds);
		if ($ret) {
		    return array($ret, null);
		}

		/* Look out for monkey business */
		foreach ($selectedItems as $selectedItem) {
		    if (!isset($permissions[$selectedItem->getId()]['core.view'])) {
			/* Avoid information disclosure, act as if the item didn't exist. */
			return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		    }

		    if (!isset($permissions[$selectedItem->getId()]['core.viewSource'])) {
			return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
		    }

		    if ($selectedItem->getParentId() != $itemId) {
			return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
		    }

		    if (!$selectedItem->getIsLinkable()) {
			return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
		    }
		}

		/*
		 * Ok we've got linkable items and a legal destination album.
		 * Lock everything up and start linkin'.  We need to read lock
		 * the source ids, source hierarchy and destination hierarchy.
		 */
		$ids = array_merge($selectedIds, array($itemId, $destinationId));
		list ($ret, $locks[]) = GalleryCoreApi::acquireReadLock($ids);
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
		$status['linked']['count'] = 0;
		foreach ($selectedItems as $selectedItem) {

		    $classType = get_class($selectedItem);
		    $linkedItem = new $classType;

		    /*
		     * If we're linking to an item that's already a link,
		     * then link to its source instead.
		     */
		    if ($selectedItem->isLinked()) {
			$linkedEntity = $selectedItem->getLinkedEntity();
			$ret = $linkedItem->createLink($linkedEntity, $destinationId);
		    } else {
			$ret = $linkedItem->createLink($selectedItem, $destinationId);
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

		    $status['linked']['count']++;
		}

		/* Release the locks */
		$ret = GalleryCoreApi::releaseLocks($locks);
		if ($ret) {
		    return array($ret, null);
		}

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'replica.ItemCreateReplica';
		$redirect['itemId'] = $itemId;
	    }
	} else if (isset($form['action']['next'])) {
	    $page = GalleryUtilities::getRequestVariables('page');
	    list ($ret, $peerIds) =
		GalleryCoreApi::fetchLinkableChildItemIdsWithPermission($itemId, 'core.view');
	    if ($ret) {
		return array($ret, null);
	    }

	    $numPages = ceil(sizeof($peerIds) / $form['numPerPage']);

	    $results['delegate']['itemId'] = $itemId;
	    $results['delegate']['page'] = min($page + 1, $numPages);
	} else if (isset($form['action']['previous'])) {
	    $page = GalleryUtilities::getRequestVariables('page');
	    $results['delegate']['itemId'] = $itemId;
	    $results['delegate']['page'] = max($page - 1, 1);
	} else if (isset($form['action']['cancel'])) {
	    $results['return'] = 1;
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    if (empty($results['return'])) {
		$results['delegate']['view'] = 'core.ItemAdmin';
		$results['delegate']['subView'] = 'replica.ItemCreateReplica';
	    }
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view lets you choose where you want to put the new replicas
 */
class ItemCreateReplicaView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	list ($selectedId, $page) =
	    GalleryUtilities::getRequestVariables('selectedId', 'page');
	if ($form['formName'] != 'ItemCreateReplica') {
	    /* First time around, load the form with item data */
	    if ($selectedId) {
		$form['selectedIds'][$selectedId] = true;
	    }
	    $form['destination'] = '';
	    $form['formName'] = 'ItemCreateReplica';
	    $form['numPerPage'] = 15;
	}

	/* Get all peers that we can link */
	list ($ret, $peerIds) = GalleryCoreApi::fetchLinkableChildItemIdsWithPermission(
	    $item->getId(), array('core.viewSource', 'core.view'));
	if ($ret) {
	    return array($ret, null);
	}

	$peers = $peerTypes = $peerDescendentCounts = $selectedIds = $albums = array();
	$numPages = 1;
	if (!empty($peerIds)) {
	    $numPages = ceil(sizeof($peerIds) / $form['numPerPage']);
	    if (empty($page)) {
		/* determine which page we're on */
		$page = 1;
		for ($i = 0; $i < sizeof($peerIds); $i++) {
		    if ($peerIds[$i] == $selectedId) {
			$page = ceil(($i + 1) / $form['numPerPage']);
		    }
		}
	    }

	    $start = $form['numPerPage'] * ($page - 1);
	    $peerIds = array_slice($peerIds, $start, $form['numPerPage']);
	    if (isset($form['selectedIds'])) {
		$selectedIds = $form['selectedIds'];
		foreach ($peerIds as $peerId) {
		    if (isset($selectedIds[$peerId])) {
			unset($selectedIds[$peerId]);
		    }
		}
	    }

	    /* Load all the peers */
	    list ($ret, $peerItems) = GalleryCoreApi::loadEntitiesById($peerIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* get peer thumbnails */
	    list ($ret, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds($peerIds);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Build our peers table */
	    $peers = array();
	    foreach ($peerItems as $peerItem) {
		$peers[$peerItem->getId()] = (array)$peerItem;
		$peers[$peerItem->getId()]['selected'] =
		    isset($form['selectedIds'][$peerItem->getId()]);

		/* While we're at it, attach the thumbnails */
		if (isset($thumbnails[$peerItem->getId()])) {
		    $thumbnail = $thumbnails[$peerItem->getId()];
		    list ($ret, $thumbnail) =
			GalleryCoreApi::rebuildDerivativeCacheIfNotCurrent($thumbnail->getId());
		    $peers[$peerItem->getId()]['thumbnail'] = (array)$thumbnail;
		}
	    }

	    /* Find all the possible locations where this item can be linked. */
	    list ($ret, $ids) =
		GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addDataItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Load all the album entities */
	    list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($ids, 'GalleryAlbumItem');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$urlGenerator =& $gallery->getUrlGenerator();

	$ItemCreateReplica = array();
	$ItemCreateReplica['canCancel'] = (boolean)GalleryUtilities::getRequestVariables('return');
	$ItemCreateReplica['peers'] = $peers;
	$ItemCreateReplica['albumTree'] = GalleryUtilities::createAlbumTree($albums);
	$ItemCreateReplica['page'] = $page;
	$ItemCreateReplica['numPages'] = $numPages;
	$ItemCreateReplica['numPerPage'] = $form['numPerPage'];
	$ItemCreateReplica['selectedIds'] = array_keys($selectedIds);

	$template->setVariable('ItemCreateReplica', $ItemCreateReplica);
	$template->setVariable('controller', 'replica.ItemCreateReplica');
	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('lib/yui/treeview-min.js');
	$template->style('modules/core/data/tree.css');
	return array(null, array('body' => 'modules/replica/templates/ItemCreateReplica.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'replica');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('create replica'));
    }
}
?>
