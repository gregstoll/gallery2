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
 * Settings for Zip Cart
 * @package ZipCart
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20954 $
 */
class ZipCartAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$error = $status = array();
	if (isset($form['action']['save']) || isset($form['action']['test'])) {
	    $form['path'] = trim($form['path']);
	    if (empty($form['path'])) {
		$error[] = 'form[error][path][missing]';
	    }
	    if (empty($error)) {
		$platform =& $gallery->getPlatform();
		if (!$platform->isRestrictedByOpenBaseDir($form['path'])) {
		    if (!$platform->is_executable($form['path'])) {
			$error[] = 'form[error][path][badPath]';
		    }
		} else {
		    $error[] = 'form[error][path][badPath]';
		}
	    }
	    if (empty($error)) {
		/* Try zipping something */
		$zipBase = $platform->tempnam($gallery->getConfig('data.gallery.tmp'), 'zip_');
		$zipFile = $zipBase . '.zip';
		$gallery->guaranteeTimeLimit(30);
		$gallery->startRecordingDebugSnippet();
		if ($success = $platform->copy(__FILE__, $zipBase)) {
		    list ($success) =
			$platform->exec(array(array($form['path'], $zipFile, $zipBase)));
		}
		$debugSnippet = $gallery->stopRecordingDebugSnippet();
		if (!$success || !$platform->file_exists($zipFile)) {
		    $error[] = 'form[error][path][badZip]';
		    GalleryUtilities::putRequestVariable('debugSnippet', $debugSnippet);
		}
		@$platform->unlink($zipFile);
		@$platform->unlink($zipBase);
	    }
	}

	if (isset($form['action']['save']) && empty($error)) {
	    $ret = GalleryCoreApi::setPluginParameter('module', 'zipcart', 'path', $form['path']);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'zipcart');
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $isActive) = $module->isActive();
	    if ($ret) {
		return array($ret, null);
	    }

	    $redirect['view'] = 'core.SiteAdmin';
	    if ($isActive) {
		$redirect['subView'] = 'zipcart.ZipCartAdmin';
		$status['saved'] = 1;
	    } else {
		$redirect['subView'] = 'core.AdminPlugins';
		$status['configured'] = 'zipcart';
	    }
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'zipcart.ZipCartAdmin';
	} else if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminPlugins';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'zipcart.ZipCartAdmin';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Zip Cart
 */
class ZipCartAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ZipCartAdmin') {
	    $form['formName'] = 'ZipCartAdmin';
	    list ($ret, $form['path']) =
		    GalleryCoreApi::getPluginParameter('module', 'zipcart', 'path');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	if (isset($form['action']['test'])) {
	    $form['test'] = empty($form['error']);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'zipcart');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isActive) = $module->isActive();
	if ($ret) {
	    return array($ret, null);
	}

	$ZipCartAdmin = array('isConfigure' => !$isActive,
			      'canExec' => !in_array('exec',
					    preg_split('/,\s*/', ini_get('disable_functions'))));
	if (isset($form['error']['path']['badZip']) > 0) {
	    $ZipCartAdmin['debugSnippet'] = GalleryUtilities::getRequestVariables('debugSnippet');
	    $template->javascript('lib/javascript/BlockToggle.js');
	}

	$template->setVariable('ZipCartAdmin', $ZipCartAdmin);
	$template->setVariable('controller', 'zipcart.ZipCartAdmin');
	return array(null, array('body' => 'modules/zipcart/templates/ZipCartAdmin.tpl'));
    }
}
?>
