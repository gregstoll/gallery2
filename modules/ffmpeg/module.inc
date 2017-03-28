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
 * Ffmpeg Graphics Module
 *
 * This module provides the Ffmpeg graphics toolkit for Gallery
 *
 * @package Ffmpeg
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 20954 $
 */
class FfmpegModule extends GalleryModule {

    function FfmpegModule() {
	global $gallery;

	$this->setId('ffmpeg');
	$this->setName($gallery->i18n('Ffmpeg'));
	$this->setDescription($gallery->i18n('A toolkit for processing movies'));
	$this->setVersion('1.0.14');
	$this->_templateVersion = 1;
	$this->setGroup('toolkits', $gallery->i18n('Graphics Toolkits'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'ffmpeg');
	if ($ret) {
	    return $ret;
	}
	foreach (array('path' => '', 'useWatermark' => 0) as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	$slash = $platform->getDirectorySeparator();
	$imageDir = $gallery->getConfig('data.gallery.plugins_data') . "modules${slash}ffmpeg";
	list ($success) = GalleryUtilities::guaranteeDirExists($imageDir);
	if (!$success) {
	    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					"Unable to create directory: $imageDir");
	}

	$imageFile = "$imageDir${slash}filmreel.png";
	if (!$platform->is_file($imageFile) &&
	    !$platform->copy(dirname(__FILE__) . "${slash}images${slash}filmreel.png",
			     $imageFile)) {
	    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					"Unable to copy filmreel.png to $imageDir");
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	/* Register our graphics class with the factory */
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryToolkit', 'FfmpegToolkit', 'Ffmpeg',
	    'modules/ffmpeg/classes/FfmpegToolkit.class', 'ffmpeg', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation('ItemAddOption',
	    'MovieThumbnailOption', 'MovieThumbnailOption',
	    'modules/ffmpeg/MovieThumbnailOption.inc', 'ffmpeg', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::autoConfigure
     */
    function autoConfigure() {
	global $gallery;

	/* This module requires exec() */
	if (in_array('exec', preg_split('/,\s*/', ini_get('disable_functions')))) {
	    return array(null, false);
	}

	list ($ret, $needsConfiguration) = $this->needsConfiguration();
	if ($ret) {
	    return array($ret, false);
	}
	if (!$needsConfiguration) {
	    return array(null, true);
	}

	/* Try a bunch of likely seeming paths to see if any of them work. */
	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();

	/*
	 * Start with system paths.  Tack on a trailing slash if necessary,
	 * then tack on other likely paths, based on our OS.
	 */
	$paths = array();
	if (GalleryUtilities::isA($platform, 'WinNtPlatform')) {
	    foreach (explode(';', getenv('PATH')) as $path) {
		$path = trim($path);
		if (empty($path)) {
		    continue;
		}
		if ($path{strlen($path)-1} != $slash) {
		    $path .= $slash;
		}
		$paths[] = $path . 'ffmpeg.exe';
	    }

	    $paths[] = 'C:\apps\ffmpeg\ffmpeg.exe';
	    $paths[] = 'C:\ffmpeg\ffmpeg.exe';
	} else if (GalleryUtilities::isA($platform, 'UnixPlatform')) {
	    foreach (explode(':', getenv('PATH')) as $path) {
		$path = trim($path);
		if (empty($path)) {
		    continue;
		}
		if ($path{strlen($path)-1} != $slash) {
		    $path .= $slash;
		}
		$paths[] = $path . 'ffmpeg';
	    }

	    $paths[] = '/usr/bin/ffmpeg';
	    $paths[] = '/usr/local/bin/ffmpeg';
	    $paths[] = '/bin/ffmpeg';
	    $paths[] = '/sw/bin/ffmpeg';
	} else {
	    return array(null, false);
	}

	/* Load any classes we require */
	GalleryCoreApi::requireOnce('modules/ffmpeg/classes/FfmpegToolkitHelper.class');

	/* Now try each path in turn to see which ones work */
	foreach ($paths as $path) {
	    list ($ret, $testResults) = FfmpegToolkitHelper::testBinary($path);
	    if ($ret) {
		/* Something went wrong with this path -- try the next path */
		continue;
	    }

	    $failCount = 0;
	    foreach ($testResults as $testResult) {
		/* At least one test should work, else this path is not a valid one */
		if (!$testResult['success']) {
		    $failCount++;
		}
	    }

	    if ($failCount == 0) {
		/* We have a winner */
		$ret = GalleryCoreApi::setPluginParameter('module', 'ffmpeg', 'path', $path);
		if ($ret) {
		    return array($ret, false);
		}

		return array(null, true);
	    }
	}

	return array(null, false);
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	/* Load any classes we require */
	GalleryCoreApi::requireOnce('modules/ffmpeg/classes/FfmpegToolkitHelper.class');

	/* Find out what operations and properties we have available to us */
	list ($ret, $results) =
	    FfmpegToolkitHelper::getOperationsAndProperties();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $priority) = GalleryCoreApi::getToolkitPriorityById('Ffmpeg');
	if ($ret) {
	    return array($ret, null);
	}

	if (!$priority) {
	    list ($ret, $priority) = GalleryCoreApi::getMaximumManagedToolkitPriority();
	    if ($ret) {
		return array($ret, null);
	    }
	    $priority = min($priority + 1, 40);
	}

	foreach ($results['operations'] as $operation => $info) {
	    $ret = GalleryCoreApi::registerToolkitOperation('Ffmpeg',
							   $info['mimeTypes'],
							   $operation,
							   $info['params'],
							   $info['description'],
							   $info['outputMimeType'], $priority);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	foreach ($results['properties'] as $property => $info) {
	    $ret = GalleryCoreApi::registerToolkitProperty('Ffmpeg',
							  $info['mimeTypes'],
							  $property,
							  $info['type'],
							  $info['description']);
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
     * @see GalleryModule::needsConfiguration
     */
    function needsConfiguration() {
	/* This module requires exec() */
	if (in_array('exec', preg_split('/,\s*/', ini_get('disable_functions')))) {
	    return array(null, true);
	}
	/* This module requires all fields to be filled out before it can be activated. */
	foreach (array('path') as $key) {
	    list ($ret, $value) = $this->getParameter($key);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (empty($value)) {
		return array(null, true);
	    }
	}
	return array(null, false);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Ffmpeg'),
				 'view' => 'ffmpeg.AdminFfmpeg')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'ffmpeg.AdminFfmpeg';
    }
}
?>
