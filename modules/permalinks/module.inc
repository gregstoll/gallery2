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
 * Permalinks Module
 *
 * This module provides support creating aliases to images and albums
 * in a flat namespace
 *
 * @package Permalinks
 * @author Pierre-Luc Paour <gallery@paour.com>
 * @version $Revision: 18172 $
 */
class PermalinksModule extends GalleryModule /* and GalleryEventListener */ {

    function PermalinksModule() {
	global $gallery;

	$this->setId('permalinks');
	$this->setName($gallery->i18n('Permalinks'));
	$this->setDescription(
		$gallery->i18n('Create simpler permanent aliases to your items space'));
	$this->setVersion('1.0.12');
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
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'permalinks');
	if ($ret) {
	    return $ret;
	}
	foreach (array('autoPermalink' => 0) as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}

	/* Automatically activate the rewrite rules, if the rewrite module is active */
	$success = false;
	list ($ret, $rewriteApi) = GalleryCoreApi::newFactoryInstance('RewriteApi');
	if ($ret) {
	    return array($ret, null);
	}
	if (isset($rewriteApi)) {
	    list ($ret, $success) = $rewriteApi->activateRewriteRulesForModule('permalinks');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	if (!$success) {
	    return array(null, array('view' => 'core.SiteAdmin',
				     'subView' => 'permalinks.ActivationWarning'));
	}
	return array(null, array());
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	   'ItemEditOption', 'PermalinksOption', 'PermalinksOption',
	   'modules/permalinks/PermalinksOption.inc', 'permalinks', array('ItemEditItem'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'PermalinksModule', 'PermalinksModule',
	    'modules/permalinks/module.inc', 'permalinks',
	    array('Gallery::DeactivatePlugin', 'GalleryEntity::delete', 'GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Permalinks'),
				 'view' => 'permalinks.PermalinksSiteAdmin')));
    }

    /**
     * Event Handler: get rid of any permalinks for items that are deleted,
     * deactivate if rewrite module is deactivated, and automatically create
     * permalinks for new items if configured.
     *
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$data = $event->getData();

	if ($event->getEventName() == 'GalleryEntity::delete') {
	    $entity = $event->getEntity();
	    if (GalleryUtilities::isA($entity, 'GalleryItem')) {
		GalleryCoreApi::requireOnce('modules/permalinks/classes/PermalinksMapHelper.class');
		list ($ret, $results) = PermalinksMapHelper::fetchAliasesForItem($entity->getId());
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($results as $name) {
		    $ret = PermalinksMapHelper::deleteAlias($name);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	} else if ($event->getEventName() == 'Gallery::DeactivatePlugin' &&
		   $data['pluginId'] == 'rewrite' && $data['pluginType'] == 'module') {

	    list ($ret, $isActive) = $this->isActive();
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($isActive) {
		/* We can't run without the rewrite module, so we might as well deactivate. */
		list ($ret, $redirect) = $this->deactivate();
		if ($ret) {
		    return array($ret, null);
		}
	    }
	} else if ($event->getEventName() == 'GalleryEntity::save') {
	    list ($ret, $autoPermalink) =
		GalleryCoreApi::getPluginParameter('module', 'permalinks', 'autoPermalink');
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($autoPermalink) {
		$album = $event->getEntity();
		if (GalleryUtilities::isA($album, 'GalleryAlbumItem') &&
			$album->testPersistentFlag(STORAGE_FLAG_NEWLY_CREATED)) {
		    GalleryCoreApi::requireOnce(
			'modules/permalinks/classes/PermalinksMapHelper.class');

		    $ret = PermalinksMapHelper::createAlias($album->getPathComponent(),
							    $album->getId());
		    if ($ret) {
			if (!($ret->getErrorCode() & ERROR_COLLISION)) {
			    return array($ret, null);
			}

			/* We have a collision... not really a problem, just eat it */
		    }
		}
	    }
	}

	return array(null, null);
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	return array(
	    array('comment' => $this->translate('Permalinks'),
		'match' => array('controller' => 'permalinks.Redirect'),
		'pattern' => 'f/%filename%',
		'keywords' => array(
		'filename' => array(
		    'pattern' => '([^?]+)',
		    'help' => $this->translate('Name you have defined as an alias to your item'))),
		'help' => $this->translate('Enable this rule to allow Permalinks you define to ' .
					   'work. Disabling this rule will break your Permalinks.')
	    )
	);
    }
}
?>
