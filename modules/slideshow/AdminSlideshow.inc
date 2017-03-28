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

GalleryCoreApi::requireOnce('modules/slideshow/classes/PicLensHelper.class');

/**
 * Settings for Slideshow
 * @package Slideshow
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminSlideshowController extends GalleryController {

    /**
     * Provide an instance of PicLensHelper for the controller to use instead of
     * the default.  This allows tests to inject this dependency.
     * @param PicLensHelper $picLensHelper
     * @protected
     */
    function _setPicLensHelper($picLensHelper=null) {
	$this->_picLensHelper = $picLensHelper;
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Allow our helper to be injected as a dependency for ease in testing */
	$picLensHelper = isset($this->_picLensHelper) ? $this->_picLensHelper : new PicLensHelper();
	$error = $status = array();
	if (isset($form['action']['install'])) {
	    list ($ret, $version) = $picLensHelper->install();
	    if ($ret) {
		if ($ret->getErrorCode() & ERROR_STORAGE_FAILURE) {
		    return array($ret, null);
		}
		/* It should be safe to swallow non storage failures. */
		$status['downloadFailed'] = 1;
	    } else {
		$status['installed'] = $version;
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'slideshow', 'piclens.version', $version);
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    $ret = GalleryCoreApi::removePluginParameter(
		'module', 'slideshow', 'piclens.uninstalled');
	    if ($ret) {
		return array($ret, null);
	    }
	} else if (isset($form['action']['uninstall'])) {
	    $ret = $picLensHelper->uninstall();
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['uninstalled'] = 1;

	    $ret = GalleryCoreApi::removePluginParameter('module', 'slideshow', 'piclens.version');
	    if ($ret) {
		return array($ret, null);
	    }

	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'slideshow', 'piclens.uninstalled', 1);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	return array(null, array('redirect' => array('view' => 'core.SiteAdmin',
						     'subView' => 'slideshow.AdminSlideshow'),
				 'status' => $status,
				 'error' => $error));
    }
}

/**
 * Settings for Slideshow
 */
class AdminSlideshowView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load our default values if we didn't just come from this form. */
	if ($form['formName'] != 'AdminSlideshow') {
	    $form['formName'] = 'AdminSlideshow';
	}

	list ($ret, $current) =
	    GalleryCoreApi::getPluginParameter('module', 'slideshow', 'piclens.version');
	if ($ret) {
	    return array($ret, null);
	}

	$picLensHelper = new PicLensHelper();
	$info = $picLensHelper->getLatestPicLensInfo();
	if (empty($info)) {
	    $update = null;
	    $ret = GalleryCoreApi::addEventLogEntry(
		'slideshow', 'Unable to get latest piclens info', '');
	    if ($ret) {
		return array($ret, null);
	    }
	} else {
	    $update = version_compare($info['version'], $current, '>') ? $info['version'] : null;
	}

	$AdminSlideshow = array('piclens' => array('current' => $current, 'update' => $update));
	$template->setVariable('AdminSlideshow', $AdminSlideshow);
	$template->setVariable('controller', 'slideshow.AdminSlideshow');
	return array(null, array('body' => 'modules/slideshow/templates/AdminSlideshow.tpl'));
    }
}
?>
