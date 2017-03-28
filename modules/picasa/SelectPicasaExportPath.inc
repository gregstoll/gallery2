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
 * First step of import process
 * @version $Revision: 17580 $
 * @package Picasa
 * @subpackage UserInterface
 * @author Waldemar Schlackow <gallery@schlackow.de> based on the migrate module by Jesse Mullan
 * @version $Revision: 17580 $
 */
class SelectPicasaExportPathController extends GalleryController {

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

	$error = $status = array();

	if (isset($form['action']['select'])) {
	    if (empty($form['picasaXmlPath'])) {
		$error[] = 'form[error][picasaXmlPath][missing]';
	    } else {
		$platform =& $gallery->getPlatform();
		$slash = $platform->getDirectorySeparator();
		$tmp = trim($form['picasaXmlPath']);
		if ($tmp[strlen($tmp)-1] != $slash) {
		    $tmp .= $slash;
		}
		GalleryUtilities::unsanitizeInputValues($tmp, false);
		$form['picasaXmlPath'] = trim($tmp);
		if (!Picasa2DataParser::isValidPicasaXmlPath($form['picasaXmlPath'])) {
		    $error[] = 'form[error][picasaXmlPath][invalid]';
		}
	    }
	    if (empty($error)) {
		$session =& $gallery->getSession();
		$recentPaths = $session->get('picasa.view.SelectPicasaExportPath.recentPaths');
		if (empty($recentPaths)) {
		    $recentPaths = array();
		}
		if (isset($recentPaths[$form['picasaXmlPath']])) {
		    $recentPaths[$form['picasaXmlPath']]++;
		} else {
		    $recentPaths[$form['picasaXmlPath']] = 1;
		}
		$session->put('picasa.view.SelectPicasaExportPath.recentPaths', $recentPaths);
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'picasa.ConfirmPicasaImport';
		$redirect['picasaXmlPath'] = $form['picasaXmlPath'];
		$redirect['destinationAlbumId'] = $form['destinationAlbumId'];
	    }
	}
	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'picasa.SelectPicasaExportPath';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * First step of import process
 */
class SelectPicasaExportPathView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'SelectPicasaExportPath') {
	    $form['formName'] = 'SelectPicasaExportPath';
	    $form['picasaXmlPath'] = '';
	    $form['destinationAlbumId'] = '';
	}

	/* Load our recent paths from the session */
	$session =& $gallery->getSession();
	$recentPaths = $session->get('picasa.view.SelectPicasaExportPath.recentPaths');

	if (empty($recentPaths)) {
	    $recentPaths = array();
	}

	list ($ret, $operations) = GalleryCoreApi::getToolkitOperations('image/jpeg');
	if ($ret) {
	    return array($ret, null);
	}

	$hasToolkit = false;
	for ($i = 0; $i < sizeof($operations); $i++) {
	    if ($operations[$i]['name'] == 'thumbnail') {
		$hasToolkit = true;
		break;
	    }
	}
	/* Get ids of all all albums where we can add new album items */
	list ($ret, $albumIds) =
	    GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load all the album entities */
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}
	$albumTree = GalleryUtilities::createAlbumTree($albums);

	$SelectPicasaExportPath = array();
	$SelectPicasaExportPath['recentPaths'] = $recentPaths;
	$SelectPicasaExportPath['hasToolkit'] = $hasToolkit;
	$SelectPicasaExportPath['g2AlbumTree'] = $albumTree;

	$template->setVariable('controller', 'picasa.SelectPicasaExportPath');
	$template->setVariable('SelectPicasaExportPath', $SelectPicasaExportPath);
	return array(null, array('body' => 'modules/picasa/templates/SelectPicasaExportPath.tpl'));
    }
}
?>
