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
 * This controller will handle changing a user password
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class UserChangePasswordController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$user = $gallery->getActiveUser();

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}
	if ($isAnonymous || $user->isLocked()) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	$results = $status = $error = array();
	if (isset($form['action']['cancel'])) {

	    /* Redirect back to user preferences */
	    $redirect['view'] = 'core.UserAdmin';
	    $redirect['subView'] = 'core.UserPreferences';

	} else if (isset($form['action']['undo'])) {

	    /* Redirect back to the same view to reset the form */
	    $redirect['view'] = 'core.UserAdmin';
	    $redirect['subView'] = 'core.UserChangePassword';

	} else if (isset($form['action']['save'])) {

	    /*
	     * Validate our data before we continue.
	     */
	    if (empty($form['currentPassword'])) {
		$error[] = 'form[error][currentPassword][missing]';
	    } else {
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
		 * @see UserPreferencesController::handleRequest
		 */
		}
	    }

	    if (empty($form['password1'])) {
		$error[] = 'form[error][password1][missing]';
	    }

	    if (empty($form['password2'])) {
		$error[] = 'form[error][password2][missing]';
	    }

	    if (!$error && $form['password1'] != $form['password2']) {
		$error[] = 'form[error][password2][mismatch]';
	    }

	    /*
	     * If all the right fields are in place then go ahead and modify
	     * the user.
	     */
	    if (empty($error)) {
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($user->getId());
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $user) = $user->refresh();
		if ($ret) {
		    return array($ret, null);
		}

		GalleryUtilities::unsanitizeInputValues($form['password1'], false);
		$user->changePassword($form['password1']);

		$ret = $user->save();
		if ($ret) {
		    return array($ret, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.UserAdmin';
		$redirect['subView'] = 'core.UserChangePassword';
		$status['changedPassword'] = 1;
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = 'core.UserChangePassword';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show a form to change a password
 */
class UserChangePasswordView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
        global $gallery;

	/* Load the form with user data the first time around. */
	if ($form['formName'] != 'UserChangePassword') {
	    $form['formName'] = 'UserChangePassword';
	}

	$user = $gallery->getActiveUser();

        if ($user->isLocked()) {
            return array(null, array('body' => 'modules/core/templates/UserLocked.tpl'));
        }

	$template->setVariable('controller', 'core.UserChangePassword');
	return array(null, array('body' => 'modules/core/templates/UserChangePassword.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('change password'));
    }
}
?>
