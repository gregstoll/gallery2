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
 * Pick which page of a multi-page file to use for the thumbnail.
 *
 * @package ThumbPage
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class ThumbPageModule extends GalleryModule /* and GalleryEventListener */ {

    function ThumbPageModule() {
	global $gallery;

	$this->setId('thumbpage');
	$this->setName($gallery->i18n('Thumbnail Page'));
	$this->setDescription(
	    $gallery->i18n('Select page/frame from a multipage file or movie for the thumbnail'));
	$this->setVersion('1.0.8');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditPlugin', 'ItemEditThumbPage', 'ItemEditThumbPage',
	    'modules/thumbpage/ItemEditThumbPage.inc', 'thumbpage', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditPlugin', 'ItemEditThumbOffset', 'ItemEditThumbOffset',
	    'modules/thumbpage/ItemEditThumbOffset.inc', 'thumbpage', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'ThumbPageModule', 'ThumbPageModule', 
	    'modules/thumbpage/module.inc', 'thumbpage', array('Gallery::DeactivatePlugin'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	list ($ret, $page) = $this->_getSupportedMultiPageTypes();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $movie) = $this->_getSupportedMovieTypes();
	if ($ret) {
	    return array($ret, null);
	}

	if (empty($page) && empty($movie)) {
	    /* Can't activate unless there's already toolkit support */
	    return array(null, array('view' => 'core.SiteAdmin',
							 'subView' => 'thumbpage.CantActivate'));
	} else {
	    $ret = $this->setParameter('pageTypes', implode('|', $page));
	    if ($ret) {
		return array($ret, null);
	    }
	    $ret = $this->setParameter('movieTypes', implode('|', $movie));
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
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'thumbpage.CantActivate';
    }

    /**
     * Find out what mime types currently have support for:
     *    thumbnail, select-page, page-count
     *
     * @return array GalleryStatus a status code
     *               array mime types
     */
    function _getSupportedMultiPageTypes() {
	list ($ret, $thumbnail) = GalleryCoreApi::getToolkitOperationMimeTypes('thumbnail');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $select) = GalleryCoreApi::getToolkitOperationMimeTypes('select-page');
	if ($ret) {
	    return array($ret, null);
	}

	$mimeTypes = array();
	foreach (array_intersect(array_keys($thumbnail), array_keys($select)) as $mimeType) {
	    list ($ret, $properties) = GalleryCoreApi::getToolkitProperties($mimeType);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($properties as $tmp) {
		if ($tmp['name'] == 'page-count') {
		    $mimeTypes[] = $mimeType;
		    break;
		}
	    }
	}

	return array(null, $mimeTypes);
    }

    /**
     * Find out what mime types currently have support for:
     *   convert-to-image/jpeg, select-offset, dimensions-and-duration
     *
     * @return array GalleryStatus a status code
     *               array mime types
     */
    function _getSupportedMovieTypes() {
	list ($ret, $convert) =
	    GalleryCoreApi::getToolkitOperationMimeTypes('convert-to-image/jpeg');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $select) = GalleryCoreApi::getToolkitOperationMimeTypes('select-offset');
	if ($ret) {
	    return array($ret, null);
	}

	$mimeTypes = array();
	foreach (array_intersect(array_keys($convert), array_keys($select)) as $mimeType) {
	    list ($ret, $properties) = GalleryCoreApi::getToolkitProperties($mimeType);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($properties as $tmp) {
		if ($tmp['name'] == 'dimensions-and-duration') {
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
	 * We're going to deactivate this module which will generate another
	 * Gallery::DeactivatePlugin event, so make sure that we don't handle
	 * *that* event and get into an infinite loop!
	 */
	$data = $event->getData();
	if ($event->getEventName() == 'Gallery::DeactivatePlugin' &&
		$data['pluginType'] == 'module' && $data['pluginId'] != 'thumbpage') {
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
}
?>
