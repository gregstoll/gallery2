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
 * @package WebCam
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20954 $
 */
class WebCamModule extends GalleryModule {

    function WebCamModule() {
	global $gallery;
	$this->setId('webcam');
	$this->setName($gallery->i18n('WebCam'));
	$this->setDescription($gallery->i18n('Support periodically updated images like a webcam'));
	$this->setVersion('1.1.7'); /* Update upgrade() function below too */
	$this->_templateVersion = 1;
	$this->setGroup('import', $gallery->i18n('Import'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 45));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'webcam');
	if ($ret) {
	    return $ret;
	}
	foreach (array('duration' => 600 /* 10 minutes */,
		       'fromweb' => 'admin') as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	if (!empty($currentVersion)) {
	    switch ($currentVersion) {
	    case '0.8':
	    case '0.8.1':
	    case '0.8.2':
	    case '0.8.3':
	    case '0.8.4':
	    case '0.8.5':
		/* Changed from GalleryLiveImage entity type to onLoadHandler */
		$storage =& $gallery->getStorage();
		$query = 'UPDATE [GalleryEntity]
		    SET [::entityType] = \'GalleryPhotoItem\',
			[::onLoadHandlers] = \'|WebCam|\'
		  WHERE [GalleryEntity::entityType] = \'GalleryLiveImage\'
		    AND [GalleryEntity::onLoadHandlers] IS NULL';
		$ret = $storage->execute($query);
		if ($ret) {
		    return $ret;
		}
		list ($ret, $handlers) = $storage->getFunctionSql('CONCAT',
		    array('[GalleryEntity::onLoadHandlers]', "'WebCam|'"));
		if ($ret) {
		    return $ret;
		}
		$query = 'UPDATE [GalleryEntity]
		    SET [::entityType] = \'GalleryPhotoItem\',
			[::onLoadHandlers] = ' . $handlers . '
		  WHERE [GalleryEntity::entityType] = \'GalleryLiveImage\'
		    AND [GalleryEntity::onLoadHandlers] IS NOT NULL';
		$ret = $storage->execute($query);
		if ($ret) {
		    return $ret;
		}
		break;

	    case '0.9.0':
	    case '0.9.1':
	    case '0.9.2':
	    case '0.9.3':
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
	    case '1.0.5.0.1':
	    case '1.0.5.1':
	    case '1.0.5.2':
	    case '1.1.0':
		/* .mo file migration */
	    case '1.1.1':
	    case '1.1.2':
	    case '1.1.3':
		/* Limiting file:// to admins and from URL to trusted people */
	    case '1.1.4':
		/* Require minimum core API 7.45 */
	    case '1.1.5':
	    case '1.1.6':

	    case 'end of upgrade path':
		break;

	    default:
		return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					     sprintf('Unknown module version %s', $currentVersion));
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryOnLoadHandler', 'WebCamModule', 'WebCam',
	    'modules/webcam/module.inc', 'webcam', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddWebCam', 'ItemAddWebCam',
	    'modules/webcam/ItemAddWebCam.inc', 'webcam', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getOnLoadHandlerIds
     */
    function getOnLoadHandlerIds() {
	return array('WebCam');
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('WebCam'),
				 'view' => 'webcam.WebCamSiteAdmin')));
    }

    /**
     * Check age and refresh from external source if needed..
     */
    function onLoad(&$item, $duringUpgrade) {
	$old = false;
	$id = $item->getId();
	$loop = GalleryDataCache::containsKey("webcam:load-$id");
	/* We don't want to refresh the external source during an upgrade */
	if ($duringUpgrade) {
	    return null;
	}
	list ($ret, $duration) = $this->getParameter('duration');
	if ($ret) {
	    return $ret;
	}
	if (time() - $item->getModificationTimestamp() > $duration && !$loop) {
	    if (GalleryCoreApi::isWriteLocked($id)) {
		/* Item already busy (maybe being deleted).. don't update now */
		return null;
	    }
	    /* Lock and refresh to ensure another request didn't just update image.. */
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($id);
	    if ($ret) {
		return $ret;
	    }
	    GalleryDataCache::put("webcam:load-$id", true);
	    list ($ret, $refresh) = $item->refresh();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    if ($refresh->getSerialNumber() == $item->getSerialNumber()) {
		$old = true;
	    }
	}
	if ($old) {
	    global $gallery;
	    $platform =& $gallery->getPlatform();
	    $tmpDir = $gallery->getConfig('data.gallery.tmp');
	    $backupFile = $platform->tempnam($tmpDir, 'webcambak');
	    list ($ret, $url) = $this->getParameter('imageUrl', $id);
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }

	    $urlComponents = parse_url($url);
	    if ($urlComponents['scheme'] == 'file') {
		/* PHP 4.3.0 trips on file:///path/here (triple slash).. result lands in 'host' */
		if (!isset($urlComponents['path']) && isset($urlComponents['host'])) {
		    $urlComponents['path'] = $urlComponents['host'];
		}
		$tmpFile = $urlComponents['path'];
		$successfullyCopied = $platform->file_exists($tmpFile);
	    } else {
		$tmpFile = $platform->tempnam($tmpDir, 'webcam');
		list ($successfullyCopied, $response, $headers) =
		    GalleryCoreApi::fetchWebFile($url, $tmpFile);
	    }
	    if (!$successfullyCopied) {
		/* Can't get the current image.. just keep what we have now */
	    } else {
		list ($ret, $path) = $item->fetchPath();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return $ret;
		}
		/* Backup previous file */
		$ret = $platform->copy($path, $backupFile);
		if (!$ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE);
		}
		$ret = $platform->copy($tmpFile, $path);
		if (!$ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE);
		}
		$ret = $item->rescan();
		if ($ret) {
		    if (!($ret->getErrorCode() & ERROR_BAD_DATA_TYPE)) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		    /* Problem with new file (maybe html response, not data file); rollback */
		    $platform->copy($backupFile, $path);
		} else {
		    $item->setOriginationTimestamp(time());
		    $ret = $item->save();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		    $ret = GalleryCoreApi::expireDerivativeTreeBySourceIds(array($id));
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		}
		@$platform->unlink($backupFile);
	    }
	    if ($urlComponents['scheme'] != 'file') {
		@$platform->unlink($tmpFile);
	    }
	}
	if (isset($lockId)) {
	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }
}
?>
