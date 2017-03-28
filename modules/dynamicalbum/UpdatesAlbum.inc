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
 * Dynamic album view of recent additions
 * @package DynamicAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class UpdatesAlbumView extends GalleryView {

    function UpdatesAlbumView() {
	global $gallery;
	$this->_view = 'dynamicalbum.UpdatesAlbum';
	$this->_title = $gallery->i18n('Recent Updates');
	$this->_itemType = array($gallery->i18n('Updates'), $gallery->i18n('updates'));
	$this->_viewDescription = $gallery->i18n('updates');
	$this->_param = 'date';
    }

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
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'dynamicalbum');
	if ($ret) {
	    return array($ret, null, null, null);
	}
	$item->create(
	    $module->translate($this->_title),
	    array($this->_itemType, array_map(array($module, 'translate'), $this->_itemType))
	);

	list ($ret, $moduleParams) =
	    GalleryCoreApi::fetchAllPluginParameters('module', 'dynamicalbum');
	if ($ret) {
	    return array($ret, null, null, null);
	}
	$item->setDescription($moduleParams['description.' . $this->_param]);

	$itemId = GalleryUtilities::getRequestVariables('itemId');
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
	    $item->urlParams = array('view' => $this->_view, 'highlightId' => $itemId);
	    $item->getChildrenFunction = array(get_class($this), 'getChildIds');
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
	list ($itemId, $show, $albumId) =
	    GalleryUtilities::getRequestVariables('itemId', 'show', 'albumId');
	$theme =& $template->getVariableByReference('theme');
	$theme['pageUrl'] = array('view' => $this->_view);
	if (!empty($show)) {
	    $theme['pageUrl']['show'] = $show;
	}
	if (!empty($albumId)) {
	    $theme['pageUrl']['albumId'] = $albumId;
	}

	if (empty($itemId)) {
	    /* Perform query for this dynamic album */
	    list ($ret, $theme['allChildIds']) = $this->getChildIds($theme['actingUserId']);
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
     * @return array GalleryStatus a status code
     *               array of item ids
     * @static
     */
    function getChildIds($userId, $param='date', $orderBy='creationTimestamp',
	    $orderDirection=ORDER_DESCENDING, $table='GalleryEntity', $id='id') {
	global $gallery;
	$storage =& $gallery->getStorage();

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'dynamicalbum');
	if ($ret) {
	    return array($ret, null);
	}
	$size = $params['size.' . $param];
	$type = $params['type.' . $param];
	if (!$size) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	list ($show, $albumId) = GalleryUtilities::getRequestVariables('show', 'albumId');
	if (!empty($show)) {
	    $type = $show;
	}
	switch ($type) {
	case 'data':
	    $class = 'GalleryDataItem';
	    break;
	case 'all':
	    $class = 'GalleryItem';
	    break;
	case 'album':
	    $class = 'GalleryAlbumItem';
	    break;
	default:
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}
	if (!isset($table)) {
	    $table = $class;
	}

	$query = '[' . $table . '::' . $id . '] IS NOT NULL';
	if (!empty($albumId)) {
	    list ($ret, $hasPermission) = GalleryCoreApi::hasItemPermission($albumId, 'core.view');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!$hasPermission) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }

	    list ($ret, $sequence) = GalleryCoreApi::fetchParentSequence($albumId);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!empty($sequence)) {
		$sequence = implode('/', $sequence) . '/' . (int)$albumId . '/%';
		$query = '[GalleryItemAttributesMap::parentSequence] LIKE ?';
		$table = 'GalleryItemAttributesMap';
		$id = 'itemId';
	    } else {
		$query = '[' . $table . '::' . $id . '] <> ' . (int)$albumId;
	    }
	}
	if ($table == $class) {
	    $class = null;
	}
	list ($ret, $query, $data) = GalleryCoreApi::buildItemQuery(
		$table, $id, $query, $orderBy, $orderDirection,
		$class, 'core.view', false, $userId);
	if ($ret) {
	    return array($ret, null);
	}
	if (empty($query)) {
	    return array(null, array());
	}
	if (!empty($sequence)) {
	    array_unshift($data, $sequence);
	}

	list ($ret, $searchResults) = $gallery->search($query, $data,
		array('limit' => array('count' => $size)));
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
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'dynamicalbum');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate($this->_viewDescription));
    }
}
?>
