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
 * This controller will handle changes made to a group
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminEditGroupController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	$groupId = GalleryUtilities::getRequestVariables('groupId');
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($form['action']['cancel'])) {

	    /* Redirect back to the AdminGroups view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminGroups';

	} else if (isset($form['action']['undo'])) {

	    /* Redirect back to the same view to reset the form */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminEditGroup';
	    $redirect['groupId'] = $groupId;

	} else if (isset($form['action']['save'])) {

	    if (empty($form['groupName'])) {
		$error[] = 'form[error][groupName][missing]';
	    } else {
		list ($ret, $tmpGroup) = GalleryCoreApi::fetchGroupByGroupname($form['groupName']);
		if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}

		if (!empty($tmpGroup) && $tmpGroup->getId() != $groupId) {
		    $error[] = 'form[error][groupName][exists]';
		}
	    }

	    if (empty($error)) {
		/* Save our changes */
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($groupId);
		if ($ret) {
		    return array($ret, null);
		}

		/* Refresh our object, now that it's locked */
		list ($ret, $group) = $group->refresh();
		if ($ret) {
		    return array($ret, null);
		}

		/* Enact the change */
		$group->setGroupName($form['groupName']);
		$ret = $group->save();
		if ($ret) {
		    return array($ret, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminGroups';
		$status['modifiedGroup'] = $group->getGroupName();
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminEditGroup';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show options available to groups
 */
class AdminEditGroupView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$groupId = GalleryUtilities::getRequestVariables('groupId');
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminEditGroup') {
	    /* First time around initialize our form */
	    $form['groupName'] = $group->getGroupName();
	    $form['formName'] = 'AdminEditGroup';
	}

	$AdminEditGroup = array();
	$AdminEditGroup['group'] = (array)$group;
	$template->setVariable('AdminEditGroup', $AdminEditGroup);
	$template->setVariable('controller', 'core.AdminEditGroup');

	return array(null, array('body' => 'modules/core/templates/AdminEditGroup.tpl'));
    }
}
?>
