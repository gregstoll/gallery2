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
 * Debug Module
 *
 * This module provides some debug information on Gallery
 *
 * @package Debug
 * @author Ernesto Baschny <ernst@baschny.de>
 * @version $Revision: 18172 $
 */
class DebugModule extends GalleryModule {

    function DebugModule() {
	global $gallery;

	$this->setId('debug');
	$this->setName($gallery->i18n('Debugging'));
	$this->setDescription($gallery->i18n('Debugging and Developer Tools'));
	$this->setVersion('1.0.8');
	$this->_templateVersion = 1;
	$this->setGroup('gallery', $gallery->i18n('Gallery'));
	$this->setCallbacks('getItemLinks');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId) {
	$links = array();

	list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup($userId);
	if ($ret) {
	    return array($ret, null);
	}

	if ($isAdmin) {
	    foreach ($items as $item) {
		$itemId = $item->getId();
		if (isset($wantsDetailedLinks[$itemId])) {
		    $links[$itemId][] =
			array('text' => $this->translate('Show Debug Tree'),
			      'params' => array('view' => 'debug.ShowTree',
						'itemId' => $itemId));
		}
	    }
	}

	return array(null, $links);
    }
}
?>
