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
 * This controller will send the user to the chosen subView in the AdminGroups
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminGroupsController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();

	$group = null;
	if (!empty($form['text']['groupName'])) {
	    list ($ret, $group) =
		GalleryCoreApi::fetchGroupByGroupName($form['text']['groupName']);
	    if ($ret) {
		if (!($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
	    }
	}

	if (isset($form['action']['filterClear'])) {

	    /* Clear the filter */
	    GalleryUtilities::putRequestVariable('form[list][filter]', null);

	} else if (isset($form['action']['create'])) {

	    /* Show the "create group" view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminCreateGroup';

	} else if (isset($form['action']['editFromText'])) {

	    if (empty($form['text']['groupName'])) {
		$error[] = 'form[error][text][noGroupSpecified]';
	    } else if ($group == null) {
		$error[] = 'form[error][text][noSuchGroup]';
	    } else {
		/* Show the "edit group" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminEditGroup';
		$redirect['groupId'] = $group->getId();
	    }

	} else if (isset($form['action']['deleteFromText'])) {

	    if (empty($form['text']['groupName'])) {
		$error[] = 'form[error][text][noGroupSpecified]';
	    } else if ($group == null) {
		$error[] = 'form[error][text][noSuchGroup]';
	    } else {
		/*
		 * In theory we should never get to this point unless we're operating on a valid
		 * group, so don't bother sending errors back in case we can't delete.
		 */
		if ($group->getGroupType() != GROUP_NORMAL) {
		    $error[] = 'form[error][text][cantDeleteGroup]';
		}
	    }

	    if (empty($error)) {
		/* Show the "delete group" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminDeleteGroup';
		$redirect['groupId'] = $group->getId();
	    }

	} else if (isset($form['action']['addRemoveUsersFromText'])) {

	    if (empty($form['text']['groupName'])) {
		$error[] = 'form[error][text][noGroupSpecified]';
	    } else if ($group == null) {
		$error[] = 'form[error][text][noSuchGroup]';
	    } else {
		/*
		 * In theory we should never get to this point unless we're operating on a valid
		 * group, so don't bother sending errors back in case we can't delete.
		 */
		if ($group->getGroupType() == GROUP_ALL_USERS ||
		    $group->getGroupType() == GROUP_EVERYBODY) {
		    $error[] = 'form[error][text][cantEditGroupUsers]';
		}
	    }

	    if (empty($error)) {
		/* Show the "edit group's users" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminEditGroupUsers';
		$redirect['groupId'] = $group->getId();
	    }

	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminGroups';
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show available options to administer the groups of Gallery
 */
class AdminGroupsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load some standard form parameters */
	if ($form['formName'] != 'AdminGroups') {
	    $form['text']['groupName'] = '';
	    $form['formName'] = 'AdminGroups';
	}

	/* Set some defaults, if necessary */
	if (!isset($form['list']['filter'])) {
	    $form['list']['filter'] = '';
	}

	if (empty($form['list']['page'])) {
	    $form['list']['page'] = 1;
	}

	list ($ret, $totalGroupCount) = GalleryCoreApi::fetchGroupCount();
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['count'] = $totalGroupCount;
	$form['list']['pageSize'] = $totalGroupCount > 10 ? 10 : $totalGroupCount + 2;

	/* If we have a filter, find out how many groups match it */
	if (!empty($form['list']['filter'])) {
	    list ($ret, $form['list']['count']) =
		GalleryCoreApi::fetchGroupCount($form['list']['filter']);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Figure out our max pages, make sure our current page fits in it */
	$form['list']['maxPages'] = ceil($form['list']['count'] / $form['list']['pageSize']);
	if ($form['list']['page'] > $form['list']['maxPages']) {
	    $form['list']['page'] = $form['list']['maxPages'];
	}

	/* Calculate the next/back pages */
	$form['list']['nextPage'] = min($form['list']['page']+1, $form['list']['maxPages']);
	$form['list']['backPage'] = max(1, $form['list']['page']-1);

	list ($ret, $groupNames) =
	    GalleryCoreApi::fetchGroupNames($form['list']['pageSize'],
						(($form['list']['page'] - 1) *
						 $form['list']['pageSize']),
						$form['list']['filter']);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $allUserGroupId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.allUserGroup');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $everybodyGroupId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.everybodyGroup');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $adminGroupId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.adminGroup');
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['groupNames'] = array();
	foreach ($groupNames as $groupId => $groupName) {
	    $form['list']['groupNames'][$groupId]['groupName'] = $groupName;
	    if ($groupId == $allUserGroupId) {
		$form['list']['groupNames'][$groupId]['can']['delete'] = false;
		$form['list']['groupNames'][$groupId]['can']['editUsers'] = false;
	    } else if ($groupId == $everybodyGroupId) {
		$form['list']['groupNames'][$groupId]['can']['delete'] = false;
		$form['list']['groupNames'][$groupId]['can']['editUsers'] = false;
	    } else if ($groupId == $adminGroupId) {
		$form['list']['groupNames'][$groupId]['can']['delete'] = false;
		$form['list']['groupNames'][$groupId]['can']['editUsers'] = true;
	    } else {
		$form['list']['groupNames'][$groupId]['can']['delete'] = true;
		$form['list']['groupNames'][$groupId]['can']['editUsers'] = true;
	    }
	}

	$AdminGroups = array();
	$AdminGroups['totalGroupCount'] = $totalGroupCount;

	$template->setVariable('AdminGroups', $AdminGroups);
	$template->setVariable('controller', 'core.AdminGroups');
	return array(null, array('body' => 'modules/core/templates/AdminGroups.tpl'));
    }
}
?>
