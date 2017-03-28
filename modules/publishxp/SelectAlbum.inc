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
 * Controller to select the album to add photos to.
 * Allows the user to select the album that should have the photos added in to it.  Also lets the
 * user select a parent album and go to the new album page should the user wish to first create a
 * new album.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class SelectAlbumController extends GalleryController {

    /**
     * @see GalleryController:handleRequest
     */
    function handleRequest($form) {
	$results = $error = $status = array();

	/*
	 * We merely provide a redirection service here.  Permission checks will be
	 * handled by the target views.
	 */
	if (isset($form['action']['newAlbum'])) {
	    /* Redirect to the new album page. */
	    if (isset($form['albumId'])) {
		$redirect['view'] = 'publishxp.NewAlbum';
		$redirect['parentId'] = $form['albumId'];
	    } else {
		$error[] = 'form[error][albumId][missing]';
	    }
	} else if (isset($form['action']['select'])) {
	    /* Redirect to the options page */
	    if (isset($form['albumId'])) {
		$redirect['view'] = 'publishxp.Options';
		$redirect['albumId'] = $form['albumId'];
	    } else {
		$error[] = 'form[error][albumId][missing]';
	    }
	}

	$results['status'] = $status;
	$results['error'] = $error;
	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'publishxp.SelectAlbum';
	}
	return array(null, $results);
    }
}

/**
 * View to select the album to add photos to.
 * Allows the user to select the album that should have the photos added in to it.  Also lets the
 * user select a parent album and go to the new album page should the user wish to first create a
 * new album.
 */
class SelectAlbumView extends GalleryView {
    /**
     * Prepares any additional data before rendering the template.
     *
     * @see GalleryController:loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	if ($form['formName'] != 'SelectAlbum') {
	    $form['formName'] = 'SelectAlbum';
	    $form['albumId'] = GalleryUtilities::getRequestVariables('albumId');
	} else {
	    /*
	     * In case they didn't select an album in the prior page (they're going
	     * to get an error message, but we require form[albumId] to be set in the template).
	     */
	    if (!isset($form['albumId'])) {
		$form['albumId'] = null;
	    }
	}

	/* Get ids of all all albums where we can add new data items */
	list ($ret, $albumIds['addDataItem']) =
	    GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addDataItem');
	if ($ret) {
	    return array($ret, null);
	}

	/* Get ids of all all albums where we can add new album items */
	list ($ret, $albumIds['addAlbumItem']) =
	    GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	/* Merge them together to get the master list of ids */
	$albumIds['allIds'] =
	    array_unique(array_merge($albumIds['addDataItem'], $albumIds['addAlbumItem']));

	if (!empty($albumIds['allIds'])) {
	    /** @todo Use fetchAllItemIds with multiple permissions once the API is available */
	    list ($ret, $permissions) =
		GalleryCoreApi::fetchPermissionsForItems($albumIds['allIds']);
	     if ($ret) {
		return array($ret, null);
	    }
	    $filteredIds = array();
	    foreach ($albumIds['allIds'] as $id) {
		if (!empty($permissions[$id]['core.view'])) {
		    $filteredIds[] = $id;
		}
	    }
	    $albumIds['allIds'] = $filteredIds;
	}

	$albums = array();
	if (!empty($albumIds['allIds'])) {
	    /* Load all the album entities */
	    list ($ret, $albums) =
		GalleryCoreApi::loadEntitiesById($albumIds['allIds'], 'GalleryAlbumItem');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$SelectAlbum = array();
	$SelectAlbum['albumTree'] = GalleryUtilities::createAlbumTree($albums);

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'publishxp');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Windows Publishing Wizard'));
	$template->setVariable('SelectAlbum', $SelectAlbum);
	$template->setVariable('controller', 'publishxp.SelectAlbum');
	$template->head('modules/publishxp/templates/Head.tpl');

	return array(null, array('body' => 'modules/publishxp/templates/SelectAlbum.tpl',
				 'useFullScreen' => true));
    }
}
?>
