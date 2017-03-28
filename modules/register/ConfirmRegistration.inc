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
 * This controller will handle the confirmation-link that is sent in the confirmation email
 * @package Registration
 * @subpackage UserInterface
 * @author Sebastian Eichner <mailsp@sebastian-eichner.de>
 * @version $Revision: 17580 $
 */
class ConfirmRegistrationController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	GalleryCoreApi::requireOnce(
	    'modules/register/classes/GalleryPendingUserHelper.class');
	$results = array('status' => array(), 'error' => array());

	/* turn the pending user into a real user */
	list ($username, $regKey) = GalleryUtilities::getRequestVariables('username', 'key');

	list ($ret, $user) = GalleryPendingUserHelper::fetchPendingUserByUsername($username);
	if ($ret) {
	    if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		/*
		 * If the username is now a real user then maybe we got here via return-url
		 * after login.. let's just return to previous url instead of showing error msg.
		 */
		list ($ret, $user) = GalleryCoreApi::fetchUserByUsername($username);
		if (!$ret) {
		    $results['redirect'] = array('return' => true);
		} else {
		    $results['error'][] = 'form[error][unknownUser]';
		}
	    } else {
		return array($ret, null);
	    }
	} else {
	    /* verify registration key: */
	    if ($regKey != $user->getRegistrationKey()) {
		$results['error'][] = 'form[error][unknownUser]';
	    } else {
		$ret = GalleryPendingUserHelper::createGalleryUser($user);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	if (empty($results['redirect'])) {
	    $results['delegate'] = array('view' => 'core.UserAdmin',
					 'subView' => 'register.ConfirmRegistration');
	}

	return array(null, $results);
    }
}

/**
 * This view shows a confirmation that the users account was activated
 */
class ConfirmRegistrationView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$template->setVariable('controller', 'register.ConfirmRegistration');
	return array(null, array('body' => 'modules/register/templates/ConfirmRegistration.tpl'));
    }
}
?>
