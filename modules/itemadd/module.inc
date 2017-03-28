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
 * The implementation of the ItemAdd module.
 * Supports ItemAddFromServer and ItemAddFromWeb
 *
 * @package ItemAdd
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20954 $
 */
class ItemAddModule extends GalleryModule {

    function ItemAddModule() {
	global $gallery;
	$this->setId('itemadd');
	$this->setName($gallery->i18n('Add Items'));
	$this->setDescription($gallery->i18n('Add items from local server or the web'));
	$this->setVersion('1.0.9');
	$this->_templateVersion = 1;
	$this->setGroup('import', $gallery->i18n('Import'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddFromWeb', 'ItemAddFromWeb',
	    'modules/itemadd/ItemAddFromWeb.inc', 'itemadd', null, 3);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddFromServer', 'ItemAddFromServer',
	    'modules/itemadd/ItemAddFromServer.inc', 'itemadd', null, 4);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('Web/Server'),
				       'view' => 'itemadd.ItemAddSiteAdmin')));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'itemadd');
	if ($ret) {
	    return $ret;
	}
	foreach (array('fromserver' => 'on', 'fromweb' => 'admin') as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }
}
?>
