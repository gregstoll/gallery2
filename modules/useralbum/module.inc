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
 * Create a private top level album for new users.
 *
 * @package UserAlbum
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class UserAlbumModule extends GalleryModule /* and GalleryEventListener */ {

    function UserAlbumModule() {
	global $gallery;
	$this->setId('useralbum');
	$this->setName($gallery->i18n('User Albums'));
	$this->setDescription($gallery->i18n('Create an album for each new user'));
	$this->setVersion('1.0.11');
	$this->_templateVersion = 1;
	$this->setGroup('gallery', $gallery->i18n('Gallery'));
	$this->setCallbacks('getSiteAdminViews|getSystemLinks');
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'UserAlbumModule', 'UserAlbumModule', 
	    'modules/useralbum/module.inc', 'useralbum', 
	    array('GalleryEntity::delete', 'GalleryEntity::save', 'Gallery::Login'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }
    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'useralbum');
	if ($ret) {
	    return $ret;
	}
	list ($ret, $rootId) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return $ret;
	}
	foreach (array('view' => 'everybody', 'fullSize' => '1', 'targetLocation' => $rootId,
		       'loginRedirect' => 0, 'homeLink' => 0, 'create' => 'access')
		as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('User Albums'),
				 'view' => 'useralbum.UserAlbumSiteAdmin')));
    }

    /**
     * @see GalleryModule::getSystemLinks
     */
    function getSystemLinks() {
	global $gallery;
	$links = array();

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'useralbum');
	if ($ret) {
	    return array($ret, null);
	}

	/* Check if link is enabled */
	if (!empty($params['homeLink'])) {
	    list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	    if ($ret) {
		return array($ret, null);
	    }

	    if (!$isAnonymous) {
		list ($ret, $albumId) =
		    GalleryCoreApi::getPluginParameter('module', 'useralbum',
						       'albumId', $gallery->getActiveUserId());
		if ($ret) {
		    return array($ret, null);
		}

		if (!empty($albumId) || $params['create'] == 'access') {
		    $links['YourAlbum'] = array('text' => $this->translate('Your Album'),
			'params' => empty($albumId) ? array('controller' => 'useralbum.UserAlbum')
						    : array('view' => 'core.ShowItem',
							    'itemId' => $albumId));
		}
	    }
	}
	return array(null, $links);
    }

    /**
     * Event handler for GalleryEntity::save, ::delete and Gallery::Login events
     * Create new top level album with appropriate permissions when a GalleryUser is created
     * Reset the targetAlbum to root if the target is ever deleted
     * Remove albumId param if the user's album is deleted
     * Redirect to user album after login, if enabled in site admin
     *
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$result = null;
	switch ($event->getEventName()) {
	case 'GalleryEntity::save':
	    $user = $event->getEntity();
	    if (GalleryUtilities::isA($user, 'GalleryUser') &&
		    $user->testPersistentFlag(STORAGE_FLAG_NEWLY_CREATED)) {
		list ($ret, $createMode) = $this->getParameter('create');
		if ($ret) {
		    return array($ret, null);
		}
		if ($createMode == 'immediate') {
		    GalleryCoreApi::requireOnce(
						'modules/useralbum/classes/UserAlbumHelper.class');

		    $ret = UserAlbumHelper::createUserAlbum($user);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	    break;

	case 'GalleryEntity::delete':
	    $entity = $event->getEntity();

	    /* We don't care unless it's an album */
	    if (!GalleryUtilities::isA($entity, 'GalleryAlbumItem')) {
		break;
	    }

	    list ($ret, $targetId) = $this->getParameter('targetLocation');
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($entity->getId() == $targetId) {
		list ($ret, $rootId) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
		if ($ret) {
		    return array($ret, null);
		}

		$ret = $this->setParameter('targetLocation', $rootId);
		if ($ret) {
		    return array($ret, null);
		}
	    } else {
		/*
		 * Delete the albumid value from the ParameterMap so we don't get
		 * MISSING_OBJECT errors when the user clicks their 'Your Album' link
		 */
		$ret = GalleryCoreApi::removePluginParameterByValue('module', 'useralbum',
								    'albumId', $entity->getId());
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    break;

	case 'Gallery::Login':
	    /* Check if redirect is enabled.. */
	    list ($ret, $loginRedirect) = $this->getParameter('loginRedirect');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!empty($loginRedirect)) {
		/* Return a redirect to the user album.. */
		$result = array('controller' => 'useralbum.UserAlbum');
	    }
	    break;
	}
	return array(null, $result);
    }
}
?>
