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
 * Theme combining thumb-view, image-view and slideshow into a single dynamic page
 * @package Themes
 * @subpackage Hybrid
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20936 $
 */
class HybridTheme extends GalleryTheme {

    function HybridTheme() {
	global $gallery;

	$this->setId('hybrid');
	$this->setName($gallery->i18n('Hybrid'));
	$this->setDescription($gallery->i18n('Combined album/image/slideshow dynamic view'));
	$this->setVersion('1.1.10');
	$this->setRequiredCoreApi(array(7, 36));
	$this->setRequiredThemeApi(array(2, 5));
	$this->setStandardSettings(array(
	    'rows' => 30, 'columns' => 2,
	    'albumFrame' => '', 'itemFrame' => '', 'colorpack' => '',
	    'sidebarBlocks' => serialize(
		array(array('search.SearchBlock', array('showAdvancedLink' => true)),
		      array('core.ItemLinks', array('useDropdown' => false)),
		      array('core.PeerList', array()),
		      array('imageblock.ImageBlock', array()))),
	    'albumBlocks' => serialize(
		array(array('comment.ViewComments', array()))),
	    'photoBlocks' => serialize(
		array(array('exif.ExifInfo', array()),
		      array('comment.ViewComments', array()))),
	    'showText' => 1, 'showAlbumDate' => 1, 'showImageDate' => 1, 'showDateInViewer' => 1,
	    'showSize' => 1, 'showViewCount' => 1, 'showAlbumOwner' => 0, 'showImageOwner' => 0,
	    'thumbTitle' => 'none',
	));
    }

    /**
     * @see GalleryTheme::getSettings
     */
    function getSettings($itemId=null) {
	list ($ret, $settings, $params) = parent::getSettings($itemId);
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Add in our custom settings */
	$settings[] = array('key' => 'showText',
			    'name' => $this->translate('Show text with thumbnails'),
			    'type' => 'checkbox',
			    'value' => $params['showText']);
	$settings[] = array('key' => 'showAlbumDate',
			    'name' => $this->translate('Show dates for albums'),
			    'type' => 'checkbox',
			    'value' => $params['showAlbumDate']);
	$settings[] = array('key' => 'showImageDate',
			    'name' => $this->translate('Show dates with thumbnails'),
			    'type' => 'checkbox',
			    'value' => $params['showImageDate']);
	$settings[] = array('key' => 'showDateInViewer',
			    'name' => $this->translate('Show dates in image view'),
			    'type' => 'checkbox',
			    'value' => $params['showDateInViewer']);
	$settings[] = array('key' => 'showSize',
			    'name' => $this->translate('Show album sizes'),
			    'type' => 'checkbox',
			    'value' => $params['showSize']);
	$settings[] = array('key' => 'showViewCount',
			    'name' => $this->translate('Show view counts'),
			    'type' => 'checkbox',
			    'value' => $params['showViewCount']);
	$settings[] = array('key' => 'thumbTitle',
			    'name' => $this->translate('Mouseover on thumbnails'),
			    'type' => 'single-select',
			    'choices' => array(
				'none' => $this->translate('None'),
				'title' => $this->translate('Title'),
				'date' => $this->translate('Date'),
				'datetime' => $this->translate('Date/Time'),
				'titledate' => $this->translate('Title (Date)'),
				'titledatetime' => $this->translate('Title (Date/Time)')),
			    'value' => $params['thumbTitle']);

	return array(null, $settings, $params);
    }

    /**
     * @see GalleryTheme::showAlbumPage
     */
    function showAlbumPage(&$template, $item, $params, $childIds) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$data = array('parents', 'systemLinks', 'itemLinks', 'childCount',
		      'itemSummaries', 'childItemLinksDetailed', 'permissions', 'jumpRange');
	if ($params['showSize']) {
	    $data[] = 'descendentCount';
	}
	if ($params['showViewCount']) {
	    $data[] = 'viewCount';
	}
	if ($params['showAlbumOwner'] || $params['showImageOwner']) {
	    $data[] = 'owner';
	}
	$ret = $this->loadCommonTemplateData($template, $item, $params, $data, $childIds);
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');

