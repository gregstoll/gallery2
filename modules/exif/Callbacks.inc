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
 * @package Exif
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ExifCallbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;
	static $blockNum = 0;

	switch ($callback) {
	case 'LoadExifInfo':
	    GalleryCoreApi::requireOnce('modules/exif/classes/ExifHelper.class');

	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($params['itemId'], 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $hasPermission) =
		GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
	    if ($ret) {
		return $ret;
	    }
	    if (!$hasPermission) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	    }

	    /* We only work on JPEG/raw data items */
	    if (!GalleryUtilities::isA($item, 'GalleryDataItem') ||
		!preg_match('{^image/(p?jpeg(-cmyk)?|x-dcraw)$}', $item->getMimeType())) {
		return null;
	    }

	    /* Feed the file to exifer */
	    list ($ret, $path) = $item->fetchPath();
	    if ($ret) {
		return $ret;
	    }

	    /* Figure out which version we're looking at */
	    $session =& $gallery->getSession();
	    if (empty($mode)) {
		$mode = $session->get('exif.module.LoadExifInfo.mode');
		if (empty($mode)) {
		    $mode = 'summary';
		}
	    }

	    $filter = ($mode == 'detailed') ? EXIF_DETAILED : EXIF_SUMMARY;

	    list ($ret, $exifData) = ExifHelper::getExifData($path, $filter);
	    if ($ret) {
		return $ret;
	    }
	    if (empty($exifData)) {
		if ($filter == EXIF_SUMMARY) {
		    list ($ret, $exifData) = ExifHelper::getExifData($path, EXIF_DETAILED);
		} else {
		    list ($ret, $exifData) = ExifHelper::getExifData($path, EXIF_SUMMARY);
		}
		if ($ret) {
		    return $ret;
		}
		$mode = null;
	    }
	    if (empty($exifData)) {
		return null;
	    }

	    $ajaxBlockNum = GalleryUtilities::getRequestVariables('blockNum');
	    $thisBlockNum = !empty($ajaxBlockNum) ? $ajaxBlockNum : ++$blockNum;
	    $block =& $smarty->_tpl_vars['block'];
	    $block['exif']['LoadExifInfo'] = array('exifData' => $exifData,
						   'mode' => $mode,
						   'blockNum' => $thisBlockNum);

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
