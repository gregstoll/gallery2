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
 * This view will show a tree of elements starting with the current item
 * @package Debug
 * @subpackage UserInterface
 * @author Ernesto Baschny <ernst@baschny.de>
 * @version $Revision: 17580 $
 */
class ShowTreeView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	/* Make sure we have permission */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Get the entity */
	$entityId = GalleryUtilities::getRequestVariables('entityId');
	if (empty($entityId)) {
	    list ($ret, $entity) = $this->getItem();
	    if ($ret) {
		return array($ret, null);
	    }
	    $entityId = $entity->getId();
	} else {
	    list ($ret, $entity) = GalleryCoreApi::loadEntitiesById($entityId, 'GalleryEntity');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$parentIds = $childIds = $entityObjects = $isItemTable = $entityTable = array();
	if (GalleryUtilities::isA($entity, 'GalleryChildEntity')) {
	    global $gallery;

	    /* Get the parent ids leading up to this entity */
	    if (GalleryUtilities::isA($entity, 'GalleryItem')) {
		list ($ret, $parentIds) = GalleryCoreApi::fetchParentSequence($entityId, true);
		if ($ret) {
		    return array($ret, null);
		}
	    } else if ($entity->getParentId()) {
		$parentIds[] = $entity->getParentId();
	    }

	    /* Get all of this entity's children */
	    list ($ret, $results) = $gallery->search('
		SELECT [::id] FROM [GalleryChildEntity] WHERE [::parentId] = ?
	    ', array($entityId));
	    if ($ret) {
		return array($ret, null);
	    }
	    while ($result = $results->nextResult()) {
		$childIds[] = (int)$result[0];
	    }
	}

	if (!empty($parentIds) || !empty($childIds)) {
	    /* Load everything at once */
	    list ($ret, $entityObjects) = GalleryCoreApi::loadEntitiesById(
		array_merge($parentIds, $childIds), 'GalleryEntity');
	    if ($ret) {
		return array($ret, null);
	    }
	}
	$entityObjects[] = $entity;

	foreach ($entityObjects as $entity) {
	    $isItemTable[$entity->getId()] = GalleryUtilities::isA($entity, 'GalleryItem');
	    $entityTable[$entity->getId()] = (array)$entity;
	}

	$ShowTree = array();
	$ShowTree['parentIds'] = $parentIds;
	$ShowTree['childIds'] = $childIds;
	$ShowTree['entityId'] = $entityId;
	$ShowTree['entityTable'] = $entityTable;
	$ShowTree['isItem'] = $isItemTable;
	$template->setVariable('ShowTree', $ShowTree);

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'debug');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Gallery Debug'));
	return array(null, array('body' => 'modules/debug/templates/ShowTree.tpl'));
    }
}
?>
