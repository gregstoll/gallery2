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
 * This ItemAddOption uses the EXIF description value for the gallery item summary/description
 * and the IPTC keywords for the gallery item keywords when the image is uploaded.
 *
 * @package Exif
 * @subpackage UserInterface
 * @author Elliot Shepherd <elliot@jarofworms.com>
 * @author Georg Rehfeld <rehfeld@georg-rehfeld.de>
 * @author Jozsef R.Nagy <jozsef.rnagy@site.hu>
 * @version $Revision: 17580 $
 */
class ExifDescriptionOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	list ($ret, $addOption) = GalleryCoreApi::getPluginParameter('module', 'exif', 'addOption');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $addOption > 0);
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	GalleryCoreApi::requireOnce('modules/exif/classes/ExifExtractor.class');
	GalleryCoreApi::requireOnce('modules/exif/classes/ExifHelper.class');
	$errors = $warnings = $needed = array();

	list ($ret, $addOption) =
	    GalleryCoreApi::getPluginParameter('module', 'exif', 'addOption');
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Check if we need to get/process any exifdata */
	if (($addOption & EXIF_ITEM_SUMMARY) || ($addOption & EXIF_ITEM_DESCRIPTION)) {
	    $needed[] = 'IPTC/Caption';
	    $needed[] = 'ImageDescription';
	    $needed[] = 'UserComment';
	}
	if ($addOption & IPTC_ITEM_KEYWORDS) {
	    $needed[] = 'IPTC/Keywords';
	}
	if ($addOption & IPTC_ITEM_TITLE) {
	    $needed[] = 'IPTC/ObjectName';
	}
	if ($addOption & EXIF_ITEM_ROTATE) {
	    $needed[] = 'Orientation';
	}

	if (!empty($needed)) {
	    /* Copy the array because we will change it with do / while / array_splice */
	    $itemsInBatches = $items;
	    /*
	     * Batch size should be <= ulimit max open files, as long as we don't query this value,
	     * assume a value of 100 which is fairly low
	     */
	    $batchSize = 100;
	    do {
		$currentItems = array_splice($itemsInBatches, 0, $batchSize);

		$currentItemIds = array();
		foreach ($currentItems as $item) {
		    $currentItemIds[] = $item->getId();
		}

		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($currentItemIds);
		if ($ret) {
		    return array($ret, null, null);
		}

		list ($ret, $exifData) = ExifExtractor::getMetaData($currentItemIds, $needed);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}

		for ($i = 0; $i < count($currentItems); $i++) {
		    $itemId = $currentItems[$i]->getId();
		    list ($ret, $currentItems[$i]) = $currentItems[$i]->refresh();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }

		    /*
		     * TODO(xlerb) reconsider, if ExifHelper should be changed to do the
		     * preferences.
		     */
		    $itemDescription = '';
		    if (!empty($exifData[$itemId]['IPTC/Caption']['value'])) {
			$itemDescription = $exifData[$itemId]['IPTC/Caption']['value'];
		    }
		    else if (!empty($exifData[$itemId]['ImageDescription']['value'])) {
			$itemDescription = $exifData[$itemId]['ImageDescription']['value'];
		    }
		    else if (!empty($exifData[$itemId]['UserComment']['value'])) {
			$itemDescription = $exifData[$itemId]['UserComment']['value'];
		    }

		    if (!empty($itemDescription)) {
			if ($addOption & EXIF_ITEM_SUMMARY) {
			    $currentItems[$i]->setSummary($itemDescription);
			}
			if ($addOption & EXIF_ITEM_DESCRIPTION) {
			    $currentItems[$i]->setDescription($itemDescription);
			}
		    }

		    if ($addOption & IPTC_ITEM_KEYWORDS
			    && !empty($exifData[$itemId]['IPTC/Keywords']['value'])) {
			$currentItems[$i]->setKeywords(
				$exifData[$itemId]['IPTC/Keywords']['value']);
		    }

		    if ($addOption & IPTC_ITEM_TITLE
			    && !empty($exifData[$itemId]['IPTC/ObjectName']['value'])) {
			$currentItems[$i]->setTitle($exifData[$itemId]['IPTC/ObjectName']['value']);
		    }

		    if ($currentItems[$i]->isModified()) {
			$ret = $currentItems[$i]->save();
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return array($ret, null, null);
			}
		    }
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null, null);
		}

		/*
		 * Rotate item based on exifdata in a separate loop because applyToolkitOperation
		 * does its own write locking.
		 */
		if ($addOption & EXIF_ITEM_ROTATE) {
		    for ($i = 0; $i < count($currentItems); $i++) {
			$itemId = $currentItems[$i]->getId();

			if (empty($exifData[$itemId]['Orientation']['value'])) {
			    continue;
			}

			$orientation = $exifData[$itemId]['Orientation']['value'];
			$args = array();

			/*
			 *  Note: Not handling the Mirrored cases this time.
			 *  Additional operation needed.
			 */

			 /*
			  //elseif ($orientation == 'Normal (O deg)') { leave $args empty }
			  else if ($orientation == 'Mirrored') {}
			  else if ($orientation == 'Upsidedown Mirrored') {}
			  else if ($orientation == '90 deg CW Mirrored') {}
			  else if ($orientation == '90 deg CCW Mirrored') {}
			 */

			if ($orientation == 'Upsidedown') {
			    $args = array(180);
			}
			else if ($orientation == '90 deg CW') {
			    $args = array(-90);
			}
			else if ($orientation == '90 deg CCW') {
			    $args = array(90);
			}

			$operation = 'rotate';

			$preserveOriginal = ($addOption & EXIF_ITEM_ROTATE_PRESERVE) ? 1 : 0;

			if (!empty($args)) {
			    list ($ret, $preferred) =
				GalleryCoreApi::fetchPreferredsByItemIds(array($itemId));
			    if ($ret) {
				return array($ret, null, null);
			    }
			    $ret = GalleryCoreApi::applyToolkitOperation(
				    $operation, $args, $preserveOriginal, $currentItems[$i],
				    empty($preferred) ? null : $preferred[$itemId]);
			    if ($ret) {
				/* Just write to error log; allow (unrotated) photo to be added */
				GalleryCoreApi::addEventLogEntry('Gallery Error',
					'Unable to rotate item ' . $itemId, $ret->getAsText());
			    }
			}
		    }
		}
	    } while (!empty($itemsInBatches));
	}
	return array(null, $errors, $warnings);
    }
}
?>
