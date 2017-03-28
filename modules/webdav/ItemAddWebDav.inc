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

GalleryCoreApi::requireOnce('modules/webdav/classes/WebDavHelper.class');

/**
 * Add items with WebDAV.  Handle WebDAV PUT requests and describe to users how to add items with
 * WebDAV.
 * @package WebDav
 * @subpackage UserInterface
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 18155 $
 */
class ItemAddWebDav extends ItemAddPlugin {

    /**
     * @see ItemAddPlugin::handleRequest
     */
    function handleRequest($form, &$item) {
	$requestMethod = strtolower(GalleryUtilities::getServerVar('REQUEST_METHOD'));
	if ($requestMethod != 'put') {
	    return array(GalleryCoreApi::error(ERROR_REQUEST_FORGED), null, null);
	}

	$path = GalleryUtilities::getRequestVariables('path');

	/* Check resource is not locked */
	$ret = WebDavHelper::checkLocks($path);
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Prepare data-structure from PUT request */
	list ($ret, $webDavOptions, $stream, $mimeType) = WebDavHelper::putRequestHelper();
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Get the mime type. */
	list ($ret, $mimeType) = GalleryCoreApi::getMimeType(basename($path), $mimeType);
	if ($ret) {
	    return array($ret, null, null);
	}

	list ($ret, $itemId) = GalleryCoreApi::fetchItemIdByPath($path);
	if ($ret) {
	    if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		/* Item doesn't already exist at this path.  Create it. */
		list ($ret, $error, $status) = $this->_addItem(
		    $item, $webDavOptions, $stream, $mimeType, $path);
		if ($ret) {
		    return array($ret, null, null);
		}

		return array(null, $error, $status);
	    }

	    return array($ret, null, null);
	}

	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.edit');
	if ($ret) {
	    return array($ret, null, null);
	}

	list ($ret, $error, $status) = $this->_replaceItem(
	    $item, $webDavOptions, $stream, $mimeType, $path, $itemId);
	if ($ret) {
	    return array($ret, null, null);
	}

