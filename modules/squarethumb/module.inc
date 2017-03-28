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
 * Square thumbnails
 *
 * This module creates thumbnails with a 1x1 aspect ratio; it relies on some other toolkit
 * being active to perform the actual image manipulation (dimensions, crop, thumbnail)
 *
 * @package SquareThumb
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class SquareThumbModule extends GalleryModule /* and GalleryEventListener */ {

    function SquareThumbModule() {
	global $gallery;

	$this->setId('squarethumb');
	$this->setName($gallery->i18n('Square Thumbnails'));
	$this->setDescription($gallery->i18n('Build all thumbnails so they are square'));
	$this->setVersion('1.0.9');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'squarethumb');
	if ($ret) {
	    return $ret;
	}
	foreach (array('mode' => 'crop', 'color' => '000000')
		 as $key => $value) {
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
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'SquareThumbModule', 'SquareThumbModule', 
	    'modules/squarethumb/module.inc', 'squarethumb', array('Gallery::DeactivatePlugin'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryToolkit', 'SquareThumbToolkit', 'SquareThumb',
	    'modules/squarethumb/classes/SquareThumbToolkit.class', 'squarethumb', null);
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
		     array(array('name' => $this->translate('Square Thumbnails'),
				 'view' => 'squarethumb.SquareThumbSiteAdmin')));
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	global $gallery;

	list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation('image/x-portable-pixmap',
								      'convert-to-image/jpeg');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $mimeTypes) = $this->_getSupportedMimeTypes(isset($toolkit));
	if ($ret) {
	    return array($ret, null);
	}

	/* Register toolkit operation using high priority */
	if (!empty($mimeTypes)) {
	    $ret = GalleryCoreApi::registerToolkitOperation('SquareThumb', $mimeTypes, 'thumbnail',
		array(array('type' => 'int', 'description' => $gallery->i18n('target size')),
		      array('type' => 'int', 'description' => $gallery->i18n('(optional)'))),
		$gallery->i18n('Create square thumbnail'), '', 10);
	    if ($ret) {
		return array($ret, null);
	    }
	} else {
	    /* Can't activate unless there's a toolkit to leech off of */
	    return array(null,
			 array('view' => 'core.SiteAdmin',
			       'subView' => 'squarethumb.CantActivate'));
	}

	if (!isset($toolkit)) {
	    /* Make sure we're in crop mode, if toolkit support for fit mode gets deactivated */
	    $ret = $this->setParameter('mode', 'crop');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $redirect);
    }

    /**
     * Find out what mime-types currently have toolkit support for:
     *    thumbnail, dimensions and crop [or composite]
     *
     * @param boolean $useComposite true to include types with composite support
     * @return array GalleryStatus a status code
     *               array mime types
     * @access private
     */
    function _getSupportedMimeTypes($useComposite) {
	list ($ret, $thumbnail) = GalleryCoreApi::getToolkitOperationMimeTypes('thumbnail');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $crop) = GalleryCoreApi::getToolkitOperationMimeTypes('crop');
	if ($ret) {
	    return array($ret, null);
	}
	if ($useComposite) {
	    list ($ret, $composite) = GalleryCoreApi::getToolkitOperationMimeTypes('composite');
	    if ($ret) {
		return array($ret, null);
	    }
	} else {
	    $composite = array();
	}

	$mimeTypes = array();
	foreach (array_intersect(array_keys(array_merge($crop, $composite)), array_keys($thumbnail))
		 as $mimeType) {
	    list ($ret, $properties) = GalleryCoreApi::getToolkitProperties($mimeType);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($properties as $tmp) {
		if ($tmp['name'] == 'dimensions') {
		    $mimeTypes[] = $mimeType;
		    break;
		}
	    }
	}

	return array(null, $mimeTypes);
    }

    /**
     * Handler for Gallery::DeactivatePlugin event.
     *
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	/*
	 * We're going to deactivate this plugin which will generate another
	 * Gallery::DeactivatePlugin event, so make sure that we don't handle
	 * *that* event and get into an infinite loop!
	 */
	$data = $event->getData();
	if ($event->getEventName() == 'Gallery::DeactivatePlugin' &&
	    $data['pluginId'] != 'squarethumb' &&
	    $data['pluginType'] == 'module') {

	    list ($ret, $isActive) = $this->isActive();
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($isActive) {
		/* Reactivate may fail if toolkit support no longer found. */
		list ($ret, $redirect) = $this->reactivate();
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}
	return array(null, null);
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'squarethumb.CantActivate';
    }
}
?>
