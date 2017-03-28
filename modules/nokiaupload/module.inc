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
 * This module implements the Nokia Image Upload Server API v1.1.
 * With this module users can upload pictures straight from their
 * mobile phone (e.g. Nokia 3650 and 6600) to their Gallery account.
 * See README file in this directory for more information and instructions.
 *
 * @package NokiaUpload
 * @author Jerome Rannikko <jeromer@hotpop.com>
 * @version $Revision: 18172 $
 */
class NokiaUploadModule extends GalleryModule {

    function NokiaUploadModule() {
	global $gallery;

	$this->setId('nokiaupload');
	$this->setName($gallery->i18n('Nokia Image Upload'));
	$this->setDescription(
	    $gallery->i18n('Implementation of Nokia Image Upload Server API v1.1'));
	$this->setVersion('1.0.12');
	$this->_templateVersion = 1;
	$this->setGroup('import', $gallery->i18n('Import'));
	$this->setCallbacks('');
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::autoConfigure
     */
    function autoConfigure() {
	list ($ret, $needsConfiguration) = $this->needsConfiguration();
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, !$needsConfiguration);
    }

    /**
     * @see GalleryModule::needsConfiguration
     */
    function needsConfiguration() {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'nokiaupload');
	if ($ret) {
	    return array($ret, null);
	}
	foreach (array('id.mobileGroup', 'id.uploadAlbum') as $key) {
	    if (empty($params[$key])) {
		return array(null, true);
	    }
	}
	return array(null, false);
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'nokiaupload.Configure';
    }
}
?>
