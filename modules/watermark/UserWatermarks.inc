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
 * Manage user watermarks
 * @package Watermark
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class UserWatermarksController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
        global $gallery;

	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');
	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isOn) =
	    GalleryCoreApi::getPluginParameter('module', 'watermark', 'allowUserWatermarks');
	if ($ret) {
	    return array($ret, null);
	}

        $user = $gallery->getActiveUser();

	if ($isAnonymous || $user->isLocked() || !$isOn) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	$status = $error = array();
	if (isset($form['action']['add']) && !empty($form['tmp_name'][1])) {
	    list ($ret, $item) = WatermarkHelper::createWatermarkImage($form);
	    if ($ret) {
		return array($ret, null);
	    }

	    $results['redirect']['view'] = 'core.UserAdmin';
	    $results['redirect']['subView'] = 'watermark.UserWatermarkEdit';
	    $results['redirect']['watermarkId'] = $item->getId();
	} else if (isset($form['action']['add'])) {
	    $error[] = 'form[error][missingFile]';
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = 'watermark.UserWatermarks';
	} else if (isset($form['action']['delete']) && isset($form['delete']['watermarkId'])) {
	    list ($ret, $wasDeleted) =
		WatermarkHelper::deleteWatermarkImageById($form['delete']['watermarkId']);
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($wasDeleted) {
		$status['delete'] = 1;
		$results['redirect']['view'] = 'core.UserAdmin';
		$results['redirect']['subView'] = 'watermark.UserWatermarks';
	    } else {
		/* Watermark is in use.. redirect to confirm page: */
		$results['redirect']['view'] = 'core.UserAdmin';
		$results['redirect']['subView'] = 'watermark.ConfirmDelete';
		$results['redirect']['watermarkId'] = $form['delete']['watermarkId'];
	    }
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Manage user watermarks
 */
class UserWatermarksView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isOn) =
	    GalleryCoreApi::getPluginParameter('module', 'watermark', 'allowUserWatermarks');
	if ($ret) {
	    return array($ret, null);
	}

        $user = $gallery->getActiveUser();

	if ($isAnonymous || $user->isLocked() || !$isOn) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	list ($ret, $watermarks) = WatermarkHelper::fetchWatermarks();
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($watermarks as $id => $watermark) {
	    /* Exclude site-wide watermarks (and, in the case of admins -- other people's) */
	    if ($watermark->getOwnerId() != $gallery->getActiveUserId()) {
		unset($watermarks[$id]);
		continue;
	    }

	    $watermarks[$id] = (array)$watermark;
	}

	if ($form['formName'] != 'UserWatermarks') {
	    $form['formName'] = 'UserWatermarks';
	    if (!empty($watermarks)) {
		$ids = array_keys($watermarks);
		$form['watermarkId'] = array_shift($ids);
	    }
	}

	/* Set the form's encoding type since we're uploading binary files */
	if ($template->hasVariable('UserAdmin')) {
	    $UserAdmin =& $template->getVariableByReference('UserAdmin');
	    $UserAdmin['enctype'] = 'multipart/form-data';
	} else {
	    $UserAdmin['enctype'] = 'multipart/form-data';
	    $template->setVariable('UserAdmin', $UserAdmin);
	}

	$template->setVariable('UserWatermarks', array('watermarks' => $watermarks));
	$template->setVariable('controller', 'watermark.UserWatermarks');
	return array(null, array('body' => 'modules/watermark/templates/UserWatermarks.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'watermark');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('user watermarks'));
    }
}
?>
