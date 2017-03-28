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
 * This controller will handle a user logging in to Gallery
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17924 $
 */
class UserLoginController extends GalleryController {

    /**
     * ValidationPlugin instances to use when handling this request.  Only used by test code.
     *
     * @var array $_plugins (array of GalleryValidationPlugin)
     * @access private
     */
    var $_pluginInstances;

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

    /**
     * @see GalleryController::isAllowedInMaintenance
     */
    function isAllowedInMaintenance() {
	return true;
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$results = array();
	$error = array();
	if (isset($form['action']['login'])) {
	    if (empty($form['username'])) {
		$error[] = 'form[error][username][missing]';
	    }

	    if (empty($form['password'])) {
		$error[] = 'form[error][password][missing]';
	    }

	    if (empty($error)) {
		list ($ret, $isDisabled) = GalleryCoreApi::isDisabledUsername($form['username']);
		if ($ret) {
		    return array($ret, null);
		}
		if ($isDisabled) {
		    $error[] = 'form[error][username][disabled]';
		}
	    }

	    if (empty($error)) {
		list ($ret, $user) = GalleryCoreApi::fetchUserByUsername($form['username']);
		if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
		GalleryUtilities::unsanitizeInputValues($form['password'], false);
		$isCorrect = (isset($user) && $user->isCorrectPassword($form['password']));

		/* Prepare for validation */
		$options = array('pass' => $isCorrect);
		list ($ret, $options['level']) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'validation.level');
		if ($ret) {
		    return array($ret, null);
		}
		if ($options['level'] == 'MEDIUM') {
		    $options['key'] = 'core.UserLogin.' . $form['username'];
		}
		if ($options['level'] == 'OFF') {
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
		    list ($ret, $pluginErrors, $continue) =
			$plugin->performValidation($form, $options);
		    if ($ret) {
			return array($ret, null);
		    }

		    $error = array_merge($error, $pluginErrors);
		    if (!$continue) {
			break;
		    }
		}
	    }

	    if (empty($error)) {
		if ($isCorrect) {
		    $gallery->setActiveUser($user);

		    $event = GalleryCoreApi::newEvent('Gallery::Login');
		    $event->setEntity($user);
		    list ($ret, $redirect) = GalleryCoreApi::postEvent($event);
		    if ($ret) {
			return array($ret, null);
		    }

		    /* Redirect if requested by event listener, otherwise return */
		    if (!empty($redirect)) {
			$results['redirect'] = array_shift($redirect);
		    } else {
			$results['return'] = 1;
		    }
		} else {
		    $error[] = 'form[error][invalidPassword]';
		}
	    }

	    if (!empty($error)) {
		if (!empty($form['username'])) {
		    $event = GalleryCoreApi::newEvent('Gallery::FailedLogin');
		    $event->setData(array('userName' => $form['username']));
		    list ($ret, $ignored) = GalleryCoreApi::postEvent($event);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	} else if (isset($form['action']['cancel'])) {
	    $results['return'] = 1;
	}

	if (!empty($error)) {
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = 'core.UserLogin';
	}
	$results['status'] = array();
	$results['error'] = $error;

	return array(null, $results);
    }

}

/**
 * This view prompts for login information
 */
class UserLoginView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$session =& $gallery->getSession();

	/* Check if the default login view URL has been overridden and redirect appropriately */
	$loginRedirect = $gallery->getConfig('loginRedirect');
	if (!(isset($loginRedirect['subView']) && $loginRedirect['subView'] == 'core.UserLogin')
		&& !empty($loginRedirect)) {
	    /* Do not redirect if we are logged in already */
	    list ($ret, $isGuest) = GalleryCoreApi::isAnonymousUser();
	    if ($ret) {
		return array($ret, null);
	    }

	    $phpVm = $gallery->getPhpVm();
	    $urlGenerator =& $gallery->getUrlGenerator();
	    if ($isGuest && !$phpVm->headers_sent()) {
		$redirectUrl = $urlGenerator->generateUrl($loginRedirect,
							  array('forceSessionId' => false,
							  	'forceFullUrl' => true));
		$redirectUrl = rtrim(str_replace('&amp;', '&', $redirectUrl), '&? ');
		$phpVm->header("Location: $redirectUrl");
		$phpVm->exit_();
	    }
	}

	if ($form['formName'] != 'UserLogin') {
	    $form['formName'] = 'UserLogin';
	    $form['username'] = '';
	}

	$reauthenticate = false;
	list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
    	    return array($ret, null);
	}
	if ($isAdmin) {
	    list ($ret, $reauthenticate) = $session->hasSiteAdminSessionExpired();
	    if ($ret) {
		return array($ret, null);
    	    }
	}

	$template->setVariable('reauthenticate', $reauthenticate);
	$template->setVariable('controller', 'core.UserLogin');
	return array(null, array('body' => 'modules/core/templates/UserLogin.tpl'));
    }
}
?>
