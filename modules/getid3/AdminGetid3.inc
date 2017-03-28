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

GalleryCoreApi::requireOnce('modules/getid3/classes/Getid3Helper.class');

/**
 * Settings for getid3
 * @package Getid3
 * @subpackage UserInterface
 * @author Don Willingham <donwillingham@users.sf.net>
 * @version $Revision: 17580 $
 */
class AdminGetid3Controller extends GalleryController {

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
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_SUMMARY);
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

		$ret = Getid3Helper::setProperties(GETID3_SUMMARY, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['added']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][available][missing]';
	    }

	} else if (isset($form['action']['availableToDetailed'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_DETAILED);
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

		$ret = Getid3Helper::setProperties(GETID3_DETAILED, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['added']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][available][missing]';
	    }
	} else if (isset($form['action']['removeFromSummary'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_SUMMARY);
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

		$ret = Getid3Helper::setProperties(GETID3_SUMMARY, $newProperties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['removed']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][summary][missing]';
	    }

	} else if (isset($form['action']['resetSummary'])) {
	    $ret = Getid3Helper::setDefaultProperties(GETID3_SUMMARY);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['restored']['summary'] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'getid3.AdminGetid3';
	} else if (isset($form['action']['removeFromDetailed'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_DETAILED);
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

		$ret = Getid3Helper::setProperties(GETID3_DETAILED, $newProperties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['removed']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][detailed][missing]';
	    }
	} else if (isset($form['action']['resetDetailed'])) {
	    $ret = Getid3Helper::setDefaultProperties(GETID3_DETAILED);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['restored']['detailed'] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'getid3.AdminGetid3';

	} else if (isset($form['action']['moveUpSummary'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_SUMMARY);
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

		$ret = Getid3Helper::setProperties(GETID3_SUMMARY, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedUp']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][summary][missing]';
	    }
	} else if (isset($form['action']['moveDownSummary'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_SUMMARY);
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

		$ret = Getid3Helper::setProperties(GETID3_SUMMARY, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedDown']['summary'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][summary][missing]';
	    }
	} else if (isset($form['action']['moveUpDetailed'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_DETAILED);
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

		$ret = Getid3Helper::setProperties(GETID3_DETAILED, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedUp']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][detailed][missing]';
	    }
	} else if (isset($form['action']['moveDownDetailed'])) {
	    list ($ret, $properties) = Getid3Helper::getProperties(GETID3_DETAILED);
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

		$ret = Getid3Helper::setProperties(GETID3_DETAILED, $properties);
		if ($ret) {
		    return array($ret, null);
		}

		$status['movedDown']['detailed'] = $count;
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'getid3.AdminGetid3';
	    } else {
		$error[] = 'form[error][detailed][missing]';
	    }
	} else if (isset($form['action']['save'])) {
	    $addOptions = (isset($form['item']['mp3title']) ? GETID3_MP3_TITLE : 0);
	    $ret = GalleryCoreApi::setPluginParameter(
		    'module', 'getid3', 'addOption', $addOptions);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['saved'] = 1;
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'getid3.AdminGetid3';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'getid3.AdminGetid3';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for getid3
 */
class AdminGetid3View extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$mode = GalleryUtilities::getRequestVariables('mode');

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'getid3');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminGetid3') {
	    foreach (array('path') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'getid3', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $form['formName'] = 'AdminGetid3';

	    list ($ret, $addOption) = $module->getParameter('addOption');
	    if ($ret) {
		return array($ret, null);
	    }
	    $form['item'] = array('mp3title' => $addOption & GETID3_MP3_TITLE);
	}

	$getid3Keys = Getid3Helper::getGetid3Keys();
	$visibleKeys = array();

	/* Prepare the summary list */
	$summaryList = array();
	list ($ret, $tmp) = Getid3Helper::getProperties(GETID3_SUMMARY);
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($tmp as $property) {
	    $summaryList[$property] = $module->translate($getid3Keys[$property][0]);
	}

	/* Prepare the detailed list */
	$detailedList = array();
	list ($ret, $tmp) = Getid3Helper::getProperties(GETID3_DETAILED);
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($tmp as $property) {
	    $detailedList[$property] = $module->translate($getid3Keys[$property][0]);
	}

	/* Anything not summary and detailed is available */
	$availableList = array();
	foreach ($getid3Keys as $key => $data) {
	    if (isset($summaryList[$key]) && isset($detailedList[$key])) {
		continue;
	    }

	    $availableList[$key] = $module->translate($data[0]);
	}

	asort($availableList);

	$AdminGetid3 = array();
	$AdminGetid3['availableList'] = $availableList;
	$AdminGetid3['summaryList'] = $summaryList;
	$AdminGetid3['detailedList'] = $detailedList;

	$template->setVariable('AdminGetid3', $AdminGetid3);
	$template->setVariable('controller', 'getid3:AdminGetid3');
	return array(null, array('body' => 'modules/getid3/templates/AdminGetid3.tpl'));
    }
}
?>
