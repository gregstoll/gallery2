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
 * Photo printing at snapgalaxy.com. 
 * Based on implementation by Alan Harder <alan.harder@sun.com>
 *
 * @package SnapGalaxy
 * @author Bhavin Suthar <bhavin.suthar@snapgalaxy.com>
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class SnapGalaxyModule extends GalleryModule {

    function __construct() {
	global $gallery;

	$this->setId('snapgalaxy');
	$this->setName($gallery->i18n('SnapGalaxy'));
	$this->setDescription($gallery->i18n('SnapGalaxy Photo Printing Module'));
	$this->setVersion('1.0.0'); /* Update upgrade() too */
	$this->_templateVersion = 1;
	$this->setGroup('commerce', $gallery->i18n('Commerce'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 43));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion, $statusMonitor=null) {
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
	    $ret = GalleryCoreApi::registerPermission($this->getId(), 'snapgalaxy.print',
		  $gallery->i18n('[snapgalaxy] Print'));
	    if ($ret) {
		return $ret;
	    }
	    /* Give everybody print permission by default */
	    $gallery->guaranteeTimeLimit(300);
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'],
		'snapgalaxy.print', true);
	    if ($ret) {
		return $ret;
	    }
	    $ret = $this->setParameter('snapgalaxyPartnerId', 'default');  
	    if ($ret) {
		return $ret;
	    }      
	    break;

	case '0.9.0':
	case '0.9.0.1':
	case '0.9.1':
	    /* Implementing new version of the cart plugin interface */
	case '0.9.2':
	case '0.9.3':
	case '0.9.4':
	case '0.9.5':

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
		'CartPluginInterface_' . $version, 'SnapGalaxyCartPlugin', 'snapgalaxy',
		'modules/snapgalaxy/classes/SnapGalaxyCartPlugin.class', 'snapgalaxy', null);
	    if ($ret) {
		return $ret;
	    }
	}
	return null;
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId=null) {
	$links = array();

	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if (isset($wantsDetailedLinks[$itemId]) 
		    && isset($permissions[$itemId]['snapgalaxy.print']) 
		    && GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
		$links[$itemId][] =
		    array('text' => $this->translate('Print on SnapGalaxy.com'),
			  'params' => array('controller' => 'snapgalaxy.SnapGalaxyPrintPhotos',
					  'itemId' => $itemId, 
					  'returnUrl' => '%CURRENT_URL%'));
	    }
	}
	return array(null, $links);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('SnapGalaxy'),
				       'view' => 'snapgalaxy.AdminSnapGalaxy')));
    }
}
?>
