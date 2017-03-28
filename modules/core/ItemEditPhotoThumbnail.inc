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
 * This edit plugin allows you to customize the photo's thumbnail
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemEditPhotoThumbnail extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	global $gallery;

	$status = null;
	$error = array();

	if (isset($form['action']['crop']) || isset($form['action']['reset'])) {
	    /* Load the thumbnail */
	    list ($ret, $thumbnails) =
		GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    if (!empty($thumbnails)) {
		$thumbnail = $thumbnails[$item->getId()];
	    }
	}

	if (isset($form['action']['crop']) && isset($thumbnail)) {
	    /* Get our source */
	    list ($ret, $source) = GalleryCoreApi::loadEntitiesById(
		$thumbnail->getDerivativeSourceId(),
		array('GalleryFileSystemEntity', 'GalleryDerivative'));
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    list ($ret, $lock) = GalleryCoreApi::acquireWriteLock($thumbnail->getId());
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    list ($ret, $thumbnail) = $thumbnail->refresh();
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    if (empty($form['crop']['width'])) {
		$cropX = 0;
		$cropY = 0;
		$cropWidth = 100;
		$cropHeight = 100;
	    } else {
		/* Sanitize inputs to fit within 0-100 (if cropper miscalculates) */
		$cropX = GalleryUtilities::roundToString(
			max(0, 100 * $form['crop']['x'] / $source->getWidth()), 3);
		$cropY = GalleryUtilities::roundToString(
			max(0, 100 * $form['crop']['y'] / $source->getHeight()), 3);
		$cropWidth = GalleryUtilities::roundToString(
			min(100, 100 * $form['crop']['width'] / $source->getWidth()), 3);
		$cropHeight = GalleryUtilities::roundToString(
			min(100, 100 * $form['crop']['height'] / $source->getHeight()), 3);
	    }

	    list ($ret, $operations) =
		GalleryCoreApi::mergeDerivativeOperations($thumbnail->getDerivativeOperations(),
							  sprintf('crop|%s,%s,%s,%s',
								  $cropX,
								  $cropY,
								  $cropWidth,
								  $cropHeight),
							  true);

	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $thumbnail->setDerivativeOperations($operations);

	    $ret = $thumbnail->save();
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    $ret = GalleryCoreApi::releaseLocks($lock);
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    /* Figure out where to redirect upon success */
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $status = $module->translate('Thumbnail cropped successfully');
	} else if (isset($form['action']['reset']) && isset($thumbnail)) {
	    list ($ret, $lock) = GalleryCoreApi::acquireWriteLock($thumbnail->getId());
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    list ($ret, $thumbnail) = $thumbnail->refresh();
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    $operations = GalleryCoreApi::removeDerivativeOperation(
					  'crop', $thumbnail->getDerivativeOperations());
	    $thumbnail->setDerivativeOperations($operations);

	    $ret = $thumbnail->save();
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    $ret = GalleryCoreApi::releaseLocks($lock);
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    /* Figure out where to redirect upon success */
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $status = $module->translate('Thumbnail reset successfully');
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	global $gallery;

	$ItemEditPhotoThumbnail = array();
	$ItemEditPhotoThumbnail['showApplet'] = $thumbnail != null;

	list ($ret, $ItemEditPhotoThumbnail['isAdmin']) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null, null);
	}

	if ($form['formName'] != 'ItemEditPhotoThumbnail') {
	    /* First time around, reset the form */
	    $form['formName'] = 'ItemEditPhotoThumbnail';
	}

	$targetThumbnailSize = 0;
	$crop = array();
	if (preg_match('/thumbnail\|(\d+)/',
		       $thumbnail->getDerivativeOperations(),
		       $matches)) {
	    $targetThumbnailSize = $matches[1];
	}

	if (preg_match('/crop\|([\d\.]+),([\d\.]+),([\d\.]+),([\d\.]+)/',
		       $thumbnail->getDerivativeOperations(),
		       $matches)) {
	    $crop['leftPercent'] = $matches[1];
	    $crop['topPercent'] = $matches[2];
	    $crop['widthPercent'] = $matches[3];
	    $crop['heightPercent'] = $matches[4];
	} else {
	    /* No cropping yet */
	    $crop['leftPercent'] = 0;
	    $crop['topPercent'] = 0;
	    $crop['widthPercent'] = 100;
	    $crop['heightPercent'] = 100;
	}

	/*
	 * The source may be quite large.  However, it's the only input image that we can really
	 * display at this point because the resized derivatives though they may be smaller, can
	 * have different derivative commands from the source.
	 */
	list ($ret, $source) = GalleryCoreApi::loadEntitiesById(
	    $thumbnail->getDerivativeSourceId(),
	    array('GalleryFileSystemEntity', 'GalleryDerivative'));
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Convert crop percentages into real pixels */
	$crop['left'] = round($crop['leftPercent'] * $source->getWidth() / 100);
	$crop['top'] = round($crop['topPercent'] * $source->getHeight() / 100);
	$crop['width'] = round($crop['widthPercent'] * $source->getWidth() / 100);
	$crop['height'] = round($crop['heightPercent'] * $source->getHeight() / 100);

	/*
	 * It's possible that the source was created before we had an appropriate image toolkit,
	 * so its dimensions can be set to zero, which will cause us problems.  In that case,
	 * try rescanning it.
	 */
	$width = $source->getWidth();
	if (empty($width)) {
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($source->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $source) = $source->refresh();
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ret = $source->rescan();
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ret = $source->save();
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	/* Make sure we have toolkit support */
	list ($ret, $toolkit) =
	    GalleryCoreApi::getToolkitByOperation($source->getMimeType(), 'crop');
	if ($ret) {
	    return array($ret, null, null);
	}

	$width = $source->getWidth();
	if (empty($width) || !isset($toolkit)) {
	    $ItemEditPhotoThumbnail['editThumbnail']['can']['crop'] = false;
	} else {
	    $ItemEditPhotoThumbnail['editThumbnail']['can']['crop'] = true;
	    if (empty($crop)) {
		$crop['left'] = 0;
		$crop['top'] = 0;
		$crop['width'] = $source->getWidth();
		$crop['height'] = $source->getHeight();
	    }

	    /*
	     * When we make the url for the image, force the session id into it.  Otherwise, the
	     * IE JVM will send a request without the session cookie, then the server will make a
	     * new session and send that cookie back to the JVM, which will overwrite the browser's
	     * session with it, effectively logging out the user.
	     */
	    $urlGenerator =& $gallery->getUrlGenerator();
	    $url = $urlGenerator->generateUrl(
		array('view' => 'core.DownloadItem', 'itemId' => $source->getId()),
		array('forceSessionId' => true, 'forceFullUrl' => true, 'htmlEntities' => false));

	    $ItemEditPhotoThumbnail['editThumbnail']['appletCodeBase'] =
		GalleryUtilities::convertPathToUrl(dirname(__FILE__)) . '/plugins';
	    $ItemEditPhotoThumbnail['editThumbnail']['appletJarFile'] = 'ImageCrop.jar';
	    $ItemEditPhotoThumbnail['editThumbnail']['imageUrl'] = $url;
	    $ItemEditPhotoThumbnail['editThumbnail']['imageWidth'] = $source->getWidth();
	    $ItemEditPhotoThumbnail['editThumbnail']['imageHeight'] = $source->getHeight();
	    $ItemEditPhotoThumbnail['editThumbnail']['cropLeft'] = $crop['left'];
	    $ItemEditPhotoThumbnail['editThumbnail']['cropTop'] = $crop['top'];
	    $ItemEditPhotoThumbnail['editThumbnail']['cropWidth'] = $crop['width'];
	    $ItemEditPhotoThumbnail['editThumbnail']['cropHeight'] = $crop['height'];
	    $ItemEditPhotoThumbnail['editThumbnail']['targetThumbnailSize'] = $targetThumbnailSize;

	    $aspectlist = array();
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $aspectRatioList[] = array('label' => $module->translate('Photo 5x3'),
				       'width' => 5,
				       'height' => 3);
	    $aspectRatioList[] = array('label' => $module->translate('Photo 6x4'),
				       'width' => 6,
				       'height' => 4);
	    $aspectRatioList[] = array('label' => $module->translate('Photo 7x5'),
				       'width' => 7,
				       'height' => 5);
	    $aspectRatioList[] = array('label' => $module->translate('Photo 10x8'),
				       'width' => 10,
				       'height' => 8);
	    $aspectRatioList[] = array('label' => $module->translate('Fullscreen 4x3'),
				       'width' => 4,
				       'height' => 3);
	    $aspectRatioList[] = array('label' => $module->translate('Widescreen 16x9'),
				       'width' => 16,
				       'height' => 9);
	    $aspectRatioList[] = array('label' => $module->translate('CinemaScope 2.35x1'),
				       'width' => 47,
				       'height' => 20);
	    $aspectRatioList[] = array('label' => $module->translate('Square 1x1'),
				       'width' => 1,
				       'height' => 1);
	    $aspectRatioList[] = array('label' => $module->translate('As Image'),
				       'width' => $item->getWidth(),
				       'height' => $item->getHeight());
	    $ItemEditPhotoThumbnail['editThumbnail']['aspectRatioList'] = $aspectRatioList;

	    $orientationList = array();
	    $orientationList['landscape'] = $module->translate('Landscape');
	    $orientationList['portrait'] = $module->translate('Portrait');
	    $ItemEditPhotoThumbnail['editThumbnail']['orientationList'] = $orientationList;


	    /*
	     * Figure out which aspect ratio / orientation is closest to the current crop
	     * settings so that we can start out with those values selected in the dropdowns.
	     */
	    $currentAspect = round($crop['width'] / $crop['height'], 2);
	    $i = 0;

	    /* Set defaults */
	    $selectedAspect = 0;
	    $selectedOrientation = 'portrait';
	    $ItemEditPhotoThumbnail['editThumbnail']['cropRatioWidth'] =
		$aspectRatioList[0]['width'];
	    $ItemEditPhotoThumbnail['editThumbnail']['cropRatioHeight'] =
		$aspectRatioList[0]['height'];

	    foreach ($aspectRatioList as $aspect) {
		$landscapeCompare = round($aspect['width'] / $aspect['height'], 2);
		$portraitCompare = round($aspect['height'] / $aspect['width'], 2);
		if (abs($currentAspect - $landscapeCompare) <= 0.03) {
		    $selectedAspect = $i;
		    $selectedOrientation = 'landscape';
		    break;
		} else if (abs($currentAspect - $portraitCompare) <= 0.03) {
		    $selectedAspect = $i;
		    $selectedOrientation = 'portrait';
		    break;
		}
		$i++;
	    }
	    $ItemEditPhotoThumbnail['editThumbnail']['selectedAspect'] = $selectedAspect;
	    $ItemEditPhotoThumbnail['editThumbnail']['selectedOrientation'] = $selectedOrientation;
	    $ItemEditPhotoThumbnail['editThumbnail']['cropRatioWidth'] =
		$aspectRatioList[$selectedAspect]['width'];
	    $ItemEditPhotoThumbnail['editThumbnail']['cropRatioHeight'] =
		$aspectRatioList[$selectedAspect]['height'];
	}

	$template->style('modules/core/templates/ItemEditPhotoThumbnail.css');
	$template->setVariable('ItemEditPhotoThumbnail', $ItemEditPhotoThumbnail);
	$template->setVariable('controller', 'core.ItemEditPhotoThumbnail');
	return array(null,
		     'modules/core/templates/ItemEditPhotoThumbnail.tpl', 'modules_core');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	return ($thumbnail != null && GalleryUtilities::isA($item, 'GalleryPhotoItem'));
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Crop Thumbnail'));
    }
}
?>
