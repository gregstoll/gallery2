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
 * This controller handles rewriting URLs from Gallery1 to the new style Gallery2 locations
 * @package Migrate
 * @subpackage Redirect
 * @author Joseph Elwell <jelwell@yahoo.com>
 * @version $Revision: 17785 $
 */
class RedirectController extends GalleryController {

    /**
     * @see GalleryController::shouldSaveSession
     */
    function shouldSaveSession() {
	return false;
    }

    /**
     * @see GalleryController::omitAuthTokenCheck
     */
    function omitAuthTokenCheck() {
	return true;
    }

    /**
     * @see GalleryController::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
    * @see GalleryController::handleRequest
    */
    function handleRequest($form) {
	global $gallery;
	$phpVm = $gallery->getPhpVm();
	GalleryCoreApi::requireOnce('modules/migrate/classes/G1MigrateHelper.class');

	$path = GalleryUtilities::getRequestVariables('path');
	$path = preg_split('|/|', $path, -1, PREG_SPLIT_NO_EMPTY);
	$view = 'core.ShowItem';

	if (count($path) > 1) {
	    /* Check for direct link to image file (name.type, name.thumb.type, name.sized.type) */
	    $file = explode('.', $path[1]);
	    switch (count($file)) {
	    case 3:
		if ($file[1] == 'sized') {
		    $derivative = DERIVATIVE_TYPE_IMAGE_RESIZE;
		} else if ($file[1] == 'thumb') {
		    $derivative = DERIVATIVE_TYPE_IMAGE_THUMBNAIL;
		}
	    case 2:
		$view = 'core.DownloadItem';
		$path[1] = $file[0];
		if (!isset($derivative)) {
		    $derivative = DERIVATIVE_TYPE_IMAGE_PREFERRED;
		}
	    }

	    list ($ret, $id) = G1MigrateHelper::fetchMapping($path[0], $path[1]);
	    if ($ret) {
		return array($ret, null);
	    }
	    $itemId = $id;

	    if (isset($id) && isset($derivative)) {
		switch ($derivative) {
		case DERIVATIVE_TYPE_IMAGE_PREFERRED:
		    list ($ret, $data) = GalleryCoreApi::fetchPreferredsByItemIds(array($id));
		    if ($ret) {
			return array($ret, null);
		    }
		    break;
		case DERIVATIVE_TYPE_IMAGE_RESIZE:
		    list ($ret, $data) = GalleryCoreApi::fetchResizesByItemIds(array($id));
		    if ($ret) {
			return array($ret, null);
		    }
		    break;
		case DERIVATIVE_TYPE_IMAGE_THUMBNAIL:
		    list ($ret, $data) = GalleryCoreApi::fetchThumbnailsByItemIds(array($id));
		    if ($ret) {
			return array($ret, null);
		    }
		    break;
		}
		if (!empty($data)) {
		    $derivative = $data[$id];
		    if (is_array($derivative)) {
			$derivative = array_shift($derivative);
		    }
		    $id = $derivative->getId();
		}
	    }
	} else {
	    list ($ret, $id) = G1MigrateHelper::fetchMapping($path[0]);
	    if ($ret) {
		return array($ret, null);
	    }
	    $itemId = $id;
	}

	if (isset($id)) {
	    list ($ret, $hasPermission) = GalleryCoreApi::hasItemPermission($itemId, 'core.view');
	    if ($ret) {
	    	return array($ret, null);
	    }
	}
	if (!isset($id) || empty($hasPermission)) {
	    $phpVm->header('HTTP/1.0 404 Not Found');
	    $phpVm->exit_();
	}

	/* Redirect browser */
	$phpVm->header('HTTP/1.0 301 Moved Permanently');
	$results = array('redirect' => array('view' => $view, 'itemId' => $id),
			 'status' => array(), 'error' => array());

	$scriptName = GalleryUtilities::getServerVar('PHP_SELF');
	if (empty($scriptName)) {
	    /* PHP as CGI needs to use PHP_SELF; fallback to SCRIPT_NAME just in case */
	    $scriptName = GalleryUtilities::getServerVar('SCRIPT_NAME');
	}

	if (strpos($scriptName, GALLERY_MAIN_PHP) !== false) {
	    /*
	     * $scriptName may not be the right path.. see http://bugs.php.net/bug.php?id=31843
	     * If not, omit 'href' in redirect params which means we hope REQUEST_URI has been
	     * rewritten to the correct path (that is the case on php 5.0.3 cgi at least).
	     */
	    $urlGenerator =& $gallery->getUrlGenerator();
	    $results['redirect']['href'] = $urlGenerator->makeUrl($scriptName);
	}

	list ($full, $page) = GalleryUtilities::getRequestVariablesNoPrefix('full', 'page');
	if (!empty($full)) {
	    $results['redirect']['imageViewsIndex'] = 1;
	}
	if (!empty($page)) {
	    $results['redirect']['page'] = $page;
	}

	return array(null, $results);
    }
}

/**
 * View returns .htaccess file
 */
class RedirectView extends GalleryView {
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
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return $ret;
	}

	header('Content-type: text/plain');

	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	$template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	$template->setVariable('l10Domain', 'modules_migrate');
	$template->display('gallery:modules/migrate/templates/Htaccess.tpl');

	return null;
    }
}
?>
