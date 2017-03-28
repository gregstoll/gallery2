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
 * This plugin enables the upload of ZIP (or similar) Picasa export archives to import them into G2
 * @package Picasa
 * @subpackage UserInterface
 * @author Waldemar Schlackow <waldemar@opencodes.org>
 * @version $Revision: 18150 $
 */
class ItemAddFromPicasa extends ItemAddPlugin {

    /**
     * @see ItemAddPlugin::handleRequest
     */
    function handleRequest($form, &$item) {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/picasa/classes/Picasa2DataParser.class');
	GalleryCoreApi::requireOnce('modules/picasa/classes/PicasaImportHelper.class');
	$platform =& $gallery->getPlatform();
	$status = $error = array();
	if (empty($form['name']['picasaZipPath'])) {
	    /* No filename was entered */
	    $error[] = 'form[error][picasaZipPath][missing]';
	} else if (isset($form['action']['addFromPicasa'])) {

	    /* Upload any new files */
	    $newItem = null;

	    /* Placeholder for later */
	    if (empty($form['tmp_name']['picasaZipPath']) ||
		    empty($form['size']['picasaZipPath'])) {
		/* Something went wrong with the upload */
		$error[] = 'form[error][picasaZipPath][uploaderror]';
	    } else {
		$file = array('name' => $form['name']['picasaZipPath'],
		      'type' => $form['type']['picasaZipPath'],
		      'tmp_name' => $form['tmp_name']['picasaZipPath'],
		      'size' => $form['size']['picasaZipPath']);

		/* Get the mime type. */
		list ($ret, $mimeType) = GalleryCoreApi::getMimeType($file['name'], $file['type']);
		if ($ret) {
		    return array($ret, null, null);
		}

		list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation($mimeType, 'extract');
		if ($ret) {
		    return array($ret, null, null);
		}
		if (!isset($toolkit)) {
		    /* No Toolkit for that mimetype */
		    $error[] = 'form[error][picasaZipPath][notsupported]';
		} else {
		    $base = $platform->tempnam($gallery->getConfig('data.gallery.tmp'), 'tmp_');
		    $tmpDir = $base . '.dir';
		    if (!$platform->mkdir($tmpDir)) {
			return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
		    }
		    list ($ret, $addedFiles) = $toolkit->performOperation($mimeType, 'extract',
			$file['tmp_name'], $tmpDir, array());
		    if ($ret) {
			@$platform->recursiveRmdir($tmpDir);
			@$platform->unlink($base);
			return array($ret, null, null);
		    }

		    $slash = $platform->getDirectorySeparator();
		    $picasaXmlPath = trim($tmpDir) . $slash;
		    if (!Picasa2DataParser::isValidPicasaXmlPath($picasaXmlPath)) {
			/* The archive does not contain valid Picasa data */
			@$platform->recursiveRmdir($tmpDir);
			@$platform->unlink($base);
			$error[] = 'form[error][picasaZipPath][invalid]';

		    } else {
			list ($ret, $importStatus) =
			    $this->performImport($picasaXmlPath, $item->getId(), $form);
			if ($ret) {
			    @$platform->recursiveRmdir($tmpDir);
			    @$platform->unlink($base);
			    return array($ret, null, null);
			}
			foreach ($importStatus['pictureImportSuccess']
				as $pictureName => $pictureId) {
			    $status['addedFiles'][] = array('fileName' => $pictureName,
				    'id' => $pictureId,
				    'warnings' => array());
			}
		    }
		    $platform->recursiveRmdir($tmpDir);
		    $platform->unlink($base);
		}
	    }
	}
	return array(null, $error, $status);
    }

