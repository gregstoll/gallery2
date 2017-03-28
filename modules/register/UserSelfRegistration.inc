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
 * This controller will handle the registration of a user
 * @package Registration
 * @subpackage UserInterface
 * @author Sebastian Eichner <mailsp@sebastian-eichner.de>
 * @version $Revision: 17580 $
 */
class UserSelfRegistrationController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	GalleryCoreApi::requireOnce('modules/register/classes/GalleryPendingUserHelper.class');

	$results = $status = $error = array();
	if (isset($form['action']['cancel'])) {

	    /* Go back to the AdminUsers view */
	    $redirect['view'] = 'core.UserAdmin';

	} else if (isset($form['action']['create'])) {

	    /* Validate inputs */
	    foreach (array('userName', 'email', 'password1', 'password2', 'fullName') as $key) {
		if (empty($form[$key])) {
		    $error[] = 'form[error][' . $key . '][missing]';
		}
	    }
	    if (!empty($form['email']) && !GalleryUtilities::isValidEmailString($form['email'])) {
		$error[] = 'form[error][email][invalid]';
	    }
	    if (!empty($form['password1']) && !empty($form['password2']) &&
			$form['password1'] != $form['password2']) {
		$error[] = 'form[error][password2][mismatch]';
	    }

	    list ($ret, $level) =
		GalleryCoreApi::getPluginParameter('module', 'register', 'validation.level');
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($level == 'OFF') {
		$pluginInstances = array();
	    } else if (isset($this->_pluginInstances)) {
		$pluginInstances = $this->_pluginInstances;
	    } else {
		list ($ret, $pluginInstances) =
		    GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
		if ($ret) {
		    return array($ret, null);
		}

		foreach (array_keys($pluginInstances) as $pluginId) {
		    list ($ret, $pluginInstances[$pluginId]) =
			GalleryCoreApi::newFactoryInstanceById('GalleryValidationPlugin',
							       $pluginId);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    /* Let each plugin do its verification */
	    foreach ($pluginInstances as $plugin) {
		list ($ret, $pluginErrors, $continue) = $plugin->performValidation($form);
		if ($ret) {
		    return array($ret, null);
		}

		$error = array_merge($error, $pluginErrors);
		if (!$continue) {
		    break;
		}
	    }

	    /* If all the right fields are in place then go ahead and create the user */
	    if (empty($error)) {
		list ($ret, $user) =
		    GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryPendingUser');
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($user)) {
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}

		$ret = $user->create($form['userName']);
		if ($ret) {
		    if (!($ret->getErrorCode() & ERROR_COLLISION)) {
			return array($ret, null);
		    }

		    /* Set our error status and fall back to the view */
		    $error[] = 'form[error][userName][exists]';
		} else {
		    $user->setEmail($form['email']);
		    $user->setFullName($form['fullName']);
		    GalleryUtilities::unsanitizeInputValues($form['password1'], false);
		    $user->changePassword($form['password1']);

		    $ret = $user->save();
		    if ($ret) {
			return array($ret, null);
		    }

		    /* Send confirmation email or create the "real" user, per module settings */
		    list ($ret, $params) =
			GalleryCoreApi::fetchAllPluginParameters('module', 'register');
		    if ($ret) {
			return array($ret, null);
		    }

		    if ($params['confirmation'] == 'email') {
			/* Send confirmation email */
			$ret = GalleryPendingUserHelper::sendConfirmationEmail($user);
			if ($ret) {
			    return array($ret, null);
			}
			$redirect['pending'] = true;
			$redirect['sentConfirmationEmail'] = true;
		    } else if ($params['confirmation'] == 'auto') {
			/* Turn the pending user into a real user right now */
			$ret = GalleryPendingUserHelper::createGalleryUser($user);
			if ($ret) {
			    return array($ret, null);
			}
		    } else { /* confirmation == 'admin' */
			$redirect['pending'] = true;
		    }

		    if ($params['emailadmins']) {
			$ret = GalleryPendingUserHelper::sendAdminEmail($user);
			if ($ret) {
			    return array($ret, null);
			}
		    }

		    /* Request a redirect to the confirmation screen */
		    $redirect['view'] = 'core.UserAdmin';
		    $redirect['subView'] = 'register.SelfRegistrationSuccess';
		}
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = 'register.UserSelfRegistration';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    /**
     * Tests can use this method to hardwire a specific set of plugin instances to use.
     * This avoids situations where some of the option instances will do unpredictable
     * things and derail the tests.
     *
     * @param array $pluginInstances of GalleryValidationPlugin
     */
    function setPluginInstances($pluginInstances) {
	$this->_pluginInstances = $pluginInstances;
    }

}

/**
 * This view will prompt for data to create a new user
 */
class UserSelfRegistrationView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	if ($form['formName'] != 'UserSelfRegistration') {
	    /* First time around, set our defaults here. */
	    $form['userName'] = '';
	    $form['email'] = '';
	    $form['fullName'] = '';
	    $form['formName'] = 'UserSelfRegistration';
	}

	$UserSelfRegistration = array();

	/* Get all the login plugins */
	list ($ret, $allPluginIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
	if ($ret) {
	    return array($ret, null);
	}

	/* Let each plugin load its template data */
	$UserSelfRegistration['plugins'] = array();
	foreach (array_keys($allPluginIds) as $pluginId) {
	    list ($ret, $plugin) =
		GalleryCoreApi::newFactoryInstanceById('GalleryValidationPlugin', $pluginId);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $data['file'], $data['l10Domain']) = $plugin->loadTemplate($form);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($data['file'])) {
		$UserSelfRegistration['plugins'][] = $data;
	    }
	}

	$template->setVariable('UserSelfRegistration', $UserSelfRegistration);
	$template->setVariable('controller', 'register.UserSelfRegistration');
	return array(null, array('body' => 'modules/register/templates/UserSelfRegistration.tpl'));
    }
}
?>
