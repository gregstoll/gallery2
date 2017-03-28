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
 * Controller to process the uploading of photos from Windows XP.
 * Each photo is uploaded via a seperate HTTP request.  The initial response from
 * this page is to return JavaScript code that instructs Windows how to upload
 * the photos.  Subsequent requests from Windows include the photo data and text.
 * This page processes the data and applies the options to be used while uploading.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class UploadItemsController extends GalleryController {
    /**
     * ItemAddOption instances to use when handling this request.  Only used by test code.
     * @var array (optionId => ItemAddOption) $_optionInstances
     * @access private
     */
    var $_optionInstances;

    /**
     * Tests can use this method to hardwire a specific set of option instances to use.
     * This avoids situations where some of the option instances will do unpredictable
     * things and derail the tests.
     *
     * @param array $optionInstances (optionId => ItemAddOption, ...)
     */
    function setOptionInstances($optionInstances) {
	$this->_optionInstances = $optionInstances;
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$results = $error = $status = array();
	if (isset($form['action']['uploadItem'])) {
	    if (!isset($form['albumId'])) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }
	    $form['albumId'] = (int)$form['albumId'];

	    list ($ret, $canView) =
		GalleryCoreApi::hasItemPermission($form['albumId'], 'core.view');
	    if ($ret) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }

	    /* Check the user has permissions in this album */
	    $ret = GalleryCoreApi::assertHasItemPermission($form['albumId'], 'core.addDataItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Get the file passed in this submission */
	    $file = GalleryUtilities::getFile('userFile', false);
	    list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLock($form['albumId']);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (empty($file['name'])) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }

	    /* The Windows Web Publishing Wizard does not send the file name in UTF-8 */
	    $sourceEncoding = $this->_getCharsetFromRequest();
	    if ($gallery->getDebug()) {
		$gallery->debug("Source encoding from Windows client: $sourceEncoding");
	    }
	    $file['name'] = GalleryCoreApi::convertToUtf8($file['name'], $sourceEncoding);

	    /* Get the mime type. */
	    list ($ret, $mimeType) = GalleryCoreApi::getMimeType($file['name'], $file['type']);
	    if ($ret) {
		return array($ret, null);
	    }

	    $title = basename($file['name']);
	    $caption = '';
	    $description = '';
	    if (!empty($form['stripExtensions'])) {
		$title = GalleryUtilities::getFileBase($title);
	    }
	    if (!empty($form['setCaptions'])) {
		$caption = $title;
	    }
	    list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
		$file['tmp_name'], basename($file['name']), $title, $caption, $description,
		$mimeType, $form['albumId']);
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockIds);
		return array($ret, null);
	    }
	    $ret = GalleryCoreApi::releaseLocks($lockIds);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($this->_optionInstances)) {
		$optionInstances = $this->_optionInstances;
	    } else {
		GalleryCoreApi::requireOnce('modules/core/ItemAdd.inc');
		list ($ret, $optionInstances) = ItemAddOption::getAllAddOptions();
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    /* Allow ItemAddOptions to process added item(s) */
	    $session =& $gallery->getSession();
	    if ($session->exists('publishxp.extraOptionsForm')) {
		$extraOptionsForm = unserialize($session->get('publishxp.extraOptionsForm'));
	    } else {
		$extraOptionsForm = array();
	    }
	    foreach ($optionInstances as $option) {
		list ($ret, $optionErrors, $optionWarnings) =
		    $option->handleRequestAfterAdd($extraOptionsForm, array($newItem));
		if ($ret) {
		    return array($ret, null);
		}

		/*
		 * Swallow option warnings and errors for now.  XP uploads the images one at a
		 * time, so the right way to handle this would probably be to store the results
		 * into the session and then redirect to a final status page after all submissions
		 * are complete.
		 */
	    }

	    $redirect['view'] = 'publishxp.UploadedItem';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'publishxp.UploadItems';
	}
	$results['status'] = $status;
	$results['error'] = $error;
	return array(null, $results);
    }

    /**
     * Detects the specified Windows locale and translates it into a charset.
     *
     * Documentation:
     * http://windowssdk.msdn.microsoft.com/en-us/library/ms649627.aspx
     *
     * @return string charset (MS Windows code page)
     * @access private
     */
    function _getCharsetFromRequest() {
	$lcid = (int)GalleryUtilities::getRequestVariablesNoPrefix('lcid');
	/*
	 * Lookup table: Locale ID to Code Page. Source:
	 * http://www.microsoft.com/globaldev/reference/oslocversion.mspx
	 */
	switch ($lcid) {
	    case 1054: /* Thai */
		$codePage = 874;
		break;
	    case 1041: /* Japanese */
		$codePage = 932;
		break;
	    case 2052: /* Chinese (Simplified) */
		$codePage = 936;
		break; 
	    case 1042: /* Korean */
		$codePage = 949;
		break;
	    case 1028: /* Chinese (Traditional) */
	    case 3076: /* Chinese (Hong Kong) */
		$codePage = 950;
		break;
	    case 1050: /* Croatian */
	    case 1029: /* Czech */
	    case 1038: /* Hungarian */
	    case 1045: /* Polish */
	    case 1048: /* Romanian */
	    case 1051: /* Slovak */
	    case 1060: /* Slovenian */
		$codePage = 1250;
		break;
	    case 1026: /* Bulgarian */
	    case 1049: /* Russian */
		$codePage = 1251;
		break;
	    case 1046: /* Brazilian */
	    case 1027: /* Catalan */
	    case 1030: /* Danish */
	    case 1043: /* Dutch */
	    case 1033: /* English */
	    case 1035: /* Finnish */
	    case 1036: /* French */
	    case 1031: /* German */
	    case 1040: /* Italian */
	    case 1044: /* Norwegian */
	    case 2070: /* Portuguese */
	    case 3082: /* Spanish */
	    case 1053: /* Swedish */
		$codePage = 1252;
		break;
	    case 1032: /* Greek */
		$codePage = 1253;
		break;
	    case 1055: /* Turkish */
		$codePage = 1254;
		break;
	    case 1037: /* Hebrew */
		$codePage = 1255;
		break;
	    case 1025: /* Arabic */
		$codePage = 1256;
		break;
	    case 1061: /* Estonian */
	    case 1062: /* Latvian */
	    case 1063: /* Lithuanian */
		$codePage = 1257;
		break;
	    default: $codePage = 1252;
	}
	return 'CP' . $codePage;
    }
}

/**
 * View to process the uploading of photos from Windows XP.
 * Each photo is uploaded via a seperate HTTP request.  The initial response from
 * this page is to return JavaScript code that instructs Windows how to upload
 * the photos.  Subsequent requests from Windows include the photo data and text.
 * This page processes the data and applies the options to be used while uploading.
 */
class UploadItemsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	if ($form['formName'] != 'UploadItems') {
	    $form['formName'] = 'UploadItems';
	}

	list ($UploadItems['albumId'],
	      $UploadItems['stripExtensions'],
	      $UploadItems['setCaptions']) = GalleryUtilities::getRequestVariables(
		  'albumId', 'stripExtensions', 'setCaptions');

	if (empty($UploadItems['stripExtensions'])) {
	    $UploadItems['stripExtensions'] = 0;
	}

	if (empty($UploadItems['setCaptions'])) {
	    $UploadItems['setCaptions'] = 0;
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'publishxp');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Windows Publishing Wizard'));
	$template->setVariable('UploadItems', $UploadItems);
	$template->head('modules/publishxp/templates/Head.tpl');

	return array(null, array('body' => 'modules/publishxp/templates/UploadItems.tpl',
				 'useFullScreen' => true));
    }
}
?>
