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

GalleryCoreApi::requireOnce('modules/picasa/classes/Picasa2DataParser.class');

/**
 * Perform import
 * @package Picasa
 * @subpackage UserInterface
 * @author Waldemar Schlackow <waldemar@opencodes.org> based on the migrate module by Jesse Mullan
 * @version $Revision: 17580 $
 */
class ConfirmPicasaImportController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	/* Verify that active user is an admin */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* build framework for error and status arrays */
	$error = $status = array();

	if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'picasa.SelectPicasaExportPath';
	} else if (isset($form['action']['import'])) {
	    $templateAdapter =& $gallery->getTemplateAdapter();
	    $templateAdapter->registerTrailerCallback(
		array($this, 'performImport'), array($form));
	    $results['delegate']['view'] = 'core.ProgressBar';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else if (empty($results['delegate'])) {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'picasa.ConfirmPicasaImport';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    /**
     * Actually performs the import from Picasa into G2
     *
     * @param array $form the form variables
     * @return GalleryStatus a status code
     */
    function performImport($form) {
	global $gallery;
	$storage = $gallery->getStorage();
	$templateAdapter =& $gallery->getTemplateAdapter();

	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();

	$finishedAlbums = $albumPosition = array();

	$status = array('albumId' => false,
	    'albumImportFailure' => array(),
	    'albumImportSuccess' => array(),
	    'pictureImportSuccess' => array(),
	    'pictureImportFailure' => array());

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'picasa');
	if ($ret) {
	    return $ret;
	}

	$sourceEncoding = 'UTF-8';

	list ($ret, $markupType) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	if ($ret) {
	    return $ret;
	}

	$itemsCreated = $picasaAlbum = array();

	/*
	 * Check to see if the user selected a destination.  If not,
	 * default to the root album of the new gallery install.
	 */
	if (!isset($form['destinationAlbumId'])) {
	    list ($ret, $rootId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return $ret;
	    }
	    $form['destinationAlbumId'] = $rootId;
	}

	/* Convert the form variables into local temporary variables */
	$picasaXmlPath = $form['picasaXmlPath'];
	$destinationAlbumId = $form['destinationAlbumId'];

	/*
	 * Import the Album (as of now Picasa supports only the export of a
	 * single album and there is no concept of subalbums)
	 */

	GalleryCoreApi::requireOnce('modules/picasa/classes/Picasa2DataParser.class');
	GalleryCoreApi::requireOnce('modules/picasa/classes/PicasaImportHelper.class');

	list ($ret, $picasaAlbum) = Picasa2DataParser::getAlbum($picasaXmlPath);
	if ($ret) {
	    return $ret;
	}

	$totalItemsToImport = count($picasaAlbum['images']);

	if (empty($totalItemsToImport)) {
	    $totalItemsToImport = 1;
	}
	$i = 0;

	$numberOfItemsImported = 0;
	$gallery->guaranteeTimeLimit(30);
	$newAlbumInstanceId = false;

	$sanitizedAlbumName = $picasaAlbum['albumName'];
	GalleryUtilities::sanitizeInputValues($sanitizedAlbumName);
	/* We definitely need notices before each album */
	$templateAdapter->updateProgressBar(
	    $module->translate('Importing albums'),
	    $sanitizedAlbumName, $numberOfItemsImported / $totalItemsToImport);

	$desiredname = $platform->legalizePathComponent($picasaAlbum['albumName']);
	$k = 0;
	$invalidName = true;
	while ($invalidName) {
	    list ($ret, $existingAlbumId) =
		GalleryCoreApi::fetchChildIdByPathComponent($destinationAlbumId, $desiredname);
	    if ($ret) {
		if (!$ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		    return $ret;
		} else {
		    $invalidName = false;
		}
	    } else {
		$desiredname = $picasaAlbum['albumName'] . '_' . $k++;
	    }
	}

	/* Make sure we have permission to edit the target item */
	$ret = GalleryCoreApi::assertHasItemPermission($destinationAlbumId, 'core.addAlbumItem');
	if ($ret) {
	    return $ret;
	}

	/* Try to load targeted parent */
	list ($ret, $targetAlbumObject) =
	    GalleryCoreApi::loadEntitiesById($destinationAlbumId, 'GalleryAlbumItem');
	if ($ret) {
	    return $ret;
	}

	/* Get a lock on said parent */
	list ($ret, $importLockIds[]) = GalleryCoreApi::acquireReadLock($destinationAlbumId);
	if ($ret) {
	    return $ret;
	}

	/*
	 * If everything is good so far, we create a new instance to be our new
	 * album
	 */
	if (!empty($desiredname) && $platform->isLegalPathComponent($desiredname)) {
	    list ($ret, $newAlbumInstance) =
		GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryAlbumItem');
	    if ($ret) {
		return $ret;
	    }
	    if (!isset($newAlbumInstance)) {
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	    }

	    /* this is where the album is actually created */
	    $ret = $newAlbumInstance->create($destinationAlbumId, $desiredname);
	    if ($ret) {
		return $ret;
	    }

	    $newAlbumInstanceId = $newAlbumInstance->getId();

	    /* load up the album with metadata from the old album */
	    if (isset($picasaAlbum['albumName'])) {
		$newAlbumInstance->setTitle(
		    PicasaImportHelper::convertHtml($markupType, $picasaAlbum['albumName'],
						    $sourceEncoding));
	    }
	    if (isset($picasaAlbum['albumCaption'])) {
		$newAlbumInstance->setDescription(
		    PicasaImportHelper::convertHtml($markupType, $picasaAlbum['albumCaption'],
						    $sourceEncoding));
		$newAlbumInstance->setSummary(
		    PicasaImportHelper::convertHtml($markupType, $picasaAlbum['albumCaption'],
						    $sourceEncoding));
	    }
	    $newAlbumInstance->setKeywords('');

	    $ret = $newAlbumInstance->save();
	    if ($ret) {
		return $ret;
	    }

	    $itemsCreated[] = $newAlbumInstance->getId();

	    $finishedAlbums[$sanitizedAlbumName] = $newAlbumInstanceId;
	    $status['albumImportSuccess'][$sanitizedAlbumName] = $newAlbumInstanceId;
	    $status['albumId'] = $newAlbumInstanceId;
	}
	/*
	 * we can let the parent album (and anything else that we might
	 * have tied up) be edited by others now
	 */

	$ret = GalleryCoreApi::releaseLocks($importLockIds);
	if ($ret) {
	    return $ret;
	} else {
	    $importLockIds = array();
	}

	list ($ret, $importLockIds[]) =
	    GalleryCoreApi::acquireReadLock($newAlbumInstanceId);
	if ($ret) {
	    return $ret;
	}

	$photos = $picasaAlbum['images'];
	$numberOfItemsImported = 1;
	foreach ($photos as $j => $importAlbumItem) {
	    $gallery->guaranteeTimeLimit(30);
	    $filepath = $picasaXmlPath . str_replace('/', $slash, $importAlbumItem['path']);
	    $sanitizedFilename = $filename = $importAlbumItem['name'];
	    GalleryUtilities::sanitizeInputValues($sanitizedFilename);
	    $filecaption = PicasaImportHelper::convertHtml($markupType, $importAlbumItem['caption'],
		$sourceEncoding);
	    $templateAdapter->updateProgressBar(
		$module->translate('Importing photos'),
		$sanitizedAlbumName . ' ' . $sanitizedFilename,
		$numberOfItemsImported / $totalItemsToImport);
	    list ($base, $extension) =
		GalleryUtilities::getFileNameComponents($sanitizedFilename);
	    list ($ret, $mimeType) =
		GalleryCoreApi::convertExtensionToMime($extension);
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
		$filepath, $filename, $base, $filecaption,
		$filecaption, $mimeType, $newAlbumInstanceId);
	    if ($ret) {
		return $ret;
	    }
	    $itemsCreated[] = $newItem->getId();
	    $numberOfItemsImported++;
	    if ($ret) {
		return $ret;
	    }
	    $status['pictureImportSuccess'][$sanitizedFilename] = $newItem->getId();

	}

	$session =& $gallery->getSession();
	$session->putStatus($status);

	$redirect['view'] = 'core.SiteAdmin';
	$redirect['subView'] = 'picasa.ImportResults';

	$urlGenerator =& $gallery->getUrlGenerator();
	$templateAdapter->completeProgressBar($urlGenerator->generateUrl($redirect));

	$ret = GalleryCoreApi::releaseLocks($importLockIds);
	if ($ret) {
	    return $ret;
	}
	return null;
    }
}

