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
 * This controller will handle changes made to an user
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminEditUserController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();

	$userId = GalleryUtilities::getRequestVariables('userId');
	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	/* We treat anonymous users differently, so see if we're editing the anonymous user */
	list ($ret, $anonymousUserId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.anonymousUser');
	if ($ret) {
	    return array($ret, null);
	}
	$isAnonymous = ($userId == $anonymousUserId);

	if (isset($form['action']['cancel'])) {

	    /* Redirect back to the AdminUsers view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminUsers';

	} else if (isset($form['action']['undo'])) {

	    /* Redirect back to the same view to reset the form */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminEditUser';
	    $redirect['userId'] = $userId;

	} else if (isset($form['action']['save'])) {
	    /* Validate our data before we continue. */

	    /* username is required and might conflict */
	    if (empty($form['userName'])) {
		$error[] = 'form[error][userName][missing]';
	    }

	    if (!empty($form['userName'])) {
		list ($ret, $tmpUser) = GalleryCoreApi::fetchUserByUsername($form['userName']);
		if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}

		if (!empty($tmpUser) && $tmpUser->getId() != $userId) {
		    $error[] = 'form[error][userName][duplicate]';
		}
	    }

	    /* full name is optional for everybody */

	    /* language is optional for everybody */

	    /* email is suggested, but optional for administrator edits */
	    if (!empty($form['email'])) {
		$form['email'] = trim($form['email']);
		if (!GalleryUtilities::isValidEmailString($form['email'])) {
		    $error[] = 'form[error][email][invalid]';
		}
	    }

	    if (!$isAnonymous) {
		if ($form['password1'] != $form['password2']) {
		    $error[] = 'form[error][password2][mismatch]';
		}
	    }

	    if (isset($form['action']['resetFailedLogins'])) {
		$ret = GalleryCoreApi::removeMapEntry(
		    'FailedLoginsMap', array('userName' => $user->getUserName()));
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    /* If all the right fields are in place then go ahead and modify the user. */
	    if (empty($error)) {
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($user->getId());
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $user) = $user->refresh();
		if ($ret) {
		    return array($ret, null);
		}

		$user->setUserName(trim($form['userName']));
		$user->setFullname(trim($form['fullName']));
                $user->setLocked(!empty($form['locked']) ? 1 : 0);
		if (!$isAnonymous) {
		    $user->setEmail($form['email']);
		    $user->setLanguage($form['language']);
		    if (!empty($form['password1'])) {
			GalleryUtilities::unsanitizeInputValues($form['password1'], false);
			$user->changePassword($form['password1']);
		    }
		}

		$ret = $user->save();
		if ($ret) {
		    return array($ret, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminUsers';
		$status['modifiedUser'] = $user->getUserName();
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminEditUser';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show a form to change user options
 */
class AdminEditUserView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$userId = GalleryUtilities::getRequestVariables('userId');
	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

        list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup($userId);
        if ($ret) {
            return array($ret, null);
        }

	/* Load the form with user data the first time around. */
	if ($form['formName'] != 'AdminEditUser') {
	    $form['userName'] = $user->getUserName();
	    $form['fullName'] = $user->getFullName();
	    $form['email'] = $user->getEmail();
	    $form['language'] = $user->getLanguage();
	    $form['formName'] = 'AdminEditUser';
            $form['locked'] = $user->isLocked();
	}

	/* Email is not required for the anonymous user */
	list ($ret, $anonymousUserId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.anonymousUser');
	if ($ret) {
	    return array($ret, null);
	}

	$show['email'] = ($userId != $anonymousUserId);
	$show['language'] = ($userId != $anonymousUserId);
	$show['password'] = ($userId != $anonymousUserId);

        /* Only show the lock option if the user being edited is not an admin */
        $show['locked'] = !$isAdmin;

	if ($show['language']) {
	    /* Set up our language selection list */
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null);
	    }

	    $languageList = array();
	    $languageList[''] = $module->translate('&lt;none&gt;');
	    $supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	    foreach ($supportedLanguages as $language => $countryList) {
		foreach ($countryList as $country => $languageData) {
		    $languageList[$language . '_' . $country] = $languageData['description'];
		}
	    }
	}

	list ($ret, $searchResults) = GalleryCoreApi::getMapEntry(
	    'FailedLoginsMap',
	    array('count'),
	    array('userName' => $user->getUserName()));
	if ($ret) {
	    return array($ret, null);
	}

	if ($searchResults->resultCount() > 0) {
	    $result = $searchResults->nextResult();
	    $failedLoginCount = $result[0];
	} else {
	    $failedLoginCount = 0;
	}

	$AdminEditUser = array();
	$AdminEditUser['show'] = $show;
	$AdminEditUser['user'] = (array)$user;
	$AdminEditUser['failedLoginCount'] = $failedLoginCount;
	if (isset($languageList)) {
	    $AdminEditUser['languageList'] = $languageList;
	}

	$template->setVariable('AdminEditUser', $AdminEditUser);
	$template->setVariable('controller', 'core.AdminEditUser');
	return array(null, array('body' => 'modules/core/templates/AdminEditUser.tpl'));
    }
}
?>
