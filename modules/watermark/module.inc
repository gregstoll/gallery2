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
 * This module provides support for adding watermarks to images
 *
 * @package Watermark
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 20954 $
 */
class WatermarkModule extends GalleryModule /* and GalleryEventListener */ {

    function WatermarkModule() {
	global $gallery;

	$this->setId('watermark');
	$this->setName($gallery->i18n('Watermark'));
	$this->setDescription($gallery->i18n('Watermark your images'));
	$this->setVersion('1.1.8'); /* Update upgrade() too */
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews|getUserAdminViews');
	$this->setRequiredCoreApi(array(7, 45));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();
	$watermarkDir =
	    $gallery->getConfig('data.gallery.plugins_data') . 'modules/watermark' . $slash;

	if (!isset($currentVersion)) {
	    $currentVersion = '0';
	}

	switch ($currentVersion) {
	case '0':
	    /* Initial install */
	    list ($success) = GalleryUtilities::guaranteeDirExists($watermarkDir);
	    if (!$success) {
		return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					     "Unable to create directory: $watermarkDir");
	    }
	    break;

	case '0.3':
	    $storage =& $gallery->getStorage();
	    $ret = $storage->configureStore($this->getId(), array('WatermarkImage:1.0'));
	    if ($ret) {
		return $ret;
	    }

	case '0.8.1':
	case '0.8.2':
	case '0.8.3':
	case '0.8.4':
	case '0.8.5':
	case '0.8.6':
	case '0.8.7':
	case '0.8.8':
	case '0.8.9':
	case '0.9.1':
	    /* Move watermark dir from g2data/ to g2data/plugins_data */
	    $oldDir = $gallery->getConfig('data.gallery.base') . 'watermark' . $slash;
	    if ($platform->is_dir($oldDir)) {
		if (!$platform->rename($oldDir, $watermarkDir)) {
		    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
						"Unable to move $oldDir to $watermarkDir");
		}
	    } else {
		list ($success) = GalleryUtilities::guaranteeDirExists($watermarkDir);
		if (!$success) {
		    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
						"Unable to create $watermarkDir");
		}
	    }

	    /* Update all derivatives that use watermarks */
	    $query = "
	    SELECT
	      [GalleryDerivative::id]
	    FROM
	      [GalleryDerivative]
	    WHERE
	      [GalleryDerivative::postFilterOperations] LIKE '%composite|watermark%'
	    ";

	    /* Do them 50 at a time to improve locking efficiency */
	    while (true) {
		list ($ret, $searchResults) =
		    $gallery->search($query, array(), array('limit' => array('count' => 50)));
		if ($ret) {
		    return $ret;
		}

		if ($searchResults->resultCount() == 0) {
		    /* We're done */
		    break;
		}

		while ($result = $searchResults->nextResult()) {
		    $ids[] = (int)$result[0];
		}

		list ($ret, $lock) = GalleryCoreApi::acquireWriteLock($ids);
		if ($ret) {
		    return $ret;
		}

		list ($ret, $derivatives) =
		    GalleryCoreApi::loadEntitiesById($ids, 'GalleryDerivative');
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lock);
		    return $ret;
		}

		foreach ($derivatives as $derivative) {
		    $derivative->setPostFilterOperations(
			str_replace('composite|watermark',
				    'composite|plugins_data/modules/watermark',
				    $derivative->getPostFilterOperations()));

		    $ret = $derivative->save();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lock);
			return $ret;
		    }
		}

		$ret = GalleryCoreApi::releaseLocks($lock);
		if ($ret) {
		    return $ret;
		}
	    }

	case '0.9.2':
	case '0.9.3':
	case '0.9.4':
	case '0.9.5':
	case '0.9.6':
	case '0.9.7':
	case '1.0.0':
	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	case '1.0.4':
	case '1.0.5':
	case '1.0.6':
	case '1.0.7':
	case '1.0.8':
	    /* Adding dependency on getRewriteRule() changes */
	case '1.0.9':
	case '1.0.10':
	case '1.1.0':
	    /* .mo file migration */
	case '1.1.1':
	case '1.1.2':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.1.3':
	    /* Combined YUI libraries into a single file */
	case '1.1.4':
	    /* Require minimum core API 7.45 */
	case '1.1.5':
	case '1.1.6':
	case '1.1.7':

	case 'end of upgrade path':
	    /*
	     * Leave this bogus case at the end of the legitimate case statements so that we
	     * always properly terminate our upgrade path with a break.
	     */
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					 sprintf('Unknown module version %s', $currentVersion));
	}

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'watermark');
	if ($ret) {
	    return $ret;
	}
	foreach (array('allowUserWatermarks' => 1, 'forceDefaultWatermark' => 0,
		       'hotlinkWatermarkId' => '', 'defaultWatermarkId' => '') as $key => $value) {
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
     * @see GalleryModule::uninstall
     */
    function uninstall() {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');

	/* Delete all watermarks and remove uses of them */
	$query = '
	SELECT [WatermarkImage::id]
	FROM [WatermarkImage]
	';
	list ($ret, $searchResults) = $gallery->search($query);
	if ($ret) {
	    return $ret;
	}
	while ($result = $searchResults->nextResult()) {
	    list ($ret) = WatermarkHelper::deleteWatermarkImageById((int)$result[0], true);
	    if ($ret) {
		return $ret;
	    }
	}

	$ret = parent::uninstall();
	if ($ret) {
	    return $ret;
	}
	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEntity', 'WatermarkImage', 'WatermarkImage',
	    'modules/watermark/classes/WatermarkImage.class', 'watermark', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditPlugin', 'ItemEditWatermark', 'ItemEditWatermark',
	    'modules/watermark/ItemEditWatermark.inc', 'watermark', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddOption', 'WatermarkOption', 'WatermarkOption',
	    'modules/watermark/WatermarkOption.inc', 'watermark', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'WatermarkModule', 'WatermarkModule', 
	    'modules/watermark/module.inc', 'watermark', array('GalleryEntity::delete'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('Watermarks'),
				       'view' => 'watermark.WatermarkSiteAdmin')));
    }

    /**
     * @see GalleryModule::getUserAdminViews
     */
    function getUserAdminViews($user) {
        global $gallery;

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser($user->getId());
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $isOn) = $this->getParameter('allowUserWatermarks');
	if ($ret) {
	    return array($ret, null);
	}

        $user = $gallery->getActiveUser();

	$views = array();
	if ($isOn && !$isAnonymous && !$user->isLocked()) {
	    $views[] = array('name' => $this->translate('Watermarks'),
			     'view' => 'watermark.UserWatermarks');
	}

	return array(null, $views);
    }

    /**
     * Handler for GalleryEntity::delete event.  Get rid of any watermarks
     * for users that are deleted.
     *
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	if ($event->getEventName() == 'GalleryEntity::delete') {
	    $entity = $event->getEntity();
	    if (GalleryUtilities::isA($entity, 'GalleryUser')) {
		GalleryCoreApi::requireOnce(
		    'modules/watermark/classes/WatermarkHelper.class');
		list ($ret, $watermarkIds) =
		    WatermarkHelper::fetchWatermarkIdsByOwnerId($entity->getId());
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($watermarkIds as $id) {
		    list ($ret) = WatermarkHelper::deleteWatermarkImageById($id, true);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	}
	return array(null, null);
    }

    /**
     * @see GalleryModule::getRewriteRules
     * @todo The explicit rewrite module version check can be removed on the next major Module API
     *       version.
     */
    function getRewriteRules() {
	/* Ensure the rewrite module is compatible, 'pattern' is optional since v1.1.8 of rewrite */
	list ($ret, $rewrite) = GalleryCoreApi::loadPlugin('module', 'rewrite', true);
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('WatermarkModule::getRewriteRules ingoring error: ' .
				$ret->getAsText());
	    }
	    $oldRewriteVersion = false;
	} else {
	    $oldRewriteVersion = version_compare($rewrite->getVersion(), '1.1.8', '<');
	}

	$rules = array();
	$rule = array(
		'comment' => $this->translate('Watermark hotlinked images'),
		'restrict' => array(
			GalleryUtilities::prefixFormVariable('view') => 'core.DownloadItem',
			GalleryUtilities::prefixFormVariable('itemId') => '([0-9]+)'),
		'queryString' => array('view' => 'watermark.DownloadItem',
				       'itemId' => '%1'),
		'flags' => array('L'),
		'exemptReferer' => 1,
		'help' => $this->translate(
			'Apply watermark to images downloaded from outside your Gallery. Select ' .
			'which watermark to use in Watermark Site Admin.'));
	if ($oldRewriteVersion) {
	    /* Add a pseudo pattern since it was mandatory until v1.1.8 of the rewrite module. */
	    $rule['pattern'] = '.';
	    $rule['locked'] = true;
	    $rule['parser'] = 'preGallery';
	}
	$rules[0] = $rule;

	return $rules;
    }
}
?>
