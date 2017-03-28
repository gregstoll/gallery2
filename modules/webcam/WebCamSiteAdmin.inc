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
 * Settings for Webcam images
 * @package WebCam
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class WebCamSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    if (!isset($form['fromweb'])
		    || !in_array($form['fromweb'], array('on', 'off', 'admin'))) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }

	    if (isset($form['duration'])) {
		/* Accept input for locales that use comma as fraction separator */
		$form['duration'] = str_replace(',', '.', $form['duration']);
	    }
	    if (!isset($form['duration']) || !is_numeric($form['duration'])) {
		$error[] = 'form[error][duration]';
	    } else {
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'webcam', 'duration', round($form['duration'] * 60));
		if ($ret) {
		    return array($ret, null);
		}
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'webcam', 'fromweb', $form['fromweb']);
		if ($ret) {
		    return array($ret, null);
		}
		$status['saved'] = 1;
	    }
	}

	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'webcam.WebCamSiteAdmin';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Webcam images
 */
class WebCamSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'webcam');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'WebCamSiteAdmin') {
	    $form['formName'] = 'WebCamSiteAdmin';
	    foreach (array('fromweb', 'duration') as $key) {
		list ($ret, $form[$key]) = $module->getParameter($key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    /* String conversion here may use comma as fraction separator, based on locale */
	    $form['duration'] = (string)round($form['duration'] / 60, 2);
	}

	$WebCamSiteAdmin = array(
	    'optionList' => array(
		'on' => $module->translate('Available to all users with permission to add items'),
		'admin' => $module->translate('Site Admins only'),
		'off' => $module->translate('Disabled')));

	$template->setVariable('WebCamSiteAdmin', $WebCamSiteAdmin);
	$template->setVariable('controller', 'webcam.WebCamSiteAdmin');
	return array(null, array('body' => 'modules/webcam/templates/WebCamSiteAdmin.tpl'));
    }
}
?>
