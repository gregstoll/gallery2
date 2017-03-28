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
 * Applies watermark on the fly and sends binary item
 * @package Watermark
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 * @todo Testability: Change the class name such that it doesn't collide with core.DownloadItem
 */
class DownloadItemView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * @see GalleryView::shouldSaveSession
     */
    function shouldSaveSession() {
	return false;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	$itemId = (int)GalleryUtilities::getRequestVariables('itemId');
	if (empty($itemId)) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}

	/* Load the item */
	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryEntity');
	if ($ret) {
	    return $ret;
	}

	if (!method_exists($item, 'fetchPath') || !method_exists($item, 'getMimeType')) {
	    /* Avoid information disclosure for bogus entities by acting as if it didn't exist */
	    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	}

	$derivativeType = null;
	if (GalleryUtilities::isA($item, 'GalleryDerivative')) {
	    $derivativeType = $item->getDerivativeType();
	}

	$ret = $this->_assertPermissions($item, $derivativeType);
	if ($ret) {
	    return $ret;
	}

	/* Figure out the filename */
	list ($ret, $pseudoFileName) = GalleryUtilities::getPseudoFileName($item);
	if ($ret) {
	    return $ret;
	}

	/* Don't allow malicious URLs */
	$fileName = GalleryUtilities::getRequestVariables('fileName');
	if (!empty($fileName) && $fileName != $pseudoFileName) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__, 'malicious url');
	}

	/* Get the path to the file */
	list ($ret, $path) = $item->fetchPath();
	if ($ret) {
	    return $ret;
	}

	/* Rebuild derivative cache, if necessary */
	if (!empty($derivativeType)) {
	    list ($ret, $item) = GalleryCoreApi::rebuildDerivativeCacheIfNotCurrent($item->getId());
	    if ($ret) {
		return $ret;
	    }
	}
	$mimeType = $item->getMimeType();

	/* Apply Watermark -- skip for thumbnails */
	if ($derivativeType != DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
	    list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation($mimeType, 'composite');
	    if ($ret) {
		return $ret;
	    }
	    if (!isset($toolkit)) {
		/*
		 * Allow items that can't be watermarked (movies, audio, etc) to be
		 * downloaded.  Uncomment the line below to block these downloads.
		 */
		/* return GalleryCoreApi::error(ERROR_PERMISSION_DENIED); */
	    } else {
		list ($ret, $hotlinkWatermarkId) =
		    GalleryCoreApi::getPluginParameter('module', 'watermark', 'hotlinkWatermarkId');
		if ($ret) {
		    return $ret;
		}
		if (!empty($hotlinkWatermarkId)) {
		    list ($ret, $watermark) =
			GalleryCoreApi::loadEntitiesById($hotlinkWatermarkId, 'WatermarkImage');
		    if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
			/* Ignore error if this watermark has been deleted */
			return $ret;
		    }
		}
	    }
	    if (isset($watermark)) {
		global $gallery;
		$platform =& $gallery->getPlatform();
		$tmpFile = $platform->tempnam($gallery->getConfig('data.gallery.tmp'), 'wmk_');
		list ($ret, $mimeType) = $toolkit->performOperation(
		    $mimeType, 'composite', $path, $tmpFile,
		    array('plugins_data/modules/watermark/' . $watermark->getFileName(),
			  $watermark->getMimeType(),
			  $watermark->getWidth(), $watermark->getHeight(),
			  'manual', $watermark->getXPercentage(), $watermark->getYPercentage()));
		if ($ret) {
		    return $ret;
		}
		$path = $tmpFile;
	    }
	}

	$ret = $this->_sendFile(array('derivativePath' => $path,
				      'mimeType' => $mimeType,
				      'pseudoFileName' => $pseudoFileName));
	if ($ret) {
	    return $ret;
	}

	if (isset($tmpFile)) {
	    @$platform->unlink($tmpFile);
	}

	return null;
    }

    function _sendFile($data) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	/**
	 * Try to prevent Apache's mod_deflate from gzipping the output since it's likely already
	 * a binary file and broken versions of mod_deflate sometimes get the byte count wrong.
	 */
	if (function_exists('apache_setenv') && !@$gallery->getConfig('apacheSetenvBroken')) {
	    @apache_setenv('no-gzip', '1');
	}

	header('Content-type: ' . $data['mimeType']);
	header('Content-Disposition: inline; filename="' . $data['pseudoFileName'] . '"');
	$stats = $platform->stat($data['derivativePath']);
	if ($stats[7] > 0) {
	    header('Content-length: ' . $stats[7]);
	}
	header('Last-Modified: ' . GalleryUtilities::getHttpDate($stats[9]));
	header('Expires: ' . GalleryUtilities::getHttpDate(time() + 31536000));

	/*
	 * Don't use readfile() because it buffers the entire file in memory.  Profiling shows
	 * that this approach is as efficient as fpassthru() but we get to call guaranteeTimeLimit
	 * which prevents it from failing on very large files.
	 */
	if ($fd = $platform->fopen($data['derivativePath'], 'rb')) {
	    while (true) {
		$bits = $platform->fread($fd, 65535);
		if (strlen($bits) == 0) {
		    break;
		}
		print $bits;
		$gallery->guaranteeTimeLimit(30);
	    }
	    $platform->fclose($fd);
	}

	return null;
    }

    /**
     * Assert the required permissions for the given item.
     * @param GalleryChildEntity $item GalleryDataItem or GalleryChildEntity with a data item as
     *        parent. Throws ERROR_MISSING_OBJECT if a non-item has no item as parent.
     * @param mixed $derivativeType
     * @return GalleryStatus
     */
    function _assertPermissions($item, $derivativeType) {
    	global $gallery;
    	$session =& $gallery->getSession();

    	$itemIdForPermission = $item->getId();
	if (!empty($derivativeType)) {
	    $itemIdForPermission = $item->getParentId();
	}

	/* Make sure we have permission */
	if (($ids = $session->get('core.isPrintService')) && in_array($item->getId(), $ids)) {
	    /* Print services only need core.view to get access to full size version of photos */
	    $permission = 'core.view';
	} else {
	    $permission = 'core.viewSource';
	    switch ($derivativeType) {
	    case DERIVATIVE_TYPE_IMAGE_THUMBNAIL:
		$permission = 'core.view';
		break;

	    case DERIVATIVE_TYPE_IMAGE_RESIZE:
		$permission = 'core.viewResizes';
		break;

		/* DERIVATIVE_TYPE_IMAGE_PREFERRED uses core.viewSource */
	    }
	}
	$ret = GalleryCoreApi::assertHasItemPermission($itemIdForPermission, $permission);
	if ($ret) {
	    /* Avoid information disclosure */
	    if ($ret->getErrorCode() & ERROR_PERMISSION_DENIED) {
	    	if ($permission != 'core.view') {
		    list ($ret2, $hasPermission) =
			GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
		    if ($ret2) {
			return $ret2;
		    }
	    	}
	    	if ($permission == 'core.view' || empty($hasPermission)) {
		    $ret->addErrorCode(ERROR_MISSING_OBJECT);
		    return $ret;
	    	}
	    }
	    return $ret;
	}

	return null;
    }
}
?>
