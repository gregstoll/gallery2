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
 * URL Rewrite Module
 *
 * Enables short URLs using Apache mod_rewrite, ISAPI_Rewrite, or PHP path info.
 *
 * @package Rewrite
 * @author Douglas Cau <douglas@cau.se>
 * @version $Revision: 20954 $
 */
class RewriteModule extends GalleryModule {

    function RewriteModule() {
	global $gallery;
	$this->setId('rewrite');
	$this->setName($gallery->i18n('URL Rewrite'));
	$this->setDescription($gallery->i18n(
	    'Enables short URLs using Apache mod_rewrite, ISAPI Rewrite, or PathInfo'));
	$this->setVersion('1.1.19'); /* Update RewriteModuleExtras::upgrade too! */
	$this->_templateVersion = 1;
	$this->setGroup('gallery', $gallery->i18n('Gallery'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	GalleryCoreApi::requireOnce('modules/rewrite/RewriteModuleExtras.inc');
	$ret = RewriteModuleExtras::upgrade($this, $currentVersion);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::uninstall
     */
    function uninstall() {
	GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
	list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
	if ($ret && !($ret->getErrorCode() & ERROR_MISSING_VALUE)) {
	    return $ret;
	}

	if (!$ret) {
	    list ($ret) = $rewriteParser->saveActiveRules(array());
	    if ($ret) {
		return $ret;
	    }
	}

	$ret = parent::uninstall();
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::needsConfiguration
     */
    function needsConfiguration() {
	GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
	list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
	if ($ret && !($ret->getErrorCode() & ERROR_MISSING_VALUE)) {
	    return array($ret, null);
	} else if ($ret && ($ret->getErrorCode() & ERROR_MISSING_VALUE)) {
	    return array(null, true);
	}

	list ($ret, $status) = $rewriteParser->needsConfiguration($this);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $status);
    }

    /**
     * @see GalleryModule::autoConfigure
     */
    function autoConfigure() {
	return array(null, false);
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
	list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $activeRules) = GalleryCoreApi::getPluginParameter(
	    'module', 'rewrite', 'activeRules');
	if ($ret) {
	    return array($ret, null);
	}
	$activeRules = unserialize($activeRules);

	if (empty($activeRules)) {
	    $activeRules = array('rewrite' => array(0 => array('pattern' => 'v/%path%')));
	    /*
	     * We'll break fast download cache if we activate the short URL for download items with
	     * an inGallery parser
	     */
	    if ($rewriteParser->getParserType() != 'inGallery') {
		$activeRules['rewrite'][1] =
		    array('pattern' => 'd/%itemId%-%serialNumber%/%fileName%');
	    }
	}

	list ($ret, $code) = $rewriteParser->saveActiveRules($activeRules);
	if ($ret) {
	    return array($ret, null);
	}
	if ($code != REWRITE_STATUS_OK) {
	    return array(GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED, null, null, $code),
			 null);
	}

	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, array());
    }

