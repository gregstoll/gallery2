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
 * This controller will handle changes made to the user-group mapping
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminEditGroupUsersController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $error = $status = array();

	/* Load the group */
	$groupId = GalleryUtilities::getRequestVariables('groupId');
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($form['action']['filterClear'])) {

	    /* Clear the filter */
	    GalleryUtilities::putRequestVariable('form[list][filter]', null);

	} else if (isset($form['action']['done'])) {

	    /* Go back to the AdminGroups view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminGroups';

	} else if ($group->getGroupType() != GROUP_ALL_USERS) {
	    if (isset($form['action']['remove'])) {

		/* Remove the user from the group */
		$removedUsers = 0;
		if (empty($form['userIds'])) {
		    $error[] = 'form[error][list][noUserSelected]';
		} else {
		    foreach ($form['userIds'] as $userId => $dummy) {
			/* Can't remove yourself from the site admins list */
			if ($group->getGroupType() == GROUP_SITE_ADMINS &&
				$userId == $gallery->getActiveUserId()) {
			    $error[] = 'form[error][list][cantRemoveSelf]';
			    break;
			}
		    }
		}

		if (empty($error)) {
		    foreach ($form['userIds'] as $userId => $dummy) {
			list ($ret, $user) =
			    GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
			if ($ret) {
			    return array($ret, null);
			}

			$ret = GalleryCoreApi::removeUserFromGroup($user->getId(), $groupId);
			if ($ret) {
			    return array($ret, null);
			}
			$removedUsers++;
		    }

		    /* Redirect back to the same view */
		    $redirect['view'] = 'core.SiteAdmin';
		    $redirect['subView'] = 'core.AdminEditGroupUsers';
		    $redirect['groupId'] = $group->getId();
		    $status['removedUsers'] = $removedUsers;
		    $status['removedUser'] =
			$removedUsers == 1 ? $user->getUsername() : $removedUsers;
		}

	    } else if (isset($form['action']['add'])) {

		/* Add the user to the group */
		if (empty($form['text']['userName'])) {
		    $error[] = 'form[error][text][userName][missing]';
		} else {
		    list ($ret, $user) =
			GalleryCoreApi::fetchUserByUsername($form['text']['userName']);
		    if ($ret && ($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
			$error[] = 'form[error][text][userName][invalid]';
		    } else if ($ret) {
			return array($ret, null);
		    } else {
			/* Is the user already in the group? */
			list ($ret, $inGroup) =
			    GalleryCoreApi::isUserInGroup($user->getId(), $groupId);
			if ($ret) {
			    return array($ret, null);
			}
			if ($inGroup) {
			    $error[] = 'form[error][text][userName][alreadyInGroup]';
			}
		    }
		}

		if (empty($error)) {
		    $ret = GalleryCoreApi::addUserToGroup($user->getId(), $groupId);
		    if ($ret) {
			return array($ret, null);
		    }

		    /* Redirect back to the same view */
		    $redirect['view'] = 'core.SiteAdmin';
		    $redirect['subView'] = 'core.AdminEditGroupUsers';
		    $redirect['groupId'] = $group->getId();
		    $status['addedUser'] = $user->getUsername();
		}
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminEditGroupUsers';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show the possibilities of user-group mappings for a specified group.
 */
class AdminEditGroupUsersView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Set some defaults, if necessary */
	if (!isset($form['list']['filter'])) {
	    $form['list']['filter'] = '';
	}
	if (empty($form['list']['page'])) {
	    $form['list']['page'] = 1;
	}

	/* Load the group */
	$groupId = GalleryUtilities::getRequestVariables('groupId');
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}

	/* First time on this page, initialize form fields */
	if ($form['formName'] != 'AdminEditGroupUsers') {
	    $form['text']['userName'] = '';
	    $form['formName'] = 'AdminEditGroupUsers';
	}

	/* Initialize the user list */
	list ($ret, $totalUserCount) = GalleryCoreApi::fetchUserCount(null, $group->getId());
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['count'] = $totalUserCount;
	$form['list']['pageSize'] = $totalUserCount > 10 ? 10 : $totalUserCount + 2;

	if (!empty($form['list']['filter'])) {
	    list ($ret, $form['list']['count']) = GalleryCoreApi::fetchUserCount(
							$form['list']['filter'],
							$group->getId()
						  );
	}

	/* Figure out our max pages, make sure our current page fits in it */
	$form['list']['maxPages'] = ceil($form['list']['count'] / $form['list']['pageSize']);
	if ($form['list']['page'] > $form['list']['maxPages']) {
	    $form['list']['page'] = $form['list']['maxPages'];
	}

	/* Calculate the next/back pages */
	$form['list']['nextPage'] = min($form['list']['page']+1, $form['list']['maxPages']);
	$form['list']['backPage'] = max(1, $form['list']['page']-1);

	list ($ret, $users) = GalleryCoreApi::fetchUsersForGroup(
					$group->getId(),
					$form['list']['pageSize'],
					(($form['list']['page'] - 1) * $form['list']['pageSize']),
					$form['list']['filter']
			      );
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['userNames'] = array();
	$canRemove = false;
	foreach ($users as $userId => $userName) {
	    $form['list']['userNames'][$userId]['userName'] = $userName;
	    if ($group->getGroupType() == GROUP_SITE_ADMINS
		&& $userId == $gallery->getActiveUserId()) {
		$form['list']['userNames'][$userId]['can']['remove'] = false;
	    } else {
		$form['list']['userNames'][$userId]['can']['remove'] = true;
		$canRemove = true;
	    }
	}

	$AdminEditGroupUsers = array();
	$AdminEditGroupUsers['totalUserCount'] = $totalUserCount;
	$AdminEditGroupUsers['canRemove'] = $canRemove;
	$AdminEditGroupUsers['group'] = (array)$group;

	$template->setVariable('AdminEditGroupUsers', $AdminEditGroupUsers);
	$template->setVariable('controller', 'core.AdminEditGroupUsers');
	return array(null, array('body' => 'modules/core/templates/AdminEditGroupUsers.tpl'));
    }
}
?>
