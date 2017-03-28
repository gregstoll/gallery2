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
 * Photo printing at digibug.com
 *
 * @package Digibug
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class DigibugModule extends GalleryModule {

    function DigibugModule() {
	global $gallery;

	$this->setId('digibug');
	$this->setName($gallery->i18n('Digibug'));
	$this->setDescription($gallery->i18n('Digibug Photo Printing Module'));
	$this->setVersion('1.0.8'); /* Update upgrade() too */
	$this->_templateVersion = 1;
	$this->setGroup('commerce', $gallery->i18n('Commerce'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 43));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	if (!isset($currentVersion)) {
	    $currentVersion = '0';
	}
	list ($ret, $coreParams) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
	if ($ret) {
	    return $ret;
	}

	switch ($currentVersion) {
	case '0':
	    /* Register our permission */
	    $ret = GalleryCoreApi::registerPermission($this->getId(), 'digibug.print',
						      $gallery->i18n('[digibug] Print'));
	    if ($ret) {
		return $ret;
	    }
	    /* Give everybody print permission by default */
	    $gallery->guaranteeTimeLimit(300);
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'],
		'digibug.print', true);
	    if ($ret) {
		return $ret;
	    }
	    $ret = $this->_setGalleryDefaultId();
	    if ($ret) {
		return $ret;
	    }
	    break;

	case '0.3.3':
	case '0.3.4':
	case '0.3.5':
	case '0.9.0':
	case '0.9.1':
	case '1.0.0':
	case '1.0.0.1':
	case '1.0.0.2':
	    /* .mo file migration */
	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	    /* Implementing new version of the cart plugin interface */
	case '1.0.4':
	case '1.0.5':
	case '1.0.6':
	    /* Created default Gallery ID */
	    $ret = $this->_setGalleryDefaultId();
	    if ($ret) {
		return $ret;
	    }

	case '1.0.7':
	    
	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					 sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	foreach (array('1_0', '1_1') as $version) {
	    $ret = GalleryCoreApi::registerFactoryImplementation(
		'CartPluginInterface_' . $version, 'DigibugCartPlugin', 'digibug',
		'modules/digibug/classes/DigibugCartPlugin.class', 'digibug', null);
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	global $gallery;
	$session =& $gallery->getSession();

	$links = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if (isset($wantsDetailedLinks[$itemId]) &&
		    isset($permissions[$itemId]['digibug.print']) &&
		    GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
		$links[$itemId][] =
		    array('text' => $this->translate('Print on Digibug.com'),
			  'params' => array('view' => 'digibug.DigibugPrintPhotos',
			  		    'itemId' => $itemId, 'returnUrl' => '%CURRENT_URL%',
					    'authToken' => $session->getAuthToken()));
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('Digibug'),
				       'view' => 'digibug.AdminDigibug')));
    }

    /**
     * @see GalleryModule::getConfigurationView
     */
    function getConfigurationView() {
	return 'digibug.AdminDigibug';
    }
    
    /**
     * Initialize digibug module with default settings (Gallery project id and price list id)
     * @return GalleryStatus a status code
     */
    function _setGalleryDefaultId() {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'digibug');
	if ($ret) {
	    return $ret;
	}
	if (empty($params['digibugCustomerId']) || empty($params['digibugPricelistId'])) {
	    foreach (array('digibugIdChoice' => 'gallery', 'digibugCustomerId' => '3153', 
			   'digibugPricelistId' => '8491') as $key => $value) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	} else {
	    $ret = $this->setParameter('digibugIdChoice', 'owner');
	    if ($ret) {
		return $ret;
	    }
	}
	return null;
    }
}
?>
