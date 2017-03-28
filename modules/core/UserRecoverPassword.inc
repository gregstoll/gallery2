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
 * @version $Revision: 20996 $
 */
class UserRecoverPasswordController extends GalleryController {
    /**
     * ValidationPlugin instances to use when handling this request.  Only used by test code.
     *
     * @var array (pluginId => ValidationPlugin) $_pluginInstances
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
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$status = $error = $results = array();

	$phpVm = $gallery->getPhpVm();
	if (isset($form['action']['recover'])) {
	    $form['userName'] = is_string($form['userName']) ? $form['userName'] : null;
	    if (empty($form['userName'])) {
		$error[] = 'form[error][userName][missing]';
	    }

	    /* If no errors have been detected, let the validation plugins do their work */
	    if (empty($error)) {
		if (isset($this->_pluginInstances)) {
		    $pluginInstances = $this->_pluginInstances;
		} else {
		    /* Get all the validation plugins */
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
	    }

	    /*
	     * Still no errors?  Check the DB for a previous request and then
	     * update, reject or add based on the results.
	     */
	    $shouldSendEmail = false;
	    if (empty($error)) {
		list ($ret, $user) = GalleryCoreApi::fetchUserByUsername($form['userName']);
		if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}

		if (isset($user) && $user->getEmail() != '') {
		    /* Generate a unique auth string based on userName, time of request and IP */
		    $authString = $this->_generateAuthString();

		    /* Generate the request expiration: Now + 7 Days */
		    $requestExpires = mktime(date('G'), date('i'), date('s'),
					     date('m'), date('d')+7, date('Y'));

		    /*
		     * Check the database to see if a previous request.
		     * If a request exists, check the timestamp to see if a new
		     * request can be generated, or if they will be denied
		     * because the window is too small.
		     */
		    list ($ret, $lastRequest) = UserRecoverPasswordHelper_simple::getRequestExpires(
			$user->getUserName(), null);
		    if ($ret) {
			return array($ret, null);
		    }

		    /*
		     * This request was made less than 20 minutes ago.  Don't update the auth
		     * string.  We'll silently succeed to thwart phishing attempts.
		     */
		    if (!empty($lastRequest)) {
			if (($lastRequest - (7 * 24 * 60 * 60) + (20 * 60)) < time()) {
			    $ret = GalleryCoreApi::updateMapEntry(
				'GalleryRecoverPasswordMap',
				array('userName' => $user->getUserName()),
				array('authString' => $authString,
				      'requestExpires' => $requestExpires));
			    $shouldSendEmail = true;
			}
		    } else {
			/*
			 * Add the map entry before sending email to the user -
			 * We don't want to send them mail if the data never gets into the DB
			 */
			$ret = GalleryCoreApi::addMapEntry(
			    'GalleryRecoverPasswordMap',
			    array('userName' => $form['userName'],
				  'authString' => $authString,
				  'requestExpires' => $requestExpires));
			if ($ret) {
			    return array($ret, null);
			}
			$shouldSendEmail = true;
		    }

		    if (empty($error) && $shouldSendEmail) {
			/* Generate baseUrl and recoverUrl for the email template */
			$generator =& $gallery->getUrlGenerator();
			$baseUrl = $generator->generateUrl(array(),
					array('forceFullUrl' => true, 'htmlEntities' => false,
					      'forceSessionId' => false));
			$recoverUrl = $generator->generateUrl(
					array('view' => 'core.UserAdmin',
					      'subView' => 'core.UserRecoverPasswordConfirm',
					      'userName' => $user->getUserName(),
					      'authString' => $authString),
					array('forceFullUrl' => true, 'htmlEntities' => false,
					      'forceSessionId' => false));

			/* email template data */
			$tplData = array('name' => $user->getfullName(),
					 'baseUrl' => $baseUrl,
					 'ip' => GalleryUtilities::getRemoteHostAddress(),
					 'date' => date('r'),
					 'userName' => $user->getUserName(),
					 'recoverUrl' => $recoverUrl,
					 );

			/* Load core for translation */
			list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
			if ($ret) {
			    return array($ret, null);
			}

			/* Send the user email based on our confirmation template */
			$ret = GalleryCoreApi::sendTemplatedEmail(
			    'modules/core/templates/UserRecoverPasswordEmail.tpl',
			    $tplData, '', $user->getEmail(),
			    $module->translate('Password Recovery'));
			if ($ret) {
			    return array($ret, null);
			}
		    }

		    /* Set the recovered info flag */
		    $status['requestSent'] = 1;
		} else {
		    /* Silently succeed; we don't reward phishing attempts */
		    /* Set the recovered info flag */
		    $status['requestSent'] = 1;
		}
	    }
	} else if (isset($form['action']['cancel'])) {
	    $results['return'] = 1;
	}

	if (empty($subView)) {
	    $subView = 'core.UserRecoverPassword';
	}

	if (empty($error)) {
	    $results['redirect']['view'] = 'core.UserAdmin';
	    $results['redirect']['subView'] = $subView;
	} else {
	    $results['delegate']['view'] = 'core.UserAdmin';
	    $results['delegate']['subView'] = $subView;
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    /**
     * Generate the authorization string used for login.txt
     * @access private
     */
    function _generateAuthString() {
        GalleryCoreApi::requireOnce('lib/joomla/crypt.php');
        $j = new JCrypt();
        return md5($j->genRandomBytes(32));
    }
}

/**
 * This view shows information about password recovery
 */
class UserRecoverPasswordView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	if ($form['formName'] == 'UserRecoverPassword') {
	    if (empty($form['userName'])) {
		$form['error']['userName']['missing'] = 1;
	    }
	} else {
	    $form['userName'] = '';
	    $form['formName'] = 'UserRecoverPassword';
	}

	$UserRecoverPassword = array();

	/* Get all the login plugins */
	list ($ret, $allPluginIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
	if ($ret) {
	    return array($ret, null);
	}

	/* Let each plugin load its template data */
	$UserRecoverPassword['plugins'] = array();
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
		$UserRecoverPassword['plugins'][] = $data;
	    }
	}

	$template->setVariable('UserRecoverPassword', $UserRecoverPassword);
	$template->setVariable('controller', 'core.UserRecoverPassword');
	return array(null, array('body' => 'modules/core/templates/UserRecoverPassword.tpl'));
    }
}
?>
