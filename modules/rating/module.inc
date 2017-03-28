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
 * Provides item rating mechanism for Gallery2
 * @package Rating
 * @author  Don Seiler <don@seiler.us>
 * @version $Revision: 18172 $
 */
class RatingModule extends GalleryModule {

    function RatingModule() {
	global $gallery;

	$this->setId('rating');
	$this->setName($gallery->i18n('Rating'));
	$this->setDescription($gallery->i18n('Item Rating Interface'));
	$this->setVersion('1.0.13'); /* Update upgrade() too */
	$this->_templateVersion = 1;
	$this->setCallbacks('getSiteAdminViews|getItemSummaries');
	$this->setGroup('data', $gallery->i18n('Extra Data'));
	$this->setRequiredCoreApi(array(7, 41));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'RatingModule', 'RatingModule', 
	    'modules/rating/module.inc', 'rating', array('GalleryEntity::delete'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation('ItemEditOption', 'RatingItemEdit',
		'RatingItemEdit', 'modules/rating/RatingItemEdit.inc', 'rating',
		array('ItemEditAlbum'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation('GallerySortInterface_1_2',
		'RatingSortOrder', 'RatingSortOrder',
		'modules/rating/classes/RatingSortOrder.class', 'rating', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getItemSummaries
     */
    function getItemSummaries($items, $permissions, &$template) {
	global $gallery;

	$itemIds = array();
	$enabledAlbums = array();
	$numEnabledAlbums = 0;
	foreach ($items as $item) {
	    $itemIds[] = $item->getId();

	    $parentId = $item->getParentId();
	    if (isset($enabledAlbums[$parentId])) {
		continue;
	    }

	    list ($ret, $enabled) = GalleryCoreApi::getPluginParameter('module', 'rating',
					'enabled', $parentId);
	    if ($ret) {
		return array($ret, null);
	    }

	    $enabledAlbums[$parentId] = !empty($enabled);
	    if ($enabledAlbums[$parentId]) {
		$numEnabledAlbums++;
	    }
	}

	if (empty($numEnabledAlbums)) {
	    return array(null, array());
	}

	/* Load the G2 templating engine */
	GalleryCoreApi::requireOnce('modules/rating/classes/RatingHelper.class');

	$template->style('modules/rating/rating.css');
	$template->javascript('lib/yui/utilities.js');
	$template->javascript('modules/rating/rating.js');

	$template->setVariable('l10Domain', $this->getL10Domain());
	$session =& $gallery->getSession();
	$authToken = $session->isPersistent() ? $session->getAuthToken() : '';
	$RatingSummary = array('ratingValues' => array(1, 2, 3, 4, 5),
			       'firstCall' => true, 'authToken' => $authToken);

	/* Check to see if album rating is allowed */
	list ($ret, $allowAlbumRating) = GalleryCoreApi::getPluginParameter('module', 'rating',
		'allowAlbumRating');
	if ($ret) {
	    return array($ret, null);
	}

	/* Acquire the current rating for the items (populate $RatingData) */

	list ($ret, $RatingData) = RatingHelper::fetchRatings($itemIds,
		$gallery->getActiveUserId());
	if ($ret) {
	    return array($ret, null);
	}

	$summaries = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    $parentId = $item->getParentId();

	    /* Check if item's parent album allows rating */
	    if (!$enabledAlbums[$parentId]) {
		continue;
	    }

	    /* Check if this user can view rating for this item */
	    if (!isset($permissions[$itemId]['rating.view'])) {
		continue;
	    }

	    if (GalleryUtilities::isA($item, 'GalleryAlbumItem') && !$allowAlbumRating) {
		continue;
	    }

	    $RatingData[$itemId]['canRate'] = isset($permissions[$itemId]['rating.add']);
	    $template->setVariable('RatingData', $RatingData[$itemId]);
	    $template->setVariable('RatingSummary', $RatingSummary);

	    /* Render and get the html */
	    list ($ret, $summaries[$itemId]) =
		$template->fetch('gallery:modules/rating/templates/RatingInterface.tpl');
	    if ($ret) {
		return array($ret, null);
	    }

	    $RatingSummary['firstCall'] = false;
	}

	return array(null, $summaries);
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	if (!isset($currentVersion)) {
	    $currentVersion = '0';
	}

	switch ($currentVersion) {
	case '0':
	    list ($ret, $coreParams) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
	    if ($ret) {
		return $ret;
	    }

	    /* Initial install.  Register our permissions */
	    $permissions[] = array('add', $gallery->i18n('[rating] Add ratings'), 0, array());
	    $permissions[] = array('view', $gallery->i18n('[rating] View ratings'), 0, array());
	    $permissions[] = array('all', $gallery->i18n('[rating] All access'),
		    GALLERY_PERMISSION_COMPOSITE, array('rating.add', 'rating.view'));
	    foreach ($permissions as $p) {
		$ret = GalleryCoreApi::registerPermission($this->getId(),
			'rating.' . $p[0],
			$p[1], $p[2], $p[3]);
		if ($ret) {
		    return $ret;
		}
	    }

	    /* Give everybody rating permission by default */
	    $gallery->guaranteeTimeLimit(300);
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'],
		'rating.all', true);
	    if ($ret) {
		return $ret;
	    }
	    break;

	case '1.0.0':
	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	case '1.0.4':
	case '1.0.5':
	case '1.0.6':
	case '1.0.7':
	case '1.0.8':
	case '1.0.8.1':
	case '1.0.8.2':
	    /* .mo file migration */
	case '1.0.9':
	case '1.0.10':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.0.11':
	    /* Combined YUI libraries into a single file */
	case '1.0.12': 

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
		    sprintf('Unknown module version %s', $currentVersion));
	}

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'rating');
	if ($ret) {
	    return $ret;
	}
	foreach (array('allowAlbumRating' => 0, 'themeId' => '', 'orderBy' => 'RatingSortOrder',
		       'orderDirection' => ORDER_DESCENDING, 'minLimit' => 2, 'description' => '')
		as $key => $value) {
	    if (!isset($params[$key])) {
		$this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}
	if (!isset($params['themeSettingsId'])) {
	    list ($ret, $entity) =
		GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryEntity');
	    if ($ret) {
		return $ret;
	    }
	    if (!isset($entity)) {
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT, __FILE__, __LINE__,
					     'Unable to get a GalleryEntity instance');
	    }
	    $ret = $entity->create();
	    if ($ret) {
		return $ret;
	    }
	    $ret = $entity->save(false);
	    if ($ret) {
		return $ret;
	    }
	    $ret = $this->setParameter('themeSettingsId', $entity->getId());
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('Rating'),
				       'view' => 'rating.RatingSiteAdmin')));
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	return array(
		array('comment' => $this->translate('Rating Album'),
		      'match' => array('view' => 'rating.RatingAlbum'),
		      'pattern' => 'rating/%limit%',
		      'keywords' => array(
			  'limit' => array('pattern' => '([^?/]+)',
			      'help' => $this->translate('Rating limit to use for dynamic album')))
		    ));
    }

    /**
     * Event handler for GalleryEntity::delete events
     * Delete any item ratings if the associated item is deleted
     *
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$result = null;
	if ($event->getEventName() == 'GalleryEntity::delete') {
	    $entity = $event->getEntity();

	    if (GalleryUtilities::isA($entity, 'GalleryItem')) {
		$ret = GalleryCoreApi::removeMapEntry('RatingMap',
						      array('itemId' => $entity->getId()));
		if ($ret) {
		    return array($ret, null);
		}
		$ret = GalleryCoreApi::removeMapEntry('RatingCacheMap',
						      array('itemId' => $entity->getId()));
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}
	return array(null, $result);
    }

    /**
     * @see GalleryModule::uninstall
     */
    function uninstall() {
	$ret = GalleryCoreApi::deleteSortOrder('RatingSortOrder');
	if ($ret) {
	    return $ret;
	}

	return parent::uninstall();
    }
}
?>
