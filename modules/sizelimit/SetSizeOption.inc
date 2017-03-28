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

GalleryCoreApi::requireOnce('modules/sizelimit/classes/SizeLimitHelper.class');

/**
 * ItemAdd option using the size limit values to resize gallery images when uploaded.
 * @package SizeLimit
 * @subpackage UserInterface
 * @author Felix Rabinovich <felix@rabinovich.org>
 * @version $Revision: 17580 $
 */
class SetSizeOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	return array(null, true);
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	$errors = array();
	$warnings = array();
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'sizelimit');
	if ($ret) {
	    return array($ret, null, null);
	}
	foreach ($items as $i => $item) {
	    $warnings[$i] = array();
	    if (!GalleryUtilities::isA($item, 'GalleryDataItem')) {
		continue;
	    }

	    list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters(
		'module', 'sizelimit', $item->getParentId());
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $param = array_merge(
		array('keepOriginal' => 0, 'width' => 0, 'height' => 0, 'size' => 0), $param);

	    if ($param['width'] && $param['height'] && method_exists($item, 'getwidth') &&
		    method_exists($item, 'getheight') &&
		    ($item->getWidth() > $param['width'] ||
		     $item->getHeight() > $param['height'])) {
		$args = array($param['width'], $param['height']);
		if ($param['keepOriginal']) {
		    $ret = SizeLimitHelper::buildDerivativeWithLimits($item, 'scale', $args);
		} else {
		    $ret = SizeLimitHelper::applyLimits($item, 'scale', $args);
		}
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_UNSUPPORTED_FILE_TYPE) {
			$warnings[$i][] = $module->translate(
			    array('text' => 'WARNING: Cannot resize mime type %s',
				  'arg1' => $item->getMimeType()));
		    } else {
			return array($ret, null, null);
		    }
		}
	    }

	    if (isset($param['size']) && $param['size'] > 0 &&
		    ($item->getSize() >> 10) > $param['size']) {
		$args = array($param['size']);
		if ($param['keepOriginal']) {
		    $ret = SizeLimitHelper::buildDerivativeWithLimits($item, 'compress',
								      $args);
		} else {
		    $ret = SizeLimitHelper::applyLimits($item, 'compress', $args);
		}
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_UNSUPPORTED_FILE_TYPE) {
			$warnings[$i][] = $module->translate(
			    array('text' => 'WARNING: Cannot compress mime type %s',
				  'arg1' => $item->getMimeType()));
		    } else {
			return array($ret, null, null);
		    }
		}
	    }
	}
	return array(null, $errors, $warnings);
    }
}
?>
