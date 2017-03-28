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

GalleryCoreApi::requireOnce('modules/search/classes/SearchUtilities.class');

/**
 * Perform a search
 * @package Search
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class SearchScanController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$results = array();
	if (isset($form['action']['slideshow'])) {

	    if (isset($form['searchCriteria']) && is_string($form['searchCriteria'])) {
		$form['searchCriteria'] = trim($form['searchCriteria']);
	    } else {
		$form['searchCriteria'] = '';
	    }

	    list ($ret, $slideshow) =
		GalleryCoreApi::newFactoryInstance('SlideshowInterface_1_0');
	    if ($ret || !isset($slideshow)) {
		return array($ret, null);
	    }

	    GalleryCoreApi::requireOnce(
		'modules/search/classes/GallerySearchInterface_1_0.class');
	    list ($ret, $ids) =
		GalleryCoreApi::getAllFactoryImplementationIds('GallerySearchInterface_1_0');
	    if ($ret) {
		return array($ret, null);
	    }

	    foreach ($ids as $id => $className) {
		list ($ret, $searchInstances[$id]) =
		    GalleryCoreApi::newFactoryInstance('GallerySearchInterface_1_0', $className);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $itemIds = array();
	    foreach ($searchInstances as $id => $instance) {
		if (isset($form['options'][$id])) {
		    list ($ret, $resultSet) =
			$instance->search($form['options'][$id],
			    SearchUtilities::sanitizeSearchCriteria($form['searchCriteria']));
		    if ($ret) {
			return array($ret, null);
		    }
		    foreach ($resultSet['results'] as $result) {
			if (isset($result['itemId'])) {
			    $itemIds[$result['itemId']] = 1;
			}
		    }
		}
	    }
	    $itemIds = array_keys($itemIds);

	    list ($ret, $results['delegate']) = $slideshow->getLinkData($itemIds);
	    if ($ret) {
		return array($ret, null);
	    }
	} else if (isset($form['action']['showAll'])) {
	    $results['delegate']['view'] = 'search.SearchShowAll';
	} else {
	    $results['delegate']['view'] = 'search.SearchScan';
	}

	$results['status'] = array();
	$results['error'] = array();
	return array(null, $results);
    }
}

/**
 * Perform a search
 */
class SearchScanView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	/* TODO: Parameterize this */
	$scanSize = 3;

	/* Find all the modules that support the Search interface */
	GalleryCoreApi::requireOnce(
	    'modules/search/classes/GallerySearchInterface_1_0.class');
	list ($ret, $ids) =
	    GalleryCoreApi::getAllFactoryImplementationIds('GallerySearchInterface_1_0');
	if ($ret) {
	    return array($ret, null);
	}

	foreach ($ids as $id => $className) {
	    list ($ret, $searchInstances[$id]) =
		GalleryCoreApi::newFactoryInstance('GallerySearchInterface_1_0', $className);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Load the search info for each module */
	foreach ($searchInstances as $id => $instance) {
	    list ($ret, $modules[$id]) = $instance->getSearchModuleInfo();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$items = array();
	$thumbnails = array();
	$searchResults = array();
	$searchResultThumbnails = array();
	if ($form['formName'] == 'SearchScan' ||
	    $form['formName'] == 'SearchShowAll' ||
	    $form['formName'] == 'search_SearchBlock') {

	    if (isset($form['searchCriteria']) && is_string($form['searchCriteria'])) {
		$form['searchCriteria'] = trim($form['searchCriteria']);
	    } else {
		 $form['searchCriteria'] = '';
	    }

	    if (empty($form['searchCriteria'])) {
		$form['error']['searchCriteria']['missing'] = 1;
	    } else {

		if (isset($form['useDefaultSettings']) || $form['formName'] == 'SearchShowAll') {
		    foreach ($searchInstances as $id => $instance) {
			$form['options'][$id] = $modules[$id]['options'];
		    }
		}

		/* Execute a "scan" search */
		foreach ($searchInstances as $id => $instance) {
		    if (isset($form['options'][$id])) {
			list ($ret, $searchResults[$id]) =
			    $instance->search($form['options'][$id],
				SearchUtilities::sanitizeSearchCriteria($form['searchCriteria']),
				0, $scanSize);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}
	    }

	    /* Pull out all the item ids and save them */
	    $itemIds = array();
	    foreach ($searchResults as $id => $resultSet) {
		foreach ($resultSet['results'] as $result) {
		    if (isset($result['itemId'])) {
			$itemIds[$result['itemId']] = 1;
		    }
		}
	    }
	    $itemIds = array_keys($itemIds);

	    $thumbnails = array();
	    if (!empty($itemIds)) {

		/* Fetch items */
		list ($ret, $itemList) = GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryItem');
		if ($ret) {
		    return array($ret, null);
		}
		foreach ($itemList as $item) {
		    $items[$item->getId()] = (array)$item;
		}

		/* Fetch thumbnails */
		list ($ret, $thumbnailList) =
		    GalleryCoreApi::fetchThumbnailsByItemIds($itemIds);
		if ($ret) {
		    return array($ret, null);
		}
		foreach ($thumbnailList as $thumbnail) {
		    $thumbnails[$thumbnail->getParentId()] = (array)$thumbnail;
		}
	    }
	} else {
	    $form['searchCriteria'] = '';
	    foreach ($searchInstances as $id => $instance) {
		$form['options'][$id] = $modules[$id]['options'];
	    }
	}

	list ($ret, $ids) =
	    GalleryCoreApi::getAllFactoryImplementationIds('SlideshowInterface_1_0');
	if ($ret) {
	    return array($ret, null);
	}
	$slideshowAvailable = !empty($ids);

	/* Render the HTML body */
	$template->setVariable('SearchScan', array(
	    'controller' => 'search.SearchScan',
	    'modules' => $modules,
	    'searchResults' => $searchResults,
	    'thumbnails' => $thumbnails,
	    'items' => $items,
	    'slideshowAvailable' => $slideshowAvailable));

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'search');
	if ($ret) {
	    return array($ret, null);
	}

	$template->javascript('modules/search/HighlightResults.js');
	$template->title($module->translate('Gallery Search'));
	return array(null, array('body' => 'modules/search/templates/SearchScan.tpl'));
    }
}
?>
