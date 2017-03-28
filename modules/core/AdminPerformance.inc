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
 * Update performance settings
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminPerformanceController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	list ($ret, $acceleration) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'acceleration');
	if ($ret) {
	    return array($ret, null);
	}
	if ($acceleration) {
	    $acceleration = unserialize($acceleration);
	}

	if (isset($form['action']['save'])) {
	    foreach (array('user', 'guest') as $class) {
		if (!isset($form['acceleration'][$class])) {
		    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
						       "Missing '$class' from the form"), null);
		}
		$acceleration[$class]['type'] = $form['acceleration'][$class]['type'];
		switch ($form['acceleration'][$class]['type']) {
		case 'partial':
		case 'full':
		    $acceleration[$class]['expiration'] =
			(int)$form['acceleration'][$class]['expiration'];
		    break;

		case 'none':
		    if (!isset($acceleration[$class]['expiration'])) {
			$acceleration[$class]['expiration'] = 0;
		    }
		    break;

		default:
		    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'core', 'acceleration', serialize($acceleration));
	    if ($ret) {
		return array($ret, null);
	    }

	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'core', 'smarty.compile_check', (int)empty($form['disableCompileCheck']));
	    if ($ret) {
		return array($ret, null);
	    }

	    $status['saved'] = 1;
	    $redirect = array('view' => 'core.SiteAdmin',
			      'subView' => 'core.AdminPerformance');
	} else if (isset($form['action']['clear'])) {
	    $ret = GalleryCoreApi::removeAllMapEntries('GalleryCacheMap');
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['cleared'] = 1;
	    $redirect = array('view' => 'core.SiteAdmin',
			      'subView' => 'core.AdminPerformance');
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else if (empty($results['delegate'])) {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminPerformance';
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Update performance settings
 */
class AdminPerformanceView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load some standard form parameters */
	if ($form['formName'] != 'AdminPerformance') {
	    $form['formName'] = 'AdminPerformance';

	    list ($ret, $acceleration) = $module->getParameter('acceleration');
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($acceleration) {
		$form['acceleration'] = unserialize($acceleration);
	    }
	    foreach (array('user', 'guest') as $class) {
		if (!isset($form['acceleration'][$class]['type'])) {
		    $form['acceleration'][$class]['type'] = 'none';
		}
		if (!isset($form['acceleration'][$class]['expiration'])) {
		    $form['acceleration'][$class]['expiration'] = '0';
		}
	    }

	    list ($ret, $compileCheck) = $module->getParameter('smarty.compile_check');
	    if ($ret) {
		return array($ret, null);
	    }
	    $form['disableCompileCheck'] = !$compileCheck;
	}

	$form['disableCompileCheck'] = !empty($form['disableCompileCheck']);

	/* Set up our type lists */
	$typeList = array('none' => $module->translate('No acceleration'),
			  'partial' => $module->translate('Partial acceleration'),
			  'full' => $module->translate('Full acceleration'));

	/* Set up our time lists */
	$expirationTimeLists = array();
	foreach (array(15, 30, 45) as $minutes) {
	    $expirationTimeList[$minutes * 60] =
		$module->translate(array('one' => '%d minute',
					 'many' => '%d minutes',
					 'count' => $minutes,
					 'arg1' => $minutes));
	}
	foreach (array(1, 6, 12) as $hours) {
	    $expirationTimeList[$hours * 3600] =
		$module->translate(array('one' => '%d hour',
					 'many' => '%d hours',
					 'count' => $hours,
					 'arg1' => $hours));
	}
	foreach (array(1, 2, 3, 4, 5, 6) as $days) {
	    $expirationTimeList[$days * 86400] =
		$module->translate(array('one' => '%d day',
					 'many' => '%d days',
					 'count' => $days,
					 'arg1' => $days));
	}
	foreach (array(1, 2, 3) as $weeks) {
	    $expirationTimeList[$weeks * 7 * 86400] =
		$module->translate(array('one' => '%d week',
					 'many' => '%d weeks',
					 'count' => $weeks,
					 'arg1' => $weeks));
	}

	$AdminPerformance = array();
	$AdminPerformance['expirationTimeList'] = $expirationTimeList;
	$AdminPerformance['typeList'] = $typeList;

	$template->setVariable('AdminPerformance', $AdminPerformance);
	$template->setVariable('controller', 'core.AdminPerformance');

	return array(null, array('body' => 'modules/core/templates/AdminPerformance.tpl'));
    }
}

?>
