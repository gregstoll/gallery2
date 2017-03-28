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
 * This controller will save many item captions at once.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @author Changpeng Zhao
 * @version $Revision: 18138 $
 */
class ItemEditCaptionsController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$page = (int)GalleryUtilities::getRequestVariables('page');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    $ids = array_keys($form['items']);

	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($ids);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* We'll check permissions one at a time below, but precache them now */
	    list ($ret, $permissions) = GalleryCoreApi::fetchPermissionsForItems($ids);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $items) = GalleryCoreApi::loadEntitiesById($ids, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $markup) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	    if ($ret) {
		return array($ret, null);
	    }

	    $status['successCount'] = 0;
	    $status['errorCount'] = 0;
	    foreach ($items as $item) {
		/* Make sure we have permission to edit this item */
		$id = $item->getId();
		if (!isset($permissions[$id]['core.view'])) {
		    /* Avoid information disclosure, act as if the item didn't exist. */
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}
		if (isset($permissions[$id]['core.edit'])) {
		    if ($item->getSerialNumber() == $form['items'][$id]['serialNumber']) {
			if ($markup == 'html') {
			    /* Strip malicious content if html markup allowed */
			    $form['items'][$id]['title'] =
				GalleryUtilities::htmlSafe($form['items'][$id]['title'], true);
			    $form['items'][$id]['summary'] =
				GalleryUtilities::htmlSafe($form['items'][$id]['summary'], true);
			    $form['items'][$id]['description'] =
			      GalleryUtilities::htmlSafe($form['items'][$id]['description'], true);
			}

			$item->setTitle($form['items'][$id]['title']);
			$item->setSummary($form['items'][$id]['summary']);
			$item->setKeywords($form['items'][$id]['keywords']);
			$item->setDescription($form['items'][$id]['description']);

			$ret = $item->save();
			if ($ret) {
			    return array($ret, null);
			}
			$status[$id]['saved'] = 1;
			$status['successCount']++;
		    } else {
			$status[$id]['obsolete'] = 1;
			$status['errorCount']++;
		    }
		} else {
		    $status[$id]['permissionDenied'] = 1;
		    $status['errorCount']++;
		}
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null);
	    }

	    /*
	     * Figure out where to redirect.  We always redirect even if we have
	     * an error since we may have saved some items, but not others.
	     */
	    $redirect['view'] = 'core.ItemAdmin';
	    $redirect['subView'] = 'core.ItemEditCaptions';
	    $redirect['itemId'] = (int)$itemId;
	    if (!$status['errorCount'] && isset($form['action']['save']['next'])) {
		$redirect['page'] = $page + 1;
	    } else if (!$status['errorCount'] && isset($form['action']['save']['previous'])) {
		$redirect['page'] = max($page-1, 0);
	    } else if (!$status['errorCount'] && isset($form['action']['save']['done'])) {
		$results['return'] = 1;
		$redirect['page'] = (int)$page;
	    } else {
		$redirect['page'] = (int)$page;
	    }
	} else if (isset($form['action']['cancel'])) {
	    $results['return'] = 1;
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'core.ItemEditCaptions';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will allow the user to edit many item captions at once.
 */
class ItemEditCaptionsView extends GalleryView {

	/**
     * Returns sizes for "maxlength" in forms
     * @todo: this function is duplicated in ItemEdit.inc, so it needs to be consolidated somehow
     * @param integer $value is the value from 
     * @return size for "maxlength" in form
     * @access private
     */
    function _getSizesForMaxlength($value) {
	switch ($value) {
	case STORAGE_SIZE_SMALL:
	    $size = 32;
	    break;

	case STORAGE_SIZE_MEDIUM:
	    $size = 128;
	    break;

	case STORAGE_SIZE_LARGE:
	    $size = 255;
	    break;
	}
	
	return $size;
    }
	
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($page, $selectedId, $albumPage) =
	    GalleryUtilities::getRequestVariables('page', 'selectedId', 'albumPage');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	if ($form['formName'] != 'ItemEditCaption') {
	    $form['formName'] = 'ItemEditCaption';
	    $form['numPerPage'] = 9;

	    list ($ret, $childIds) = GalleryCoreApi::fetchChildItemIdsWithPermission(
		$itemId, array('core.edit', 'core.view'));
	    if ($ret) {
		return array($ret, null);
	    }

	    $form['childItems'] = array();
	    $numPages = 1;
	    $numPages = ceil(sizeof($childIds) / $form['numPerPage']);
	    if (empty($page) && !empty($selectedId)) {
		/* No page given.  Determine which page we're on from the selectedId */
		for ($i = 0; $i < count($childIds); $i++) {
		    if ($childIds[$i] == $selectedId) {
			$page = ceil(($i + 1) / $form['numPerPage']);
		    }
		}
	    }
	    if (empty($page) && !empty($albumPage)) {
		/* Still no page.  Determine which page we're on from albumPage */
		list ($ret, $theme) = $this->loadThemeForItem($item);
		if ($ret) {
		    return array($ret, null);
		}
		list ($ret, $params) = $theme->fetchParameters($itemId);
		if ($ret) {
		    return array($ret, null);
		}
		$albumPageSize = $theme->getPageSize($params);
		if (!empty($albumPageSize)) {
		    $page = ceil((($albumPage - 1) * $albumPageSize + 1) / $form['numPerPage']);
		}
	    }
	    if (empty($page)) {
		$page = 1;
	    }

	    $start = $form['numPerPage'] * ($page - 1);
	    $childIds = array_slice($childIds, $start, $form['numPerPage']);

	    list ($ret, $childItems) = GalleryCoreApi::loadEntitiesById($childIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Get child thumbnails and resizes */
	    list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds($childIds);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* build peers table */
	    foreach ($childItems as $child) {
		$childId = $child->getId();
		$form['items'][$childId] = (array)$child;

		/* While we're at it, attach thumbnails and resizes */
		if (isset($derivatives[$childId])) {
		    foreach ($derivatives[$childId] as $derivative) {
			$type = $derivative->getDerivativeType();
			if (empty($form['items'][$childId]['resize']) &&
			        $type == DERIVATIVE_TYPE_IMAGE_RESIZE) {
			    $form['items'][$childId]['resize'] = (array)$derivative;
			} else if ($type == DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
			    $form['items'][$childId]['thumbnail'] = (array)$derivative;
			}
		    }
		}
	    }
	}

	$urlGenerator =& $gallery->getUrlGenerator();

	$ItemEditCaptions = array();
	$ItemEditCaptions['canCancel'] = (boolean)GalleryUtilities::getRequestVariables('return');
	$ItemEditCaptions['page'] = $page;
	$ItemEditCaptions['numPages'] = $numPages;

	list ($ret, $entityInfo) = GalleryCoreApi::describeEntity('GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}

	$ItemEditCaptions['fieldLengths'] = array();
	$ItemEditCaptions['fieldLengths']['title'] = 
	    $this->_getSizesForMaxLength($entityInfo['GalleryItem']['members']['title']['size']);

	$template->setVariable('ItemEditCaptions', $ItemEditCaptions);
	$template->setVariable('controller', 'core.ItemEditCaptions');
	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('lib/yui/container-min.js');
	return array(null, array('body' => 'modules/core/templates/ItemEditCaptions.tpl'));

    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('edit captions'));
    }
}
?>