    /**
     * Actually performs the import from Picasa into G2. (I realise that this very much resembles
     * ConfirmPicasaImport::performImport, and will unify these at a later time, as this is
     * not too easy (mainly because of the progressBar in ConfirmPicasaImport.)
     *
     * @param string $picasaXmlPath the export path, integer the ID of the destination album,
     *        array the form variables
     * @return GalleryStatus a status code
     */
    function performImport($picasaXmlPath, $destinationAlbumId, $form) {
	global $gallery;
	$storage = $gallery->getStorage();

	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();
	$finishedAlbums = $albumPosition = array();

	$status = array('albumId' => false,
			'albumImportFailure' => array(),
			'albumImportSuccess' => array(),
			'pictureImportSuccess' => array(),
			'pictureImportFailure' => array());

	$sourceEncoding = 'UTF-8';

	list ($ret, $markupType) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	if ($ret) {
	    return array($ret, null);
	}

	$itemsCreated = $picasaAlbum = array();

	/*
	 * Check to see if the user selected a destination.  If not,
	 * default to the root album of the new gallery install.
	 */
	if (!isset($destinationAlbumId)) {
	    list ($ret, $rootId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return array($ret, null);
	    }
	    $destinationAlbumId = $rootId;
	}

	/*
	 * Import the Album (as of now Picasa supports only the export of a
	 * single album and there is no concept of subalbums)
	 */

	list ($ret, $picasaAlbum) = Picasa2DataParser::getAlbum($picasaXmlPath);
	if ($ret) {
	    return array($ret, null);
	}

	$totalItemsToImport = count($picasaAlbum['images']);

	if (empty($totalItemsToImport)) {
	    $totalItemsToImport = 1;
	}
	$i = 0;

	$numberOfItemsImported = 0;
	$gallery->guaranteeTimeLimit(30);
	$newAlbumInstanceId = false;

	$desiredname = $platform->legalizePathComponent($picasaAlbum['albumName']);
	$k = 0;
	$invalidName = true;
	while ($invalidName) {
	    list ($ret, $existingAlbumId) =
		GalleryCoreApi::fetchChildIdByPathComponent($destinationAlbumId, $desiredname);
	    if ($ret) {
		if (!$ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		    return array($ret, null);
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
	    return array($ret, null);
	}

	/* Try to load targeted parent */
	list ($ret, $targetAlbumObject) =
	    GalleryCoreApi::loadEntitiesById($destinationAlbumId, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	/* Get a lock on said parent */
	list ($ret, $importLockIds[]) = GalleryCoreApi::acquireReadLock($destinationAlbumId);
	if ($ret) {
	    return array($ret, null);
	}

	/* If everything is good so far, we create a new instance to be our new album */
	if (!empty($desiredname) && $platform->isLegalPathComponent($desiredname)) {
	    list ($ret, $newAlbumInstance) =
		GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryAlbumItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!isset($newAlbumInstance)) {
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }

	    /* this is where the album is actually created */
	    $ret = $newAlbumInstance->create($destinationAlbumId, $desiredname);
	    if ($ret) {
		return array($ret, null);
	    }

	    $newAlbumInstanceId = $newAlbumInstance->getId();

	    /* load up the album with metadata from the old album */
	    if (isset($picasaAlbum['albumName'])) {
		$newAlbumInstance->setTitle(
			PicasaImportHelper::convertHtml($markupType,
			    $picasaAlbum['albumName'], $sourceEncoding));
	    }
	    if (isset($picasaAlbum['albumCaption'])) {
		$newAlbumInstance->setDescription(
			PicasaImportHelper::convertHtml($markupType,
			    $picasaAlbum['albumCaption'], $sourceEncoding));
		$newAlbumInstance->setSummary(
			PicasaImportHelper::convertHtml($markupType,
			    $picasaAlbum['albumCaption'], $sourceEncoding));
	    }
	    $newAlbumInstance->setKeywords('');

	    $ret = $newAlbumInstance->save();
	    if ($ret) {
		return array($ret, null);
	    }

	    $itemsCreated[] = $newAlbumInstance->getId();

	    $finishedAlbums[$picasaAlbum['albumName']] = $newAlbumInstanceId;
	    $status['albumImportSuccess'][$picasaAlbum['albumName']] = $newAlbumInstanceId;
	    $status['albumId'] = $newAlbumInstanceId;
	}

	/*
	 * we can let the parent album (and anything else that we might
	 * have tied up) be edited by others now
	 */
	$ret = GalleryCoreApi::releaseLocks($importLockIds);
	if ($ret) {
	    return array($ret, null);
	} else {
	    $importLockIds = array();
	}

	list ($ret, $importLockIds[]) =
	    GalleryCoreApi::acquireReadLock($newAlbumInstanceId);
	if ($ret) {
	    return array($ret, null);
	}

	$photos = $picasaAlbum['images'];
	$numberOfItemsImported = 1;
	foreach ($photos as $j => $importAlbumItem) {
	    $gallery->guaranteeTimeLimit(30);
	    $filepath = $picasaXmlPath . str_replace('/', $slash, $importAlbumItem['path']);
	    $sanitizedFilename = $filename = $importAlbumItem['name'];
	    GalleryUtilities::sanitizeInputValues($sanitizedFilename);
	    list ($base, $extension) = GalleryUtilities::getFileNameComponents($sanitizedFilename);
	    $filecaption = PicasaImportHelper::convertHtml($markupType, $importAlbumItem['caption'],
		    $sourceEncoding);
	    $filetitle = ($form['set']['title'] == 'filename') ? $base
		: (($form['set']['title'] == 'caption') ? $filecaption : '');
	    $filesummary = empty($form['set']['summary']) ? '' : $filecaption;
	    $filedescription = empty($form['set']['description']) ? '' : $filecaption;
	    list ($ret, $mimeType) = GalleryCoreApi::convertExtensionToMime($extension);
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
		    $filepath, $filename, $filetitle, $filesummary,
		    $filedescription, $mimeType, $newAlbumInstanceId);
	    if ($ret) {
		return array($ret, null);
	    }
	    $itemsCreated[] = $newItem->getId();
	    $numberOfItemsImported++;
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['pictureImportSuccess'][$sanitizedFilename] = $newItem->getId();
	}

	$ret = GalleryCoreApi::releaseLocks($importLockIds);
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $status);
    }

