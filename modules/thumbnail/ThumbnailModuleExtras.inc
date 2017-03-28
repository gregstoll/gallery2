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
 * Upgrade / activation code
 * @package Thumbnail
 * @author Alan Harder <alan.harder@sun.com>
 * @author Andy Staudacher <ast@gmx.ch>
 * @version $Revision: 18172 $
 * @static
 */
class ThumbnailModuleExtras {

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($module, $currentVersion) {
	global $gallery;

	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();
	$thumbnailDir = $gallery->getConfig('data.gallery.plugins_data') . 'modules' .
	    $slash . 'thumbnail' . $slash;

	if (!isset($currentVersion)) {
	    $currentVersion = '0';
	}

	switch ($currentVersion) {
	case '0':
	    /* Initial install */
	    list ($success) = GalleryUtilities::guaranteeDirExists($thumbnailDir);
	    if (!$success) {
		return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					    "Unable to create directory: $thumbnailDir");
	    }

	    $ret = ThumbnailModuleExtras::_createThumbnailContainer($module);
	    if ($ret) {
		return $ret;
	    }
	    break;

	case '0.8.1':
	case '0.8.2':
	    /* Assign parent to existing ThumbnailImage entities */
	    list ($ret, $rootId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return $ret;
	    }

	    $storage =& $gallery->getStorage();
	    $ret = $storage->configureStore($module->getId(), array('ThumbnailImage:1.0'));
	    if ($ret) {
		return $ret;
	    }

	    $query = '
	    INSERT INTO [GalleryChildEntity] SELECT [ThumbnailImage::id], ' . (int)$rootId . '
	    FROM [ThumbnailImage]
	    ';
	    $ret = $storage->execute($query);
	    if ($ret) {
		return $ret;
	    }

	case '0.8.3':
	case '0.8.4':
	case '0.8.5':
	case '0.8.6':
	case '0.8.7':
	case '0.8.8':
	case '0.9.1':
	    /* Move thumbnail dir from g2data/ to g2data/plugins_data */
	    $oldDir = $gallery->getConfig('data.gallery.base') . 'thumbnail' . $slash;
	    if ($platform->is_dir($oldDir)) {
		if (!$platform->rename($oldDir, $thumbnailDir)) {
		    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
						"Unable to move $oldDir to $thumbnailDir");
		}
	    } else {
		list ($success) = GalleryUtilities::guaranteeDirExists($thumbnailDir);
		if (!$success) {
		    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
						"Unable to create $thumbnailDir");
		}
	    }

	case '0.9.2':
	case '0.9.3':
	    /* Now allow thumbnail for root album; remove parent for mime type thumbnails */
	    /* NOTE:
	     * Upgrade code merged into case '1.0.7' since it changes a superset of this case.
	     */

	case '0.9.4':
	case '0.9.5':
	case '0.9.6':
	case '0.9.7':
	case '0.9.8':
	case '1.0.0':
	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	case '1.0.4':
	case '1.0.5':
	case '1.0.6':
	case '1.0.7':
	    /* Use a new childEntity as the container for all custom thumbnails. */
	    $gallery->guaranteeTimeLimit(60);
	    $storage =& $gallery->getStorage();
	    GalleryCoreApi::requireOnce('modules/thumbnail/classes/ThumbnailImage.class');

	    $ret = ThumbnailModuleExtras::_createThumbnailContainer($module);
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $containerId) = $module->getParameter('id.container');
	    if ($ret) {
		return $ret;
	    }
	    $containerId = (int)$containerId;

	    list ($ret, $rootId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return $ret;
	    }

	    /* Before 0.9.3, mime-type thumbnails had a parentId of rootId. After 0.9.3 it was 0. */
	    $query = '
	    SELECT
	      [ThumbnailImage::id], [ThumbnailImage::fileName]
	    FROM
	      [ThumbnailImage], [GalleryChildEntity]
	    WHERE
	      [ThumbnailImage::id] = [GalleryChildEntity::id]
	      AND
	      [GalleryChildEntity::parentId] IN (?,?)
	    ';
	    list ($ret, $results) =
	    	$gallery->search($query, array((int)$rootId, 0));
	    if ($ret) {
		return $ret;
	    }
	    $ids = $fileNames = array();
	    while ($result = $results->nextResult()) {
		$ids[] = (int)$result[0];
		$fileNames[(int)$result[0]] = $result[1];
	    }

	    $fileSystemEntityQuery = '
		INSERT INTO [GalleryFileSystemEntity] ([::id], [::pathComponent])
		VALUES (?, ?)
	    ';

	    if (!empty($ids)) {
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($ids);
		if ($ret) {
		    return $ret;
		}

		/*
		 * Since the class hierarchy for ThumbnailImage changed in this upgrade,
		 * we can't use loadEntitiesById().
		 */
		$parentIdQuery = '
		UPDATE [GalleryChildEntity]
		SET [::parentId] = ?
		WHERE [GalleryChildEntity::id] = ?
		';

		foreach ($ids as $id) {
		    $gallery->guaranteeTimeLimit(30);
		    /* Change the parent. */
		    $ret = $storage->execute($parentIdQuery, array($containerId, $id));
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }

		    /* Add the necessary rows for a FileSystemEntity. */
		    $ret = $storage->execute($fileSystemEntityQuery, array($id, $fileNames[$id]));
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return $ret;
		}

		$ret = $storage->checkPoint();
		if ($ret) {
		    return $ret;
		}
	    }

	    /* Custom thumbnails are now stored hierarchically, not flat anymore. */
	    $query = '
	    SELECT
	      [ThumbnailImage::id], [ThumbnailImage::fileName]
	    FROM
	      [ThumbnailImage], [GalleryChildEntity]
	    WHERE
	      [ThumbnailImage::id] = [GalleryChildEntity::id]
	      AND
	      [GalleryChildEntity::parentId] <> ?
	    ';
	    $gallery->guaranteeTimeLimit(30);
	    list ($ret, $results) = $gallery->search($query, array($containerId));
	    if ($ret) {
		return $ret;
	    }
	    $ids = $fileNames = array();
	    while ($result = $results->nextResult()) {
		$ids[] = (int)$result[0];
		$fileNames[(int)$result[0]] = $result[1];
	    }

	    if (!empty($ids)) {
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($ids);
		if ($ret) {
		    return $ret;
		}

		foreach ($ids as $id) {
		    $gallery->guaranteeTimeLimit(30);
		    /* Add the necessary rows for a FileSystemEntity. */
		    $ret = $storage->execute($fileSystemEntityQuery, array($id, $fileNames[$id]));
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }

		    /* Move the custom thumbnail to its new home. */
		    list ($ret, $thumb) = GalleryCoreApi::loadEntitiesById($id, 'ThumbnailImage');
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		    /* The entity might be obsolete from the cache. */
		    list ($ret, $thumb) = $thumb->refresh();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		    list ($ret, $path) = $thumb->fetchPath();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		    $dir = dirname($path);
		    list ($success) = GalleryUtilities::guaranteeDirExists($dir);
		    if (!$success) {
			GalleryCoreApi::releaseLocks($lockId);
			return GalleryCoreApi::error(ERROR_BAD_PATH, __FILE__, __LINE__,
						     sprintf("Could not create folder '%s'", $dir));
		    }
		    $oldPath = $thumb->getThumbnailDir() . $fileNames[$id];
		    if ($platform->file_exists($oldPath)) {
			$success = $platform->rename($oldPath, $path);
		    	if (!$success) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return GalleryCoreApi::error(ERROR_BAD_PATH, __FILE__, __LINE__,
					sprintf("Failed to rename(%s, %s)", $oldPath, $path));
			}
		    } else {
			$message = sprintf('Moving custom thumbnail with id %d and parentId %d ' .
					   'failed! File "%s" does not exist.', $thumb->getId(),
					   $thumb->getparentId(), $oldPath);
			if (!$platform->file_exists($path)) {
			    /* Ignore failure */
			    $platform->copy(
				dirname(dirname(__FILE__)) . '/core/data/broken-image.gif', $path);
			    $gallery->debug($message . ' Using image placeholder instead.');
			} else {
			    $gallery->debug($message . " And file $path already exists, skipping.");
			}
		    }
		    $ret = $thumb->save();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }

		    /* Atomic transactions for each operation that involves moving of a file */
		    $ret = $storage->checkPoint();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return $ret;
		}
	    }

	    /*
	     * We no longer need the fileName in the ThumbnailImage class since it inherits
	     * the pathComponent from FileSystemEntity.
	     */
	    $ret = $storage->configureStore($module->getId(), array('ThumbnailImage:1.1'));
	    if ($ret) {
		return $ret;
	    }

	case '1.0.8':
	case '1.0.9':
	case '1.0.9.1':
	    /* .mo file migration */
	case '1.0.10':
	case '1.0.11':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.0.12':
	    /* Require minimum core API 7.45 */
	case '1.0.13':

	case 'end of upgrade path':
	    /*
	     * Leave this bogus case at the end of the legitimate case statements so that we
	     * always properly terminate our upgrade path with a break.
	     */
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					 sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	GalleryCoreApi::requireOnce('modules/thumbnail/classes/ThumbnailHelper.class');

	/* Register toolkit operation for any mime type settings already in the database */
	list ($ret, $mimeTypeMap) = ThumbnailHelper::fetchMimeTypeMap();
	if ($ret) {
	    return array($ret, null);
	}
	if (!empty($mimeTypeMap)) {
	    $ret = ThumbnailHelper::registerToolkitOperation(array_keys($mimeTypeMap));
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Set some defaults if not already supported */
	list ($ret, $toolkitMime) = ThumbnailHelper::fetchToolkitSupport();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = array();
	GalleryCoreApi::requireOnce('modules/thumbnail/classes/ThumbnailImage.class');
	foreach (array(
	    'G2audio.jpg' => array('audio/mpeg', 'audio/x-wav', 'audio/x-aiff',
				   'audio/midi', 'audio/basic', 'audio/x-ms-wma'),
	    'G2video.jpg' => array('video/mpeg', 'video/quicktime', 'video/x-msvideo',
				   'video/x-ms-asf', 'video/x-ms-wmv', 'video/x-flv')
	    ) as $mimeImage => $mimeList) {
	    foreach ($mimeList as $mime) {
		if (!isset($toolkitMime[$mime])) {
		    if (!isset($itemId[$mimeImage])) {
			list ($ret, $itemId[$mimeImage]) = ThumbnailHelper::addItem($mime, null,
			    $mimeImage, dirname(__FILE__) . '/images/' . $mimeImage, 'image/jpeg',
			    array('width' => 400, 'height' => 352),
			    new ThumbnailImage());
			if ($ret) {
			    return array($ret, null);
			}
		    } else {
			$ret = ThumbnailHelper::updateItem($itemId[$mimeImage], $mime);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}
	    }
	}

	return array(null, null);
    }

    /**
     * Create the container entity for all the custom thumbnails.
     * @param ThumbnailModule $module
     * @return GalleryStatus a status code
     */
    function _createThumbnailContainer($module) {
	list ($ret, $container) =
		GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryChildEntity');
	if ($ret) {
	    return $ret;
	}
	$ret = $container->createRoot();
	if ($ret) {
	    return $ret;
	}
	$ret = $container->save();
	if ($ret) {
	    return $ret;
	}

	$ret = $module->setParameter('id.container', $container->getId());
	if ($ret) {
	    return $ret;
	}

	return null;
    }
}
?>
