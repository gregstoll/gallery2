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
 * DHTML-enabled slideshow theme
 * @package Themes
 * @subpackage Ajaxian
 * @author Ross Shannon <ross.shannon@gmail.com>
 * @version $Revision: 18172 $
 */
class AjaxianTheme extends GalleryTheme {

    function AjaxianTheme() {
	global $gallery;

	$this->setId('ajaxian');
	$this->setName($gallery->i18n('Ajaxian'));
	$this->setDescription($gallery->i18n('DHTML-enabled Slideshow theme'));
	$this->setVersion('1.0.4');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredThemeApi(array(2, 5));
	$this->setStandardSettings(
	    array('rows' => 3, 'columns' => 8,
		  'showAlbumOwner' => 0,
		  'itemFrame' => '', 'photoFrame' => '',
		  'colorpack' => '',
		  'showMicroThumbs' => 0,
		  'sidebarBlocks' => serialize(array()),
		  'albumBlocks' => serialize(array(
		     array('core.ItemLinks', array('useDropdown' => true)))),
		  'photoBlocks' => serialize(array(
                     array('exif.ExifInfo', array())))));
    }

    /**
     * @see GalleryTheme::showAlbumPage
     */
    function showAlbumPage(&$template, $item, $params, $childIds) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params,
	    array('owner', 'viewCount', 'childCount', 'descendentCount', 'parents', 'peers',
		  'systemLinks', 'itemLinks', 'itemSummaries', 'permissions',
		  'thumbnails', 'pageNavigator', 'jumpRange'),
	     $childIds);
	if ($ret) {
	    return array($ret, null);
	}
	$ret = $this->preloadBlock($template, 'search.SearchBlock');
	if ($ret) {
	    return array($ret, null);
	}

	/* Add in our extra stuff */
	$theme =& $template->getVariableByReference('theme');

	/* From slider */
	list ($ret, $imageWidths, $imageHeights) =
	    $this->_buildItemList($childIds, $theme['children']);
	if ($ret) {
	    return array($ret, null);
	}

	$theme['imageWidths'] = implode(',', $imageWidths);
	$theme['imageHeights'] = implode(',', $imageHeights);
	$theme['imageCount'] = count($imageWidths);

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	$dataTypes = array('owner', 'parents', 'systemLinks', 'itemLinks', 'permissions',
			   'itemLinksDetailed', 'itemNavigator', 'imageViews');
	if (!empty($params['showMicroThumbs'])) {
	    $dataTypes[] = 'navThumbnails';
	}
	$ret = $this->loadCommonTemplateData($template, $item, $params, $dataTypes);
	if ($ret) {
	    return array($ret, null);
	}
	$ret = $this->preloadBlock($template, 'search.SearchBlock');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showModulePage
     */
    function showModulePage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData($template, $item, $params,
					     array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}
	$ret = $this->preloadBlock($template, 'search.SearchBlock');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showAdminPage
     */
    function showAdminPage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData($template, $item, $params,
					     array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}
	$ret = $this->preloadBlock($template, 'search.SearchBlock');
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
	$ret = $this->loadCommonTemplateData($template, $item, $params,
					     array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}
	$ret = $this->preloadBlock($template, 'search.SearchBlock');
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
	    list ($ret, $resizedImages) = GalleryCoreApi::fetchResizesByItemIds($childIds);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $preferredFullImages) = GalleryCoreApi::fetchPreferredsByItemIds($childIds);
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
	    if (!GalleryUtilities::isA($child, 'GalleryDataItem')) {
		continue;
	    }

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

	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($childId);
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $images = array();
	    if (isset($permissions['core.viewResizes']) && isset($resizedImages[$childId])) {
		foreach ($resizedImages[$childId] as $resize) {
		    $images[] = $resize;
		}
	    }
	    if (empty($images)) {
		if (isset($permissions['core.viewSource'])) {
		    /* Full size; check for preferred copy */
		    if (isset($preferredFullImages[$childId])) {
			$images[] = $preferredFullImages[$childId];
		    } else if ($child->canBeViewedInline()) {
			$images[] = $child;
		    }
		}
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
	    }
	    $children[$i]['imageIndex'] = count($imageWidths);
	    if (GalleryUtilities::isExactlyA($child, 'GalleryPhotoItem')
		    && !empty($image) && $image['width'] > 0 && $image['height'] > 0) {
		$children[$i]['image'] = $image;
		$imageWidths[] = $image['width'];
		$imageHeights[] = $image['height'];
	    } else {
		$imageWidths[] = $imageHeights[] = -1;
	    }
	}
	return array(null, $imageWidths, $imageHeights);
    }
}
?>
