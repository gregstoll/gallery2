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
 * Show sample of all image frames
 * @package ImageFrame
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class SampleView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	list ($ret, $imageframe) =
	    GalleryCoreApi::newFactoryInstance('ImageFrameInterface_1_1');
	if ($ret) {
	    return array($ret, null);
	}
	if (!isset($imageframe)) {
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}

	list ($ret, $list) = $imageframe->getImageFrameList();
	if ($ret) {
	    return array($ret, null);
	}
	$ret = $imageframe->init($template, array_keys($list));
	if ($ret) {
	    return array($ret, null);
	}

	$ImageFrameSample = array('list' => $list);
	$itemId = GalleryUtilities::getRequestVariables('itemId');
	if (!empty($itemId)) {
	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $thumbnail) = GalleryCoreApi::fetchThumbnailsByItemIds(array($itemId));
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $canSee) = GalleryCoreApi::hasItemPermission($itemId, 'core.view');
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($canSee && isset($thumbnail[$itemId])) {
		$ImageFrameSample['item'] = (array)$item;
		$ImageFrameSample['thumb'] = (array)$thumbnail[$itemId];
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'imageframe');
	if ($ret) {
	    return array($ret, null);
	}

	$template->setVariable('ImageFrameSample', $ImageFrameSample);
	$template->title($module->translate('Image Frame Samples'));
	return array(null, array('body' => 'modules/imageframe/templates/SampleBody.tpl',
				 'useFullScreen' => true));
    }

    /**
     * @see GalleryView::loadThemeAndParameters
     */
    function loadThemeAndParameters() {
	list ($ret, $theme, $params, $item) = parent::loadThemeAndParameters();
	if ($ret) {
	    if (!($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		return array($ret, null, null, null);
	    }
	    /* Fallback to default theme if given invalid itemId */
	    GalleryUtilities::removeRequestVariable('itemId');
	    list ($ret, $theme, $params, $item) = parent::loadThemeAndParameters();
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	}
	return array(null, $theme, $params, $item);
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'imageframe');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate('Image Frame Samples'));
    }
}
?>
