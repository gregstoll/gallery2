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
 * Multiroot setup assistant
 * @package Multiroot
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ConfigureMultirootController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['generate'])) {
	    if (empty($form['viewUri'])) {
		$error[] = 'form[error][viewUri]';
	    }
	    if (empty($error)) {
		$error[] = 'form[generate][go]';
	    }
	}

	$method = empty($error) ? 'redirect' : 'delegate';
	return array(null, array(
		    $method => array('view' => 'core.SiteAdmin',
				     'subView' => 'multiroot.ConfigureMultiroot'),
		    'status' => $status,
		    'error' => $error));
    }
}

/**
 * Multiroot setup assistant
 */
class ConfigureMultirootView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ConfigureMultiroot') {
	    $form['formName'] = 'ConfigureMultiroot';
	    $form['viewUri'] = $form['viewRootId'] = $form['guestUser'] = '';
	}

	$multiroot = array();
	$multiroot['baseUri'] = $urlGenerator->generateUrl(array(),
		array('forceServerRelativeUrl' => true, 'forceSessionId' => false));

	/* Load all the album entities */
	list ($ret, $albumIds) = GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.view');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}
	$multiroot['albumTree'] = GalleryUtilities::createAlbumTree($albums);

	if (isset($form['generate']['go'])) {
	    $platform =& $gallery->getPlatform();
	    $multiroot['basePath'] =
		dirname(dirname(dirname(__FILE__))) . $platform->getDirectorySeparator();
	    $multiroot['baseUriDir'] =
		substr($multiroot['baseUri'], 0, strrpos($multiroot['baseUri'], '/') + 1);
	    if (strpos($form['viewUri'], '/') === false) {
		$form['viewUri'] = '/' . $form['viewUri'];
	    }

	    if (($ht = $platform->file_get_contents($multiroot['basePath'] . '.htaccess'))
		    && preg_match('|# BEGIN Url Rewrite section.*# END Url Rewrite section|s',
				  $ht, $match)) {
		$baseFile = basename($multiroot['baseUri']);
		$viewFile = basename($form['viewUri']);
		$multiroot['viewBase'] =
		    substr($form['viewUri'], 0, strrpos($form['viewUri'], '/') + 1);
		$multiroot['htaccess'] = preg_replace('#^( |$)#m', '&nbsp;',
			htmlspecialchars(strtr($match[0], array(
			    $multiroot['baseUriDir'] => $multiroot['viewBase'],
			    $baseFile => $viewFile,
			    str_replace('.', '\\.', $baseFile) =>
				str_replace('.', '\\.', $viewFile)))));
	    }

	    if (!empty($form['guestUser'])) {
		list ($ret, $user) = GalleryCoreApi::fetchUserByUserName($form['guestUser']);
		if (!$ret) {
		    $multiroot['viewUserId'] = $user->getId();
		} else if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		    /* Create user, remove from registered users group, assign permissions */
		    list ($ret, $multiroot['viewUserId']) =
			$this->_newGuestUser($form['guestUser'], $form['viewRootId']);
		    if ($ret) {
			return array($ret, null);
		    }
		    $multiroot['createdUser'] = true;
		} else {
		    return array($ret, null);
		}
	    }

	    $template->javascript('lib/javascript/BlockToggle.js');
	}

	$template->setVariable('Multiroot', $multiroot);
	$template->setVariable('controller', 'multiroot.ConfigureMultiroot');
	return array(null, array('body' => 'modules/multiroot/templates/ConfigureMultiroot.tpl'));
    }

    /**
     * Helper function to create an alternate guest user.
     * @access private
     */
    function _newGuestUser($userName, $albumId) {
	list ($ret, $user) = GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}
	if (!isset($user)) {
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}
	$ret = $user->create($userName);
	if ($ret) {
	    return array($ret, null);
	}
	$user->setHashedPassword('nopasswd');
	$ret = $user->save();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $allUserGroupId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.allUserGroup');
	if ($ret) {
	    return array($ret, null);
	}
	$ret = GalleryCoreApi::removeUserFromGroup($user->getId(), $allUserGroupId);
	if ($ret) {
	    return array($ret, null);
	}

	$ret = GalleryCoreApi::addUserPermission($albumId, $user->getId(), 'core.viewAll', true);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $user->getId());
    }
}
?>
