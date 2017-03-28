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
 * This ItemAddOption uses an mp3's id3 title value for the item title when an mp3 is uploaded
 * @package Getid3
 * @subpackage UserInterface
 * @author Don Willingham <donwillingham@users.sf.net>
 * @version $Revision: 17580 $
 */
class Getid3DescriptionOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	list ($ret, $addOption) =
	     GalleryCoreApi::getPluginParameter('module', 'getid3', 'addOption');
	if ($ret) {
	    return array($ret, false);
	}
	return array(null, $addOption > 0);
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	$errors = $warnings = array();
	GalleryCoreApi::requireOnce('modules/getid3/classes/Getid3Extractor.class');
	GalleryCoreApi::requireOnce('modules/getid3/classes/Getid3Helper.class');

	list ($ret, $addOption) =
	    GalleryCoreApi::getPluginParameter('module', 'getid3', 'addOption');
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Copy the array because we will change it with do / while / array_splice */
	$itemsInBatches = $items;
	/*
	 * Batch size should be <= ulimit max open files, as long as we don't query this value,
	 * assume a value of 100 which is fairly low
	 */
	$batchSize = 100;
	do {
	    $currentItems = array_splice($itemsInBatches, 0, $batchSize);
	    $itemIds = array();
	    foreach ($currentItems as $item) {
		$itemIds[] = $item->getId();
	    }
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($itemIds);
	    if ($ret) {
		return array($ret, null, null);
	    }

	    for ($i = 0; $i < count($currentItems); $i++) {
		if ($currentItems[$i]->getMimeType() == 'audio/mpeg') {
		    list ($ret, $currentItems[$i]) = $currentItems[$i]->refresh();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }

		    $itemId = $currentItems[$i]->getId();

		    list ($ret, $getid3Data) =
			Getid3Extractor::getMetaData(array($itemId), array('Title'));
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }

		    $mustSave = false;

		    if (!empty($getid3Data[$itemId]['Title']['value'])) {
			if ($addOption & GETID3_MP3_TITLE) {
			    $currentItems[$i]->setTitle($getid3Data[$itemId]['Title']['value']);
			    $mustSave = true;
			}
		    }

		    if ($mustSave) {
			$ret = $currentItems[$i]->save();
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return array($ret, null, null);
			}
		    }
		}
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	} while (!empty($itemsInBatches));

	return array(null, $errors, $warnings);
    }
}
?>
