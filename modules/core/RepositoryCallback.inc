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
 * This view lets you make very simple callbacks to the framework to get very specific data.
 * Eventually this will probably get refactored into a much more sophisticated framework.
 *
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class RepositoryCallbackView extends GalleryView {
    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isControllerLike
     */
    function isControllerLike() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return $ret;
	}

	list ($command, $form) = GalleryUtilities::getRequestVariables('command', 'form');
	if ($command == 'saveRepositoryList') {
	    /*
	     * Filter the form values through our list of valid keys to avoid exploits
	     * This is duplicated from AdminRepositoryController.  Success is assumed here
	     * since the UI is not configured to actually do anything on failure.  This probably
	     * isn't the best long term approach.
	     */
	    $data = array();
	    foreach (array('released', 'experimental', 'community') as $key) {
		if ($form['repositories'][$key]) {
		    $data[$key] = 1;
		}
	    }

	    if (empty($data)) {
		$data['released'] = 1;
	    }

	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'core', 'core.repositories', serialize($data));
	    if ($ret) {
		return $ret;
	    }

	    /* Remove the default repositories from the data cache */
	    GalleryRepository::clearRepositoryCache();
	}
	return null;
    }
}
?>
