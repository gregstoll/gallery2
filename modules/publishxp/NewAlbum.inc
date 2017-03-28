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
 * Controller to create a new album for the photos.
 * Allows the user to create a new album underneath the parent they selected.  The user can
 * specify the name, title, and summary for the album.  The user is redirected back to the select
 * album page after creating the album with the newly created album automatically selected.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17589 $
 */
class NewAlbumController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest(&$form) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$results = $error = $status = array();

	if (isset($form['action']['newAlbum'])) {
	    if (empty($form['parentId'])) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }
	    list ($ret, $permissions) = GalleryCoreApi::getPermissions((int)$form['parentId']);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (empty($permissions['core.view'])) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }
	    if (empty($permissions['core.addAlbumItem'])) {
		return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	    }

	    /* Now check the value of name and title */
	    if (empty($form['pathComponent'])) {
		$error[] = 'form[error][pathComponent][missing]';
	    } else if (!$platform->isLegalPathComponent($form['pathComponent'])) {
		$error[] = 'form[error][pathComponent][invalid]';
	    }
	    if (empty($form['title'])) {
		$error[] = 'form[error][title][missing]';
	    }
	    if (empty($error)) {
		list ($ret, $newAlbum) = GalleryCoreApi::createAlbum(
		    (int)$form['parentId'], $form['pathComponent'], $form['title'],
		    $form['summary'], $form['description'], null);
		if ($ret) {
		    return array($ret, null);
		}
		if (empty($error)) {
		    $redirect['view'] = 'publishxp.SelectAlbum';
		    $redirect['albumId'] = $newAlbum->getId();
		}
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'publishxp.NewAlbum';
	}
	$results['status'] = $status;
	$results['error'] = $error;
	return array(null, $results);
    }
}

/**
 * View to create a new album for the photos.
 * Allows the user to create a new album underneath the parent they selected.  The user can
 * specify the name, title, and summary for the album.  The user is redirected back to the select
 * album page after creating the album with the newly created album automatically selected.
 */
class NewAlbumView extends GalleryView {
    /**
     * @see GalleryView:loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	if ($form['formName'] != 'NewAlbum') {
	    $form['formName'] = 'NewAlbum';
	    $parentId = GalleryUtilities::getRequestVariables('parentId');
	    if (empty($parentId)) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }

	    $form['parentId'] = $parentId;
	    $form['pathComponent'] = '';
	    $form['title'] = '';
	    $form['summary'] = '';
	    $form['keywords'] = '';
	    $form['description'] = '';
	}
	$form['parentId'] = (int)$form['parentId'];

	list ($ret, $canView) = GalleryCoreApi::hasItemPermission($form['parentId'], 'core.view');
	if ($ret) {
	    return array($ret, null);
	}
	if (!$canView) {
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}

	$ret = GalleryCoreApi::assertHasItemPermission($form['parentId'], 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $parent) =
	    GalleryCoreApi::loadEntitiesById($form['parentId'], 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load the item's parents */
	list ($ret, $parents) = GalleryCoreApi::fetchParents($parent, 'core.view', true);
	if ($ret) {
	    return array($ret, null);
	}
	for ($i = 0; $i < sizeof($parents); $i ++) {
	    $parents[$i] = (array)$parents[$i];
	}
	$parents[] = (array)$parent;

	$NewAlbum = array();
	$NewAlbum['parents'] = $parents;

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'publishxp');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Windows Publishing Wizard'));
	$template->setVariable('NewAlbum', $NewAlbum);
	$template->setVariable('controller', 'publishxp.NewAlbum');
	$template->head('modules/publishxp/templates/Head.tpl');

	return array(null, array('body' => 'modules/publishxp/templates/NewAlbum.tpl',
				 'useFullScreen' => true));
    }
}
?>
