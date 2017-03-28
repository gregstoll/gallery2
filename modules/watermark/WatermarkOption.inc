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
 * This ItemAddOption allows the user to pick a watermark to apply to the image upon upload.
 * @package Watermark
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class WatermarkOption extends ItemAddOption {

    /**
     * @see ItemAddOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'watermark');
	if ($ret) {
	    return array($ret, null, null);
	}

	$WatermarkOption = array();
	if (!$params['forceDefaultWatermark'] || empty($params['defaultWatermarkId'])) {
	    GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');
	    list ($ret, $watermarks) = WatermarkHelper::fetchWatermarks();
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $currentId = null;
	    foreach (array_keys($watermarks) as $id) {
		if (!isset($currentId)) {
		    $currentId = $id;
		}
		$watermarks[$id] = (array)$watermarks[$id];
	    }

	    $WatermarkOption['watermarks'] = $watermarks;
	    $WatermarkOption['currentId'] = $currentId;
	}

	$template->setVariable('WatermarkOption', $WatermarkOption);
	return array(null,
		     'modules/watermark/templates/WatermarkOption.tpl',
		     'modules_watermark');
    }

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');

	/* Only appropriate if we've got some watermarks to show */
	list ($ret, $watermarks) = WatermarkHelper::fetchWatermarks();
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, !empty($watermarks));
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'watermark');
	if ($ret) {
	    return array($ret, null, null);
	}
	if ($params['forceDefaultWatermark'] && !empty($params['defaultWatermarkId'])) {
	    $form['WatermarkOption']['watermarkId'] = $params['defaultWatermarkId'];
	}

	if (!empty($form['WatermarkOption']['watermarkId'])) {
	    list ($ret, $watermark) = GalleryCoreApi::loadEntitiesById(
		$form['WatermarkOption']['watermarkId'], 'WatermarkImage');
	    if ($ret) {
		if (!$ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		    return array($ret, null, null);
		}
		$ret = GalleryCoreApi::addEventLogEntry('WATERMARK ERROR', 
		    'Watermark doesn\'t exist', $form['WatermarkOption']['watermarkId']);
		if ($ret) {
		    return array($ret, null, null);
		}

		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'watermark');
		if ($ret) {
		    return array($ret, null, null);
		}
		$message = 
		    $module->translate('Error: Missing watermark. Please see the Event Logs '
				       . 'page in the Site Admin.');

		return array(null, array($message), array());
	    }

	    foreach ($items as $i => $item) {
		if (GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
		    $watermarkArray =
			array(DERIVATIVE_TYPE_IMAGE_THUMBNAIL => $watermark->getApplyToThumbnail(),
			      DERIVATIVE_TYPE_IMAGE_RESIZE => $watermark->getApplyToResizes(),
			      DERIVATIVE_TYPE_IMAGE_PREFERRED => $watermark->getApplyToPreferred());
		    $ret = WatermarkHelper::watermarkItem($watermark, $item,
							  $watermark->getXPercentage(),
							  $watermark->getYPercentage(),
							  $watermarkArray);
		    if ($ret) {
			return array($ret, null, null);
		    }
		}
	    }
	}
	return array(null, array(), array());
    }
}
?>
