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

define('FOTOKASTEN_GALLERY_AFFILIATE_ID', '1927');
define('FOTOKASTEN_GALLERY_AFFILIATE_PASSWORD', 'f12a65d90445f95b90e5fd30c75ee74e');

/**
 * Print photos on fotokasten.de
 *
 * @package Fotokasten
 * @author Ioana Ciocanescu <service@fotokasten.de>
 * @version $Revision: 20960 $
 */
class FotokastenModule extends GalleryModule {

    function FotokastenModule() {
	global $gallery;

	$this->setId('fotokasten');
	$this->setName($gallery->i18n('Fotokasten'));
	$this->setDescription($gallery->i18n('Fotokasten Photo Printing Module'));
	$this->setVersion('1.0.12');
	$this->_templateVersion = 1;
	$this->setGroup('commerce', $this->translate('Commerce'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 30));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	list ($ret, $params) =
	    GalleryCoreApi::fetchAllPluginParameters('module', 'fotokasten');
	if ($ret) {
	    return $ret;
	}
	foreach (array('affiliateId' => FOTOKASTEN_GALLERY_AFFILIATE_ID,
		       'affiliateIdPass' => FOTOKASTEN_GALLERY_AFFILIATE_PASSWORD)
		as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	if (!isset($currentVersion)) {
	    list ($ret, $coreParams) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
	    if ($ret) {
		return $ret;
	    }

	    /* Register our permission */
	    $ret = GalleryCoreApi::registerPermission($this->getId(), 'fotokasten.print',
						      $gallery->i18n('[fotokasten] Print'));
	    if ($ret) {
		return $ret;
	    }
	    /* Give everybody print permission by default */
	    $gallery->guaranteeTimeLimit(300);
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'],
		'fotokasten.print', true);
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	foreach (array('1_0', '1_1') as $version) {
	    $ret = GalleryCoreApi::registerFactoryImplementation(
		'CartPluginInterface_' . $version, 'FotokastenCartPlugin', 'fotokasten',
		'modules/fotokasten/classes/FotokastenCartPlugin.class', 'fotokasten', null);
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Fotokasten'),
				 'view' => 'fotokasten.FotokastenSiteAdmin')));
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
		    isset($permissions[$itemId]['fotokasten.print']) &&
		    GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
		$links[$itemId][] =
		    array('text' => $this->translate('Print on Fotokasten.de'),
			  'params' => array('controller' => 'fotokasten.PrintPhotos',
					    'itemId' => $itemId,
					    'authToken' => $session->getAuthToken(),
					    'return' => 1));
	    }
	}

	return array(null, $links);
    }
}
?>
