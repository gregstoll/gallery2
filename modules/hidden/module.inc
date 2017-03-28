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
 * Hidden Items
 *
 * This module provides the ability to hide items and allow them to be viewed only
 * by directly accessing the correct URL.
 *
 * @package Hidden
 * @author Alan Harder <alan.harder@sun.com>
 * @author Jess Martin <jmartin@cs.unc.edu>
 * @version $Revision: 17588 $
 */
class HiddenModule extends GalleryModule /* and GalleryEventListener */ {

    function HiddenModule() {
	global $gallery;

	$this->setId('hidden');
	$this->setName($gallery->i18n('Hidden Items'));
	$this->setDescription($gallery->i18n('Hide items/albums from guests until directly ' .
					     'accessed with the right URL'));
	$this->setVersion('1.0.9');
	$this->_templateVersion = 1;
	$this->setGroup('display', $this->translate('Display'));
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion, $statusMonitor) {
	global $gallery;
	$storage =& $gallery->getStorage();

	if (empty($currentVersion)) {
	    $currentVersion = '0';
	}

	if (version_compare($currentVersion, '1.0.8', '<')) {
	    $ret = $storage->configureStore($this->getId());
	    if ($ret) {
		return $ret;
	    }
	    $gallery->guaranteeTimeLimit(60);
	    $ret = $storage->execute('DELETE FROM [ItemHiddenMap]');
	    if ($ret) {
		return $ret;
	    }

	    $gallery->guaranteeTimeLimit(60);
	    $ret = $storage->execute('INSERT INTO [ItemHiddenMap] ([::itemId])
				   SELECT [GalleryEntity::id]
				     FROM [GalleryEntity]
				    WHERE [GalleryEntity::onLoadHandlers] LIKE \'%|Hidden|%\'');
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
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'HiddenModule', 'HiddenModule', 
	    'modules/hidden/module.inc', 'hidden', 
	    array('Gallery::ViewableTreeChange', 'GalleryEntity::save', 'GalleryEntity::delete'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryOnLoadHandler', 'HiddenModule', 'Hidden',
	    'modules/hidden/module.inc', 'hidden', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditOption', 'HiddenItemOption', 'HiddenItemOption',
	    'modules/hidden/HiddenItemOption.inc', 'hidden', array('ItemEditItem'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'HiddenInterface_1_0', 'HiddenHelper', 'Hidden',
	    'modules/hidden/classes/HiddenHelper.class', 'hidden', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getOnLoadHandlerIds
     */
    function getOnLoadHandlerIds() {
	return array('Hidden');
    }

    /**
     * Unhide all hidden items.
     * @see GalleryModule::uninstall
     */
    function uninstall() {
	GalleryCoreApi::requireOnce('modules/hidden/classes/HiddenHelper.class');

	list ($ret, $searchResults) = GalleryCoreApi::getMapEntry('ItemHiddenMap', array('itemId'));
	if ($ret) {
	    return $ret;
	}
	while ($result = $searchResults->nextResult()) {
	    $itemIds[] = (int)$result[0];
	}

	while (!empty($itemIds)) {
	    list ($ret, $items) =
		GalleryCoreApi::loadEntitiesById(array_splice($itemIds, 0, 100), 'GalleryEntity');
	    if ($ret) {
		return $ret;
	    }
	    foreach ($items as $item) {
		$ret = HiddenHelper::unHideItem($item);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	$ret = parent::uninstall();
	if ($ret) {
	    return $ret;
	}
	return null;
    }

    /**
     * Add session based permission when core.ShowItem accessed directly for this item
     */
    function onLoad(&$item, $duringUpgrade) {
	list ($view, $itemId) = GalleryUtilities::getRequestVariables('view', 'itemId');
	if ($view == 'core.ShowItem' && $itemId == $item->getId()) {
	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($itemId);
	    if ($ret) {
		return $ret;
	    }
	    if (!isset($permissions['core.view']) && (isset($permissions['core.viewResizes'])
		    || isset($permissions['core.viewSource']))) {
		GalleryCoreApi::addPermissionToSession($itemId);

		/* Use a blunt instrument to clear the permission cache. */
		GalleryDataCache::reset();
	    }
	}
	return null;
    }

    /**
     * Update permissions when items move in/out of hidden albums
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/hidden/classes/HiddenHelper.class');
	switch ($event->getEventName()) {
	case 'GalleryEntity::save':
	    $item = $event->getEntity();
	    if (GalleryUtilities::isA($item, 'GalleryItem')
		    && !$item->testPersistentFlag(STORAGE_FLAG_NEWLY_CREATED)
		    && $item->isModified('parentId')
		    && !$item->hasOnLoadHandler('Hidden')) {
		$ret = HiddenHelper::handleMoveItem($item);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    break;
	case 'GalleryEntity::delete':
	    $item = $event->getEntity();
	    if (GalleryUtilities::isA($item, 'GalleryItem')) {
		$ret = GalleryCoreApi::removeMapEntry('ItemHiddenMap', 
						      array('itemId' => $item->getId()));
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    break;
	case 'Gallery::ViewableTreeChange':
	    /*
	     * This code gets triggered when the core.view permission was added to an item. Hiding
	     * an item requires that the 'core.view' permission is removed.  If the permission 
	     * change involves adding the 'core.view' we need to check that the item was not hidden,
	     * and it was then undo it. If the permission change was also 'applied to children'
	     * then we need to do the same check for all the descendents.
	     */
	    $data = $event->getData();
	    if (!empty($data['permission']) 
		    && in_array($data['permission'], array('core.view', 'core.viewAll', 'core.all'))
		    && $data['changeType'] == 'add') {
		$itemId = $data['itemId'];
		$ids = array($itemId);

		if ($data['applyToChildren']) {
		    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
		    if ($ret) {
			return array($ret, null);
		    }
		    list ($ret, $childIds) = GalleryCoreApi::fetchDescendentItemIds(
			$item, null, null, $data['permission']);
		    if ($ret) {
			return array($ret, null);
		    }

		    $ids = array_merge($ids, $childIds);
		}
		$query = 'SELECT [::itemId] FROM [ItemHiddenMap] 
			   WHERE [ItemHiddenMap::itemId] IN ('
			 . GalleryUtilities::makeMarkers(count($ids)) . ')';
		list ($ret, $searchResults) = $gallery->search($query, $ids);
		if ($ret) {
		    return array($ret, null);
		}

		if ($searchResults->resultCount() > 0) {
		    $gallery->guaranteeTimeLimit(60);
		    list ($ret, $anonymousUserId) = 
			GalleryCoreApi::getPluginParameter('module', 'core', 'id.anonymousUser');
		    if ($ret) {
			return array($ret, null);
		    }
		    list ($ret, $everybodyGroup) = 
			GalleryCoreApi::getPluginParameter('module', 'core', 'id.everybodyGroup');
		    if ($ret) {
			return array($ret, null);
		    }
		    while ($result = $searchResults->nextResult()) {
			$id = $result[0];
			/* Remove public view permission */
			$ret = GalleryCoreApi::removeUserPermission(
			    $id, $anonymousUserId, 'core.view');
			if ($ret) {
			    return array($ret, null);
			}
			$ret = GalleryCoreApi::removeGroupPermission(
			    $id, $everybodyGroup, 'core.view');
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}
	    }
	    break;
	}
	return array(null, null);
    }
}
?>
