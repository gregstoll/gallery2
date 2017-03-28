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
 * The implementation of the Image Block module
 *
 * @package ImageBlock
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class ImageBlockModule extends GalleryModule {

    function ImageBlockModule() {
	global $gallery;
	$this->setId('imageblock');
	$this->setName($gallery->i18n('Image Block'));
	$this->setDescription($gallery->i18n('Random, Most Recent or Most Viewed Photo or Album'));
	$this->setVersion('1.1.9');
	$this->_templateVersion = 1;
	$this->setGroup('blocks', $gallery->i18n('Blocks'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 47));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	/**
	 * Our cache is probably out of date. Flush it. 
	 * @todo What about the ImageBlockDisabledMap which is out of date as well?
	 */
	$ret = GalleryCoreApi::removeAllMapEntries('ImageBlockCacheMap');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $redirect) = parent::activate($postActivationEvent);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $redirect);
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	$storage =& $gallery->getStorage();

	if (!empty($currentVersion) && version_compare($currentVersion, '1.0.1', '<')) {
	    $ret = $storage->configureStore($this->getId(), array('ImageBlockCacheMap:1.0'));
	    if ($ret) {
		return $ret;
	    }
	}

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'imageblock');
	if ($ret) {
	    return $ret;
	}
	foreach (array('show' => 'heading|title|date|views', 'albumFrame' => '', 'itemFrame' => '')
		as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	/* Remove the blocks parameter if necessary, we forgot to remove it in beta 4 */
	if (isset($params['blocks'])) {
	    $ret = GalleryCoreApi::removePluginParameter('module', 'imageblock', 'blocks');
	    if ($ret) {
		return $ret;
	    }
	}

	if (!empty($currentVersion) && version_compare($currentVersion, '1.1.7', '<=')) {
	    GalleryCoreApi::requireOnce('modules/imageblock/classes/ImageBlockHelper.class');
	    $ret = $storage->removeAllMapEntries('ImageBlockCacheMap', true);
	    if ($ret) {
		return $ret;
	    }
	}
	if (!empty($currentVersion) && version_compare($currentVersion, '1.1.5', '<=')) {
	    /* Add ImageBlockCacheMap::random */
	    $ret = $storage->configureStore($this->getId(), array('ImageBlockCacheMap:1.1'));
	    if ($ret) {
		return $ret;
	    }
	}
	if (!empty($currentVersion) && version_compare($currentVersion, '1.1.7', '<=')) {
	    /* Add proper index with new ::random column */
	    $ret = $storage->configureStore($this->getId(), array('ImageBlockCacheMap:1.2'));
	    if ($ret) {
		return $ret;
	    }
	}
	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'ImageBlockHelper', 'ImageBlockHelper', 
	    'modules/imageblock/classes/ImageBlockHelper.class', 'imageblock', 
	    array('Gallery::ViewableTreeChange', 'GalleryEntity::delete', 'GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	   'ItemEditOption', 'ImageBlockOption', 'ImageBlockOption',
	   'modules/imageblock/ImageBlockOption.inc', 'imageblock', array('ItemEditAlbum'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Image Block'),
				 'view' => 'imageblock.ImageBlockSiteAdmin')));
    }
}
?>
