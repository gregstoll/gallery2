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

GalleryCoreApi::requireOnce('modules/jpegtran/classes/JpegtranToolkitHelper.class');

/**
 * Settings for Jpegtran
 * @package Jpegtran
 * @subpackage UserInterface
 * @author Matthew Miller <mattdm@mattdm.org>
 * @author Andy Staudacher <ast@gmx.ch>
 * @version $Revision: 20954 $
 */
class AdminJpegtranController extends GalleryController {

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
	    if (empty($form['path'])) {
		$error[] = 'form[error][path][missing]';
	    } else {
		/* Verify the path */
		$platform =& $gallery->getPlatform();
		$slash = $platform->getDirectorySeparator();
		$form['path'] = trim($form['path']);
		if (!$platform->isRestrictedByOpenBaseDir($form['path'])) {
		    if ($platform->is_dir($form['path'])) {
			if (!empty($form['path'])) {
			    $form['path'] = rtrim($form['path'], $slash) . $slash;
			}

			/* Try adding on "jpegtran" */
			$path = $form['path'] . 'jpegtran';
			if (GalleryUtilities::isA($platform, 'WinNtPlatform')) {
			    $path .= '.exe';
			}

			if ($platform->file_exists($path)) {
			    /* Got a match */
			    $form['path'] = $path;
			    GalleryUtilities::putRequestVariable('form[path]', $path);
			}
		    }
 
		    if (empty($error)) {
			if ($platform->is_file($form['path'])) {
			    if (!$platform->is_executable($form['path'])) {
				$error[] = 'form[error][path][notExecutable]';
			    }
			} else {
			    $error[] = 'form[error][path][badPath]';
			}
		    }
		} else {
		    $error[] = 'form[error][path][badPath]';
		}
	    }
	}

	if (isset($form['action']['save']) && empty($error)) {
	    $realPath = $platform->realpath($form['path']);
	    list ($ret, $testResults) = JpegtranToolkitHelper::testBinary($realPath);

	    if ($ret) {
		if ($ret->getErrorCode() & ERROR_BAD_PATH) {
		    $error[] = 'form[error][path][badPath]';
		} else {
		    return array($ret, null);
		}
	    } else {
		$failCount = 0;
		foreach ($testResults as $testResult) {
		    /* Rotate must work, other operations are optional */
		    if ($testResult['name'] == 'rotate' && !$testResult['success']) {
			$failCount++;
		    }
		}
		if ($failCount > 0) {
		    $error[] = 'form[error][path][testError]';
		}
	    }

	    if (empty($error)) {
		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'jpegtran');
		if ($ret) {
		    return array($ret, null);
		}

		$ret = $module->setParameter('path', $realPath);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $isActive) = $module->isActive();
		if ($ret) {
		    return array($ret, null);
		}
		$redirect['view'] = 'core.SiteAdmin';
		if ($isActive) {
		    $redirect['subView'] = 'jpegtran.AdminJpegtran';
		    $status['saved'] = 1;
		} else {
		    $redirect['subView'] = 'core.AdminPlugins';
		    $status['configured'] = 'jpegtran';
		}
	    }
    	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'jpegtran.AdminJpegtran';
	} else if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminPlugins';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'jpegtran.AdminJpegtran';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Jpegtran
 */
class AdminJpegtranView extends GalleryView {

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

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'jpegtran');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load our default values if we didn't just come from this form. */
	if ($form['formName'] != 'AdminJpegtran') {
	    $form['formName'] = 'AdminJpegtran';

	    list ($ret, $form['path']) = $module->getParameter('path');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$tests = array();
	$debugSnippet = '';
	$failCount = 0;
	if (isset($form['action']['test']) && empty($form['error'])) {
	    if (!empty($form['path'])) {
		$gallery->startRecordingDebugSnippet();
		list ($ret, $testResults) =
			JpegtranToolkitHelper::testBinary($platform->realpath($form['path']));
		$debugSnippet = $gallery->stopRecordingDebugSnippet();

		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_BAD_PATH) {
			$testResults = array();
			$form['error']['path']['badPath'] = 1;
		    } else {
			return array($ret, null);
		    }
		} else {
		    foreach ($testResults as $testResult) {
			if (!$testResult['success']) {
			    $failCount++;
			}
			$tests[] = $testResult;
		    }
		}
	    } else {
		$form['error']['path']['missing'] = 1;
	    }
	}

	list ($ret, $isActive) = $module->isActive();
	if ($ret) {
	    return array($ret, null);
	}

	$AdminJpegtran = array();
	$AdminJpegtran['tests'] = $tests;
	$AdminJpegtran['debugSnippet'] = $debugSnippet;
	$AdminJpegtran['failCount'] = $failCount;
	$AdminJpegtran['isConfigure'] = !$isActive;
	$AdminJpegtran['canExec'] =
	    !in_array('exec', preg_split('/,\s*/', ini_get('disable_functions')));

	if ($failCount > 0) {
	    $template->javascript('lib/javascript/BlockToggle.js');
	}

	$template->setVariable('AdminJpegtran', $AdminJpegtran);
	$template->setVariable('controller', 'jpegtran.AdminJpegtran');

	return array(null, array('body' => 'modules/jpegtran/templates/AdminJpegtran.tpl'));
    }
}
?>
