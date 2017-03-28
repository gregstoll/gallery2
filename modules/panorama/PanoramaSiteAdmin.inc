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
 * Settings for Panorama
 * @package Panorama
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class PanoramaSiteAdminController extends GalleryController {

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
	    if (!isset($form['width']) || ((int)$form['width']) < 100) {
		$error[] = 'form[error][width]';
	    } else {
		foreach (array('itemType', 'itemLink') as $key) {
		    $ret = GalleryCoreApi::setPluginParameter(
			'module', 'panorama', $key,
			(isset($form[$key]) && $form[$key]) ? 1 : 0);
		    if ($ret) {
			return array($ret, null);
		    }
		}
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'panorama', 'width', (int)$form['width']);
		if ($ret) {
		    return array($ret, null);
		}
		$status['saved'] = 1;
	    }
	}

	/* Figure out where to redirect upon success */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'panorama.PanoramaSiteAdmin';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Panorama
 */
class PanoramaSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/panorama/classes/PanoramaHelper.class');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	foreach (array('itemType', 'itemLink') as $key) {
	    if (!isset($form[$key])) {
		$form[$key] = false;
	    }
	}
	if ($form['formName'] != 'PanoramaSiteAdmin') {
	    $form['formName'] = 'PanoramaSiteAdmin';
	    foreach (array('itemType', 'itemLink', 'width') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'panorama', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	$template->setVariable('controller', 'panorama.PanoramaSiteAdmin');
	return array(null, array('body' => 'modules/panorama/templates/PanoramaSiteAdmin.tpl'));
    }
}
?>
