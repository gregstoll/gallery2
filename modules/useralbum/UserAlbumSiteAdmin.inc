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
 * Settings for User Albums
 * @package UserAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class UserAlbumSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = array();
	if (isset($form['action']['save'])) {
	    foreach (array('create', 'view', 'fullSize', 'targetLocation') as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'useralbum', $key, $form[$key]);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    foreach (array('loginRedirect', 'homeLink') as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'useralbum', $key,
							  isset($form[$key]) ? 1 : 0);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['saved'] = 1;
	} /* else $form['action']['reset'] */

	/* Figure out where to redirect upon success */
	$results['redirect']['view'] = 'core.SiteAdmin';
	$results['redirect']['subView'] = 'useralbum.UserAlbumSiteAdmin';
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * Settings for User Albums
 */
class UserAlbumSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'UserAlbumSiteAdmin') {
	    $form['formName'] = 'UserAlbumSiteAdmin';
	    foreach (array('create', 'view', 'fullSize', 'targetLocation',
			   'loginRedirect', 'homeLink') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'useralbum', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'useralbum');
	if ($ret) {
	    return array($ret, null);
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
	$g2AlbumTree = GalleryUtilities::createAlbumTree($albums);

	$UserAlbumSiteAdmin = array(
	    'createList' => array('immediate' => $module->translate('When user is created'),
				  'access' => $module->translate('When first accessed')),
	    'viewList' => array('everybody' => $module->translate('Everybody'),
				'allusers' => $module->translate('Registered Users'),
				'user' => $module->translate('User Only')),
	    'sizeList' => array('1' => $module->translate('Yes'),
				'0' => $module->translate('No')),
	    'targetLocation' => $g2AlbumTree
	);

	$template->setVariable('UserAlbumSiteAdmin', $UserAlbumSiteAdmin);
	$template->setVariable('controller', 'useralbum.UserAlbumSiteAdmin');
	return array(null, array('body' => 'modules/useralbum/templates/UserAlbumSiteAdmin.tpl'));
    }
}
?>
