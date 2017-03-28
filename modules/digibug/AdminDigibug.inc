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
 * Digibug4CPG - a plugin for gallery to create photo prints, and print product gift items
 * derived from code written by Bharat Mediratta
 * Copyright (C) 2006 Digibug Express, Inc.
 *
 * @package Digibug
 * @subpackage UserInterface
 * @author James Andrews <jandrews@digibug.com>
 * @author Bharat Mediratta <bharat@menalto.com>
 */
class AdminDigibugController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest(&$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Provide compatibility with 2.1 and 2.2 */
	$apiVersion = GalleryCoreApi::getApiVersion();
	$adminPlugins = 'core.Admin'
	    . ($apiVersion[0] > 7 || $apiVersion[1] >= 7 ? 'Plugins' : 'Modules');

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    if ($form['digibugIdChoice'] == 'owner') {
		if (empty($form['digibugCustomerId'])
			|| !preg_match('/^\d+$/', trim($form['digibugCustomerId']))) {
		    $error[] = 'form[error][digibugCustomerId][invalid]';
		}
		if (empty($form['digibugPricelistId'])
			|| !preg_match('/^\d+$/', trim($form['digibugPricelistId']))) {
		    $error[] = 'form[error][digibugPricelistId][invalid]';
		}
	    }
	    if (empty($error)) {
		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'digibug');
		if ($ret) {
		    return array($ret, null);
		}
		list ($ret, $isActive) = $module->isActive();
		if ($ret) {
		    return array($ret, null);
		}

		if ($form['digibugIdChoice'] == 'gallery') {
		    foreach (array('digibugIdChoice' => 'gallery', 'digibugCustomerId' => '3153',
				   'digibugPricelistId' => '8491') as $key => $value) {
			$ret = $module->setParameter($key, $value);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		} else {
		    foreach (array('digibugIdChoice', 'digibugCustomerId', 'digibugPricelistId')
			     as $key) {
			$ret = $module->setParameter($key, trim($form[$key]));
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}

		$redirect = array('view' => 'core.SiteAdmin');
		if ($isActive) {
		    $redirect['subView'] = 'digibug.AdminDigibug';
		    $status['saved'] = 1;
		} else {
		    $redirect['subView'] = $adminPlugins;
		    $status['configured'] = 'digibug';
		}
	    }
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'digibug.AdminDigibug';
	} else if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = $adminPlugins;
	}

	$results = array('status' => $status, 'error' => $error);
	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'digibug.AdminDigibug';
	}

	return array(null, $results);
    }
}

/**
 * Settings for Digibug
 */
class AdminDigibugView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'digibug');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isActive) = $module->isActive();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminDigibug') {
	    foreach (array('digibugIdChoice', 'digibugCustomerId', 'digibugPricelistId') as $key) {
		list ($ret, $form[$key]) = $module->getParameter($key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $form['formName'] = 'AdminDigibug';
	}
	$template->style('modules/digibug/digibug.css');
	$template->setVariable('AdminDigibug', array('isConfigure' => !$isActive));
	$template->setVariable('controller', 'digibug.AdminDigibug');
	return array(null, array('body' => 'modules/digibug/templates/AdminDigibug.tpl'));
    }
}
?>
