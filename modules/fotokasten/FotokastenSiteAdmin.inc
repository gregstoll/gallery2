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
 * Settings for Fotokasten
 * @package Fotokasten
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class FotokastenSiteAdminController extends GalleryController {

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
	    if (empty($form['affiliateId']) || !preg_match('/^\d+$/', trim($form['affiliateId']))) {
		$error[] = 'form[error][affiliateId]';
	    }
	    if (empty($form['affiliateIdPass'])
		    || !preg_match('/^[A-Za-z0-9]+$/', trim($form['affiliateIdPass']))) {
		$error[] = 'form[error][affiliateIdPass]';
	    }
	    if (empty($error)) {
		foreach (array('affiliateId', 'affiliateIdPass') as $key) {
		    $ret = GalleryCoreApi::setPluginParameter(
			    'module', 'fotokasten', $key, trim($form[$key]));
		    if ($ret) {
			return array($ret, null);
		    }
		}
		$status['saved'] = 1;
	    }
	} else if (isset($form['action']['useGalleryId'])) {
	    foreach (array('affiliateId' => FOTOKASTEN_GALLERY_AFFILIATE_ID,
			   'affiliateIdPass' => FOTOKASTEN_GALLERY_AFFILIATE_PASSWORD)
		     as $key => $value) {
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'fotokasten', $key, $value);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['usingGalleryId'] = 1;
	}


	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'fotokasten.FotokastenSiteAdmin';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Fotokasten
 */
class FotokastenSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'FotokastenSiteAdmin') {
	    $form['formName'] = 'FotokastenSiteAdmin';
	    foreach (array('affiliateId', 'affiliateIdPass') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'fotokasten', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	$FotokastenSiteAdmin = array();
	$FotokastenSiteAdmin['usingDefaultGalleryAffiliateId'] =
	    ($form['affiliateId'] == FOTOKASTEN_GALLERY_AFFILIATE_ID);

	$template->setVariable('FotokastenSiteAdmin', $FotokastenSiteAdmin);
	$template->setVariable('controller', 'fotokasten.FotokastenSiteAdmin');
	return array(null, array('body' => 'modules/fotokasten/templates/FotokastenSiteAdmin.tpl'));
    }
}
?>
