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
 * NetPBM Graphics Module.
 * This module provides the NetPBM graphics toolkit for Gallery.
 * @package NetPbm
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 20954 $
 */
class NetPbmModule extends GalleryModule {

    function NetPbmModule() {
	global $gallery;

	$this->setId('netpbm');
	$this->setName($gallery->i18n('NetPBM'));
	$this->setDescription($gallery->i18n('NetPBM Graphics Toolkit'));
	$this->setVersion('1.1.5');
	$this->_templateVersion = 1;
	$this->setGroup('toolkits', $gallery->i18n('Graphics Toolkits'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'netpbm');
	if ($ret) {
	    return $ret;
	}
	foreach (array('path' => '', 'jpegQuality' => '75', 'pnmtojpeg' => 'pnmtojpeg',
		       'bmptopnm' => 'bmptopnm', 'pnmcomp' => 'pnmcomp') as $key => $value) {
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
	/* Register our graphics class with the factory */
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryToolkit', 'NetPbmToolkit', 'NetPBM',
	    'modules/netpbm/classes/NetPbmToolkit.class', 'netpbm', null);
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
		$paths[] = $path;
	    }

	    /*
	     * Double-quoting the paths removes any ambiguity about the
	     * double-slashes being escaped or not
	     */
	    $paths[] = "C:\\Program Files\\netpbm\\";
	    $paths[] = "C:\\apps\\netpbm\\";
	    $paths[] = "C:\\apps\\jhead\\";
	    $paths[] = "C:\\netpbm\\";
	    $paths[] = "C:\\jhead\\";
	    $paths[] = "C:\\cygwin\\bin\\";
	} else if (GalleryUtilities::isA($platform, 'UnixPlatform')) {
	    foreach (explode(':', getenv('PATH')) as $path) {
		$path = trim($path);
		if (empty($path)) {
		    continue;
		}
		if ($path{strlen($path)-1} != $slash) {
		    $path .= $slash;
		}
		$paths[] = $path;
	    }

	    $paths[] = '/usr/bin/';
	    $paths[] = '/usr/local/bin/';
	    $paths[] = '/usr/local/netpbm/bin/';
	    $paths[] = '/bin/';
	    $paths[] = '/sw/bin/';
	} else {
	    return array(null, false);
	}

	/* Load any classes we require */
	GalleryCoreApi::requireOnce('modules/netpbm/classes/NetPbmToolkitHelper.class');

	/* Now try each path in turn to see which ones work */
	$success = false;
	foreach ($paths as $path) {
	    if ($platform->isRestrictedByOpenBaseDir($path) || !$platform->file_exists($path)) {
		continue;
	    }

	    list ($ret, $testResults, $mimeTypes) = NetPbmToolkitHelper::testBinaries($path);
	    if ($ret) {
		return array($ret, false);
	    }

	    /* Take the first path found with at least jpeg support */
	    if (in_array('image/jpeg', $mimeTypes)) {
		$ret = $this->setParameter('path', $path);
		if ($ret) {
		    return array($ret, false);
		}
		$ret = NetPbmToolkitHelper::saveParameters($testResults);
		if ($ret) {
		    return array($ret, false);
		}
		$success = true;
		break;
	    }
	}
	/* Now try each path in turn to see which ones work for jhead */
	foreach ($paths as $path) {
	    if (!$platform->isRestrictedByOpenBaseDir($path) && !$platform->file_exists($path)) {
		continue;
	    }
	    list ($ret, $testResults) = NetPbmToolkitHelper::testJhead($path);
	    if ($ret) {
		return array($ret, false);
	    }
	    foreach ($testResults as $testResult) {
		if ($testResult['success'] && ($testResult['name'] == 'jhead')) {
		    $ret = $this->setParameter('jheadPath', $path);
		    if ($ret) {
			return array($ret, false);
		    }
		    break 2;
		}
	    }
	}

	return array(null, $success);
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	/* Load any classes we require */
	GalleryCoreApi::requireOnce('modules/netpbm/classes/NetPbmToolkitHelper.class');

	/* Find out what operations and properties we have available to us */
	list ($ret, $results) = NetPbmToolkitHelper::getOperationsAndProperties(true);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $priority) = GalleryCoreApi::getToolkitPriorityById('NetPBM');
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
	    $outputMimeType = isset($info['outputMimeType']) ? $info['outputMimeType'] : '';
	    $ret = GalleryCoreApi::registerToolkitOperation('NetPBM',
							   $info['mimeTypes'],
							   $operation,
							   $info['params'],
							   $info['description'],
							   $outputMimeType, $priority);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	foreach ($results['properties'] as $property => $info) {
	    $ret = GalleryCoreApi::registerToolkitProperty('NetPBM',
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
	foreach (array('path', 'jpegQuality', 'pnmtojpeg', 'bmptopnm', 'pnmcomp') as $key) {
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
		     array(array('name' => $this->translate('NetPBM'),
				 'view' => 'netpbm.AdminNetPbm')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'netpbm.AdminNetPbm';
    }
}
?>
