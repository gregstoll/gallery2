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
 * Jpegtran Rotation Module
 *
 * This module allows Gallery to use jpegtran for lossless JPEG transformations.
 *
 * @package Jpegtran
 * @author Matthew Miller <mattdm@mattdm.org>
 * @author Andy Staudacher <ast@gmx.ch>
 * @version $Revision: 20954 $
 */
class JpegtranModule extends GalleryModule {

    function JpegtranModule() {
	global $gallery;

	$this->setId('jpegtran');
	$this->setName($gallery->i18n('Jpegtran'));
	$this->setDescription(
	    $gallery->i18n('Graphics toolkit for lossless JPEG transformations'));
	$this->setVersion('1.0.1');
	$this->_templateVersion = 1;
	$this->setGroup('toolkits', $gallery->i18n('Graphics Toolkits'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations()
     */
    function performFactoryRegistrations() {
	/* Register our graphics class with the factory */

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryToolkit', 'JpegtranToolkit', $this->getId(),
	    'modules/jpegtran/classes/JpegtranToolkit.class', $this->getId(), null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::activate
     * @todo Add getMinimumManagedToolkitPriority() method to core API and use it here.
     */
    function activate($postActivationEvent=true) {
	GalleryCoreApi::requireOnce('modules/jpegtran/classes/JpegtranToolkitHelper.class');

	/* Test for available operations */
	list ($ret, $results) = JpegtranToolkitHelper::getOperations();
	if ($ret) {
	    return array($ret, null);
	}

	if (empty($results) || !isset($results['operations']['rotate'])) {
	    return array(GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED), null);
	}

        list ($ret, $priority) = GalleryCoreApi::getToolkitPriorityById($this->getId());
        if ($ret) {
            return array($ret, null);
        }
        $priority = $priority ? $priority : 5;

	foreach ($results['operations'] as $operation => $info) {
	    $outputMimeType = isset($info['outputMimeType']) ? $info['outputMimeType'] : '';
	    $ret = GalleryCoreApi::registerToolkitOperation($this->getId(),
							    $info['mimeTypes'],
							    $operation,
							    $info['params'],
							    $info['description'],
							    $outputMimeType, $priority);
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
     * @see GalleryModule::deactivate
     */
    function deactivate($postDeactivationEvent=true) {
	/*
	 * Unregister all our operations.  Do this before the parent deactivates
	 * so that any event handlers triggered by the deactivation will see the world as it will
	 * be after the deactivation is done.
	 */
	$ret = GalleryCoreApi::unregisterToolkit($this->getId());
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $redirect) = parent::deactivate($postDeactivationEvent);
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

	list ($ret, $path) = $this->getParameter('path');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, empty($path));
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
	 * then tack on other likely paths, based on our OS
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
		$paths[] = $path . 'jpegtran.exe';
	    }

	    /*
	     * Double-quoting the paths removes any ambiguity about the
	     * double-slashes being escaped or not
	     */
	    $paths[] = "C:\\Program Files\\Jpegtran\\jpegtran.exe";
	    $paths[] = "C:\\apps\Jpegtran\\jpegtran.exe";
	    $paths[] = "C:\\Jpegtran\\jpegtran.exe";
	    $paths[] = "C:\\Program Files\\LibJPEG\\jpegtran.exe";
	    $paths[] = "C:\\apps\LibJPEG\\jpegtran.exe";
	    $paths[] = "C:\\LibJPEG\\jpegtran.exe";
	} else if (GalleryUtilities::isA($platform, 'UnixPlatform')){
	    foreach (explode(':', getenv('PATH')) as $path) {
		$path = trim($path);
		if (empty($path)) {
		    continue;
		}
		if ($path{strlen($path)-1} != $slash) {
		    $path .= $slash;
		}
		$paths[] = $path . 'jpegtran';
	    }

	    $paths[] = '/usr/bin/jpegtran';
	    $paths[] = '/usr/local/bin/jpegtran';
	    $paths[] = '/bin/jpegtran';
	    $paths[] = '/opt/bin/jpegtran';
	    $paths[] = '/opt/libjpg/bin/jpegtran';
	    $paths[] = '/opt/libjpg-6b/bin/jpegtran';
	    $paths[] = '/sw/bin/jpegtran';
	} else {
	    return array(null, false);
	}

	GalleryCoreApi::requireOnce('modules/jpegtran/classes/JpegtranToolkitHelper.class');

	/* Now try each path in turn to see which ones work */
	foreach ($paths as $path) {
	    list ($ret, $testResults) = JpegtranToolkitHelper::testBinary($path);
	    if ($ret) {
		/* This path failed.  Continue with the next one */
		continue;
	    }

	    $failCount = 0;
	    $canRotate = false;
	    foreach ($testResults as $testResult) {
		/* Rotate is mandatory, the rest is optional */
		if (isset($testResult['name']['rotate'])) {
		    if ($testResult['success']) {
			$canRotate = true;
		    } else {
			$failCount++;
		    }
		}
	    }

	    if ($failCount == 0 && $canRotate) {
		/* We have a winner */
		$ret = $this->setParameter('path', $path);
		if ($ret) {
		    return array($ret, false);
		}

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
		     array(array('name' => $this->translate('Jpegtran'),
				 'view' => 'jpegtran.AdminJpegtran')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'jpegtran.AdminJpegtran';
    }
}
?>
