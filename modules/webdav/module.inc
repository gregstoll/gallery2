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
 * WebDAV Module.  Mount Gallery on your desktop.
 * @package WebDav
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 20954 $
 */
class WebDavModule extends GalleryModule /* and GalleryEventListener */ {

    function WebDavModule() {
	global $gallery;

	$this->setId('webdav');
	$this->setName($gallery->i18n('WebDAV'));
	$this->setVersion('1.0.10'); /* Update upgrade() too! */
	$this->_templateVersion = 1;
	$this->setDescription($gallery->i18n('Mount Gallery on your desktop.'));
	$this->setGroup('interfaces', $gallery->i18n('Remote Interfaces'));
	$this->setCallbacks('getItemLinks|getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddWebDav', 'ItemAddWebDav',
	    'modules/webdav/ItemAddWebDav.inc', 'webdav', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'WebDavModule', 'WebDavModule', 
	    'modules/webdav/module.inc', 'webdav', array('Gallery::ActivatePlugin'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	switch ($currentVersion) {
	case null:
	    /* Initial install */

	    /* Activate essential rewrite rules */
	    $ret = $this->_activateRewriteRules();
	    if ($ret && !($ret->getErrorCode() & ERROR_CONFIGURATION_REQUIRED)) {
		return $ret;
	    }

	    break;

	case '0.0.3':
	    /* Add WebDavSiteAdmin and WebDavLockMap */

	case '0.0.4':
	    /* Add modules/webdav/webdav.php and WebDavUrlGenerator */

	case '0.0.5':
	    /* Add WebDavInitEventListener */

	case '0.0.6':
	    /* Add item link */

	case '0.2.0':
	    /*
	     * Activate essential short URL rules automatically
	     * Set HTTP response status on all errors
	     */

	case '0.3.0':
	    /*
	     * Eliminate WebDavUrlGenerator, WebDavInitEventListener and WebDavSiteAdmin
	     * Register WebDavActivatePluginEventListener
	     */

	case '0.4.0':
	    /*
	     * Move parsePath from WebDavHelper to WebDavRewriteHelper, to avoid loading
	     * WebDavHelper just to generate URLs
	     */

	case '0.4.1':
	    /* Add GalleryView::isControllerLike to WebDAV view */

	case '0.4.2':
	    /* Add support for the php-cgi server API */

	case '0.6.0':
	    /* Remove view with WebDAV permission */
	    $ret = GalleryCoreApi::unregisterModulePermissions($this->getId());
	    if ($ret) {
		return $ret;
	    }
	case '0.6.1':
	    /*
	     * Add dependency on latest changes in GalleryModule::getRewriteRules and on
	     * GalleryUrlGenerator::generateUrl's protocol option.
	     */

	case '0.6.2':
	    /* Add configuration checks */
	case '0.7.0':
	    /* Add whitelist for external interface */
	case '0.7.1':
	    /* Rename WebDavMount to DownloadDavMount */
	case '0.7.2':
	    /* Add HttpAuth check through interface */
	case '0.7.3':
	    /* Version 1.0.0 for Gallery 2.2 release */

	case '1.0.0':
	    /* .mo file migration */

	case '1.0.1':
	    /* Avoid matching paths starting with 'w[^/]' with URL rewrite rule keyword */
	    $ret = $this->_activateRewriteRules();
	    if ($ret && !($ret->getErrorCode() & ERROR_CONFIGURATION_REQUIRED)) {
		return $ret;
	    }

	case '1.0.0.1':
	    /* Changes from version 1.0.2 merged into branch_2_2 */
	case '1.0.2':
	case '1.0.0.2':
	case '1.0.3':
	case '1.0.0.3':
	case '1.0.0.4':
	    /* Gallery 2.2.4 security release */
	case '1.0.4':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.0.5':
	case '1.0.6':
	    /* Require minimum core API 7.45 */
	case '1.0.7':
	case '1.0.8':
	case '1.0.9':

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
		sprintf('Unknown module version %s', $currentVersion));
	}
    }

    /**
     * @see GalleryModule::activate
     * @todo The explicit rewrite module version check can be removed on the next major Module API
     *       version.
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
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId) {
	$links = array();
	foreach ($items as $item) {
	    if (!$item->getCanContainChildren() || empty($wantsDetailedLinks[$item->getId()])) {
		continue;
	    }
	    $links[$item->getId()][] = array(
			'text' => $this->translate('Mount with WebDAV'),
			'params' => array('view' => 'webdav.WebDavMount',
					  'itemId' => $item->getId()));
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate($this->getName()),
				       'view' => 'webdav.WebDavSiteAdmin')));
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$rules = array();

	/* Use short URL because most WebDAV clients don't support query strings */
	$rules['connect'] = array(
	    'comment' => $this->translate('Connect to WebDAV'),
	    'help' => $this->translate(
		'Use short URL because most WebDAV clients don\'t support query strings.'
		. '  The Windows WebDAV client requires that you don\'t add a slash before the'
		. ' %path% variable.'),
	    'match' => array('controller' => 'webdav.WebDav'),
	    'pattern' => 'w%path%',
	    'keywords' => array(
		'path' => array(
		    'pattern' => '(/[^?]*)?',
		    'function' => array('webdav', 'WebDavRewriteHelper', 'parsePath'),
		    'help' => $this->translate('Path to an item (eg. /album/image.jpg.html)'))));

	/* Give davmount resources the correct extension */
	$rules['davmount'] = array(
	    'comment' => $this->translate('Mount WebDAV'),
	    'help' => $this->translate(
		'Give davmount resources the correct extension.'),
	    'match' => array('view' => 'webdav.DownloadDavMount'),

	    /* TODO: How to work with the root album? */
	    'pattern' => 'v/%path%.davmount',
	    'keywords' => array(
		'path' => array(
		    'pattern' => '([^?]+)',
		    'function' => array('rewrite', 'RewriteSimpleHelper', 'parsePath'),
		    'help' => $this->translate('Path to an item (eg. /album/image.jpg.html)'))),
	    'onLoad' => array('rewrite', 'RewriteSimpleHelper', 'loadItemIdFromPath'));

	/*
	 * Only define the rule to redirect OPTIONS requests so we can set DAV headers if it is
	 * already active, or if OPTIONS responses are missing DAV headers
	 */
	list ($ret, $rewriteApi) = GalleryCoreApi::newFactoryInstance('RewriteApi');
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('Error in WebDavModule::getRewriteRules: ' . $ret->getAsText());
	    }
	    return $rules;
	}
	if (!isset($rewriteApi)) {
	    return $rules;
	}

	list ($ret, $isCompatible) = $rewriteApi->isCompatibleWithApi(array(1, 1));
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('Error in WebDavModule::getRewriteRules: ' . $ret->getAsText());
	    }
	    return $rules;
	}
	if (!$isCompatible) {
	    return $rules;
	}

	list ($ret, $activeRules) = $rewriteApi->fetchActiveRulesForModule($this->getId());
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('Error in WebDavModule::getRewriteRules: ' . $ret->getAsText());
	    }
	    return $rules;
	}
	if (!in_array('options', $activeRules)) {
	    /* Check that OPTIONS responses include the DAV headers */
	    GalleryCoreApi::requireOnce('modules/webdav/classes/WebDavHelper.class');
	    if (WebDavHelper::checkDavHeaders($urlGenerator->generateUrl(
		    array('controller' => 'webdav.WebDav'),
		    array('forceFullUrl' => true,
			  'htmlEntities' => false)))) {
		return $rules;
	    }
	}

	/* Redirect OPTIONS requests so we can set DAV headers */
	$rules['options'] = array(
	    'comment' => $this->translate('OPTIONS Requests'),
	    'help' => $this->translate(
		'Redirect OPTIONS requests so we can set DAV headers.'),

	    /* Use trailing '/' to avoid 301 Moved Permanently */
	    'match' => array('href' => 'modules/webdav/data/options/'),
	    'conditions' => array(array('test' => 'REQUEST_METHOD',
					'pattern' => 'OPTIONS')));

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
	    if (($data['error']->getErrorCode() & ERROR_PERMISSION_DENIED)) {
		/* Prompt for authentication */
		list ($ret, $httpAuthInterface) =
		    GalleryCoreApi::newFactoryInstance('HttpAuthInterface_1_0');
		if ($ret) {
		    return array($ret, null);
		}
		if (isset($httpAuthInterface)) {
		    $ret = $httpAuthInterface->requestAuthentication();
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    /* Suppress generating HTML error pages for WebDAV clients */
	    return array(null, array('suppressBody' => true, 'errorHandled' => true));
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
