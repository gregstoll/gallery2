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
 * @package LinkItem
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class LinkItemModule extends GalleryModule /* and GalleryEventListener */ {

    function LinkItemModule() {
	global $gallery;
	$this->setId('linkitem');
	$this->setName($gallery->i18n('Link Items'));
	$this->setDescription($gallery->i18n('Create links to other albums or external urls'));
	$this->setVersion('1.1.7');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 41));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();

	$imageDir = $gallery->getConfig('data.gallery.plugins_data') . "modules${slash}linkitem";
	list ($success) = GalleryUtilities::guaranteeDirExists($imageDir);
	if (!$success) {
	    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					 "Unable to create directory: $imageDir");
	}

	$imageFile = "$imageDir${slash}arrow.png";
	if (!$platform->is_file($imageFile) &&
	    !$platform->copy(dirname(__FILE__) . "${slash}images${slash}arrow.png", $imageFile)) {
	    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					 "Unable to copy arrow.png to $imageDir");
	}

	if (!empty($currentVersion) && version_compare($currentVersion, '1.0.4', '<=')) {
	    $storage =& $gallery->getStorage();
	    $ret = $storage->configureStore($this->getId(), array('GalleryLinkItem:1.0'));
	    if ($ret) {
		return $ret;
	    }
	}

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'linkitem');
	if ($ret) {
	    return $ret;
	}
	foreach (array('arrowWatermark' => 1, 'urlSnapshot' => 0, 'snapshotExec' => 'khtml2png')
		 as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEntity', 'GalleryLinkItem', 'GalleryLinkItem',
	    'modules/linkitem/classes/GalleryLinkItem.class', 'linkitem', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddLinkItem', 'ItemAddLinkItem',
	    'modules/linkitem/ItemAddLinkItem.inc', 'linkitem', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditOption', 'LinkItemOption', 'LinkItemOption',
	    'modules/linkitem/LinkItemOption.inc', 'linkitem', array('ItemEditItem'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryToolkit', 'LinkItemToolkit', 'LinkItemToolkit',
	    'modules/linkitem/classes/LinkItemToolkit.class', 'linkitem', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'LinkItemModule', 'LinkItemModule', 
	    'modules/linkitem/module.inc', 'linkitem', array('GalleryEntity::delete'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getModuleEntityTypes
     */
    function getModuleEntityTypes() {
	return array('GalleryLinkItem');
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	global $gallery;

	$ret = GalleryCoreApi::registerToolkitOperation(
		'LinkItemToolkit', array('gallery/linkitem'), 'convert-to-image/jpeg', array(),
		$gallery->i18n('Get image for linkitem thumbnail'), 'image/jpeg');
	if ($ret) {
	    return array($ret, null);
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
	return array(null,
		     array(array('name' => $this->translate('Link Items'),
				 'view' => 'linkitem.LinkItemSiteAdmin')));
    }

    /**
     * Delete links to an album if the album is deleted.
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$entity = $event->getEntity();
	if (GalleryUtilities::isA($entity, 'GalleryAlbumItem')) {
	    global $gallery;
	    $query = '
	      SELECT
		[GalleryLinkItem::id]
	      FROM
		[GalleryLinkItem]
	      WHERE
		[GalleryLinkItem::link] = ?
	    ';
	    list ($ret, $searchResults) = $gallery->search($query, array((string)$entity->getId()));
	    if ($ret) {
		return array($ret, null);
	    }
	    while ($result = $searchResults->nextResult()) {
		$ret = GalleryCoreApi::deleteEntityById($result[0], 'GalleryLinkItem');
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}
	return array(null, null);
    }
}
?>