	list ($ret, $imageWidths, $imageHeights) =
	    $this->_buildItemList($childIds, $theme['children']);
	if ($ret) {
	    return array($ret, null);
	}

	$theme['imageWidths'] = implode(',', $imageWidths);
	$theme['imageHeights'] = implode(',', $imageHeights);
	$theme['imageCount'] = count($imageWidths);
	$theme['columnWidthPct'] = floor(90 / $params['columns']);
	$theme['cookiePath'] = preg_replace('#^.*?://.*?(/|$)#', '/',
					    $urlGenerator->getCurrentUrlDir());

	list ($showId, $show, $slide) =
	    GalleryUtilities::getRequestVariables('showId', 'show', 'slide');
	if ($show == 'first' && !empty($imageWidths)) {
	    $viewIndex = 0;
	}
	else if ($show == 'last' && !empty($imageWidths)) {
	    $viewIndex = count($imageWidths) - 1;
	}
	else if (!empty($showId)) {
	    /* Start with initial view of this item */
	    foreach ($theme['children'] as $tmp) {
		if ($tmp['id'] == $showId) {
		    if (isset($tmp['imageIndex'])) {
			$viewIndex = $tmp['imageIndex'];
		    }
		    break;
		}
	    }
	}
	if (isset($viewIndex)) {
	    $theme['viewIndex'] = $viewIndex;
	    /* Set cookie to popup image just once */
	    setcookie('G2_hybrid_view', $slide ? '2' : '1');
	}

	$template->head('themes/hybrid/templates/header.tpl');
	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	list ($detail, $renderId) = GalleryUtilities::getRequestVariables('detail', 'renderId');
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

	if (!empty($detail)) {
	    /* View item details */
	    $ret = $this->loadCommonTemplateData($template, $item, $params,
						 array('owner', 'viewCount'));
	    if ($ret) {
		return array($ret, null);
	    }
	    $theme =& $template->getVariableByReference('theme');
	    list ($ret, $thumbnail) =
		GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!empty($thumbnail)) {
		$theme['thumbnail'] = (array)$thumbnail[$item->getId()];
	    }
	    $template->style('themes/hybrid/theme.css');
	    return array(null, 'detail.tpl');
	}

	/* Otherwise, redirect to parent with initial view of this item  */
	list ($ret, $parent) =
	    GalleryCoreApi::loadEntitiesById($item->getParentId(), 'GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');
	list ($ret, $childIds) =
	    GalleryCoreApi::fetchChildItemIds($parent, null, null, $theme['actingUserId']);
	if ($ret) {
	    return array($ret, null);
	}
	$perPage = $this->getPageSize($params);
	$page = 1;
	for ($i = 0; $i < count($childIds); $i++) {
	    if ($childIds[$i] == $item->getId()) {
		$page = ceil(($i + 1) / $perPage);
		break;
	    }
	}
	$redirect = array('view' => 'core.ShowItem', 'itemId' => $parent->getId());
	if ($page != 1) {
	    $redirect['page'] = $page;
	}
	$redirect['showId'] = $item->getId();
	return array(null, array('redirect' => $redirect));
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

    /**
     * Prepare data about items in this album.
     * @access private
     */
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
	    list ($ret, $preferredFullImages) =
		GalleryCoreApi::fetchPreferredsByItemIds($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $resizedImages) =
		GalleryCoreApi::fetchResizesByItemIds($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $thumbnails) =
		GalleryCoreApi::fetchThumbnailsByItemIds($childIds);
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

	    if (!GalleryUtilities::isA($child, 'GalleryDataItem')) {
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
		} else if ($child->canBeViewedInline()) {
		    $images[] = $child;
		}
	    }
	    if (isset($permissions['core.viewResizes']) && isset($resizedImages[$childId])) {
		foreach ($resizedImages[$childId] as $resize) {
		    $images[] = $resize;
		}
	    }
	    if (!isset($permissions['core.viewSource']) && isset($thumbnails[$childId])) {
		/* Only use thumbnail as view if we can't link to original file */
		$images[] = $thumbnails[$childId];
	    }

	    if (!empty($images)) {
		$image = $images[0];

		/* Rebuild derivative if needed so width/height known.. */
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
}
?>
