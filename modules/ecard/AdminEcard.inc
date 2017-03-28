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
 * Settings for eCards
 * @package Ecard
 * @subpackage UserInterface
 * @author Nico Kaiser <nico@siriux.net>
 * @author Dariush Molavi <dari@nukedgallery.net>
 * @version $Revision: 17580 $
 */
class AdminEcardController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $error = $status = array();

	if (isset($form['action']['save'])) {
	    foreach (array('from', 'subject', 'bcc', 'header', 'footer', 'format') as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'ecard', $key, $form[$key]);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'ecard.AdminEcard';
	    $status['saved'] = 1;
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'ecard.AdminEcard';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'ecard.AdminEcard';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for eCards
 */
class AdminEcardView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminEcard') {
	    $form['formName'] = 'AdminEcard';
	    foreach (array('from', 'subject', 'bcc', 'header', 'footer', 'format') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'ecard', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'ecard');
	if ($ret) {
	    return array($ret, null);
	}

	$AdminEcard = array(
	    'formatList' => array(
		'plain' => $module->translate('Plain text only'),
		'html'  => $module->translate('HTML')
	));

	$template->setVariable('AdminEcard', $AdminEcard);
	$template->setVariable('controller', 'ecard.AdminEcard');
	return array(null, array('body' => 'modules/ecard/templates/AdminEcard.tpl'));
    }
}
?>
