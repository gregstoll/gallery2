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

GalleryCoreApi::requireOnce('modules/gd/classes/GdToolkitHelper.class');

/**
 * Settings for GD
 * @package Gd
 * @subpackage UserInterface
 * @author Ernesto Baschny <ernst@baschny.de>
 * @version $Revision: 17580 $
 */
class AdminGdController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$error = $status = array();
	if (isset($form['action']['save'])) {
	    $ret = GalleryCoreApi::setPluginParameter('module', 'gd',
						      'jpegQuality', $form['jpegQuality']);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'gd');
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $isActive) = $module->isActive();
	    if ($ret) {
		return array($ret, null);
	    }

	    $redirect['view'] = 'core.SiteAdmin';
	    if ($isActive) {
		$redirect['subView'] = 'gd.AdminGd';
		$status['saved'] = 1;
	    } else {
		$redirect['subView'] = 'core.AdminPlugins';
	    }
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'gd.AdminGd';
	} else if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminPlugins';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'gd.AdminGd';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for GD
 */
class AdminGdView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load our default values if we didn't just come from this form. */
	if ($form['formName'] != 'AdminGd') {
	    foreach (array('path', 'jpegQuality') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'gd', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $form['formName'] = 'AdminGd';
	}

	/* Set up our jpeg quality selection list */
	for ($i = 100; $i >= 50; $i -= 5) {
	    $jpegQualityList[] = $i;
	}

	list ($ret, $gdVersion) = GdToolkitHelper::discoverGdVersion();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isGdBundled) = GdToolkitHelper::isBundled();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isGdVersionSupported) = GdToolkitHelper::isGdVersionSupported($gdVersion);
	if ($ret) {
	    return array($ret, null);
	}
	$minGdVersion = GdToolkitHelper::minGdVersion();

	$mimeTypes = array();
	if ($isGdVersionSupported) {
	    list ($ret, $mimeTypes) = GdToolkitHelper::discoverMimeTypes();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'gd');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isActive) = $module->isActive();
	if ($ret) {
	    return array($ret, null);
	}

	$AdminGd = array();
	$AdminGd['gdVersionTooOld'] = !$isGdVersionSupported;
	$AdminGd['minGdVersion'] = $minGdVersion;
	$AdminGd['mimeTypes'] = $mimeTypes;
	$AdminGd['jpegQualityList'] = $jpegQualityList;
	$AdminGd['gdVersion'] = $gdVersion;
	$AdminGd['isGdBundled'] = $isGdBundled;
	$AdminGd['isConfigure'] = !$isActive;

	$template->setVariable('AdminGd', $AdminGd);
	$template->setVariable('controller', 'gd.AdminGd');
	return array(null, array('body' => 'modules/gd/templates/AdminGd.tpl'));
    }
}
?>
