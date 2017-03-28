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
 * This ItemAdd plugin adds items uploaded from the web browser.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemAddFromBrowser extends ItemAddPlugin {

    /**
     * @see ItemAddPlugin::handleRequest
     */
    function handleRequest($form, &$item) {
	global $gallery;

	$status = array();
	$error = array();
	$uploaded = false;
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null, null);
	}
	if (isset($form['action']['addFromBrowser'])) {
	    list ($ret, $markup) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	    if ($ret) {
		return array($ret, null, null);
	    }

	    /* Upload any new files */
	    for ($i = 1; $i <= count($form['name']); $i++) {
		$newItem = null;

		/* Placeholder for later */
		if (!empty($form['tmp_name'][$i]) && !empty($form['size'][$i])) {
		    $file = array('name' => $form['name'][$i],
				  'type' => $form['type'][$i],
				  'tmp_name' => $form['tmp_name'][$i],
				  'size' => $form['size'][$i],
				  'caption' => $form['caption'][$i]);

		    if ($markup == 'html') {
			/* Strip malicious content if html markup allowed */
			$file['caption'] = GalleryUtilities::htmlSafe($file['caption'], true);
		    }

		    $filename = basename($file['name']);
		    $base = GalleryUtilities::getFileBase($filename);

		    list ($ret, $mimeType) = GalleryCoreApi::getMimeType($filename, $file['type']);
		    if ($ret) {
			return array($ret, null, null);
		    }

		    $title = ($form['set']['title'] == 'filename') ? $base
			   : (($form['set']['title'] == 'caption') ? $file['caption'] : '');
		    $summary = empty($form['set']['summary']) ? '' : $file['caption'];
		    $description = empty($form['set']['description']) ? '' : $file['caption'];

		    /*
		     * Don't use uploaded files, because the framework cannot safely copy them.
		     * Move it to our temporary directory first.
		     */

		    $platform =& $gallery->getPlatform();
		    if ($platform->is_uploaded_file($file['tmp_name'])) {
			$tmpFile = $platform->move_uploaded_file($file['tmp_name']);
			if (!$tmpFile) {
			    return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
			}
			$needToDeleteTmpFile = true;
		    } else {
			$tmpFile = $file['tmp_name'];
			$needToDeleteTmpFile = false;
		    }

		    list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
			$tmpFile, $filename, $title, $summary,
			$description, $mimeType, $item->getId());

		    /* Get rid of the tmp file if necessary */
		    if ($needToDeleteTmpFile) {
			@$platform->unlink($tmpFile);
		    }

		    if ($ret) {
			return array($ret, null, null);
		    }

		    $status['addedFiles'][] = array('fileName' => $file['name'],
						    'id' => $newItem->getId(),
						    'warnings' => array());
		    $uploaded = true;
		} else if (!empty($form['name'][$i])) {
		    $error[] = 'form[error][upload][' . $i . ']';
		    if (empty($form['error'])) {
			$form['error'][$i] = -1;
		    }
		    switch($form['error'][$i]) {
		    case UPLOAD_ERR_INI_SIZE:
			$warning = $module->translate(array(
			    'text' => 'Input file %s exceeds maximum permitted file size',
			    'arg1' => $form['name'][$i]));
			break;

		    case UPLOAD_ERR_FORM_SIZE:
			$warning = $module->translate(array(
			    'text' => 'Input file %s exceeds file size specified in the form',
			    'arg1' => $form['name'][$i]));
			break;

		    case UPLOAD_ERR_PARTIAL:
			$warning = $module->translate(array(
			    'text' => 'Input file %s was only partially uploaded',
			    'arg1' => $form['name'][$i]));
			break;

		    default:
			$warning = $module->translate(array(
			    'text' => 'Input file %s was not uploaded. Error %d',
			    'arg1' => $form['name'][$i],
			    'arg2' => $form['error'][$i]));
		    }
		    $status['addedFiles'][] = array('fileName' => $form['name'][$i],
						    'id' => null,
						    'warnings' => array($warning));
		}
	    }
	    if ($uploaded || empty($status)) {
		/*
		 * Some files were uploaded successfully (or not even attempted)
		 * Go to confirmation page, even if there were errors in uploads
		 */
		$error = array();
	    } else {
		GalleryUtilities::putRequestVariable('ItemAddFromBrowserStatus',
						     $status['addedFiles']);
	    }
	}

	return array(null, $error, $status);
    }

    /**
     * @see ItemAdd:loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	$fileUploadsBool = GalleryUtilities::getPhpIniBool('file_uploads');
	$totalUploadSize = ini_get('post_max_size');
	if (preg_match("/(\d+)M/", $totalUploadSize, $matches)) {
	    $totalUploadSize = $matches[1] * 1024 * 1024;
	}

	$maxFileSize = ini_get('upload_max_filesize');
	if (preg_match("/(\d+)M/", $maxFileSize, $matches)) {
	    $maxFileSize = $matches[1] * 1024 * 1024;
	}
	$status = GalleryUtilities::getRequestVariables('ItemAddFromBrowserStatus');

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
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

	if ($form['formName'] != 'ItemAddFromBrowser') {
	    $form['set'] = array('title' => 'filename', 'summary' => 1, 'description' => 1);
	    $form['formName'] = 'ItemAddFromBrowser';
	}

	$titleList = array('filename' => $module->translate('Base filename'),
			   'caption' => $module->translate('Caption'),
			   '' => $module->translate('Blank'));

	$template->setVariable('ItemAddFromBrowser',
	    array('totalUploadSize' => $totalUploadSize,
		  'maxFileSize' => $maxFileSize,
		  'uploadsPermitted' => $fileUploadsBool,
		  'titleList' => $titleList,
		  'status' => $status));

	/* Set the ItemAdmin form's encoding type specially since we're uploading binary files */
	if ($template->hasVariable('ItemAdmin')) {
	    $ItemAdmin =& $template->getVariableByReference('ItemAdmin');
	    $ItemAdmin['enctype'] = 'multipart/form-data';
	} else {
	    $ItemAdmin['enctype'] = 'multipart/form-data';
	    $template->setVariable('ItemAdmin', $ItemAdmin);
	}

	return array(null, 'modules/core/templates/ItemAddFromBrowser.tpl', 'modules_core');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('From Web Browser'));
    }
}
?>
