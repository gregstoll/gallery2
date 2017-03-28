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
 * ItemAdd plugin to add a new webcam image
 * @package WebCam
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ItemAddWebCam extends ItemAddPlugin {

    /**
     * @see ItemAddPlugin::handleRequest
     */
    function handleRequest($form, &$item) {
	global $gallery;

	$status = $error = array();

    	list ($ret, $hasPermission) = $this->isAppropriate();
	if ($ret) {
	    return array($ret, null, null);
	}
	if (!$hasPermission) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null, null);
	}

	if (isset($form['action']['checkUrl'])) {
	    /* Delegate back to the same view */
	} else if (isset($form['action']['addWebCam'])) {
	    /* fetch url, create item, set WebCam handler, save params */
	    $platform =& $gallery->getPlatform();
	    $tmpDir = $gallery->getConfig('data.gallery.tmp');
	    $url = $form['imageUrl'];
	    GalleryUtilities::unsanitizeInputValues($url, false);
	    $urlComponents = parse_url($url);

	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret) {
		return array($ret, null, null);
	    }

	    if (empty($urlComponents['scheme'])
		    || !in_array($urlComponents['scheme'], array('http', 'file'))) {
		$error[] = 'form[error][imageUrl][invalid]';
		$successfullyCopied = true;
	    } else if ($urlComponents['scheme'] == 'http') {
		$tmpFile = $platform->tempnam($tmpDir, 'webcam');
		list ($successfullyCopied, $response, $headers) =
		    GalleryCoreApi::fetchWebFile($url, $tmpFile);
		if (!$successfullyCopied) {
		    @$platform->unlink($tmpFile);
		}
	    } else if ($isAdmin) {
		/* PHP 4.3.0 trips on file:///path/here (triple slash).. result lands in 'host' */
		if (!isset($urlComponents['path']) && isset($urlComponents['host'])) {
		    $urlComponents['path'] = $urlComponents['host'];
		}
		$tmpFile = $urlComponents['path'];
		$successfullyCopied = $platform->file_exists($tmpFile);
	    } else {
		$error[] = 'form[error][imageUrl][invalid]';
		$successfullyCopied = true;
	    }
	    if (!$successfullyCopied) {
		$error[] = 'form[error][imageUrl][unavailable]';
	    }

	    if (empty($error)) {
		$basename = basename($urlComponents['path']);
		if (!empty($headers['Content-Type'])) {
		    $mimeType = $headers['Content-Type'];
		} else if (!empty($headers['Content-type'])) {
		    $mimeType = $headers['Content-type'];
		} else {
		    $mimeType = 'application/unknown';
		}

		/* Get the mime type. */
		$base = GalleryUtilities::getFileBase($basename);
		list ($ret, $mimeType) = GalleryCoreApi::getMimeType($basename, $mimeType);
		if ($ret) {
		    return array($ret, null, null);
		}

		$title = $basename;
		GalleryUtilities::sanitizeInputValues($title);
		list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
		    $tmpFile, $basename, $title, '', '', $mimeType, $item->getId());
		if ($urlComponents['scheme'] == 'http') {
		    @$platform->unlink($tmpFile);
		}
		if ($ret) {
		    return array($ret, null, null);
		}

		if (GalleryUtilities::isA($newItem, 'GalleryPhotoItem')) {
		    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($newItem->getId());
		    if ($ret) {
			return array($ret, null, null);
		    }
		    $newItem->addOnLoadHandler('WebCam');
		    $ret = $newItem->save();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }
		    $ret = GalleryCoreApi::setPluginParameter('module', 'webcam',
							      'imageUrl', $url, $newItem->getId());
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }
		    $ret = GalleryCoreApi::releaseLocks($lockId);
		    if ($ret) {
			return array($ret, null, null);
		    }

		    $status['addedFiles'][] = array('fileName' => basename($form['imageUrl']),
						    'id' => $newItem->getId(),
						    'warnings' => array());
		} else {
		    $ret = GalleryCoreApi::deleteEntityById($newItem->getId(), 'GalleryDataItem');
		    if ($ret) {
			return array($ret, null, null);
		    }

		    $error[] = 'form[error][imageUrl][notImage]';
		}
	    }
	}

	return array(null, $error, $status);
    }

    /**
     * @see ItemAdd:loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	global $gallery;

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'webcam');
	if ($ret) {
	    return array($ret, null, null);
	}

	if ($form['formName'] != 'ItemAddWebCam') {
	    $form['formName'] = 'ItemAddWebCam';
	    $form['imageUrl'] = '';
	}

	if (isset($form['action']['checkUrl'])) {
	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret) {
		return array($ret, null, null);
	    }
	    if (empty($form['imageUrl'])) {
		$form['error']['imageUrl']['missing'] = 1;
	    } else {
		$urlComponents = parse_url($form['imageUrl']);
		if (empty($urlComponents['scheme'])
			|| !in_array($urlComponents['scheme'], array('http', 'file'))
			|| ($urlComponents['scheme'] == 'file' && !$isAdmin)) {
		    $form['error']['imageUrl']['invalid'] = 1;
		}
	    }
	}

	return array(null, 'modules/webcam/templates/ItemAddWebCam.tpl', 'modules_webcam');
    }

    /**
     * @see ItemAddPlugin::isAppropriate
     */
    function isAppropriate() {
	list ($ret, $param) = GalleryCoreApi::getPluginParameter('module', 'webcam', 'fromweb');
	if ($ret) {
	    return array($ret, null);
	}
	$isAdmin = false;
	if ($param == 'admin') {
	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret) {
		return array($ret, null);
	    }
	}
	return array(null, $param == 'on' || ($isAdmin && $param == 'admin'));
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'webcam');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('WebCam/Live Image'));
    }
}
?>
