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

GalleryCoreApi::requireOnce('modules/migrate/classes/Gallery1DataParser.class');
GalleryCoreApi::requireOnce('modules/migrate/classes/G1MigrateHelper.class');

/**
 * First step of migration process
 * @package Migrate
 * @subpackage UserInterface
 * @author Jesse Mullan <jmullan@visi.com>
 * @version $Revision: 17580 $
 */
class SelectGalleryController extends GalleryController {

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
	    if (empty($form['albumsPath'])) {
		$error[] = 'form[error][albumsPath][missing]';
	    } else {
		$platform =& $gallery->getPlatform();
		$slash = $platform->getDirectorySeparator();
		$tmp = trim($form['albumsPath']);
		if ($tmp[strlen($tmp)-1] != $slash) {
		    $tmp .= $slash;
		}
		$form['albumsPath'] = trim($tmp);

		if (!Gallery1DataParser::isValidAlbumsPath($form['albumsPath'])) {
		    $error[] = 'form[error][albumsPath][invalid]';
		}
	    }

	    if (empty($error)) {
		/* Save our albums path in the session */
		$session =& $gallery->getSession();
		$recentPaths = $session->get('migrate.view.SelectGallery.recentPaths');
		if (empty($recentPaths)) {
		    $recentPaths = array();
		}
		if (isset($recentPaths[$form['albumsPath']])) {
		    $recentPaths[$form['albumsPath']]++;
		} else {
		    $recentPaths[$form['albumsPath']] = 1;
		}
		$session->put('migrate.view.SelectGallery.recentPaths', $recentPaths);

		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'migrate.ChooseObjects';
		$redirect['albumsPath'] = $form['albumsPath'];
	    }
	} else if (isset($form['action']['deleteMap'])) {
	    $ret = GalleryCoreApi::removeAllMapEntries('G1MigrateMap');
	    if ($ret) {
		return array($ret, null);
	    }

	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'migrate.SelectGallery';
	    $status['mapDeleted'] = 1;
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'migrate.SelectGallery';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * First step of migration process
 */
class SelectGalleryView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'SelectGallery') {
	    $form['formName'] = 'SelectGallery';
	    $form['albumsPath'] = '';
	}

	/* Load our recent paths from the session */
	$session =& $gallery->getSession();
	$recentPaths = $session->get('migrate.view.SelectGallery.recentPaths');
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

	list ($ret, $mapCount) = G1MigrateHelper::fetchMapCount();
	if ($ret) {
	    return array($ret, null);
	}

	$SelectGallery = array();
	$SelectGallery['recentPaths'] = $recentPaths;
	$SelectGallery['hasToolkit'] = $hasToolkit;
	$SelectGallery['mapCount'] = $mapCount;

	$template->setVariable('SelectGallery', $SelectGallery);
	$template->setVariable('controller', 'migrate.SelectGallery');
	return array(null, array('body' => 'modules/migrate/templates/SelectGallery.tpl'));
    }
}
?>
