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
 * @package Getid3
 * @version $Revision: 17580 $
 */
class Getid3Callbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;

	switch ($callback) {
	case 'LoadGetid3Info':
	    GalleryCoreApi::requireOnce('modules/getid3/classes/Getid3Helper.class');

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

	    /* Ensure Item is a data item */
	    if (!GalleryUtilities::isA($item, 'GalleryDataItem')) {
		return null;
	    }

	    /* Feed the file to getid3 */
	    list ($ret, $path) = $item->fetchPath();
	    if ($ret) {
		return $ret;
	    }

	    /* Figure out which version we're looking at */
	    $session =& $gallery->getSession();
	    if (empty($mode)) {
		$mode = $session->get('getid3.module.LoadGetid3Info.mode');
		if (empty($mode)) {
		    $mode = 'summary';
		}
	    }

	    if ($mode == 'detailed') {
		$filter = GETID3_DETAILED;
	    } else {
		$filter = GETID3_SUMMARY;
	    }

	    list ($ret, $getid3Data) = Getid3Helper::getId3Data($path, $filter);
	    if ($ret) {
		return $ret;
	    }
	    if (empty($getid3Data)) {
		if ($filter == GETID3_SUMMARY) {
		    list ($ret, $getid3Data) = Getid3Helper::getId3Data($path, GETID3_DETAILED);
		} else {
		    list ($ret, $getid3Data) = Getid3Helper::getId3Data($path, GETID3_SUMMARY);
		}
		if ($ret) {
		    return $ret;
		}
		$mode = null;
	    }
	    list ($ret, $getid3ArchiveData) = Getid3Helper::getId3ArchiveData($path, $filter);
	    if ($ret) {
		return $ret;
	    }
	    if (empty($getid3Data) && empty($getid3ArchiveData)) {
		return null;
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['getid3']['LoadGetid3Info']['getid3Data'] = $getid3Data;
	    $block['getid3']['LoadGetid3Info']['getid3ArchiveData'] = $getid3ArchiveData;
	    $block['getid3']['LoadGetid3Info']['mode'] = $mode;

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
