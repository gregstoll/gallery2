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

GalleryCoreApi::requireOnce('modules/netpbm/classes/NetPbmToolkitHelper.class');

/**
 * Settings for NetPbm
 * @package NetPbm
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 20954 $
 */
class AdminNetPbmController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save'])) {

	    if (empty($form['path'])) {
		$error[] = 'form[error][path][missing]';
	    } else {

		/* Set the path.  Guarantee that it's got a trailing slash. */
		$platform =& $gallery->getPlatform();
		$slash = $platform->getDirectorySeparator();
		$form['path'] = trim($form['path']);
		if (!empty($form['path'])) {
		    if ($form['path']{strlen($form['path'])-1} != $slash) {
			$form['path'] .= $slash;
		    }
		}

		list ($ret, $testResults, $mimeTypes) =
		    NetPbmToolkitHelper::testBinaries($form['path']);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_BAD_PATH) {
			$error[] = 'form[error][path][badPath]';
		    } else {
			return array($ret, null);
		    }
		} else if (empty($mimeTypes)) {
		    /* At least one mime type must be supported, else this path is invalid */
		    $error[] = 'form[error][path][testError]';
		}
	    }

	    if (!empty($form['jheadPath'])) {
		list ($ret, $jheadTests, $jheadMimeTypes) =
		    NetPbmToolkitHelper::testJhead($form['jheadPath']);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_BAD_PATH) {
			$error[] = 'form[error][jheadPath][badPath]';
		    } else {
			return array($ret, null);
		    }
		} else {
		    $successCount = 0;
		    foreach ($jheadTests as $test) {
			$successCount += $test['success'];
		    }
		    if ($successCount != count($jheadTests)) {
			$error[] = 'form[error][jheadPath][badPath]';
		    }
		}
	    }

	    if (empty($error)) {
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'netpbm', 'path', $form['path']);
		if ($ret) {
		    return array($ret, null);
		}

		/* Set the image quality */
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'netpbm', 'jpegQuality', $form['jpegQuality']);
		if ($ret) {
		    return array($ret, null);
		}

		/* Set the jhead binary name */
		$jheadPath = isset($form['jheadPath']) ? $form['jheadPath'] : '';
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'netpbm', 'jheadPath', $jheadPath);
		if ($ret) {
		    return array($ret, null);
		}

		/* Save binaries to use for this netpbm path */
		$ret = NetPbmToolkitHelper::saveParameters($testResults);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'netpbm');
		if ($ret) {
		    return array($ret, null);
		}
		list ($ret, $isActive) = $module->isActive();
		if ($ret) {
		    return array($ret, null);
		}

		$redirect['view'] = 'core.SiteAdmin';
		if ($isActive) {
		    $redirect['subView'] = 'netpbm.AdminNetPbm';
		    $status['saved'] = 1;
		} else {
		    $redirect['subView'] = 'core.AdminPlugins';
		    $status['configured'] = 'netpbm';
		}
	    }
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'netpbm.AdminNetPbm';
	} else if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminPlugins';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'netpbm.AdminNetPbm';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for NetPbm
 */
class AdminNetPbmView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load our default values if we didn't just come from this form. */
	if ($form['formName'] != 'AdminNetPbm') {
	    foreach (array('path', 'jpegQuality', 'jheadPath') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'netpbm', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $form['formName'] = 'AdminNetPbm';
	}

	/* Guarantee that the path and jheadPath have a trailing slash. */
	$slash = $platform->getDirectorySeparator();
	foreach (array('path', 'jheadPath') as $key) {
	    $form[$key] = trim($form[$key]);
	    if (!empty($form[$key])) {
		if ($form[$key]{strlen($form[$key])-1} != $slash) {
		    $form[$key] .= $slash;
		}
	    }
	}

	/* Set up our jpeg quality selection list */
	for ($i = 100; $i >= 50; $i -= 5) {
	    $jpegQualityList[] = $i;
	}

	$tests = $mimeTypes = array();
	$failCount = 0;
	$debugSnippet = '';
	if (isset($form['action']['test'])) {
	    if (empty($form['path'])) {
		$form['error']['path']['missing'] = 1;
	    } else {
		$gallery->startRecordingDebugSnippet();
		list ($ret, $tests, $mimeTypes) = NetPbmToolkitHelper::testBinaries($form['path']);
		$debugSnippet = $gallery->stopRecordingDebugSnippet();

		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_BAD_PATH) {
			$form['error']['path']['badPath'] = 1;
		    } else {
			return array($ret, null);
		    }
		} else {
		    foreach ($tests as $test) {
			$failCount += $test['success'] ? 0 : 1;
		    }
		}
	    }

	    if (empty($form['jheadPath'])) {
		$form['error']['jheadPath']['missing'] = 1;
	    } else {
		$gallery->startRecordingDebugSnippet();
		list ($ret, $jheadTests, $jheadMimeTypes) =
		    NetPbmToolkitHelper::testJhead($form['jheadPath']);
		$debugSnippet .= $gallery->stopRecordingDebugSnippet();
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_BAD_PATH) {
			$form['error']['jheadPath']['badPath'] = 1;
		    } else {
			return array($ret, null);
		    }
		} else {
		    foreach ($jheadTests as $test) {
			$tests[] = $test;
			$failCount += $test['success'] ? 0 : 1;
		    }
		}
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'netpbm');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isActive) = $module->isActive();
	if ($ret) {
	    return array($ret, null);
	}

	$AdminNetPbm = array();
	$AdminNetPbm['tests'] = $tests;
	$AdminNetPbm['mimeTypes'] = $mimeTypes;
	$AdminNetPbm['jpegQualityList'] = $jpegQualityList;
	$AdminNetPbm['failCount'] = $failCount;
	$AdminNetPbm['debugSnippet'] = $debugSnippet;
	$AdminNetPbm['isConfigure'] = !$isActive;
	$AdminNetPbm['canExec'] =
	    !in_array('exec', preg_split('/,\s*/', ini_get('disable_functions')));

	if ($failCount > 0) {
	    $template->javascript('lib/javascript/BlockToggle.js');
	}

	$template->setVariable('AdminNetPbm', $AdminNetPbm);
	$template->setVariable('controller', 'netpbm.AdminNetPbm');
	return array(null, array('body' => 'modules/netpbm/templates/AdminNetPbm.tpl'));
    }
}
?>
