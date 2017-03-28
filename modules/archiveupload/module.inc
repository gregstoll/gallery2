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
 * ArchiveUpload module.
 * This module provides toolkit support to extract items from archive files such as a zip file.
 *
 * @package ArchiveUpload
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20954 $
 */
class ArchiveUploadModule extends GalleryModule {

    function ArchiveUploadModule() {
	global $gallery;

	$this->setId('archiveupload');
	$this->setName($gallery->i18n('Archive Upload'));
	$this->setDescription($gallery->i18n('Extract items from uploaded zip files'));
	$this->setVersion('1.0.10');
	$this->_templateVersion = 1;
	$this->setGroup('import', $gallery->i18n('Import'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'archiveupload');
	if ($ret) {
	    return $ret;
	}
	foreach (array('unzipPath' => '', 'removeMeta' => 1) as $key => $value) {
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
	    'GalleryToolkit', 'ArchiveExtractToolkit', 'ArchiveUpload',
	    'modules/archiveupload/classes/ArchiveExtractToolkit.class', 'archiveupload', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::autoConfigure
     */
    function autoConfigure() {
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

	GalleryCoreApi::requireOnce(
	    'modules/archiveupload/classes/ArchiveUploadHelper.class');
	$unzipPath = ArchiveUploadHelper::findUnzipBinary();
	if (empty($unzipPath)) {
	    return array(null, false);
	}

	$this->setParameter('unzipPath', $unzipPath);
	if ($ret) {
	    return array($ret, false);
	}

	return array(null, true);
    }

    /**
     * @see GalleryModule::needsConfiguration
     */
    function needsConfiguration() {
	/* This module requires exec() */
	if (in_array('exec', preg_split('/,\s*/', ini_get('disable_functions')))) {
	    return array(null, true);
	}

	foreach (array('unzipPath') as $key) {
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
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	$ret = GalleryCoreApi::registerToolkitOperation('ArchiveUpload',
							array('application/zip'),
							'extract',
							array(),
							'extract files from an archive');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $redirect);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Archive'),
				 'view' => 'archiveupload.ArchiveUploadSiteAdmin')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'archiveupload.ArchiveUploadSiteAdmin';
    }
}
?>
