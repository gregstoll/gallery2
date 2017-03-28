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
 * This controller will handle the deletion of items
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 18138 $
 */
class ItemDeleteController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$status = $error = array();

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	if (isset($form['action']['delete'])) {
	    $status['deleted']['count'] = 0;
	    if (!empty($form['selectedIds'])) {
		$selectedIds = array_keys($form['selectedIds']);

		/* get the rootId, so we don't try to delete it */
		list ($ret, $rootId) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $permissions) = GalleryCoreApi::fetchPermissionsForItems($selectedIds);
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($selectedIds as $selectedId) {
		    if (!isset($permissions[$selectedId]['core.view'])) {
			/* Avoid information disclosure, act as if the item didn't exist. */
			return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		    }
		    /* Make sure we have permission to delete this item */
		    if (!isset($permissions[$selectedId]['core.delete'])) {
			return array(
			    GalleryCoreApi::error(ERROR_PERMISSION_DENIED, __FILE__, __LINE__,
				"Don't have permission to delete this item"), null);
		    }

		    /* Make sure we're not deleting the root album */
		    if ($selectedId == $rootId) {
			return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
							  "Can't delete the root album"), null);
		    }
		}

		/* if we're still here then all are deletable */
		$storage =& $gallery->getStorage();
		foreach ($selectedIds as $selectedId) {
		    $ret = GalleryCoreApi::deleteEntityById($selectedId, 'GalleryItem');
		    if ($ret) {
			return array($ret, null);
		    }
		    if (!(++$status['deleted']['count'] % 20)) {
			$ret = $storage->checkPoint();
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}

		list ($ret, $success) = GalleryCoreApi::guaranteeAlbumHasThumbnail($itemId);
		if ($ret) {
		    return array($ret, null);
		}

		/* What do we do if we weren't successful?  No thumbnail, I guess */
	    }

	    /*
	     * Figure out where to redirect upon success.
	     * Can't use return url because it may be for an item we just deleted
	     */
	    $results['redirect'] = array('view' => 'core.ShowItem',
					 'itemId' => $itemId);
	} else if (isset($form['action']['next'])) {
	    $page = GalleryUtilities::getRequestVariables('page');
	    list ($ret, $peerIds) = GalleryCoreApi::fetchChildItemIdsWithPermission(
		$itemId, array('core.delete', 'core.view'));
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

	if (!empty($results['delegate'])) {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'core.ItemDelete';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for selecting items to delete
 */
class ItemDeleteView extends GalleryView {

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

	$variables = GalleryUtilities::getRequestVariables('selectedId', 'anchorId', 'page');

	foreach ($variables as $key => $value) {
	    $variables[$key] = is_numeric($value) ? (int) $value : null;
	}
	list ($selectedId, $anchorId, $page) = $variables;

	if ($form['formName'] != 'ItemDelete') {
	    if ($selectedId) {
		$form['selectedIds'][$selectedId] = true;
	    } else if ($anchorId) {
		$selectedId = $anchorId;
	    }
	    $form['destination'] = '';
	    $form['formName'] = 'ItemDelete';
	    $form['numPerPage'] = 30;
	}

	list ($ret, $peerIds) = GalleryCoreApi::fetchChildItemIdsWithPermission(
	    $itemId, array('core.delete', 'core.view'));
	if ($ret) {
	    return array($ret, null);
	}

	$peers = array();
	$peerTypes = array();
	$peerDescendentCounts = array();
	$numPages = 1;
	$selectedIds = array();
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

	    /* load peers */
	    list ($ret, $peerItems) = GalleryCoreApi::loadEntitiesById($peerIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* get peer thumbnails and resizes */
	    list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds($peerIds);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* build peers table */
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
	    if (isset($peerTypes['album'])) {
		list ($ret, $peerDescendentCounts) =
		    GalleryCoreApi::fetchDescendentCounts(array_keys($peerTypes['album']));
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}


	$urlGenerator =& $gallery->getUrlGenerator();

	$ItemDelete = array();
	$ItemDelete['canCancel'] = (boolean)GalleryUtilities::getRequestVariables('return');
	$ItemDelete['peers'] = $peers;
	$ItemDelete['peerTypes'] = $peerTypes;
	$ItemDelete['peerDescendentCounts'] = $peerDescendentCounts;
	$ItemDelete['page'] = $page;
	$ItemDelete['numPages'] = $numPages;
	$ItemDelete['numPerPage'] = $form['numPerPage'];
	$ItemDelete['selectedIds'] = array_keys($selectedIds);
	$ItemDelete['selectedIdCount'] = count($selectedIds);

	$template->setVariable('ItemDelete', $ItemDelete);
	$template->setVariable('controller', 'core.ItemDelete');
	$template->javascript('lib/yui/yahoo-dom-event.js');
	return array(null, array('body' => 'modules/core/templates/ItemDelete.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('delete an item'));
    }
}
?>
