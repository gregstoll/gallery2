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
 * This ItemEditOption will handle adding/removing the Panorama renderer
 * @package Panorama
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class PanoramaOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	if (!GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
	    return array(null, false);
	}
	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'panorama');
	if ($ret || !$param['itemType']) {
	    return array(null, false);
	}
	list ($ret, $preferred) = GalleryCoreApi::fetchPreferredsByItemIds(array($item->getId()));
	if ($ret) {
	    return array(null, false);
	}
	$width = empty($preferred)
	    ? $item->getWidth() : $preferred[$item->getId()]->getWidth();
	$mimeType = empty($preferred)
	    ? $item->getMimeType() : $preferred[$item->getId()]->getMimeType();
	return array(null,
		     $item->getRenderer() == 'PanoramaRenderer' ||
		     ($width > $param['width'] &&
		      ($mimeType == 'image/jpeg' || $mimeType == 'image/gif')));
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	list ($ret, $preferred) = GalleryCoreApi::fetchPreferredsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null, null);
	}
	$form['PanoramaOption']['isPanorama'] = $item->getRenderer() == 'PanoramaRenderer';
	return array(null, 'modules/panorama/templates/PanoramaOption.tpl', 'modules_panorama');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	$isUsingPanoramaRenderer = $item->getRenderer() == 'PanoramaRenderer';
	if ($isUsingPanoramaRenderer && !isset($form['PanoramaOption']['isPanorama'])) {
	    $item->setRenderer(null);
	} else if (!$isUsingPanoramaRenderer && isset($form['PanoramaOption']['isPanorama'])) {
	    $item->setRenderer('PanoramaRenderer');
	}

	if ($item->isModified()) {
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $ret = $item->save();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null);
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, array(), array());
    }
}
?>
