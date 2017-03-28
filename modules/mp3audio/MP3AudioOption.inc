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
 * This ItemAddOption adds the MP3AudioRenderer for any audio/mpeg item.
 * @package MP3Audio
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class MP3AudioOption extends ItemAddOption {

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
	global $gallery;
	$storage =& $gallery->getStorage();
	$errors = $mp3Items = array();

	foreach ($items as $item) {
	    if (GalleryUtilities::isA($item, 'GalleryDataItem')
		    && $item->getMimeType() == 'audio/mpeg') {
		$mp3Items[] = $item;
	    }
	}
	while (!empty($mp3Items)) {
	    $currentItems = array_splice($mp3Items, 0, 100);
	    $itemIds = array();
	    foreach ($currentItems as $item) {
		$itemIds[] = $item->getId();
	    }

	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($itemIds);
	    if ($ret) {
		return array($ret, null, null);
	    }

	    foreach ($currentItems as $item) {
		list ($ret, $item) = $item->refresh();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}

		$item->setRenderer('MP3AudioRenderer');
		$ret = $item->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ret = $storage->checkPoint();
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, $errors, array());
    }
}
?>
