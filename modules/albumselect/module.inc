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
 * The implementation of the AlbumSelect module
 *
 * @package AlbumSelect
 * @author Alan Harder <alan.harder@sun.com>
 * @author Jonas Forsberg <jonas@gargamel.nu> (Initial dTree integration)
 * @version $Revision: 18172 $
 */
class AlbumSelectModule extends GalleryModule {

    function AlbumSelectModule() {
	global $gallery;
	$this->setId('albumselect');
	$this->setName($gallery->i18n('Album Select'));
	$this->setDescription(
	    $gallery->i18n('Jump directly to any album using a select box or tree view'));
	$this->setVersion('1.0.11');
	$this->_templateVersion = 1;
	$this->setGroup('blocks', $gallery->i18n('Blocks'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'AlbumSelectModule', 'AlbumSelectModule', 
	    'modules/albumselect/module.inc', 'albumselect', 
	    array('Gallery::ViewableTreeChange', 'Gallery::ItemOrder', 'GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'albumselect');
	if ($ret) {
	    return $ret;
	}

	/* We removed the 'show' parameter in 0.9.8 and 'type' in 0.9.11 */
	foreach (array('show', 'type') as $param) {
	    if (isset($params[$param])) {
		$ret = $this->removeParameter($param);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	foreach (array('sort' => 'manual',
		       'treeLines' => 1, 'treeIcons' => 0, 'treeCookies' => 0,
		       'treeExpandCollapse' => 0, 'treeCloseSameLevel' => 0) as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    } else if ($key == 'sort' && ($params[$key] == '0' || $params[$key] == '1')) {
		/* v0.9.4 changed 'sort' param from 0/1 to manual/title */
		$value = ($params[$key] == 1) ? 'title' : 'manual';
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	/* Changed cache keys to include language code in 0.9.13 */
	if (!empty($currentVersion) && version_compare($currentVersion, '0.9.13', '<')) {
	    GalleryDataCache::removeFromDisk(
		array('type' => 'module-data', 'module' => 'albumselect'));
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Album Select'),
				 'view' => 'albumselect.AlbumSelectSiteAdmin')));
    }

    /**
     * Event handler for Gallery::ViewableTreeChange
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$invalidateCache = false;
	if ($event->getEventName() == 'Gallery::ViewableTreeChange') {
	    $invalidateCache = true;
	} else if ($event->getEventName() == 'Gallery::ItemOrder') {
	    list ($ret, $sort) = $this->getParameter('sort');
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($sort == 'manual') {
		$invalidateCache = true;
	    }
	} else if ($event->getEventName() == 'GalleryEntity::save') {
	    $entity = $event->getEntity();
	    if ($entity->getEntityType() == 'GalleryAlbumItem') {
		if ($entity->isModified('orderBy') || $entity->isModified('orderDirection')) {
		    list ($ret, $sort) = $this->getParameter('sort');
		    if ($ret) {
			return array($ret, null);
		    }
		    if ($sort == 'album') {
			$invalidateCache = true;
		    }
		} else if ($entity->isModified('title')) {
		    $invalidateCache = true;
		}
	    }
	}

	if ($invalidateCache) {
	    GalleryDataCache::removeFromDisk(array('type' => 'module-data',
						   'module' => 'albumselect'));
	}

	return array(null, null);
    }
}
?>
