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
 * HTTP Auth Module.  Login using HTTP authentication.
 * @package HttpAuth
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 20960 $
 */
class HttpAuthModule extends GalleryModule /* and GalleryEventListener */ {

    function HttpAuthModule() {
	global $gallery;

	$this->setId('httpauth');
	$this->setName($gallery->i18n('HTTP Auth'));
	$this->setVersion('1.0.4'); /* Update upgrade() too! */
	$this->_templateVersion = 1;
	$this->setDescription($gallery->i18n('Login using HTTP authentication.'));
	$this->setGroup('authentication', $gallery->i18n('Authentication'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::activate
     * @todo Explicit rewrite module version check can be removed on next major module API version.
     */
    function activate($postActivationEvent=true) {
	/* Ensure the rewrite module is compatible, 'pattern' is optional since v1.1.8 of rewrite */
	list ($ret, $modules) = GalleryCoreApi::fetchPluginList('module');
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($modules['rewrite'])) {
	    list ($ret, $rewrite) = GalleryCoreApi::loadPlugin('module', 'rewrite', true);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (version_compare($rewrite->getVersion(), '1.1.8', '<')) {
		return array(GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED), null);
	    }
	}

	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $redirect);
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	list ($ret, $params) = $this->fetchParameters();
	if ($ret) {
	    return $ret;
	}

	if (!empty($params['httpAuthPlugin'])) {
	    $ret = GalleryCoreApi::registerFactoryImplementation(
		'GalleryAuthPlugin', 'HttpAuthPlugin', 'HttpAuthPlugin',
		'modules/httpauth/classes/HttpAuthPlugin.class', $this->getId(), null);
	    if ($ret) {
		return $ret;
	    }
	}

	if (!empty($params['serverAuthPlugin'])) {
	    $ret = GalleryCoreApi::registerFactoryImplementation(
		'GalleryAuthPlugin', 'ServerAuthPlugin', 'ServerAuthPlugin',
		'modules/httpauth/classes/ServerAuthPlugin.class', $this->getId(), null);
	    if ($ret) {
		return $ret;
	    }
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
		'HttpAuthInterface_1_0', 'HttpAuthHelper', 'HttpAuthInterface',
		'modules/httpauth/classes/HttpAuthHelper.class', $this->getId(), null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'HttpAuthModule', 'HttpAuthModule', 
	    'modules/httpauth/module.inc', 'httpauth', 
	    array('Gallery::ActivatePlugin', 'Gallery::Error', 'Gallery::Logout'));
	if ($ret) {
	    return $ret;
	}
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	switch ($currentVersion) {
	case null:
	    /* Initial install */
	    foreach (array('httpAuthPlugin' => true,
		    'authName' => 'Gallery',
		    'authtypePattern' => '//',
		    'usernamePattern' => '/^(.+\\\\)?([^\\\\@]+)(@.+)?$/',
		    'usernameReplace' => '$2',
		    'useGlobally' => false) as $key => $value) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }

	case '0.0.1':
	    /* Add AuthFilterPlugin and RegexAuthFilterPlugin */

	case '0.1.0':
	    /* Update to lighter event subsystem */

	case '0.1.1':
	    /* Introduce auth plugins */

	    list ($ret, $params) = $this->fetchParameters();
	    if ($ret) {
		return $ret;
	    }

	    if (isset($params['serverAuth'])) {
		$ret = $this->setParameter('serverAuthPlugin', $params['serverAuth']);
		if ($ret) {
		    return $ret;
		}

		$ret = $this->removeParameter('serverAuth');
		if ($ret) {
		    return $ret;
		}
	    }

	    if (isset($params['regexFilter'])) {
		$ret = $this->setParameter('regexAuthPlugin', $params['regexFilter']);
		if ($ret) {
		    return $ret;
		}

		$ret = $this->removeParameter('regexFilter');
		if ($ret) {
		    return $ret;
		}
	    }

	    if (isset($params['usernameReplacement'])) {
		$ret = $this->setParameter('usernameReplace', $params['usernameReplacement']);
		if ($ret) {
		    return $ret;
		}

		$ret = $this->removeParameter('usernameReplacement');
		if ($ret) {
		    return $ret;
		}
	    }

	case '0.2.0':
	    /* Some 2.2 API changes */

	case '0.2.1':
	    /* Simplify auth plugin system */

	case '0.3.0':
	    /* Add support for the php-cgi server API */
	    $ret = $this->_activateRewriteRules();
	    if ($ret && !($ret->getErrorCode() & ERROR_CONFIGURATION_REQUIRED)) {
		return $ret;
	    }

	case '0.4.0':
	    /* Add logout view to clear browsers' authentication cache */

	case '0.5.0':
	    /* Adding HttpAuthInterface and new site admin option */
	    $ret = $this->setParameter('useGlobally', false);
	    if ($ret) {
		return $ret;
	    }

	case '0.5.1':
	    /* Version 1.0.0 for Gallery 2.2 release */

	case '1.0.0':
	    /* .mo file migration */
	case '1.0.1':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.0.2':
	case '1.0.3':

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryStatus::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
		sprintf('Unknown module version %s', $currentVersion));
	}
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate($this->getName()),
				       'view' => 'httpauth.HttpAuthSiteAdmin')));
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	global $gallery;

	$rules = array();

	/*
	 * Only define the rule to pass the Authorization header to Gallery in a request variable if
	 * it is already active, or if Gallery can't access HTTP usernames and passwords
	 */
	list ($ret, $rewriteApi) = GalleryCoreApi::newFactoryInstance('RewriteApi');
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('Error in HttpAuthModule::getRewriteRules: ' . $ret->getAsText());
	    }
	    return $rules;
	}
	if (!isset($rewriteApi)) {
	    return $rules;
	}

	list ($ret, $isCompatible) = $rewriteApi->isCompatibleWithApi(array(1, 1));
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('Error in HttpAuthModule::getRewriteRules: ' . $ret->getAsText());
	    }
	    return $rules;
	}
	if (!$isCompatible) {
	    return $rules;
	}

	list ($ret, $activeRules) = $rewriteApi->fetchActiveRulesForModule($this->getId());
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('Error in HttpAuthModule::getRewriteRules: ' . $ret->getAsText());
	    }
	    return $rules;
	}
	if (!in_array('authorization', $activeRules)) {
	    /* Check that Gallery can access HTTP usernames and passwords */
	    GalleryCoreApi::requireOnce('modules/httpauth/classes/HttpAuthHelper.class');
	    list ($ret, $success) = HttpAuthHelper::checkHttpAuth();
	    if ($ret) {
		if ($gallery->getDebug()) {
		    $gallery->debug(
			'Error in HttpAuthModule::getRewriteRules: ' . $ret->getAsText());
		}
		return $rules;
	    }
	    if ($success) {
		return $rules;
	    }
	}

	/* Pass the Authorization header to Gallery in a request variable */
	$rules['authorization'] = array(
	    'comment' => $this->translate('Authorization Header'),
	    'help' => $this->translate(
		'Pass the Authorization header to Gallery in a request variable.'),
	    'conditions' => array(array('test' => 'HTTP:Authorization',
					'pattern' => '%authorization%'),
				  array('test' => 'QUERY_STRING',
					'pattern' => '!' . GalleryUtilities::prefixFormVariable(
					    'authorization') . '=')),
	    'options' => array('baseUrl' => '%{REQUEST_URI}'),
	    'flags' => array('QSA'),
	    'keywords' => array(
		'authorization' => array(
		    'pattern' => '(.+)',
		    'help' => $this->translate('Authorization header.'))));

	return $rules;
    }

    /**
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	switch ($event->getEventName()) {
	case 'Gallery::ActivatePlugin':
	    $data = $event->getData();
	    if ($data['pluginType'] != 'module' || $data['pluginId'] != 'rewrite') {
		return array(null, null);
	    }

	    $ret = $this->_activateRewriteRules();
	    if ($ret && !($ret->getErrorCode() & ERROR_CONFIGURATION_REQUIRED)) {
		return array($ret, null);
	    }

	    return array(null, null);

	case 'Gallery::Error':
	    $data = $event->getData();
	    if (!($data['error']->getErrorCode() & ERROR_PERMISSION_DENIED)) {
		return array(null, null);
	    }

	    GalleryCoreApi::requireOnce('modules/httpauth/classes/HttpAuthHelper.class');
	    $ret = HttpAuthHelper::requestAuthentication(false);
	    if ($ret) {
		return array($ret, null);
	    }

	    return array(null, array('errorHandled' => true));

	case 'Gallery::Logout':
	    /*
	     * If this request includes an HTTP username, delegate to the HTTP auth logout view
	     * which tries to clear the browser's authentication cache
	     */
	    GalleryCoreApi::requireOnce('modules/httpauth/classes/HttpAuthHelper.class');
	    list ($authtype, $username, $password) = HttpAuthHelper::getHttpAuth();
	    $remoteUser = GalleryUtilities::getServerVar('REMOTE_USER');
	    if (!empty($username) || !empty($remoteUser)) {
		return array(null, array('delegate' => array('view' => 'httpauth.TryLogout')));
	    }
	}

	return array(null, null);
    }

    /**
     * Activate essential rewrite rules.
     * @return GalleryStatus a status code
     */
    function _activateRewriteRules() {
	list ($ret, $rewriteApi) = GalleryCoreApi::newFactoryInstance('RewriteApi');
	if ($ret) {
	    return $ret;
	}
	if (!isset($rewriteApi)) {
	    return GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED);
	}

	list ($ret, $isCompatible) = $rewriteApi->isCompatibleWithApi(array(1, 1));
	if ($ret) {
	    return $ret;
	}
	if (!$isCompatible) {
	    return GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED);
	}

	list ($ret, $success) = $rewriteApi->activateRewriteRulesForModule($this->getId());
	if ($ret) {
	    return $ret;
	}
	if (!$success) {
	    return GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED);
	}

	return null;
    }
}
?>
