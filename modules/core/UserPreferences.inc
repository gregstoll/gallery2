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
 * This controller will handle changes made to a user
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class UserPreferencesController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}
	if ($isAnonymous) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	$results = $status = $error = array();
	if (isset($form['action']['cancel'])) {

	    /* Redirect back to the previous view */
	    $results['return'] = true;

	} else if (isset($form['action']['undo'])) {

	    /* Redirect back to the same view to reset the form */
	    $redirect['view'] = 'core.UserAdmin';
	    $redirect['subView'] = 'core.UserPreferences';

	} else if (isset($form['action']['save'])) {

	    /* Validate our data before we continue. */

	    /* full name is optional */

	    /* language is optional for everybody */

	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret) {
		return array($ret, null);
	    }

	    $userId = $gallery->getActiveUserId();
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($userId);
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	    if ($ret) {
		return array($ret, null);
	    }

            /* Locked users are not allowed to edit themselves */
            if ($user->isLocked()) {
                GalleryCoreApi::releaseLocks($lockId);
                return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
            }

	    /* email is required for non-anonymous but optional for administrators */
	    if (empty($form['email']) && !$isAdmin) {
		$error[] = 'form[error][email][missing]';
	    } else if (!empty($form['email'])) {
		$form['email'] = trim($form['email']);
		if (!GalleryUtilities::isValidEmailString($form['email'])) {
		    $error[] = 'form[error][email][invalid]';
		}
	    }

	    if (!empty($form['currentPassword'])) {
		GalleryUtilities::unsanitizeInputValues($form['currentPassword'], false);
		if (!$user->isCorrectPassword($form['currentPassword'])) {
		    $error[] = 'form[error][currentPassword][incorrect]';

		    $event = GalleryCoreApi::newEvent('Gallery::FailedLogin');
		    $event->setData(array('userName' => $user->getUserName()));
		    list ($ret, $ignored) = GalleryCoreApi::postEvent($event);
		    if ($ret) {
			return array($ret, null);
		    }
		} else {
		/*
		 * To be consistent, we really should post a Gallery::Login event here to show
		 * that the user successfully authenticated.  But they're not really logging in so
		 * that's misleading.  Perhaps we should rename the events to Gallery::Authenticate
		 * and Gallery::FailedAuthenticate to be more accurate, and then post those here?
		 *
		 * @see UserChangePasswordController::handleRequest
		 */
		}
	    }

	    /* Set a default if for some reason it doesn't get sent from the browser */
	    $form['email'] = empty($form['email']) ? '' : $form['email'];

	    /* Require a password to change the email address */
	    if ($form['email'] != $user->getEmail()) {
		if (empty($form['currentPassword'])) {
		    $error[] = 'form[error][currentPassword][missing]';
		}
	    }

	    /* If all the right fields are in place then go ahead and modify the user. */
	    if (empty($error)) {
		$user->setFullname(trim($form['fullName']));
		$user->setEmail($form['email']);

		if (isset($form['language'])) {
		    $user->setLanguage($form['language']);
		    /* update the effective language in the current session */
		    $session =& $gallery->getSession();
		    $session->put('core.language', $form['language']);
		}

		$ret = $user->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.UserAdmin';
		$redirect['subView'] = 'core.UserPreferences';
		$status['saved'] = 1;
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = 'core.UserPreferences';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show a form to change user options
 */
class UserPreferencesView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}
	if ($isAnonymous) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	$user = $gallery->getActiveUser();

        /* Locked users aren't allowed to edit themselves - display message */
        if ($user->isLocked()) {
            return array(null, array('body' => 'modules/core/templates/UserLocked.tpl'));
        }

	/* Load the form with user data the first time around. */
	if ($form['formName'] != 'UserPreferences') {
	    $form['userName'] = $user->getUserName();
	    $form['fullName'] = $user->getFullName();
	    $form['email'] = $user->getEmail();
	    $form['language'] = $user->getLanguage();
	    $form['formName'] = 'UserPreferences';
	}

	/* Set up our language selection list */
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	$languageList = array();
	$languageList[''] = $module->translate('&lt;none&gt;');
	$translator =& $gallery->getTranslator();
	$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	foreach ($supportedLanguages as $language => $countryList) {
	    foreach ($countryList as $country => $languageData) {
		$languageList[$language . '_' . $country] = $languageData['description'];
	    }
	}

	$UserPreferences['translationsSupported'] = $translator->canTranslate();
	$UserPreferences['languageList'] = $languageList;

	$template->setVariable('UserPreferences', $UserPreferences);
	$template->setVariable('controller', 'core.UserPreferences');
	return array(null, array('body' => 'modules/core/templates/UserPreferences.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('your account'));
    }
}
?>
