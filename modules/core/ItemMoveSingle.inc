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
 * This controller will handle moving an item from one album to another.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemMoveSingleController extends GalleryController {

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

	if (isset($form['action']['move'])) {
	    if (empty($form['destination'])) {
		$error[] = 'form[error][destination][empty]';
	    }

	    $ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.delete');
	    if ($ret) {
		return array($ret, null);
	    }

	    if (empty($error)) {
		$newParentId = $form['destination'];

		/*
		 * Assert that our permissions are correct.  The view should never try to make a
		 * move that isn't legal so we can just bail if there's an inconsistency.
		 */
		list ($ret, $canView) =
		    GalleryCoreApi::hasItemPermission($newParentId, 'core.view');
		if ($ret) {
		    return array($ret, null);
		}
		if (!$canView) {
		    /* Avoid information disclosure, act as if the item didn't exist. */
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}
		if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
		    $ret = GalleryCoreApi::assertHasItemPermission($newParentId,
								   'core.addDataItem');
		    if ($ret) {
			return array($ret, null);
		    }
		} else if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		    $ret = GalleryCoreApi::assertHasItemPermission($newParentId,
								   'core.addAlbumItem');
		    if ($ret) {
			return array($ret, null);
		    }
		} else {
		    /* The view should never let this happen */
		    return array(GalleryCoreApi::error(ERROR_BAD_DATA_TYPE),
				 null);
		}
	    }

	    /* Make sure the destination is the right type of item */
	    if (empty($error)) {
		list ($ret, $newParent) =
		    GalleryCoreApi::loadEntitiesById($newParentId, 'GalleryAlbumItem');
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    /* Make sure we don't have recursive moves */
	    if (empty($error)) {
		list ($ret, $newParentAncestorIds) =
		    GalleryCoreApi::fetchParentSequence($newParentId);
		if ($ret) {
		    return array($ret, null);
		}

		if ($itemId == $newParentId || in_array($itemId, $newParentAncestorIds)) {
		    $error[] = 'form[error][destination][selfMove]';
		}
	    }

	    if (empty($error)) {
		/*
		 * Read lock both parent hierarchies
		 * TODO: Optimize this
		 */
		list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLockParents($newParentId);
		if ($ret) {
		    return array($ret, null);
		}

		$oldParentId = $item->getParentId();
		list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLockParents($oldParentId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		list ($ret, $lockIds[]) =
		    GalleryCoreApi::acquireReadLock(array($newParentId, $oldParentId));
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		/* Write lock the item we're moving */
		list ($ret, $lockIds[]) = GalleryCoreApi::acquireWriteLock($itemId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		/* Refresh the item in case it changed before it was locked */
		list ($ret, $item) = $item->refresh();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		/* Do the move */
		$ret = $item->move($newParentId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		$ret = $item->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
		    /* Update for derivative preferences of new parent */
		    $ret = GalleryCoreApi::addExistingItemToAlbum($item, $newParentId);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null);
		    }
		}

		/* Release all locks */
		$ret = GalleryCoreApi::releaseLocks($lockIds);
		if ($ret) {
		    return array($ret, null);
		}

		/* Fix thumbnail integrity */
		list ($ret, $success) = GalleryCoreApi::guaranteeAlbumHasThumbnail($oldParentId);
		if ($ret) {
		    return array($ret, null);
		}

		$status['moved'] = 1;

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'core.ItemMoveSingle';
		$redirect['itemId'] = $itemId;
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'core.ItemMoveSingle';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for the destination of moving this item
 */
class ItemMoveSingleView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     * @todo Scalability - Don't load all album-items into memory. Need a way to load the items
     *       incrementally, e.g. with an AJAX-powered tree widget.
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	/* itemId is the album where we want to move items from */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	if ($form['formName'] != 'ItemMoveSingle') {
	    /* First time around, load the form with item data */
	    $form['destination'] = '';
	    $form['formName'] = 'ItemMoveSingle';
	}

	if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
	    $permissions = 'core.addDataItem';
	} else if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    $permissions = 'core.addAlbumItem';
	} else {
	    return array(GalleryCoreApi::error(ERROR_BAD_DATA_TYPE), null);
	}
	$permissions = array($permissions, 'core.view');

	/* Get ids of all all albums where we can add this items */
	list ($ret, $albumIds) = GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', $permissions);
	if ($ret) {
	    return array($ret, null);
	}

	/* Load all the album entities */
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	$albumTree = GalleryUtilities::createAlbumTree($albums);

	$ItemMoveSingle = array();
	$ItemMoveSingle['albumTree'] = $albumTree;
	$ItemMoveSingle['itemTypeNames'] = $item->itemTypeName();

	$template->setVariable('ItemMoveSingle', $ItemMoveSingle);
	$template->setVariable('controller', 'core.ItemMoveSingle');
	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('lib/yui/treeview-min.js');
	$template->style('modules/core/data/tree.css');
	return array(null, array('body' => 'modules/core/templates/ItemMoveSingle.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	$itemTypeNames = $item->itemTypeName(true);

	return array(null,
		     $core->translate(array('text' => 'move %s', 'arg1' => $itemTypeNames[1])));
    }
}
?>
