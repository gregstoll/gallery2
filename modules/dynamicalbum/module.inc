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
 * Dynamic albums for newest, most viewed or random items.
 *
 * @package DynamicAlbum
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class DynamicAlbumModule extends GalleryModule {

    function DynamicAlbumModule() {
	global $gallery;
	$this->setId('dynamicalbum');
	$this->setName($gallery->i18n('Dynamic Albums'));
	$this->setDescription(
		$gallery->i18n('Dynamic albums for newest, most viewed or random items'));
	$this->setVersion('1.0.3');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'dynamicalbum');
	if ($ret) {
	    return $ret;
	}
	foreach (array('themeId' => '', 'size.date' => 24, 'size.views' => 24, 'size.random' => 24,
		    'type.date' => 'data', 'type.views' => 'data', 'type.random' => 'data',
		    'description.date' => '', 'description.views' => '', 'description.random' => '',
		    'itemlink.date' => 0, 'itemlink.views' => 0, 'itemlink.random' => 0)
		as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
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
	return array(null, array(array('name' => $this->translate('Dynamic Albums'),
				       'view' => 'dynamicalbum.DynamicAlbumSiteAdmin')));
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	$links = array();
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'dynamicalbum');
	if ($ret) {
	    return array($ret, null);
	}
	if (!$params['itemlink.date'] && !$params['itemlink.views']
		&& !$params['itemlink.random']) {
	    return array(null, $links);
	}
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if ($item->getCanContainChildren() && isset($wantsDetailedLinks[$itemId])) {
		if ($params['itemlink.date']) {
		    $links[$itemId][] = array(
			    'text' => $this->translate('View Latest Updates'),
			    'params' => array('view' => 'dynamicalbum.UpdatesAlbum',
					      'albumId' => $itemId));
		}
		if ($params['itemlink.views']) {
		    $links[$itemId][] = array(
			    'text' => $this->translate('View Popular Items'),
			    'params' => array('view' => 'dynamicalbum.PopularAlbum',
					      'albumId' => $itemId));
		}
		if ($params['itemlink.random']) {
		    $links[$itemId][] = array(
			    'text' => $this->translate('View Random Items'),
			    'params' => array('view' => 'dynamicalbum.RandomAlbum',
					      'albumId' => $itemId));
		}
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	return array(
		array('comment' => $this->translate('Updates Album'),
		      'match' => array('view' => 'dynamicalbum.UpdatesAlbum'),
		      'pattern' => 'updates',
		      'help' => $this->translate('Short URL for album of recent items')),
		array('comment' => $this->translate('Popular Album'),
		      'match' => array('view' => 'dynamicalbum.PopularAlbum'),
		      'pattern' => 'popular',
		      'help' => $this->translate('Short URL for album of most viewed items')),
		array('comment' => $this->translate('Random Album'),
		      'match' => array('view' => 'dynamicalbum.RandomAlbum'),
		      'pattern' => 'random',
		      'help' => $this->translate('Short URL for album of random items')),
		);
    }
}
?>
