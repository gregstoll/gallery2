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
 * This ItemAddOption rejects uploads for selected mime types.
 * @package Mime
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class RestrictMimeOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	list ($ret, $allowMime) = GalleryCoreApi::getPluginParameter('module', 'mime', 'allowMime');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $allowMime != 'all');
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	global $gallery;
	$errors = $warnings = array();
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mime');
	if ($ret) {
	    return array($ret, null, null);
	}
	list ($ret, $allowMime) = $module->getParameter('allowMime');
	if ($ret) {
	    return array($ret, null, null);
	}
	list ($ret, $mimeList) = $module->getParameter('mimeList');
	if ($ret) {
	    return array($ret, null, null);
	}
	$mimeList = array_flip(explode(' | ', $mimeList));

	for ($i = 0; $i < count($items); $i++) {
	    if (!GalleryUtilities::isA($items[$i], 'GalleryDataItem')) {
		continue;
	    }

	    $warnings[$i] = array();
	    $mimeType = $items[$i]->getMimeType();
	    $extension = GalleryUtilities::getFileExtension($items[$i]->getPathComponent());
	    if (!empty($extension)) {
		list ($ret, $derivedMimeType) = GalleryCoreApi::convertExtensionToMime($extension);
		if ($ret) {
		    return array($ret, null, null);
		}
	    } else {
		$derivedMimeType = $mimeType;
	    }
	    if (($allowMime == 'block'
			&& (isset($mimeList[$mimeType]) || isset($mimeList[$derivedMimeType])))
		    || ($allowMime == 'allow'
			&& (!isset($mimeList[$mimeType]) || !isset($mimeList[$derivedMimeType])))) {
		$warnings[$i][] = $module->translate(
			'This type of file not allowed; item will not be added.');
		$gallery->addShutdownAction(array('GalleryCoreApi', 'deleteEntityById'),
					    array($items[$i]->getId()));
	    }
	}

	return array(null, $errors, $warnings);
    }
}
?>
