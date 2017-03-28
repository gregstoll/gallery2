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

GalleryCoreApi::requireOnce('modules/exif/classes/ExifHelper.class');

/**
 * Settings for Exif
 * @package Exif
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @author Georg Rehfeld <rehfeld@georg-rehfeld.de>
 * @author Jozsef R.Nagy <jozsef.rnagy@site.hu>
 * @version $Revision: 17580 $
 */
class AdminExifController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$error = $status = array();
	if (isset($form['action']['availableToSummary'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_SUMMARY);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['available'])) {
		$count = 0;
		foreach ($form['available'] as $property) {
		    if (!in_array($property, $properties)) {
			$properties[] = $property;
			$count++;
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_SUMMARY, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['added']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][available][missing]';
	    }

	} else if (isset($form['action']['availableToDetailed'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_DETAILED);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['available'])) {
		$count = 0;
		foreach ($form['available'] as $property) {
		    if (!in_array($property, $properties)) {
			$properties[] = $property;
			$count++;
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_DETAILED, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['added']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][available][missing]';
	    }
	} else if (isset($form['action']['removeFromSummary'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_SUMMARY);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['summary'])) {
		$count = 0;
		$newProperties = array();
		for ($i = 0; $i < sizeof($properties); $i++) {
		    if (!in_array($properties[$i], $form['summary'])) {
			$newProperties[] = $properties[$i];
		    } else {
			$count++;
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_SUMMARY, $newProperties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['removed']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][summary][missing]';
	    }

	} else if (isset($form['action']['resetSummary'])) {
	    $ret = ExifHelper::setDefaultProperties(EXIF_SUMMARY);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['restored']['summary'] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'exif.AdminExif';
	} else if (isset($form['action']['removeFromDetailed'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_DETAILED);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['detailed'])) {
		$count = 0;
		$newProperties = array();
		for ($i = 0; $i < sizeof($properties); $i++) {
		    if (!in_array($properties[$i], $form['detailed'])) {
			$newProperties[] = $properties[$i];
		    } else {
			$count++;
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_DETAILED, $newProperties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['removed']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][detailed][missing]';
	    }
	} else if (isset($form['action']['resetDetailed'])) {
	    $ret = ExifHelper::setDefaultProperties(EXIF_DETAILED);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['restored']['detailed'] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'exif.AdminExif';
	} else if (isset($form['action']['moveUpSummary'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_SUMMARY);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['summary'])) {

		/* Iterate through each property that we want to shift */
		$count = 0;
		foreach ($form['summary'] as $propertyToShift) {
		    /* Find its spot in the array and swap it with the one ahead of it */
		    for ($i = 0; $i < sizeof($properties); $i++) {
			if ($properties[$i] == $propertyToShift) {
			    if ($i > 0) {
				$save = $properties[$i - 1];
				$properties[$i - 1] = $properties[$i];
				$properties[$i] = $save;
				$count++;
			    }
			}
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_SUMMARY, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedUp']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][summary][missing]';
	    }
	} else if (isset($form['action']['moveDownSummary'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_SUMMARY);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['summary'])) {

		/* Iterate through each property that we want to shift */
		$count = 0;
		foreach ($form['summary'] as $propertyToShift) {
		    /* Find its spot in the array and swap it with the one ahead of it */
		    for ($i = sizeof($properties) - 1; $i >= 0; $i--) {
			if ($properties[$i] == $propertyToShift) {
			    if ($i < sizeof($properties) - 1) {
				$save = $properties[$i + 1];
				$properties[$i + 1] = $properties[$i];
				$properties[$i] = $save;
				$count++;
			    }
			}
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_SUMMARY, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedDown']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][summary][missing]';
	    }
	} else if (isset($form['action']['moveUpDetailed'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_DETAILED);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['detailed'])) {

		/* Iterate through each property that we want to shift */
		$count = 0;
		foreach ($form['detailed'] as $propertyToShift) {
		    /* Find its spot in the array and swap it with the one ahead of it */
		    for ($i = 0; $i < sizeof($properties); $i++) {
			if ($properties[$i] == $propertyToShift) {
			    if ($i > 0) {
				$save = $properties[$i - 1];
				$properties[$i - 1] = $properties[$i];
				$properties[$i] = $save;
				$count++;
			    }
			}
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_DETAILED, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedUp']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][detailed][missing]';
	    }
	} else if (isset($form['action']['moveDownDetailed'])) {
	    list ($ret, $properties) = ExifHelper::getProperties(EXIF_DETAILED);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($form['detailed'])) {

		/* Iterate through each property that we want to shift */
		$count = 0;
		foreach ($form['detailed'] as $propertyToShift) {
		    /* Find its spot in the array and swap it with the one ahead of it */
		    for ($i = sizeof($properties) - 1; $i >= 0; $i--) {
			if ($properties[$i] == $propertyToShift) {
			    if ($i < sizeof($properties) - 1) {
				$save = $properties[$i + 1];
				$properties[$i + 1] = $properties[$i];
				$properties[$i] = $save;
				$count++;
			    }
			}
		    }
		}

		$ret = ExifHelper::setProperties(EXIF_DETAILED, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedDown']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'exif.AdminExif';
	    } else {
		$error[] = 'form[error][detailed][missing]';
	    }
	} else if (isset($form['action']['save'])) {
	    $addOption = (isset($form['item']['summary']) ? EXIF_ITEM_SUMMARY : 0)
		| (isset($form['item']['description']) ? EXIF_ITEM_DESCRIPTION : 0)
		| (isset($form['item']['keywords']) ? IPTC_ITEM_KEYWORDS : 0)
		| (isset($form['item']['objectName']) ? IPTC_ITEM_TITLE : 0)
		| (isset($form['item']['exifrotate']) ? EXIF_ITEM_ROTATE : 0)
		| (isset($form['item']['exifrotatepreserve']) ? EXIF_ITEM_ROTATE_PRESERVE : 0);

	    $ret = GalleryCoreApi::setPluginParameter('module', 'exif', 'addOption', $addOption);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['saved'] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'exif.AdminExif';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'exif.AdminExif';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Exif
 */
class AdminExifView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$mode = GalleryUtilities::getRequestVariables('mode');

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'exif');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminExif') {
	    /* Load up our form data */
	    $form['formName'] = 'AdminExif';

	    list ($ret, $addOption) = $module->getParameter('addOption');
	    if ($ret) {
		return array($ret, null);
	    }
	    $form['item'] = array('summary' => $addOption & EXIF_ITEM_SUMMARY,
				  'description' => $addOption & EXIF_ITEM_DESCRIPTION,
				  'keywords' => $addOption & IPTC_ITEM_KEYWORDS,
				  'objectName' => $addOption & IPTC_ITEM_TITLE,
				  'exifrotate' => $addOption & EXIF_ITEM_ROTATE,
				  'exifrotatepreserve' => $addOption & EXIF_ITEM_ROTATE_PRESERVE);
	}

	$exifKeys = ExifHelper::getExifKeys();
	$visibleKeys = array();

	/* Prepare the summary list */
	$summaryList = array();
	list ($ret, $tmp) = ExifHelper::getProperties(EXIF_SUMMARY);
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($tmp as $property) {
	    $summaryList[$property] = $module->translate($exifKeys[$property][0]);
	}

	/* Prepare the detailed list */
	$detailedList = array();
	list ($ret, $tmp) = ExifHelper::getProperties(EXIF_DETAILED);
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($tmp as $property) {
	    $detailedList[$property] = $module->translate($exifKeys[$property][0]);
	}

	/* Anything not summary and detailed is available */
	$availableList = array();
	foreach ($exifKeys as $key => $data) {
	    if (isset($summaryList[$key]) && isset($detailedList[$key])) {
		continue;
	    }

	    $availableList[$key] = $module->translate($data[0]);
	}
	asort($availableList);

	/* Check toolkit support for rotation */
	list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation('image/jpeg', 'rotate');
	if ($ret) {
	    return array($ret, null);
	}

	$AdminExif = array();
	$AdminExif['availableList'] = $availableList;
	$AdminExif['summaryList'] = $summaryList;
	$AdminExif['detailedList'] = $detailedList;
	$AdminExif['canRotate'] = isset($toolkit);

	$template->setVariable('AdminExif', $AdminExif);
	$template->setVariable('controller', 'exif.AdminExif');
	return array(null, array('body' => 'modules/exif/templates/AdminExif.tpl'));
    }
}
?>
