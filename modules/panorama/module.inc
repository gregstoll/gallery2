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
 * View wide images in a java applet viewer.
 * Metamorphose applet Copyright (C) 2000 Rüdiger Appel
 * @see http://www.3quarks.com/Applets/Metamorphose/
 *
 * @package Panorama
 * @author Alan Harder <alan.harder@sun.com>
 * @author Java applet: Rüdiger Appel
 * @version $Revision: 18172 $
 */
class PanoramaModule extends GalleryModule {

    function PanoramaModule() {
	global $gallery;
	$this->setId('panorama');
	$this->setName($gallery->i18n('Panorama'));
	$this->setDescription($gallery->i18n('View wide jpeg/gif images in a java applet viewer'));
	$this->setVersion('1.0.10');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	if (!isset($currentVersion)) {
	    /* Initial install */
	    foreach (array('itemType' => 1,
			   'itemLink' => 1,
			   'width' => 800) as $key => $value) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	} else {
	    if (version_compare($currentVersion, '1.0.6', '<')) {
		GalleryCoreApi::requireOnce('modules/panorama/classes/PanoramaUpgradeHelper.class');
		$ret = PanoramaUpgradeHelper::revertEntities();
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
	    'GalleryRenderer', 'PanoramaRenderer', 'PanoramaRenderer',
	    'modules/panorama/classes/PanoramaRenderer.class', 'panorama', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditOption', 'PanoramaOption', 'PanoramaOption',
	    'modules/panorama/PanoramaOption.inc', 'panorama', array('ItemEditPhoto'));
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
		     array(array('name' => $this->translate('Panorama'),
				 'view' => 'panorama.PanoramaSiteAdmin')));
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	$links = array();

	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'panorama');
	if ($ret) {
	    return array($ret, null);
	}
	if ($param['itemLink']) {
	    GalleryCoreApi::requireOnce('modules/panorama/classes/PanoramaHelper.class');

	    /*
	     * This won't obey the acting user id, but that's ok because it's only used to
	     * determine if a panorama is even possible.  We'll still also gate on the acting
	     * user's permissions that we receive.
	     */
	    list ($ret, $images) = PanoramaHelper::fetchViewableImages($items);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($items as $item) {
		$itemId = $item->getId();
		if (!isset($images[$itemId])) {
		    continue;
		}
		$image = $images[$itemId];
		$mimeType = $item->getMimeType();
		$width = $image->getWidth();
		/* Applet supports jpeg and gif images */
		if (GalleryUtilities::isA($item, 'GalleryPhotoItem') &&
		    $item->getRenderer() != 'PanoramaRenderer' &&
		    preg_match(':^image/(gif|p?jpeg)$:', $mimeType) &&
		    $width > $param['width'] &&
		    ($image->getHeight() / $width < 0.6)) {
		    $links[$itemId][] =
			array('text' => $this->translate('View Panorama'),
			      'params' => array('view' => 'panorama.Panorama',
						'itemId' => $itemId));
		}
	    }
	}
	return array(null, $links);
    }

    /**
     * @see GalleryPlugin::uninstall
     */
    function uninstall() {
	$ret = GalleryCoreApi::deleteRenderer('PanoramaRenderer');
	if ($ret) {
	    return $ret;
	}
	return parent::uninstall();
    }
}
?>
