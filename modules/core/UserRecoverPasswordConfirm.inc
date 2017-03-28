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

GalleryCoreApi::requireOnce('modules/core/classes/helpers/UserRecoverPasswordHelper_simple.class');

/**
 * This controller will handle the recovery of passwords that have been lost or forgotten
 * by the user.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Jay Rossiter <cryptographite@users.sf.net>
 * @version $Revision: 17580 $
 */
class UserRecoverPasswordConfirmController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$status = $results = $error = array();

	if (isset($form['action']['submit'])) {
	    if (empty($form['userName'])) {
		$error[] = 'form[error][userName][missing]';
	    }

	    if (empty($form['authString'])) {
		$error[] = 'form[error][authString][missing]';
	    }

	    if (empty($form['password1']) || empty($form['password2'])) {
		$error[] = 'form[error][password][missing]';
	    } else if ($form['password1'] != $form['password2']) {
		$error[] = 'form[error][password][mismatch]';
	    }

	    /* No errors?  Check the DB for the request and then update the user's password */
	    if (empty($error)) {
		list ($ret, $user) = GalleryCoreApi::fetchUserByUsername($form['userName']);
		if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
		/*
		 * Check the database to see if a previous request.
		 * If a request exists, check the timestamp to see if a new request can be
		 * generated, or if they will be denied because the window is too small.
		 */
		list ($ret, $requestExpires) = UserRecoverPasswordHelper_simple::getRequestExpires(
		    $form['userName'], $form['authString']);
		if ($ret) {
		    return array($ret, null);
		}

		if ($user && !empty($requestExpires)) {
		    if ($requestExpires < time()) {
			/*
			 * This request was made more than 7 days ago
			 * purge it from the system and redirect to the request page
			 */
			$error[] = 'form[error][request][tooOld]';
		    } else if (!empty($user)) {
			list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($user->getId());
			if ($ret) {
			    return array($ret, null);
			}

			list ($ret, $user) = $user->refresh();
			if ($ret) {
			    return array($ret, null);
			}

			$user->changePassword($form['password1']);

			$ret = $user->save();
			if ($ret) {
			    return array($ret, null);
			}

			$ret = GalleryCoreApi::releaseLocks($lockId);
			if ($ret) {
			    return array($ret, null);
			}

			$ret = GalleryCoreApi::removeMapEntry(
			    'FailedLoginsMap',
			    array('userName' => $user->getUserName()));
			if ($ret) {
			    return array($ret, null);
			}

			$status['passwordRecovered'] = 1;
		    }
		    $ret = GalleryCoreApi::removeMapEntry(
			'GalleryRecoverPasswordMap',
			array('userName' => $user->getUserName()));
		    if ($ret) {
			return array($ret, null);
		    }
		} else {
		    /* There is no matching request for this userName and authString combo */
		     $error[] = 'form[error][request][missing]';
		}
	    }
	} else if (isset($form['action']['cancel'])) {
	    $results['return'] = 1;
	}

	if (empty($error) && !empty($status)) {
	    $results['redirect']['view'] = 'core.UserAdmin';
	    $results['redirect']['subView'] = 'core.UserLogin';
	    $results['return'] = 0;
	} else {
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = 'core.UserRecoverPasswordConfirm';
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view shows information about password recovery
 */
class UserRecoverPasswordConfirmView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	if ($form['formName'] == 'UserRecoverPasswordConfirm') {
	    if (empty($form['password1']) || empty($form['password2'])) {
		$form['error']['password']['missing'] = 1;
	    }
	} else {
	    $form['userName'] = GalleryUtilities::getRequestVariables('userName');
	    $form['authString'] = GalleryUtilities::getRequestVariables('authString');
	    $form['formName'] = 'UserRecoverPasswordConfirm';
	}

	list ($ret, $requestExpires) = UserRecoverPasswordHelper_simple::getRequestExpires(
	    $form['userName'], $form['authString']);
	if ($ret) {
	    return array($ret, null);
	}

	if (empty($requestExpires)) {
	    return array(null,
			 array('redirect' => array('view' => 'core.UserAdmin',
						   'subView' => 'core.UserRecoverPassword')));
	} else {
	    $UserRecoverPasswordConfirm = array();
	    $template->setVariable('UserRecoverPasswordConfirm', $UserRecoverPasswordConfirm);
	    $template->setVariable('controller', 'core.UserRecoverPasswordConfirm');
	    return array(null,
			 array('body' => 'modules/core/templates/UserRecoverPasswordConfirm.tpl'));
	}
    }
}
?>
