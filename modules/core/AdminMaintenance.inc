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

GalleryCoreApi::requireOnce('modules/core/classes/helpers/MaintenanceHelper_simple.class');

/**
 * This controller will perform the chosen task in AdminMaintenance
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminMaintenanceController extends GalleryController {

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
	if (isset($form['action']['runTask'])) {
	    list ($ret, $this->_module) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null);
	    }

	    $mode = $gallery->getConfig('mode.maintenance');

	    $taskId = GalleryUtilities::getRequestVariables('taskId');

	    list ($ret, $tasks) = MaintenanceHelper_simple::fetchTasks();
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($tasks[$taskId])) {
		$task =& $tasks[$taskId];

		if (!$task->requiresMaintenanceMode() || $mode) {
		    if ($task->requiresProgressBar()) {
			$templateAdapter =& $gallery->getTemplateAdapter();
			$templateAdapter->registerTrailerCallback(
					array($this, 'runTask'), array($taskId, $task, true));
			$results['delegate']['view'] = 'core.ProgressBar';
		    } else {
			list ($ret, $success)  = $this->runTask($taskId, $task, false);
			if ($ret) {
			    return array($ret, null);
			}

			$status['run'] = array('taskId' => $taskId, 'success' => $success);
			$redirect['view'] = 'core.SiteAdmin';
			$redirect['subView'] = 'core.AdminMaintenance';
		    }
		} else {
		    $results['delegate']['view'] = 'core.SiteAdmin';
		    $results['delegate']['subView'] = 'core.AdminMaintenance';
		    $error[] = 'form[error][maintenanceModeRequired]';
		}
	    }
	} else if (isset($form['action']['setMode'])) {
	    $platform =& $gallery->getPlatform();

	    $newMode = !empty($form['setMode']['mode']);
	    $maintenanceUrl = $form['setMode']['url'];

	    $statusMsg = $newMode ? 'setOn' : 'setOff';
	    if ($newMode && !empty($maintenanceUrl)) {
		$urlGenerator =& $gallery->getUrlGenerator();
	    	$newMode = $urlGenerator->makeAbsoluteUrl($maintenanceUrl);
	    }
	    $ret = GalleryCoreApi::setMaintenanceMode($newMode);
	    if ($ret) {
		return array($ret, null);
	    }

	    $ret = GalleryCoreApi::setPluginParameter('module', 'core',
						       'mode.maintenance.lastUrl', $maintenanceUrl);
	    if ($ret) {
		return array($ret, null);
	    }

	    $status['setMode'][$statusMsg] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminMaintenance';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else if (empty($results['delegate'])) {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminMaintenance';
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    function runTask($taskId, $task, $useProgressBar) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	list ($ret1, $success, $details) = $task->run();
	if ($ret1) {
	    $details = $ret1->getAsHtml();
	}
	$ret2 = MaintenanceHelper_simple::addRun($taskId, time(), $success, $details);

	if ($ret1 || $ret2) {
	    $ret = $ret1 ? $ret1 : $ret2;
	    return array($ret, null);
	}

	$status['run']['success'] = $success;
	$status['run']['taskId'] = $taskId;
	$session =& $gallery->getSession();
	$session->putStatus($status);

	$redirect['view'] = 'core.SiteAdmin';
	$redirect['subView'] = 'core.AdminMaintenance';

	if ($useProgressBar) {
	    $templateAdapter =& $gallery->getTemplateAdapter();
	    $templateAdapter->completeProgressBar($urlGenerator->generateUrl($redirect));
	}

	return array(null, $success);
    }
}

/**
 * This view will show available maintenance tasks
 */
class AdminMaintenanceView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load some standard form parameters */
	if ($form['formName'] != 'AdminMaintenance') {
	    $form['formName'] = 'AdminMaintenance';
	}

	list ($ret, $tasks) = MaintenanceHelper_simple::fetchTasks();
	if ($ret) {
	    return array($ret, null);
	}

	$AdminMaintenance = array();
	foreach ($tasks as $taskId => $task) {
	    list ($ret, $info) = MaintenanceHelper_simple::fetchLastRun($taskId);
	    if ($ret) {
		return array($ret, null);
	    }

	    $AdminMaintenance['tasks'][$taskId] = $task->getInfo();
	    if (isset($info)) {
		$AdminMaintenance['tasks'][$taskId]['runId'] = $info['runId'];
		$AdminMaintenance['tasks'][$taskId]['timestamp'] = $info['timestamp'];
		$AdminMaintenance['tasks'][$taskId]['success'] = $info['success'];
		$AdminMaintenance['tasks'][$taskId]['details'] = $info['details'];
	    } else {
		$AdminMaintenance['tasks'][$taskId]['runId'] = null;
		$AdminMaintenance['tasks'][$taskId]['timestamp'] = null;
		$AdminMaintenance['tasks'][$taskId]['success'] = null;
		$AdminMaintenance['tasks'][$taskId]['details'] = null;
	    }
	}

	$maintenanceMode = $gallery->getConfig('mode.maintenance');
	list ($ret, $maintenanceModeUrl) = GalleryCoreApi::getPluginParameter('module',
								'core', 'mode.maintenance.lastUrl');
	if ($ret) {
	    return array($ret, null);
	}

	if (!empty($maintenanceMode)) {
	    if (is_string($maintenanceMode)) {
	    	$maintenanceModeUrl = $maintenanceMode;
		$maintenanceMode = true;
	    }
	} else {
	}
	$AdminMaintenance['setMode']['mode'] = $maintenanceMode;
	$AdminMaintenance['setMode']['url'] = $maintenanceModeUrl;

	$template->setVariable('AdminMaintenance', $AdminMaintenance);
	$template->setVariable('controller', 'core.AdminMaintenance');

	$template->javascript('lib/javascript/BlockToggle.js');
	return array(null,
		     array('body' => 'modules/core/templates/AdminMaintenance.tpl'));
    }
}

/**
 * Base class for all maintenance tasks
 * @abstract
 */
class MaintenanceTask {

    /**
     * Return internationalized text about this task
     * @return array('title' => ..., 'description' => ..., 'l10Domain' => ...)
     */
    function getInfo() {
	return array();
    }

    /**
     * Execute the task
     * @return array GalleryStatus a status code
     *               boolean success or failure
     *               array string output text
     */
    function run() {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Will this task run so long that it requires a progress bar?
     * @return true or false
     */
    function requiresProgressBar() {
	return false;
    }

    /**
     * This task requires that the administrator sets Gallery into maintenance mode.
     * @return true or false
     */
    function requiresMaintenanceMode() {
	return false;
    }
}
?>