/**
 * Confirm selections before performing import
 */
class ConfirmPicasaImportView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	/* Verify that active user is an admin */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$platform =& $gallery->getPlatform();
	$illegalAlbumNames = $existingAlbums = $titles = array();

	$picasaXmlPath = GalleryUtilities::getRequestVariables('picasaXmlPath');
	$destinationAlbumId = GalleryUtilities::getRequestVariables('destinationAlbumId');

	list ($ret, $album) = Picasa2DataParser::getAlbum($picasaXmlPath);
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $targetAlbum) =
	    GalleryCoreApi::loadEntitiesById($destinationAlbumId, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	$albumName = $album['albumName'];
	list ($ret, $existingAlbumId) =
	    GalleryCoreApi::fetchChildIdByPathComponent($destinationAlbumId,
	    $platform->legalizePathComponent($albumName));
	if ($ret) {
	    if (!$ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		return array($ret, null);
	    }
	} else {
	    $existingAlbums[$albumName] = $existingAlbumId;
	}
	if (!$platform->isLegalPathComponent($albumName)) {
	    $illegalAlbumNames[$albumName] = $platform->legalizePathComponent($albumName);
	}

	$ConfirmPicasaImport = array();
	$ConfirmPicasaImport['album'] = $album;
	$ConfirmPicasaImport['picasaXmlPath'] = $picasaXmlPath;
	$ConfirmPicasaImport['existingAlbums'] = $existingAlbums;
	$ConfirmPicasaImport['illegalAlbumNames'] = $illegalAlbumNames;
	$ConfirmPicasaImport['destinationAlbumId'] = $destinationAlbumId;
	$ConfirmPicasaImport['targetAlbum'] = (array)$targetAlbum;

	$template->setVariable('ConfirmPicasaImport', $ConfirmPicasaImport);
	$template->setVariable('controller', 'picasa.ConfirmPicasaImport');
	return array(null, array('body' => 'modules/picasa/templates/ConfirmPicasaImport.tpl'));
    }
}
?>
