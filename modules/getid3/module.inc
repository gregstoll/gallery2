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
 * getID3() ID3 metadata Module
 *
 * This module provides the getID3 metadata toolkit for Gallery
 *
 * @package Getid3
 * @author Don Willingham <donwillingham@users.sf.net>
 * @version $Revision: 20960 $
 */
class Getid3Module extends GalleryModule {

    function Getid3Module() {
	global $gallery;

	$this->setId('getid3');
	$this->setName($gallery->i18n('Getid3'));
	$this->setDescription($gallery->i18n('A toolkit for getting id3 tag information'));
	$this->setVersion('1.0.7'); /* Update upgrade() function below too */
	$this->_templateVersion = 1;
	$this->setGroup('data', $gallery->i18n('Extra Data'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	if (!isset($currentVersion)) {
	    /* Initial install */
	    GalleryCoreApi::requireOnce('modules/getid3/classes/Getid3Helper.class');
	    $ret = Getid3Helper::setDefaultProperties(GETID3_SUMMARY);
	    if ($ret) {
		return $ret;
	    }

	    $ret = Getid3Helper::setDefaultProperties(GETID3_DETAILED);
	    if ($ret) {
		return $ret;
	    }
	} else {
	    switch ($currentVersion) {
	    case '0.0.1':
	    case '0.9.0':
	    case '0.9.1':
	    case '1.0.0':
	    case '1.0.1':
	    case '1.0.2':
		/* .mo file migration */
	    case '1.0.3':
	    case '1.0.4':
		/* Upgrade to GetID3 1.7.7 */
	    case '1.0.5':
	    case '1.0.6':

	    case 'end of upgrade path':
		break;

	    default:
		return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					     sprintf('Unknown module version %s', $currentVersion));
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
	    'Getid3Interface_1_0', 'Getid3Extractor', 'Getid3',
	    'modules/getid3/classes/Getid3Extractor.class', 'getid3', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryToolkit', 'Getid3Toolkit', 'Getid3',
	    'modules/getid3/classes/Getid3Toolkit.class', 'getid3', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation('ItemAddOption',
	    'Getid3DescriptionOption', 'Getid3DescriptionOption',
	    'modules/getid3/Getid3DescriptionOption.inc', 'getid3', null);
	if ($ret) {
	    return $ret;
	}


	return null;
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	global $gallery;
	$ret = GalleryCoreApi::registerToolkitProperty(
		'Getid3',
		array('video/x-msvideo'),
		'originationTimestamp',
		'int',
		$gallery->i18n('Get the origination timestamp'));
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * Stopgap for now, don't allow installation on Windows until
	 * we resolve the fact that the helperapps dir is missing from
	 * the getid3 lib and the lib won't work properly if there are
	 * spaces in the path.
	 */
	$platform =& $gallery->getPlatform();
	if (GalleryUtilities::isA($platform, 'WinNtPlatform')) {
	  return array(null,
		       array('view' => 'core.SiteAdmin',
			     'subView' => 'getid3.CantActivate'));
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
	return array(null, array(array('name' => $this->translate('Getid3'),
				       'view' => 'getid3:AdminGetid3')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'getid3.CantActivate';
    }
}
?>
