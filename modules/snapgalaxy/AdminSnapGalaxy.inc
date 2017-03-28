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
 * Settings for SnapGalaxy
 * Copyright (C) 2006 SnapGalaxy Inc.
 *
 * @package SnapGalaxy
 * @subpackage UserInterface
 * @author Bhavin Suthar <bhavin.suthar@snapgalaxy.com>
 * @author Bharat Mediratta <bharat@menalto.com>
 */
class AdminSnapGalaxyController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest(&$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    if (empty($form['snapgalaxyPartnerId'])
		    || !preg_match('/^[A-Za-z0-9_\-]+$/', $form['snapgalaxyPartnerId'])) {
		$error[] = 'form[error][snapgalaxyPartnerId][invalid]';
	    }
	    if (empty($error)) {
		foreach (array('snapgalaxyPartnerId') as $key) {
		    $ret = GalleryCoreApi::setPluginParameter(
			'module', 'snapgalaxy', $key, trim($form[$key]));
		    if ($ret) {
			return array($ret, null);
		    }
		}

		$redirect = array('view' => 'core.SiteAdmin');
		$redirect['subView'] = 'snapgalaxy.AdminSnapGalaxy';
		$status['saved'] = 1;
	    }
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'snapgalaxy.AdminSnapGalaxy';
	}

	$results = array('status' => $status, 'error' => $error);
	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'snapgalaxy.AdminSnapGalaxy';
	}
	return array(null, $results);
    }
}

/**
 * Settings for SnapGalaxy
 */
class AdminSnapGalaxyView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminSnapGalaxy') {
	    foreach (array('snapgalaxyPartnerId') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'snapgalaxy', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $form['formName'] = 'AdminSnapGalaxy';
	}

	$template->setVariable('controller', 'snapgalaxy.AdminSnapGalaxy');
	return array(null, array('body' => 'modules/snapgalaxy/templates/AdminSnapGalaxy.tpl'));
    }
}
?>
