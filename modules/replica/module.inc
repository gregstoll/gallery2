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
 * Create item copies sharing the same original data file.
 * Underlying core code still refers to a "replica" as a "linked entity".
 *
 * @package Replica
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class ReplicaModule extends GalleryModule {

    function ReplicaModule() {
	global $gallery;
	$this->setId('replica');
	$this->setName($gallery->i18n('Replica'));
	$this->setDescription(
		$gallery->i18n('Create item copies sharing the same original data file'));
	$this->setVersion('1.0.5');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getItemLinks|getItemAdminViews');
	$this->setRequiredCoreApi(array(7, 42));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId) {
	global $gallery;
	$links = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if ($item->getIsLinkable() && isset($wantsDetailedLinks[$itemId])
		    && isset($permissions[$itemId]['core.viewSource'])) {
		if (!isset($count)) {
		    list ($ret, $count) = GalleryCoreApi::fetchItemIdCount(
			'GalleryAlbumItem',
			$item->getCanContainChildren() ? 'core.addAlbumItem' : 'core.addDataItem',
			$userId);
		    if ($ret) {
			return array($ret, null);
		    }
		    if (!$count) {
			break;
		    }
		}
		$links[$itemId][] =
		    array('text' => $this->_translate('Create Replica'),
			  'params' => array('view' => 'core.ItemAdmin',
					    'subView' => 'replica.ItemCreateReplica',
					    'itemId' => $item->getParentId(),
					    'selectedId' => $itemId,
					    'return' => 1));
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getItemAdminViews
     */
    function getItemAdminViews($item) {
	global $gallery;
	$views = array();
	list ($ret, $permissions) = GalleryCoreApi::getPermissions($item->getId());
	if ($ret) {
	    return array($ret, null);
	}

	if ($item->getIsLinkable() && isset($permissions['core.viewSource'])) {
	    list ($ret, $count) = GalleryCoreApi::fetchItemIdCount(
		    'GalleryAlbumItem',
		    $item->getCanContainChildren() ? 'core.addAlbumItem' : 'core.addDataItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($count > 0) {
		if (false) {
		    /* Specific translations: */
		    _('Replicate Album'); _('Replicate Photo'); _('Replicate Movie');
		}
		$itemTypeNames = array_merge($item->itemTypeName(), $item->itemTypeName(false));
		$views[] = array('name' => $this->_translate(
				   array('text' => 'Replicate %s', 'arg1' => $itemTypeNames[0]),
				   $itemTypeNames[2]),
				 'view' => 'replica.ItemCreateReplicaSingle');
	    }
	}

	return array(null, $views);
    }
}
?>