    /**
     * @see ItemAdd:loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	$matches = array();
	$fileUploadsBool = GalleryUtilities::getPhpIniBool('file_uploads');
	$totalUploadSize = ini_get('post_max_size');
	if (preg_match("/(\d+)M/", $totalUploadSize, $matches)) {
	    $totalUploadSize = $matches[1] * 1024 * 1024;
	}

	$maxFileSize = ini_get('upload_max_filesize');
	if (preg_match("/(\d+)M/", $maxFileSize, $matches)) {
	    $maxFileSize = $matches[1] * 1024 * 1024;
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'picasa');
	if ($ret) {
	    return array($ret, null, null);
	}

	foreach (array('totalUploadSize', 'maxFileSize') as $key) {
	    if ($$key >= 1024 * 1024) {
		$$key = $module->translate(array('one' => '%d megabyte',
			    'many' => '%d megabytes',
			    'count' => (int)($$key / (1024 * 1024)),
			    'arg1' => (int)($$key / (1024 * 1024))));
	    } else if ($$key >= 1024) {
		$$key = $module->translate(array('one' => '%d kilobytes',
			    'many' => '%d kilobytes',
			    'count' => (int)($$key / (1024)),
			    'arg1' => (int)($$key / (1024))));
	    }
	}

	if ($form['formName'] != 'ItemAddFromPicasa') {
	    /* First time around, load the form with item data */
	    $form['set'] = array('title' => 'filename', 'summary' => 1, 'description' => 1);
	    $form['formName'] = 'ItemAddFromPicasa';
	}

	$titleList = array('filename' => $module->translate('Base filename'),
			   'caption' => $module->translate('Picasa Caption'),
			   '' => $module->translate('Blank'));

	$template->setVariable('ItemAddFromPicasa',
		array('totalUploadSize' => $totalUploadSize,
		      'maxFileSize' => $maxFileSize,
		      'uploadsPermitted' => $fileUploadsBool,
		      'titleList' => $titleList));

	/* Set the ItemAdmin form's encoding type specially since we're uploading binary files */
	if ($template->hasVariable('ItemAdmin')) {
	    $ItemAdmin =& $template->getVariableByReference('ItemAdmin');
	    $ItemAdmin['enctype'] = 'multipart/form-data';
	} else {
	    $ItemAdmin['enctype'] = 'multipart/form-data';
	    $template->setVariable('ItemAdmin', $ItemAdmin);
	}

	return array(null, 'modules/picasa/templates/ItemAddFromPicasa.tpl', 'modules_picasa');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'picasa');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('From Picasa 2'));
    }
}
?>