	return array(null, $error, $status);
    }

    /**
     * Add new item.
     * @param GalleryItem $parentItem The parent item of the item to be added
     * @param array $webDavOptions WebDAV library options
     * @param resource $stream request body file handle
     * @param string $mimeType request content type
     * @param string $path the path to the destination in the Gallery hierarchy
     * @see ItemAddPlugin::handleRequest for the returned data
     * @access private
     */
    function _addItem($parentItem, $webDavOptions, $stream, $mimeType, $path) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	/* Following pattern from ItemAddWebCam */
	$tmpDir = $gallery->getConfig('data.gallery.tmp');
	$tmpFile = $platform->tempnam($tmpDir, 'webdav');

	$handle = $platform->fopen($tmpFile, 'wb');
	if (!$handle) {
	    return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
	}

	while (!$platform->feof($stream)) {
	    $buf = $platform->fread($stream, 4096);
	    if ($platform->fwrite($handle, $buf) != 4096) {
		break;
	    }
	}

	$platform->fclose($handle);

	$originalPath = GalleryUtilities::getRequestVariables('originalPath');
	$title = empty($originalPath) ? basename($path) : basename($originalPath);
	list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum($tmpFile, basename($path),
	    $title, '', '', $mimeType, $parentItem->getId());
	@$platform->unlink($tmpFile);
	if ($ret) {
	    return array($ret, null, null);
	}

	WebDavServer::setResponseStatus('201 Created');
	return array(null, array(), array('addedFiles' => array(array(
	    'fileName' => basename($path), 'id' => $newItem->getId()))));
    }

    /**
     * Replace existing item.
     * @param GalleryItem $parentItem The parent item of the item to be added
     * @param array $webDavOptions WebDAV library options
     * @param resource $stream request body file handle
     * @param string $mimeType request content type
     * @param string $path the path to the destination in the Gallery hierarchy
     * @param int $itemId The id of the item to be replaced
     * @see ItemAddPlugin::handleRequest for the returned data
     * @access private
     */
    function _replaceItem($parentItem, $webDavOptions, $stream, $mimeType, $path, $itemId) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	/*
	 * The parent is already read-locked by the ItemAddController. Read-lock the whole ancestor
	 * sequence to ensure that filesystem path stays the same.
	 */
	list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLockParents($parentItem->getId());
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Write lock the item we're replacing */
	list ($ret, $lockIds[]) = GalleryCoreApi::acquireWriteLock($itemId);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryDataItem');
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	list ($ret, $hasLink) = $this->_hasLinkedEntity($item);
	if ($ret || $hasLink) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret ? $ret : GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null, null);
	}

	/* Replace the file content */
	list ($ret, $filePath) = $item->fetchPath();
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	if (($handle = $platform->fopen($filePath, 'wb')) === false) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
	}

	/* Format PUT response */
	$ret = WebDavHelper::putResponseHelper($webDavOptions, $handle);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	/* Get a new item by mime type */
	list ($ret, $newItem) = GalleryCoreApi::newItemByMimeType($mimeType);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}
	if (!isset($newItem)) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT, __FILE__, __LINE__,
		'Failed to get new item by mime type: ' . $mimeType), null, null);
	}

	$ret = $newItem->create($parentItem->getId(), $filePath, $mimeType);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	/* Make the new item as close a copy of the original item as possible */
	list ($ret, $newItem) = WebDavHelper::mirrorEntity($item, $newItem);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	/* Fall back on an unknown item if the new class doesn't support the file content */
	$ret = $newItem->rescan();
	if ($ret) {
	    if (!($ret->getErrorCode() & ERROR_BAD_DATA_TYPE)) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array($ret, null, null);
	    }

	    $gallery->debug('Error in ItemEditWebDav::handleRequest: ' . $newItem->getClassName()
		. ' doesn\'t support the file content.  Falling back on an unknown item.');

	    list ($ret, $newItem) = GalleryCoreApi::newFactoryInstanceById('GalleryEntity',
		'GalleryUnknownItem');
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array($ret, null, null);
	    }
	    if (!isset($newItem)) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT, __FILE__, __LINE__,
		    'Failed to get GalleryUnknownItem instance'), null, null);
	    }

	    $ret = $newItem->create($parentItem->getId(), $filePath, $mimeType);
		if ($ret) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array($ret, null, null);
	    }

	    /* Make the new item as close a copy of the original item as possible */
	    list ($ret, $newItem) = WebDavHelper::mirrorEntity($item, $newItem);
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array($ret, null, null);
	    }

	    $ret = $newItem->rescan();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array($ret, null, null);
	    }
	}

	/* Follow pattern from ItemAddWebCam */
	$tmpDir = $gallery->getConfig('data.gallery.tmp');
	$tmpFile = $platform->tempnam($tmpDir, 'webdav');

	/* Backup the file content */
	if (!$platform->copy($filePath, $tmpFile)) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
	}

	$ret = $item->delete();
	if ($ret) {
	    @$platform->unlink($tmpFile);
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	/*
	 * Restore the file content.  If this fails, we rollback the database transaction but the
	 * file content is missing.
	 */
	if (!$platform->copy($tmpFile, $filePath)) {
	    @$platform->unlink($tmpFile);
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
	}
	@$platform->unlink($tmpFile);

	$newItem->setPersistentFlag(STORAGE_FLAG_NEWLY_CREATED);
	$ret = $newItem->save();
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	$ret = GalleryCoreApi::addExistingItemToAlbum($newItem, $parentItem->getId(), true);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockIds);
	    return array($ret, null, null);
	}

	$ret = GalleryCoreApi::releaseLocks($lockIds);
	if ($ret) {
	    return array($ret, null, null);
	}

	return array(null, array(), array(
	    'addedFiles' => array(array('fileName' => basename($path),
					'id' => $newItem->getId()))));
    }

    /**
     * @see ItemAddPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	return array(null, 'modules/webdav/templates/ItemAddWebDav.tpl', 'modules_webdav');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'webdav');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('WebDAV'));
    }

    /**
     * Determine if item is part of a linked(replica) set.
     * @param GalleryItem $item
     * @return array GalleryStatus a status code,
     *               bool true if linked
     * @access private
     */
    function _hasLinkedEntity($item) {
	$hasLink = $item->isLinked();
	if (!$hasLink) {
	    list ($ret, $linkedIds) = GalleryCoreApi::fetchEntitiesLinkedTo($item->getId());
	    if ($ret) {
		return array($ret, null);
	    }
	    $hasLink = !empty($linkedIds);
	}
	return array(null, $hasLink);
    }
}
?>
