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
 * Delete a watermark
 * @package Watermark
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ConfirmDeleteController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	list ($ret, $canView) =
	    GalleryCoreApi::hasItemPermission($form['watermarkId'], 'core.view');
	if ($ret) {
	    return array($ret, null);
	}
	if (!$canView) {
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}
	/* Make sure we have permission to edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($form['watermarkId'], 'core.edit');
	if ($ret) {
	    return array($ret, null);
	}

	$status = array();
	if (isset($form['action']['delete'])) {
	    GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');

	    list ($ret) = WatermarkHelper::deleteWatermarkImageById($form['watermarkId'], true);
	    if ($ret) {
		return array($ret, null);
	    }

	    $status['delete'] = 1;
	} /* else $form['action']['cancel'] */

	$results['redirect'] = isset($form['fromAdmin'])
	    ? array('view' => 'core.SiteAdmin', 'subView' => 'watermark.WatermarkSiteAdmin')
	    : array('view' => 'core.UserAdmin', 'subView' => 'watermark.UserWatermarks');
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * Confirm delete of a watermark
 */
class ConfirmDeleteView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');
	if ($form['formName'] != 'ConfirmDelete') {
	    $form['formName'] = 'ConfirmDelete';
	    list ($form['watermarkId'], $form['fromAdmin']) =
		GalleryUtilities::getRequestVariables('watermarkId', 'fromAdmin');
	}

	list ($ret, $item) =
	    GalleryCoreApi::loadEntitiesById($form['watermarkId'], 'WatermarkImage');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $canView) =
	    GalleryCoreApi::hasItemPermission($form['watermarkId'], 'core.view');
	if ($ret) {
	    return array($ret, null);
	}
	if (!$canView) {
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}

	/* Make sure we have permission to edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($form['watermarkId'], 'core.edit');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $itemIds) = WatermarkHelper::fetchWatermarkedItemIds($item);
	if ($ret) {
	    return array($ret, null);
	}

	$template->setVariable('ConfirmDelete',
			       array('item' => (array)$item,
				     'count' => count($itemIds)));
	$template->setVariable('controller', 'watermark.ConfirmDelete');

	return array(null, array('body' => 'modules/watermark/templates/ConfirmDelete.tpl'));
    }
}
?>
