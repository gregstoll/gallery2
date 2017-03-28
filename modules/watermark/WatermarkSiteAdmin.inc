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
 * Manage watermarks available for all users
 * @package Watermark
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class WatermarkSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = array();
	if (isset($form['action']['add']) && !empty($form['tmp_name'][1])) {
	    list ($ret, $item) = WatermarkHelper::createWatermarkImage($form, true);
	    if ($ret) {
		return array($ret, null);
	    }

	    $status['add'] = 1;
	    $results['redirect'] = array('view' => 'core.SiteAdmin',
					 'subView' => 'watermark.WatermarkSiteAdminEdit',
					 'watermarkId' => $item->getId());
	} else if (isset($form['action']['add'])) {
	    $status['addError'] = 1;
	} else if (isset($form['action']['delete']) && isset($form['delete']['watermarkId'])) {
	    list ($ret, $wasDeleted) =
		WatermarkHelper::deleteWatermarkImageById($form['delete']['watermarkId']);
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($wasDeleted) {
		$status['delete'] = 1;
	    } else {
		/* Watermark is in use.. redirect to confirm page */
		$results['redirect'] = array('view' => 'core.SiteAdmin',
					     'subView' => 'watermark.ConfirmDelete',
					     'watermarkId' => $form['delete']['watermarkId'],
					     'fromAdmin' => 1);
	    }
	} else if (isset($form['action']['save'])) {
	    foreach (array('allowUserWatermarks', 'forceDefaultWatermark') as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'watermark',
							  $key, empty($form[$key]) ? 0 : 1);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    foreach (array('hotlinkWatermarkId', 'defaultWatermarkId') as $key) {
		if (!empty($form[$key])) {
		    $ret = GalleryCoreApi::setPluginParameter('module', 'watermark',
							      $key, $form[$key]);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	    if (!empty($form['forceDefaultWatermark']) && empty($form['defaultWatermarkId'])) {
		$status['missingDefault'] = 1;
	    }
	    $status['savedSettings'] = 1;
	}

	if (!isset($results['redirect'])) {
	    $results['redirect'] = array('view' => 'core.SiteAdmin',
					 'subView' => 'watermark.WatermarkSiteAdmin');
	}
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * Manage watermarks available for all users
 */
class WatermarkSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'WatermarkSiteAdmin') {
	    $form['formName'] = 'WatermarkSiteAdmin';
	    foreach (array('allowUserWatermarks', 'forceDefaultWatermark',
			   'hotlinkWatermarkId', 'defaultWatermarkId') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'watermark', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}
	list ($ret, $list) = WatermarkHelper::fetchWatermarks();
	if ($ret) {
	    return array($ret, null);
	}
	$owners = array();
	$form['list'] = array();

	if (!empty($list)) {
	    $ownerIds = array();
	    foreach ($list as $id => $watermark) {
		$ownerIds[] = $watermark->getOwnerId();
		$form['list'][] = (array)$watermark;
	    }
	    list ($ret, $tmp) = GalleryCoreApi::loadEntitiesById($ownerIds, 'GalleryUser');
	    if ($ret) {
		return array($ret, null);
	    }

	    for ($i = 0; $i < sizeof($tmp); $i++) {
		$owners[$tmp[$i]->getId()] = (array)$tmp[$i];
	    }
	}

	/* Set the form's encoding type since we're uploading binary files */
	if ($template->hasVariable('SiteAdmin')) {
	    $SiteAdmin =& $template->getVariableByReference('SiteAdmin');
	    $SiteAdmin['enctype'] = 'multipart/form-data';
	} else {
	    $SiteAdmin['enctype'] = 'multipart/form-data';
	    $template->setVariable('SiteAdmin', $SiteAdmin);
	}

	$WatermarkSiteAdmin = array('owners' => $owners);

	$template->setVariable('WatermarkSiteAdmin', $WatermarkSiteAdmin);
	$template->setVariable('controller', 'watermark.WatermarkSiteAdmin');
	return array(null,
		     array('body' => 'modules/watermark/templates/WatermarkSiteAdmin.tpl'));
    }
}
?>
