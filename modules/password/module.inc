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
 * Password protected items
 *
 * This module provides the ability to set a password that is required for guests
 * or users without direct permission to view an item or album.
 *
 * @package Password
 * @author Alan Harder <alan.harder@sun.com>
 * @author Jess Martin <jmartin@cs.unc.edu>
 * @version $Revision: 18172 $
 */
class PasswordModule extends GalleryModule /* and GalleryEventListener */ {

    function PasswordModule() {
	global $gallery;

	$this->setId('password');
	$this->setName($gallery->i18n('Password Protected Albums'));
	$this->setDescription(
	    $gallery->i18n('Assign passwords for viewing particular albums'));
	$this->setVersion('1.0.9');
	$this->_templateVersion = 1;
	$this->setGroup('display', $this->translate('Display'));
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'password');
	if ($ret) {
	    return $ret;
	}
	foreach (array('validation.level' => 'MEDIUM') as $key => $value) {
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
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'PasswordModule', 'PasswordModule',
	    'modules/password/module.inc', 'password', array('GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryOnLoadHandler', 'PasswordModule', 'Password',
	    'modules/password/module.inc', 'password', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditOption', 'PasswordOption', 'PasswordOption',
	    'modules/password/PasswordOption.inc', 'password', array('ItemEditItem'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'PasswordInterface_1_0', 'PasswordHelper', 'Password',
	    'modules/password/classes/PasswordHelper.class', 'password', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'CaptchaAdminOption', 'PasswordCaptchaAdminOption', 'PasswordCaptchaAdminOption',
	    'modules/password/classes/PasswordCaptchaAdminOption.class', 'password', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getOnLoadHandlerIds
     */
    function getOnLoadHandlerIds() {
	return array('Password');
    }

    /**
     * Remove password from all items.
     * @see GalleryModule::uninstall
     */
    function uninstall() {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/password/classes/PasswordHelper.class');

	$query = '
	    SELECT [GalleryEntity::id]
	    FROM [GalleryEntity]
	    WHERE [GalleryEntity::onLoadHandlers] LIKE \'%|Password|%\'
	    ';
	list ($ret, $searchResults) = $gallery->search($query);
	if ($ret) {
	    return $ret;
	}
	while ($result = $searchResults->nextResult()) {
	    $itemIds[] = $result[0];
	}

	while (!empty($itemIds)) {
	    list ($ret, $items) =
		GalleryCoreApi::loadEntitiesById(array_splice($itemIds, 0, 100), 'GalleryEntity');
	    if ($ret) {
		return $ret;
	    }
	    foreach ($items as $item) {
		$ret = PasswordHelper::removePassword($item);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	$ret = parent::uninstall();
	if ($ret) {
	    return $ret;
	}
	return null;
    }

    /**
     * Redirect to getPassword view when core.ShowItem accessed directly for this item
     * and user doesn't already have permission.
     */
    function onLoad(&$item, $duringUpgrade) {
	list ($view, $itemId) = GalleryUtilities::getRequestVariables('view', 'itemId');
	if (empty($itemId)) {
	    /* Root album may not specify itemId */
	    $itemId = $item->getId();
	}
	if ($view == 'core.ShowItem' && $itemId == $item->getId() && !$duringUpgrade) {
	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($itemId);
	    if ($ret) {
		return $ret;
	    }
	    if (!isset($permissions['core.viewResizes'])
		    && !isset($permissions['core.viewSource'])) {
		if (!isset($permissions['core.view'])) {
		    list ($ret, $hiddenInterface) =
			GalleryCoreApi::newFactoryInstance('HiddenInterface_1_0');
		    if ($ret) {
			return $ret;
		    }
		    if (isset($hiddenInterface)) {
			list ($ret, $isHidden) = $hiddenInterface->isHidden($item);
			if ($ret) {
			    return $ret;
			}
		    }
		}
		if (isset($permissions['core.view']) || !empty($isHidden)) {
		    /* Redirect to PasswordEntry if we have core.view or item is also hidden */
		    global $gallery;
		    $session =& $gallery->getSession();
		    $ret = $session->start();
		    if ($ret) {
			return $ret;
		    }
		    $session->doNotUseTempId();
		    $urlGenerator =& $gallery->getUrlGenerator();
		    $phpVm = $gallery->getPhpVm();
		    $phpVm->header('Location: ' . $urlGenerator->generateUrl(
				   array('view' => 'password.PasswordEntry', 'itemId' => $itemId),
				   array('forceFullUrl' => true, 'htmlEntities' => false)));
		    $phpVm->exit_();
		}
	    }
	}
	return null;
    }

    /**
     * Update permissions when items move in/out of password protected albums
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$item = $event->getEntity();
	if (GalleryUtilities::isA($item, 'GalleryItem')
		&& !$item->testPersistentFlag(STORAGE_FLAG_NEWLY_CREATED)
		&& $item->isModified('parentId')) {
	    GalleryCoreApi::requireOnce('modules/password/classes/PasswordHelper.class');
	    $ret = PasswordHelper::handleMoveItem($item);
	    if ($ret) {
		return array($ret, null);
	    }
	} else if (GalleryUtilities::isA($item, 'GalleryItem')
		&& $item->testPersistentFlag(STORAGE_FLAG_NEWLY_CREATED)) {
	    GalleryCoreApi::requireOnce('modules/password/classes/PasswordHelper.class');
	    $ret = PasswordHelper::handleNewItem($item);
	    if ($ret) {
		return array($ret, null);
	    }
	}
	return array(null, null);
    }
}
?>
