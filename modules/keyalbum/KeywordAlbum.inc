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
 * Dynamic album view using a query on keywords
 * @package KeyAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class KeywordAlbumView extends GalleryView {

    /**
     * @see GalleryView::getViewType
     */
    function getViewType() {
	return VIEW_TYPE_SHOW_ITEM;
    }

    /**
     * @see GalleryView::loadThemeAndParameters
     */
    function loadThemeAndParameters() {
	list ($ret, $item) = GalleryCoreApi::newFactoryInstance('GalleryDynamicAlbum');
	if ($ret) {
	    return array($ret, null, null, null);
	}
	if (!isset($item)) {
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null, null, null);
	}
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'keyalbum');
	if ($ret) {
	    return array($ret, null, null, null);
	}
	list ($keyword, $itemId) = GalleryUtilities::getRequestVariables('keyword', 'itemId');
	$item->create(
	    $module->translate(array('text' => 'Keyword Album: %s', 'arg1' => $keyword)),
	    array(array('Keyword Album', 'keyword album'),
		  array($module->translate('Keyword Album'), $module->translate('keyword album')))
	);

	list ($ret, $moduleParams) =
	    GalleryCoreApi::fetchAllPluginParameters('module', 'keyalbum');
	if ($ret) {
	    return array($ret, null, null, null);
	}
	$item->setDescription($moduleParams['description']);

	if (!empty($itemId)) {
	    /* Viewing an item in this dynamic album */
	    list ($ret, $viewItem) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    list ($ret, $hasPermission) = GalleryCoreApi::hasItemPermission($itemId, 'core.view');
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    if (!$hasPermission) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null, null, null);
	    }

	    /* Provide parent, parent URL and get-children function to Theme API */
	    $item->urlParams = array('view' => 'keyalbum.KeywordAlbum', 'keyword' => $keyword,
				     'highlightId' => $itemId);
	    $item->getChildrenFunction = array('KeywordAlbumView', 'getChildIds');
	    $viewItem->parent = $item;
	    $item = $viewItem;
	}

	if (empty($moduleParams['themeId'])) {
	    list ($ret, $theme) = $this->loadThemeForItem();
	    if ($ret || !isset($theme)) {
		/* Ignore errors here so fallback theme can be used */
		return array(null, null, array(), $item);
	    }
	} else {
	    list ($ret, $theme) = GalleryView::_loadTheme($moduleParams['themeId']);
	    if ($ret || !isset($theme)) {
		/* Ignore errors here so fallback theme can be used */
		return array(null, null, array(), $item);
	    }
	}

	list ($ret, $params) = $theme->fetchParameters($moduleParams['themeSettingsId']);
	if ($ret) {
	    return array($ret, null, null, null);
	}

	return array(null, $theme, $params, $item);
    }

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$theme =& $template->getVariableByReference('theme');

	list ($keyword, $itemId) = GalleryUtilities::getRequestVariables('keyword', 'itemId');
	$rawKeyword = $keyword;
	GalleryUtilities::unsanitizeInputValues($rawKeyword, false);
	$theme['pageUrl'] = array('view' => 'keyalbum.KeywordAlbum', 'keyword' => $rawKeyword);

	if (empty($itemId)) {
	    /* Perform query for this dynamic album */
	    list ($ret, $theme['allChildIds']) =
		$this->getChildIds($theme['actingUserId'], $keyword);
	    if ($ret) {
		return array($ret, null);
	    }
	} else {
	    /* Item in dynamic album; use core.ShowItem to check permission, increment view count */
	    list ($ret, $showItem) = GalleryView::loadView('core.ShowItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $result) = $showItem->loadTemplate($template, $form);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (isset($result['redirect'])) {
		return array(null, $result);
	    }
	}

	return array(null, array());
    }

    /**
     * Dynamic query for items
     * @param int $userId
     * @param string $keyword (optional) keyword for query; get from request if not specified
     * @return array GalleryStatus a status code
     *               array of item ids
     * @static
     */
    function getChildIds($userId, $keyword=null) {
	global $gallery;
	$storage =& $gallery->getStorage();

	if (!isset($keyword)) {
	    $keyword = GalleryUtilities::getRequestVariables('keyword');
	}
	if (empty($keyword)) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'keyalbum');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'keyalbum');
	if ($ret) {
	    return array($ret, null);
	}

	$keywords = $where = array();
	foreach ($module->splitKeywords($keyword, $params['split']) as $k) {
	    $keywords[] = '%' . $k . '%';
	    $where[] = '[GalleryItem::keywords] LIKE ?';
	}

	list ($ret, $query, $data) = GalleryCoreApi::buildItemQuery(
		'GalleryItem', 'id', implode(' AND ', $where),
		$params['orderBy'], $params['orderDirection'], null, 'core.view', false, $userId);
	if ($ret) {
	    return array($ret, null);
	}
	if (empty($query)) {
	    return array(null, array());
	}

	list ($ret, $searchResults) = $gallery->search($query, array_merge($keywords, $data));
	if ($ret) {
	    return array($ret, null);
	}
	$itemIds = array();
	while ($result = $searchResults->nextResult()) {
	    $itemIds[] = $result[0];
	}

	return array(null, $itemIds);
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'keyalbum');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate('keyword album'));
    }
}
?>
