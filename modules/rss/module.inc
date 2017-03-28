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
 * Implementation of the RSS module
 *
 * @package Rss
 * @author Jonatan Heyman <http://heyman.info>
 * @author Pierre-Luc Paour
 * @author Daniel Grund <http://www.photogrund.nl>
 * @version $Revision: 20954 $
 */
class RssModule extends GalleryModule /* and GalleryEventListener */ {

    function RssModule() {
	global $gallery;

	$this->setId('rss');
	$this->setName($gallery->i18n('RSS'));
	$this->setDescription($gallery->i18n('RSS'));
	$this->setVersion('1.1.8');
	$this->_templateVersion = 1;
	$this->setGroup('export', $gallery->i18n('Export'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('RSS'),
				       'view' => 'rss.RssSiteAdmin')));
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId) {
	GalleryCoreApi::requireOnce('modules/rss/classes/RssMapHelper.class');
	$links = array();

	/* check that this type of link is allowed */
	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'rss');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $modulestatus) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
	    return array($ret, null);
	}
	
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if (!isset($wantsDetailedLinks[$itemId])) {
		continue;
	    }

	    if ($param['allowSimpleFeed'] == 1) {
		$disableLink = GalleryUtilities::isA($item, 'GalleryDataItem')
			&& $modulestatus['comment']['active'] != '1';
		
		$itemTypeNames = array_merge($item->itemTypeName(), $item->itemTypeName(false));

		if (false) {
		    /* Specific translations: */
		    _('RSS Feed for this Album');
		    _('RSS Feed for this Photo');
		    _('RSS Feed for this Movie');
		}
		/*
		 * Because SimpleFeeds for DataItems can only be comments, don't add
		 * the link if the Comments module is disabled.
		 */
		if (!GalleryUtilities::isA($item, 'GalleryDataItem')
			|| !empty($modulestatus['comment']['active'])) {
		    $links[$itemId][] = array(
			    'text' => $this->_translate(array(
				'text' => 'RSS Feed for this %s',
				'arg1' => $itemTypeNames[0]), $itemTypeNames[2]),
			    'params' => array(
				'view' => 'rss.SimpleRender',
				'itemId' => $itemId));
		}
	    }

	    list ($ret, $canConfigure) = RssMapHelper::canConfigureFeed($item, $param, $userId);
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($canConfigure) {
		$links[$itemId][] = array(
			'text' => $this->translate('Configure RSS Feeds'),
			'params' => array(
			    'view' => 'core.ItemAdmin',
			    'subView' => 'rss.EditFeed',
			    'itemId' => $itemId,
			    'return' => 1));
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	$rules = array();
	$rules[] = array(
	    'comment' => $this->translate('Simple RSS Feed'),
	    'match' => array('view' => 'rss.SimpleRender'),
	    'pattern' => 'srss/%itemId%',
	    'help' => $this->translate('Short URL for Simple RSS Feeds'));

	$rules[] = array(
	    'comment' => $this->translate('RSS Feed'),
	    'match' => array('view' => 'rss.Render'),
	    'pattern' => 'rss/%name%',
	    'keywords' => array(
		'name' => array(
		    'pattern' => '([^\/\?]+)',
		    'help' => $this->translate('The name of the configured RSS Feed'))),
	    'help' => $this->translate('Short URL for Configurable RSS Feeds'));

	return $rules;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'RssModule', 'RssModule', 
	    'modules/rss/module.inc', 'rss', array('GalleryEntity::delete'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * Event Handler: get rid of any feeds for items that are deleted
     *
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$data = $event->getData();

	if ($event->getEventName() == 'GalleryEntity::delete') {
	    $entity = $event->getEntity();
	    if (GalleryUtilities::isA($entity, 'GalleryItem')) {
		GalleryCoreApi::requireOnce('modules/rss/classes/RssMapHelper.class');
		list ($ret, $results) = RssMapHelper::fetchFeedsForItem($entity->getId());
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($results as $feed) {
		    $ret = RssMapHelper::deleteFeed($feed['name']);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	}

	return array(null, null);
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {

	/**
	 * @todo add a default to this switch statement and include all historical versions, like
	 * we do in other modules.
	 */
	switch($currentVersion) {
	    case null:
		/* Initial install */

		foreach (array('defaultVersion' => '2.0',
			       'defaultCount' => 10,
			       'maxCount' => 50,
			       'allowSimpleFeed' => 1,
			       'allowConfigurableFeed' => 0,
			       'allowAlbums' => 0,
			       'allowPhotos' => 0,
			       'allowPhotosRecursive' => 0,
			       'allowCommentsAlbum' => 0,
			       'allowCommentsPhoto' => 0,
			       'defaultCopyright' => '',
			       'defaultTtl' => 120,
			    ) as $key => $value) {
		    $ret = $this->setParameter($key, $value);
		    if ($ret) {
			return $ret;
		    }
		}
		/* keep going */

	    case '1.0.0':
		foreach (array('sfAlbumType' => 'photos',
			       'sfDate' => 'new',
			       'sfPhotosRecursiveLimit' => 10,
			       'allowCommentsRecursive' => 0,
			    ) as $key => $value) {
		    $ret = $this->setParameter($key, $value);
		    if ($ret) {
			return $ret;
		    }
		}

	    case '1.1.1':
		foreach (array('maxAge' => 21,
			       'allowPhotosRandom' => 0,
			       'allowPhotosRandomRecursive' => 0,
			    ) as $key => $value) {
		    $ret = $this->setParameter($key, $value);
		    if ($ret) {
			return $ret;
		    }
		}

	    case '1.1.2':
		GalleryCoreApi::requireOnce('modules/rss/classes/RssMapHelper.class');
		
		/* consistent naming */
		
		/* allowPhotosRecurse -> allowPhotosRecursive */
		list ($ret, $allowPhotosRecursive) = $this->getParameter('allowPhotosRecurse');
		if ($ret) {
		    return $ret;
		}
		
		if (isset($allowPhotosRecursive)) {
		    $ret = $this->setParameter('allowPhotosRecursive', $allowPhotosRecursive);
		    if ($ret) {
			return $ret;
		    }
		
		    $ret = $this->removeParameter('allowPhotosRecurse');
		    if ($ret) {
			return $ret;
		    }
		}
		
		/* sfPhotosRecurseLimit -> sfPhotosRecursiveLimit */
		list ($ret, $sfPhotosRecursiveLimit) = $this->getParameter('sfPhotosRecurseLimit');
		if ($ret) {
		    return $ret;
		}
		
		if (isset($sfPhotosRecursiveLimit)) {
		    $ret = $this->setParameter('sfPhotosRecursiveLimit', $sfPhotosRecursiveLimit);
		    if ($ret) {
			return $ret;
		    }
		
		    $ret = $this->removeParameter('sfPhotosRecurseLimit');
		    if ($ret) {
			return $ret;
		    }
		}
		
		/* sfAlbumType: photosRecurse -> photosRecursive */
		list ($ret, $sfAlbumType) = $this->getParameter('sfAlbumType');
		if ($ret) {
		    return $ret;
		}
		
		if ($sfAlbumType == 'photosRecurse') {
		    $ret = $this->setParameter('sfAlbumType', 'photosRecursive');
		    if ($ret) {
			return $ret;
		    }
		}
		
		/* feed parameters */
		list ($ret, $names) = RssMapHelper::fetchFeedNames();
		if ($ret) {
		    return $ret;
		}
		
		foreach ($names as $name) {
		    list ($ret, $feed) = RssMapHelper::fetchFeed($name);
		    if ($ret) {
			return $ret;
		    }
		    
		    if ($feed['feedType'] == 'photosRecurse') {
			$feed['feedType'] = 'photosRecursive';
		    }
		    
		    if (isset($feed['photosRecurseLimit'])) {
			$feed['photosRecursiveLimit'] = $feed['photosRecurseLimit'];
			unset($feed['photosRecurseLimit']);
		    }
		    
		    $ret = RssMapHelper::updateFeed($name, $feed);
		    if ($ret) {
			return $ret;
		    }
		}
	    case '1.1.3':
	    case '1.1.4':
		/* Refactored to new event registration from core API 7.34 */
	}

	return null;
    }
}
?>
