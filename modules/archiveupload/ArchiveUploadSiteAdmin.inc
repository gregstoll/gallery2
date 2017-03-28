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
 * Settings for Archive Upload
 * @package ArchiveUpload
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20954 $
 */
class ArchiveUploadSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest(&$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save']) || isset($form['action']['test'])) {
	    GalleryCoreApi::requireOnce('modules/archiveupload/classes/ArchiveUploadHelper.class');

	    if (empty($form['unzipPath'])) {
		$error[] = 'form[error][unzipPath][missing]';
	    }
	    if (empty($error)) {
		list ($valid, $errorType, $output) =
		    ArchiveUploadHelper::testBinary($form['unzipPath']);
		if (!$valid && $errorType == 'badPath') {
		    /* Maybe a directory was entered.. try adding /unzip */
		    $platform =& $gallery->getPlatform();
		    $slash = $platform->getDirectorySeparator();
		    $retryPath = $form['unzipPath'];
		    if ($retryPath{strlen($retryPath)-1} != $slash) {
			$retryPath .= $slash;
		    }
		    $retryPath .= 'unzip';
		    list ($valid) = ArchiveUploadHelper::testBinary($retryPath);
		    if ($valid) {
			$form['unzipPath'] = $retryPath;
		    }
		}
		if (!$valid) {
		    $error[] = 'form[error][unzipPath][' . $errorType . ']';
		}
	    }
	    if (empty($error) && isset($form['action']['save'])) {
		$ret = GalleryCoreApi::setPluginParameter(
			'module', 'archiveupload', 'unzipPath', $form['unzipPath']);
		if ($ret) {
		    return array($ret, null);
		}
		$ret = GalleryCoreApi::setPluginParameter('module', 'archiveupload',
			'removeMeta', empty($form['removeMeta']) ? 0 : 1);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'archiveupload');
		if ($ret) {
		    return array($ret, null);
		}
		list ($ret, $isActive) = $module->isActive();
		if ($ret) {
		    return array($ret, null);
		}

		if ($isActive) {
		    $redirectSubview = 'archiveupload.ArchiveUploadSiteAdmin';
		    $status['saved'] = 1;
		} else {
		    $redirectSubview = 'core.AdminPlugins';
		    $status['configured'] = 'archiveupload';
		}
	    }
	} else if (isset($form['action']['cancel'])) {
	    $redirectSubview = 'core.AdminPlugins';
	} else if (isset($form['action']['reset'])) {
	    $redirectSubview = 'archiveupload.ArchiveUploadSiteAdmin';
	}

	if (!empty($redirectSubview)) {
	    $results['redirect'] = array('view' => 'core.SiteAdmin',
					 'subView' => $redirectSubview);
	} else {
	    $results['delegate'] = array('view' => 'core.SiteAdmin',
					 'subView' => 'archiveupload.ArchiveUploadSiteAdmin');
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Archive Upload
 */
class ArchiveUploadSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ArchiveUploadSiteAdmin') {
	    $form['formName'] = 'ArchiveUploadSiteAdmin';
	    foreach (array('unzipPath', 'removeMeta') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'archiveupload', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'archiveupload');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isActive) = $module->isActive();
	if ($ret) {
	    return array($ret, null);
	}
	$form['isConfigure'] = !$isActive;
	$form['canExec'] = !in_array('exec', preg_split('/,\s*/', ini_get('disable_functions')));

	if (isset($form['action']['test'])) {
	    if (empty($form['error']['unzipPath'])) {
		$test = array('name' => 'unzip', 'success' => true);
	    } else if (isset($form['error']['unzipPath']['exec'])) {
		$test = array('name' => 'unzip', 'success' => false);
		list ($valid, $errorType, $output) =
		    ArchiveUploadHelper::testBinary($form['unzipPath']);
		if (!empty($output)) {
		    $test['message'] = $output;
		}
	    }
	    if (isset($test)) {
		$template->setVariable('ArchiveUploadSiteAdmin', array('tests' => array($test)));
	    }
	}

	$template->setVariable('controller', 'archiveupload.ArchiveUploadSiteAdmin');
	return array(null,
		     array('body' => 'modules/archiveupload/templates/ArchiveUploadSiteAdmin.tpl'));
    }
}
?>
