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
 * Theme with a tiled background image and image thumbnails on top of it.
 * @package Themes
 * @subpackage Tile
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class TileTheme extends GalleryTheme {

    function TileTheme() {
	global $gallery;

	$this->setId('tile');
	$this->setName($gallery->i18n('Tile'));
	$this->setDescription($gallery->i18n('Tile view of background image and image thumbnails'));
	$this->setVersion('1.0.10');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredThemeApi(array(2, 5));
	$this->setStandardSettings(array('colorpack' => '',
	    'sidebarBlocks' => serialize(array(
		array('search.SearchBlock', array('showAdvancedLink' => true)),
		array('core.ItemLinks', array('useDropdown' => false)),
		array('core.PeerList', array()),
		array('imageblock.ImageBlock', array())))));
    }

    function _setDefaultParameters(&$params) {
	foreach (array('backgroundId', 'rows', 'cols', 'cellWidth', 'cellHeight') as $key) {
	    if (!isset($params[$key])) {
		$params[$key] = '0';
	    }
	}
    }

    /**
     * @see GalleryTheme::showAlbumPage
     */
    function showAlbumPage(&$template, $item, $params, $childIds, $showHead=true) {
	$this->_setDefaultParameters($params);
	$ret = $this->loadCommonTemplateData($template, $item, $params,
	    array('parents', 'systemLinks', 'itemLinks'),
	    $childIds);
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');

	if (!empty($params['backgroundId'])) {
	    /* XXX: Use an interface for images instead of 'GalleryEntity' */
	    list ($ret, $bgImage) =
		GalleryCoreApi::loadEntitiesById($params['backgroundId'], 'GalleryEntity');
	    if ($ret) {
		return array($ret, null);
	    }
	    $params['bgSerialNumber'] = $bgImage->getSerialNumber();
	}

	$map = array();
	for ($i = 0; $i < $params['rows']; $i++) {
	    for ($j = 0; $j < $params['cols']; $j++) {
		$map[$i][$j] = 0;
	    }
	}

	if (!empty($params['map'])) {
	    foreach (explode(';', $params['map']) as $key) {
		$value = explode(',', $key);
		if (count($value) == 3) {
		    $map[$value[0]][$value[1]] = $value[2];
		    $params['row_' . $value[2]] = $value[0] + 1;
		    $params['col_' . $value[2]] = $value[1] + 1;
		}
	    }
	}
	$theme['map'] = $map;

	list ($ret, $imageWidths, $imageHeights) =
	    $this->_buildItemList($childIds, $theme['children']);
	if ($ret) {
	    return array($ret, null);
	}
	$itemMap = array();
	foreach ($theme['children'] as $child) {
	    $itemMap[$child['id']] = $child;
	}

	$theme['itemMap'] = $itemMap;
	$theme['imageWidths'] = implode(',', $imageWidths);
	$theme['imageHeights'] = implode(',', $imageHeights);
	$theme['imageCount'] = count($imageWidths);
	$theme['param'] = $params;

	if ($showHead) {
	    $template->head('themes/tile/templates/header.tpl');
	}
	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	$renderId = GalleryUtilities::getRequestVariables('renderId');
	if (!empty($renderId)) {
	    /* Render this item */
	    list ($ret, $image) = GalleryCoreApi::loadEntitiesById(
		    $renderId, array('GalleryDataItem', 'GalleryDerivative'));
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($renderId != ($itemId = $item->getId()) && $image->getParentId() != $itemId) {
		/* Malicious URL.. renderId not for this item */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }
	    $template->setVariable('theme', array('item' => (array)$item,
						  'image' => (array)$image));
	    return array(null, 'render.tpl');
	}

	/* Otherwise, load the parent with initial view of this item  */
	list ($ret, $parent) =
	    GalleryCoreApi::loadEntitiesById($item->getParentId(), 'GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $childIds) = GalleryCoreApi::fetchChildItemIds($parent);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $tplFile) = $this->showAlbumPage($template, $parent, $params, $childIds);
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');
	foreach ($theme['children'] as $tmp) {
	    if ($tmp['id'] == $item->getId()) {
		if (isset($tmp['imageIndex'])) {
		    $theme['viewIndex'] = $tmp['imageIndex'];
		    /* Set cookie to popup image just once */
		    setcookie('G2_tile_view', '1');
		}
		break;
	    }
	}

	return array(null, $tplFile);
    }

    /**
     * @see GalleryTheme::showModulePage
     */
    function showModulePage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showAdminPage
     */
    function showAdminPage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showErrorPage
     */
    function showErrorPage(&$template) {
	return array(null, 'error.tpl');
    }

    /**
     * @see GalleryTheme::showProgressBarPage
     */
    function showProgressBarPage(&$template, $item, $params) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'theme.tpl');
    }

    function _buildItemList($childIds, &$children) {
	$imageWidths = $imageHeights = $childItems = array();
	if (!empty($childIds)) {
	    $ret = GalleryCoreApi::studyPermissions($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $childItems) = GalleryCoreApi::loadEntitiesById($childIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $preferredFullImages) = GalleryCoreApi::fetchPreferredsByItemIds($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $resizedImages) = GalleryCoreApi::fetchResizesByItemIds($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}
	$i = -1;
	foreach ($childItems as $child) {
	    $i++;
	    $childId = $child->getId();
	    if (isset($thumbnails[$childId])) {
		if (!($thumbnails[$childId]->getWidth() && $thumbnails[$childId]->getHeight())) {
		    list ($ret, $thumbnails[$childId]) =
			GalleryCoreApi::rebuildDerivativeCache($thumbnails[$childId]->getId());
		    if ($ret) {
			return array($ret, null, null);
		    }
		}
		$children[$i]['thumbnail'] = (array)$thumbnails[$childId];
	    }

	    if (!GalleryUtilities::isA($child, 'GalleryDataItem') || !$child->canBeViewedInline()) {
		continue;
	    }

	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($childId);
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $images = array();
	    if (isset($permissions['core.viewSource'])) {
		/* Full size; check for preferred copy */
		if (isset($preferredFullImages[$childId])) {
		    $images[] = $preferredFullImages[$childId];
		} else {
		    $images[] = $child;
		}
	    }
	    if (isset($permissions['core.viewResizes']) && isset($resizedImages[$childId])) {
		foreach ($resizedImages[$childId] as $resize) {
		    $images[] = $resize;
		}
	    }
	    if (isset($thumbnails[$childId])) {
		$images[] = $thumbnails[$childId];
	    }

	    if (!empty($images)) {
		$image = $images[0];

		/* Rebuild derivative if needed so width/height known */
		if (GalleryUtilities::isA($image, 'GalleryDerivativeImage')
			&& !($image->getWidth() && $image->getHeight())) {
		    list ($ret, $image) = GalleryCoreApi::rebuildDerivativeCache($image->getId());
		    if ($ret) {
			return array($ret, null, null);
		    }
		}
		$image = (array)$image;
		$children[$i]['image'] = $image;
		$children[$i]['imageIndex'] = count($imageWidths);
		if (GalleryUtilities::isExactlyA($child, 'GalleryPhotoItem') &&
			$image['width'] > 0 && $image['height'] > 0) {
		    /* Display in <img> */
		    $imageWidths[] = $image['width'];
		    $imageHeights[] = $image['height'];
		} else {
		    /* Item must render itself */
		    $children[$i]['renderItem'] = 1;
		    $imageWidths[] = $imageHeights[] = -1;
		}
	    }
	}
	return array(null, $imageWidths, $imageHeights);
    }

    /**
     * @see GalleryTheme::getPageSize
     */
    function getPageSize($params) {
	/* This theme may not be configured yet... */
	if (isset($params['rows']) && isset($params['cols'])) {
	    return $params['rows'] * $params['cols'];
	}

	return 0;
    }

    /**
     * @see GalleryTheme::isAdvancedSettings
     */
    function isAdvancedSettings() {
	return true;
    }

    /**
     * @see GalleryTheme::loadSettingsTemplate
     */
    function loadSettingsTemplate(&$template, &$form, $itemId=null) {
	global $gallery;
	if (!isset($itemId)) {
	    /* Site admin settings -- we have none. */
	    return array(null, null);
	}

	/*
	 * Our settings template needs everything that an album render requires
	 * so load enough information to call showAlbumPage.
	 */
	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}
	if (!GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    /* Can't configure for dynamic album */
	    return array(null, null);
	}
	list ($ret, $params) = $this->fetchParameters($itemId);
	if ($ret) {
	    return array($ret, null);
	}
	$this->_setDefaultParameters($params);

	$theme =& $template->getVariableByReference('theme');
	$theme['param'] = $params;
	$theme['actingUserId'] = $gallery->getActiveUserId();

	list ($ret, $childIds) = GalleryCoreApi::fetchChildItemIds($item);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $tmp) = $this->showAlbumPage($template, $item, $params, $childIds, false);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'themes/tile/templates/edit.tpl');
    }

    /**
     * @see GalleryTheme::handleSettingsRequest
     */
    function handleSettingsRequest($form, $itemId=null) {
	if (!isset($itemId)) {
	    /* No site admin settings */
	    return array(null, array(), '');
	}
	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	if ($ret) {
	    return array($ret, null, null);
	}

	$status = null;
	$error = array();
	if (isset($form['action']['save'])) {
	    foreach (array('backgroundId', 'rows', 'cols', 'cellWidth', 'cellHeight') as $key) {
		if (isset($form[$key])) {
		    $ret = GalleryCoreApi::setPluginParameter('theme', 'tile', $key, $form[$key],
							      $item->getId());
		    if ($ret) {
			return array($ret, null, null);
		    }
		}
	    }
	    $map = array();
	    foreach ($form as $key => $value) {
		if (!strncmp($key, 'row_', 4)) {
		    $id = substr($key, 4);
		    $k2 = 'col_' . $id;
		    if (isset($form[$k2]) && $value > 0 && $form[$k2] > 0) {
			$map[] = ($value - 1) . ',' . ($form[$k2] - 1) . ',' . $id;
		    }
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter('theme', 'tile', 'map', implode(';', $map),
						      $item->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $status = $this->translate('Successfully saved theme settings');
	} /* else $form['action']['undo'] */

	return array(null, $error, $status);
    }
}
?>
