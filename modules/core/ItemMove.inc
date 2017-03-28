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
 * This controller will handle moving one or more items from one album to another.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Ernesto Baschny <ernst@baschny.de>
 * @version $Revision: 18138 $
 */
class ItemMoveController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     * @todo Scalability - Don't load all album-items into memory. Need a way to load the items
     *       incrementally, e.g. with an AJAX-powered tree widget.
     */
    function handleRequest($form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	$status = $error = array();
	if (isset($form['action']['move'])) {

	    /* First check if everything would be okay with the change */
	    $canAddItem = $canAddAlbum = false;
	    if (!empty($form['destination'])) {
		/* Check if we can add albums or items here */
		$newParentId = $form['destination'];
		list ($ret, $permissions) = GalleryCoreApi::getPermissions($newParentId);
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($permissions['core.view'])) {
		    /* Avoid information disclosure, act as if the item didn't exist. */
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}

		$canAddItem = isset($permissions['core.addDataItem']);
		$canAddAlbum = isset($permissions['core.addAlbumItem']);
		if (!$canAddAlbum && !$canAddItem) {
		    $error[] = 'form[error][destination][permission]';
		}

		/* Load destination parent ids: we don't want recursive moves */
		list ($ret, $newParentAncestorIds) =
		    GalleryCoreApi::fetchParentSequence($newParentId);
		if ($ret) {
		    return array($ret, null);
		}
		$newParentAncestorIds[] = $newParentId;
	    } else {
		$error[] = 'form[error][destination][empty]';
	    }

	    if (empty($error) && !empty($form['selectedIds'])) {
		$selectedIds = array_keys($form['selectedIds']);

		/* Load the source items */
		list ($ret, $selectedItems) =
		    GalleryCoreApi::loadEntitiesById($selectedIds, 'GalleryItem');
		if ($ret) {
		    return array($ret, null);
		}
		$ret = GalleryCoreApi::studyPermissions($selectedIds);
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($selectedItems as $selectedItem) {
		    $selectedId = $selectedItem->getId();

		    /* Can't move into a tree that is included in the source */
		    if (in_array($selectedId, $newParentAncestorIds)) {
			$error[] = 'form[error][source][' . $selectedId . '][selfMove]';
			continue;
		    }

		    list ($ret, $permissions) = GalleryCoreApi::getPermissions($selectedId);
		    if ($ret) {
			return array($ret, null);
		    }

		    /* Can we delete this item from here? */
		    if (!isset($permissions['core.delete'])) {
			$error[] = 'form[error][source][' . $selectedId . '][permission][delete]';
		    }

		    /* Check if the destination allows this source to be added */
		    if ($selectedItem->getCanContainChildren() && !$canAddAlbum) {
			$error[] = 'form[error][source][' . $selectedId .
				   '][permission][addAlbumItem]';
		    } else if (!$selectedItem->getCanContainChildren() && !$canAddItem) {
			$error[] = 'form[error][source][' . $selectedId .
				   '][permission][addDataItem]';
		    }
		}
	    }

	    if (empty($error) && !empty($selectedIds) && !empty($newParentId)) {
		$storage =& $gallery->getStorage();

		/* Read lock old and new parent album, and all ancestor albums */
		$lockIds = array();
		list ($ret, $oldParents) = GalleryCoreApi::fetchParentSequence($itemId);
		if ($ret) {
		    return array($ret, null);
		}
		list ($ret, $newParents) = GalleryCoreApi::fetchParentSequence($newParentId);
		if ($ret) {
		    return array($ret, null);
		}
		$oldParents[] = $itemId;
		$newParents[] = $newParentId;
		list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLock(
			array_unique(array_merge($oldParents, $newParents)));
		if ($ret) {
		    return array($ret, null);
		}

		/* Do the move / locking in batches to prevent too many open files issues */
		$batchSize = 100;
		$status['moved']['count'] = 0;
		do {
		    $currentItems = array_splice($selectedItems, 0, $batchSize);
		    $currentIds = array();
		    foreach ($currentItems as $item) {
			$currentIds[] = $item->getId();
		    }
		    /* Write lock all the items we're moving */
		    list ($ret, $currentLockId) = GalleryCoreApi::acquireWriteLock($currentIds);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null);
		    }

		    /* If we have no problems, do the moves */
		    foreach ($currentItems as $selectedItem) {
			$ret = $selectedItem->move($newParentId);
			if ($ret) {
			    $lockIds[] = $currentLockId;
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null);
			}
			$status['moved']['count']++;

			$ret = $selectedItem->save();
			if ($ret) {
			    $lockIds[] = $currentLockId;
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null);
			}

			if (GalleryUtilities::isA($selectedItem, 'GalleryDataItem')) {
			    /* Update for derivative preferences of new parent */
			    $ret =
				GalleryCoreApi::addExistingItemToAlbum($selectedItem, $newParentId);
			    if ($ret) {
				$lockIds[] = $currentLockId;
				GalleryCoreApi::releaseLocks($lockIds);
				return array($ret, null);
			    }
			}

			$ret = $storage->checkPoint();
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null);
			}
		    }
		    $ret = GalleryCoreApi::releaseLocks($currentLockId);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null);
		    }
		    $ret = $storage->checkPoint();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null);
		    }
		} while (!empty($selectedItems));

		$ret = GalleryCoreApi::releaseLocks($lockIds);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $success) = GalleryCoreApi::guaranteeAlbumHasThumbnail($itemId);
		if ($ret) {
		    return array($ret, null);
		}

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'core.ItemMove';
		$redirect['itemId'] = $itemId;
	    }
	} else if (isset($form['action']['next'])) {
	    $page = GalleryUtilities::getRequestVariables('page');
	    list ($ret, $peerIds) =
		GalleryCoreApi::fetchChildItemIdsWithPermission($itemId, 'core.delete');
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
	    $results['return'] = true;
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'core.ItemMove';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for which items to move and where is the destination.
 */
class ItemMoveView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	/* itemId is the album where we want to move items from */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	list ($selectedId, $page) = GalleryUtilities::getRequestVariables('selectedId', 'page');
	if ($form['formName'] != 'ItemMove') {
	    /* First time around, load the form with item data */
	    if ($selectedId) {
		$form['selectedIds'][$selectedId] = true;
	    }
	    $form['destination'] = '';
	    $form['formName'] = 'ItemMove';
	    $form['numPerPage'] = 15;
	}

	/* Get all peers that we can delete */
	list ($ret, $peerIds) = GalleryCoreApi::fetchChildItemIdsWithPermission(
	    $itemId, array('core.delete', 'core.view'));
	if ($ret) {
	    return array($ret, null);
	}

	$albumIds = $albumTree = $selectedIds = $peers = $peerDescendentCounts = array();
	$peerTypes = array('album' => array(), 'data' => array());
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

	    /* Add any items with error messages that would otherwise not be shown */
	    if (!empty($form['error']['source'])) {
		foreach ($form['error']['source'] as $id => $tmp) {
		    if (!in_array($id, $peerIds)) {
			array_unshift($peerIds, $id);
			unset($selectedIds[$id]);
		    }
		}
	    }

	    /* Load all the peers */
	    list ($ret, $peerItems) = GalleryCoreApi::loadEntitiesById($peerIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* get peer thumbnails and resizes */
	    list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds($peerIds);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Build our peers table */
	    $peers = array();
	    foreach ($peerItems as $peerItem) {
		$peers[$peerItem->getId()] = (array)$peerItem;
		if (GalleryUtilities::isA($peerItem, 'GalleryAlbumItem')) {
		    $peerTypes['album'][$peerItem->getId()] = 1;
		} else {
		    $peerTypes['data'][$peerItem->getId()] = 1;
		}
		$peers[$peerItem->getId()]['selected'] =
		    isset($form['selectedIds'][$peerItem->getId()]);

		/* While we're at it, attach thumbnails and resizes */
		if (isset($derivatives[$peerItem->getId()])) {
		    foreach ($derivatives[$peerItem->getId()] as $derivative) {
			$type = $derivative->getDerivativeType();
			if (empty($peers[$peerItem->getId()]['resize']) &&
			        $type == DERIVATIVE_TYPE_IMAGE_RESIZE) {
			    $peers[$peerItem->getId()]['resize'] = (array)$derivative;
			} else if ($type == DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
			    $peers[$peerItem->getId()]['thumbnail'] = (array)$derivative;
			}
		    }
		}
	    }

	    /* Get child counts */
	    if (!empty($peerTypes['album'])) {
		list ($ret, $peerDescendentCounts) =
		    GalleryCoreApi::fetchDescendentCounts(array_keys($peerTypes['album']));
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    /* Get ids of all albums where we can add new data items */
	    list ($ret, $albumIds['addDataItem']) = GalleryCoreApi::fetchAllItemIds(
		'GalleryAlbumItem', array('core.addDataItem', 'core.view'));
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Get ids of all all albums where we can add new album items */
	    list ($ret, $albumIds['addAlbumItem']) = GalleryCoreApi::fetchAllItemIds(
		'GalleryAlbumItem', array('core.addAlbumItem', 'core.view'));
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Merge them together to get the master list of ids */
	    $albumIds['allIds'] = array_unique(array_merge($albumIds['addDataItem'],
							   $albumIds['addAlbumItem']));

	    /* Load all the album entities */
	    list ($ret, $albums) =
		GalleryCoreApi::loadEntitiesById($albumIds['allIds'], 'GalleryAlbumItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    $albumTree = GalleryUtilities::createAlbumTree($albums);
	}

	$urlGenerator =& $gallery->getUrlGenerator();

	$ItemMove = array();
	$ItemMove['canCancel'] = (boolean)GalleryUtilities::getRequestVariables('return');
	$ItemMove['albumIds'] = $albumIds;
	$ItemMove['peers'] = $peers;
	$ItemMove['peerTypes'] = $peerTypes;
	$ItemMove['peerDescendentCounts'] = $peerDescendentCounts;
	$ItemMove['albumTree'] = $albumTree;
	$ItemMove['page'] = $page;
	$ItemMove['numPages'] = $numPages;
	$ItemMove['numPerPage'] = $form['numPerPage'];
	$ItemMove['selectedIds'] = array_keys($selectedIds);
	$ItemMove['selectedIdCount'] = count($selectedIds);

	$template->setVariable('ItemMove', $ItemMove);
	$template->setVariable('controller', 'core.ItemMove');
	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('lib/yui/treeview-min.js');
	$template->style('modules/core/data/tree.css');
	return array(null, array('body' => 'modules/core/templates/ItemMove.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('move item'));
    }
}
?>