    /**
     * @see GalleryModule::handleEvent
     */
    function handleEvent($event) {
	$data = $event->getData();
	if ($data['pluginType'] == 'module' && $data['pluginId'] != 'rewrite') {
	    GalleryCoreApi::requireOnce('modules/rewrite/RewriteModuleExtras.inc');
	    GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');

	    switch ($event->getEventName()) {
	    case 'Gallery::ActivatePlugin':
		return array(RewriteModuleExtras::handleActivatePlugin($this, $data['pluginId']),
		    null);

	    case 'Gallery::DeactivatePlugin':
		return array(RewriteModuleExtras::handleDeactivatePlugin($this, $data['pluginId']),
		    null);

	    case 'Gallery::UninstallPlugin':
		return array(RewriteModuleExtras::handleUninstallPlugin($this, $data['pluginId']),
		    null);
	    }
	}

	return array(null, null);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('URL Rewrite'),
	    'view' => 'rewrite.AdminRewrite')));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'RewriteModule', 'RewriteModule', 
	    'modules/rewrite/module.inc', 'rewrite', array('Gallery::DeactivatePlugin', 
		'Gallery::ActivatePlugin', 'Gallery::UninstallPlugin'));
	if ($ret) {
	    return $ret;
	}

	GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
	list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
	if ($ret) {
	    return $ret;
	}

	$rewriteUrlGenerator = $rewriteParser->getUrlGeneratorId();
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryUrlGenerator', $rewriteUrlGenerator, $rewriteUrlGenerator,
	    'modules/rewrite/classes/parsers/' . $rewriteParser->getParserId() . '/'
	    . $rewriteUrlGenerator . '.class', 'rewrite', null, 2);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'MaintenanceTask', 'RewriteCheckConflictTask', 'RewriteCheckConflictTask',
	    'modules/rewrite/classes/RewriteCheckConflictTask.class', 'rewrite', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'RewriteApi', 'RewriteApi', 'RewriteApi',
	    'modules/rewrite/classes/RewriteApi.class', 'rewrite', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'rewrite.SetupRewrite';
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	$rules = array();

	/* Rule 0: Show Item */
	$rules[0] = array(
	    'comment' => $this->translate('Show Item'),
	    'match' => array('view' => 'core.ShowItem'),
	    'pattern' => 'v/%path%',
	    'keywords' => array(
		'path' => array(
		    'pattern' => '([^?]+)',
		    'help' => $this->translate('Path to an item (eg, /album/image.jpg.html)'),
		    'function' => array('rewrite', 'RewriteSimpleHelper', 'parsePath')),
		'page' => array(
		    'pattern' => '([0-9]*)',
		    'help' => $this->translate('Page number in an album'),
		    'function' => array('rewrite', 'RewriteSimpleHelper', 'parsePage')),
		'language' => array(
		    'pattern' => '([a-z]{2}|[a-z]{2}_[A-Z]{2})',
		    'help' => $this->translate('Current language (eg, en or de)'),
		    'function' => array('rewrite', 'RewriteSimpleHelper', 'parseLanguage'))),
	    'onLoad' => array('rewrite', 'RewriteSimpleHelper', 'loadItemIdFromPath'),
	    'help' => $this->translate('The general URL for viewing items.'));

	/* Rule 1: Download Item */
	$rules[1] = array(
	    'comment' => $this->translate('Download Item'),
	    'match' => array('view' => 'core.DownloadItem'),
	    'pattern' => 'd/%itemId%-%serialNumber%/%fileName%',
	    'keywords' => array(
		'serialNumber' => array(
		    'pattern' => '([0-9]+)',
		    'help' => $this->translate(
			'Ensures browsers do not use cached version when image has changed'),
		    'function' => array('rewrite', 'RewriteSimpleHelper', 'parseSerialNumber')),
		'fileName' => array(
		    'pattern' => '([^/?]+)',
		    'help' => $this->translate('The item file name.'),
		    'function' => array('rewrite', 'RewriteSimpleHelper', 'parseFileName'))),
	    'help' => $this->translate('The general URL for downloading items.'));

	/* Rule 4: Site Admin */
	$rules[4] = array(
	    'comment' => $this->translate('Site Admin'),
	    'match' => array('view' => 'core.SiteAdmin'),
	    'pattern' => 'admin/',
	    'help' => $this->translate('Quick and easy way to access Site Admin.'));

	/* Rule 2: File not found */
	$rules[2] = array(
	    'comment' => $this->translate('404 File not found'),
	    'keywords' => array(
		'path' => array(
		    'pattern' => '([^?]+)',
		    'ignore' => true)),
	    'queryString' => array('view' => 'rewrite.FileNotFound'),
	    'pattern' => '%path%',
	    'locked' => true);

	/* Rule 3: Block hotlinked items */
	$rules[3] = array(
	    'comment' => $this->translate('Block hotlinked items'),
	    'flags' => array('F'),
	    'restrict' => array('view' => 'core.DownloadItem'),
	    'exemptReferer' => true);

	/* Rule 5: Send HTML for hotlinked items */
	$rules[5] = array(
	    'comment' => $this->translate('Send HTML for hotlinked items'),
	    'restrict' => array(
		GalleryUtilities::prefixFormVariable('view') => 'core.DownloadItem',
		GalleryUtilities::prefixFormVariable('itemId') => '%itemId%'),
	    'exemptReferer' => true,
	    'queryString' => array('view' => 'rewrite.DownloadItem'),
	    'options' => array('forceDirect' => true),
	    'flags' => array('L'),
	    'help' => $this->translate('Blocks use of images in other sites (HTML creates broken'
		. ' image), but allows linking to images.'));

	return $rules;
    }
}
?>
