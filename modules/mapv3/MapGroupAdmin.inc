<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
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
 * @package map
 * @author Steven N. Severinghaus <sns@severinghaus.org>
 * @version $Revision: 1253 $
 *
 * This is some of the ugliest code I've ever written. Please do not
 * judge me by it. :)
 * @todo Change the map groups to derive from GalleryEntity and use that framework.
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');

/**
 * @package map
 * @subpackage UserInterface
 */
class MapGroupAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		global $gallery;

		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		$BASE_PATH_STRING = 'modules/mapv3';
		$basegallerydir   = substr(dirname(__FILE__), 0, -strlen($BASE_PATH_STRING));


		$status = array();
		$error  = array();
		// Cancel hit, return to the default Group adminpage
		if (isset($form['action']['cancel'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
		}

		if (isset($form['action']['add'])) {
			$results['status']              = $status;
			$results['error']               = $error;
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
			$results['redirect']['mode']    = 'AddGroup';
		}

		if (isset($form['action']['edit'])) {
			$id = substr($form['action']['edit'], 11);

			$tmp       = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');
			$AllGroups = explode('|', $tmp[1]);

			$newgroups = '';

			foreach ($AllGroups as $group) {
				$temp = explode('`', $group);
				// Warning: if you have duplicate IDs (in the old code), this will erase them.
				if ($temp[0] != $id) {
					$newgroups = ($newgroups == '') ? $group : $newgroups . '|' . $group;
				} else {
					$editgroup = $group;
				}
			}
			// Get the coordinates and such in the map database
			$query               = '
            SELECT
	      [mapv3Map::field],
	      [mapv3Map::value]
	    FROM
	      [mapv3Map]
	    WHERE
	      [mapv3Map::itemId] = \'G' . $id . '\'
	    ';
			list($ret, $result) = $gallery->search($query, array(), array());

			$params = '';

			while ($param = $result->nextResult()) {
				$params .= ($params != '') ? '|' : '';
				$params .= $param[0] . ':' . $param[1];
			}

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapGroups', $newgroups);
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroup', $editgroup);
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroupParam', $params);

			$results['status']              = $status;
			$results['error']               = $error;
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
			$results['redirect']['mode']    = 'AddGroup';
		}

		if (isset($form['action']['uploadpic'])) {
			// checking for error
			if ($form['name'][3] == '') {
				$error[] = 'form[error][nofileselected]';
			} elseif ($form['tmp_name'][3] == '') {
				$error[] = 'form[error][notuploaded]';
			} elseif (!move_uploaded_file(
				$form['tmp_name'][3],
				$basegallerydir . 'modules/mapv3/images/groups/' . $form['name'][3]
			)
			) {
				$error[] = 'form[error][rightserror]';
			} else {
				$status['uploaded'] = 1;
			}

			$GroupName = $_POST['GroupTitle'];
			$GroupDesc = $_POST['GroupDesc'];
			$GroupSumm = $_POST['GroupSumm'];
			$GroupDate = strtotime($_POST['GroupDate']);
			$Groupfile = $form['name'][3];

			$oldgroups = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');

			if ($oldgroups[1] != '') {
				// Get the Group ID if editing an existing one, otherwise initialize to 0
				$num = isset($_POST['oldid']) ? $_POST['oldid'] : 0;

				// Make sure all the groups have unique IDs
				$groupstrings = explode('|', $oldgroups[1]);

				foreach ($groupstrings as $tempgroup) {
					$tempgroups[] = explode('`', $tempgroup);
				}
				$maxGroupId = 0;
				$editIdDupe = false;
				$groupsById = array();

				foreach ($tempgroups as $groupKey => $groupArray) {
					$maxGroupId = max(array($maxGroupId, $groupArray[0]));

					if (array_key_exists($groupArray[0], $groupsById)) { // Duplicate ID
						// Check if the ID being edited is a duplicate
						if ($num == $groupArray[0]) {
							$editIdDupe = true;
						}
						// Move the dupe to the (currently-known) end
						$maxGroupId = $maxGroupId + 1;
						// reassign group ID to highest ID + 1
						$tempgroups[$groupKey][0] = $maxGroupId;
					}
					// Store the group string into the array, keyed by group ID
					$groupsById[$tempgroups[$groupKey][0]] = implode('`', $tempgroups[$groupKey]);
				}
				/*
				 * if $num was not found in the existing Group IDs (i.e., it's new)
				 * or if it was a dupe, then add it as a new group, else leave it alone
				 */
				$num = ($num == 0 or $editIdDupe) ? $maxGroupId + 1 : $num;
			} else {
				$num = 1;
			}

			$hash = array();

			foreach ($_POST as $key => $value) {
				if (substr($key, 0, 2) == 'P:') {
					if ($value) {
						$hash[] = $value;
					}
				}
			}

			$GroupItems = implode('`', $hash);

			if ($GroupName == '') {
				$GroupName = 'Group ' . $num;
			}
			$editgroup = $num . '`'
			 . $GroupName . '`'
			 . $GroupDesc . '`'
			 . $GroupSumm . '`'
			 . $Groupfile . '`'
			 . $GroupDate . '`'
			 . $GroupItems;

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroup', $editgroup);

			$id         = 'G' . $num;
			$coordinate = (isset($_POST['GroupGPS'])) ? $_POST['GroupGPS'] : '';
			$zoom       = (isset($_POST['GroupZoomlevel'])) ? $_POST['GroupZoomlevel'] : '';
			$color      = (isset($_POST['GroupColor'])) ? $_POST['GroupColor'] : '';

			if ($coordinate != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'GPS',
						'value'   => $coordinate,
						'setId'   => 0,
						'setType' => 2,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}
			// Actually update the database with the zoomlevel passed on to the ItemID
			if ($zoom != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'ZoomLevel',
						'value'   => $zoom,
						'setId'   => 0,
						'setType' => 1,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}
			// Actually update the database with the color passed on to the ItemID
			if ($color != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'Color',
						'value'   => $color,
						'setId'   => 0,
						'setType' => 2,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}

			$results['status']              = $status;
			$results['error']               = $error;
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
			$results['redirect']['mode']    = 'AddGroup';
		}

		if (isset($form['action']['delete'])) {
			$id = substr($form['action']['delete'], 13);

			$tmp           = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');
			$currentgroups = explode('|', $tmp[1]);

			$newgroups = '';

			foreach ($currentgroups as $group) {
				$temp = explode('`', $group);

				if ($temp[0] != $id) {
					$newgroups = ($newgroups == '') ? $group : $newgroups . '|' . $group;
				}
			}

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapGroups', $newgroups);

			// Also need to delete any (map) parameters in the database related to this item
			$id  = 'G' . $id;
			$ret = GalleryCoreApi::removeMapEntry(
				'mapv3Map',
				array(
					'itemId' => $id,
				)
			);

			if ($ret) {
				return $ret->wrap(__FILE__, __LINE__);
			}

			$status['groupdeleted'] = 1;

			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
			$results['redirect']['mode']    = 'Filter';
		}

		if (isset($form['action']['save'])) {
			$GroupName = $_POST['GroupTitle'];
			$GroupDesc = $_POST['GroupDesc'];
			$GroupSumm = $_POST['GroupSumm'];
			$GroupDate = strtotime($_POST['GroupDate']);
			$Groupfile = basename($_POST['pic']);

			$oldgroups = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');

			if ($oldgroups[1] != '') {
				// Get the Group ID if editing an existing one, otherwise initialize to 0
				$num = isset($_POST['oldid']) ? $_POST['oldid'] : 0;

				// Make sure all the groups have unique IDs
				$groupstrings = explode('|', $oldgroups[1]);

				foreach ($groupstrings as $tempgroup) {
					$tempgroups[] = explode('`', $tempgroup);
				}
				$maxGroupId = 0;
				$editIdDupe = false;
				$groupsById = array();

				foreach ($tempgroups as $groupKey => $groupArray) {
					$maxGroupId = max(array($maxGroupId, $groupArray[0]));

					if (array_key_exists($groupArray[0], $groupsById)) { // Duplicate ID
						// Check if the ID being edited is a duplicate
						if ($num == $groupArray[0]) {
							$editIdDupe = true;
						}
						// Move the dupe to the (currently-known) end
						$maxGroupId = $maxGroupId + 1;
						// reassign group ID to highest ID + 1
						$tempgroups[$groupKey][0] = $maxGroupId;
					}
					// Store the group string into the array, keyed by group ID
					$groupsById[$tempgroups[$groupKey][0]] = implode('`', $tempgroups[$groupKey]);
				}
				/*
				 * if $num was not found in the existing Group IDs (i.e., it's new)
				 * or if it was a dupe, then add it as a new group, else leave it alone
				 */
				$num = ($num == 0 or $editIdDupe) ? $maxGroupId + 1 : $num;
			} else {
				$num = 1;
			}

			$hash = array();

			foreach ($_POST as $key => $value) {
				if (substr($key, 0, 2) == 'P:') {
					if ($value) {
						$hash[] = $value;
					}
				}
			}

			$GroupItems = implode('`', $hash);

			if ($GroupName == '') {
				$GroupName = 'Group ' . $num;
			}
			$newgroup = $num . '`'
			. $GroupName . '`'
			. $GroupDesc . '`'
			. $GroupSumm . '`'
			. $Groupfile . '`'
			. $GroupDate . '`'
			. $GroupItems;

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroup', $newgroup);


			$id         = 'G' . $num;
			$coordinate = (isset($_POST['GroupGPS'])) ? $_POST['GroupGPS'] : '';
			$zoom       = (isset($_POST['GroupZoomlevel'])) ? $_POST['GroupZoomlevel'] : '';
			$color      = (isset($_POST['GroupColor'])) ? $_POST['GroupColor'] : '';

			// remove entries in the database related to this item
			$ret = GalleryCoreApi::removeMapEntry(
				'mapv3Map',
				array(
					'itemId' => $id,
				)
			);

			if ($ret) {
				return $ret->wrap(__FILE__, __LINE__);
			}

			// Actually update the database with the coordinated passed on to the ItemID
			if ($coordinate != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'GPS',
						'value'   => $coordinate,
						'setId'   => 0,
						'setType' => 2,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}
			// Actually update the database with the zoomlevel passed on to the ItemID
			if ($zoom != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'ZoomLevel',
						'value'   => $zoom,
						'setId'   => 0,
						'setType' => 1,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}
			// Actually update the database with the color passed on to the ItemID
			if ($color != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'Color',
						'value'   => $color,
						'setId'   => 0,
						'setType' => 2,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}


			$results['status']              = $status;
			$results['error']               = $error;
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
			$results['redirect']['mode']    = 'Filter';
		}

		if (isset($form['action']['getcoord'])) {
			$GroupName = $_POST['GroupTitle'];
			$GroupDesc = $_POST['GroupDesc'];
			$GroupSumm = $_POST['GroupSumm'];
			$GroupDate = strtotime($_POST['GroupDate']);
			$Groupfile = basename($_POST['pic']);

			$oldgroups = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');

			if ($oldgroups[1] != '') {
				// Get the Group ID if editing an existing one, otherwise initialize to 0
				$num = isset($_POST['oldid']) ? $_POST['oldid'] : 0;

				// Make sure all the groups have unique IDs
				$groupstrings = explode('|', $oldgroups[1]);

				foreach ($groupstrings as $tempgroup) {
					$tempgroups[] = explode('`', $tempgroup);
				}
				$maxGroupId = 0;
				$editIdDupe = false;
				$groupsById = array();

				foreach ($tempgroups as $groupKey => $groupArray) {
					$maxGroupId = max(array($maxGroupId, $groupArray[0]));

					if (array_key_exists($groupArray[0], $groupsById)) { // Duplicate ID
						// Check if the ID being edited is a duplicate
						if ($num == $groupArray[0]) {
							$editIdDupe = true;
						}
						// Move the dupe to the (currently-known) end
						$maxGroupId = $maxGroupId + 1;
						// reassign group ID to highest ID + 1
						$tempgroups[$groupKey][0] = $maxGroupId;
					}
					// Store the group string into the array, keyed by group ID
					$groupsById[$tempgroups[$groupKey][0]] = implode('`', $tempgroups[$groupKey]);
				}
				/*
				 * if $num was not found in the existing Group IDs (i.e., it's new)
				 * or if it was a dupe, then add it as a new group, else leave it alone
				 */
				$num = ($num == 0 or $editIdDupe) ? $maxGroupId + 1 : $num;
			} else {
				$num = 1;
			}

			$hash = array();

			foreach ($_POST as $key => $value) {
				if (substr($key, 0, 2) == 'P:') {
					if ($value) {
						$hash[] = $value;
					}
				}
			}

			$GroupItems = implode('`', $hash);

			if ($GroupName == '') {
				$GroupName = 'Group ' . $num;
			}
			$editgroup = $num . '`'
			 . $GroupName . '`'
			 . $GroupDesc . '`'
			 . $GroupSumm . '`'
			 . $Groupfile . '`'
			 . $GroupDate . '`'
			 . $GroupItems;

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroup', $editgroup);

			$id         = 'G' . $num;
			$coordinate = (isset($_POST['GroupGPS'])) ? $_POST['GroupGPS'] : '';
			$zoom       = (isset($_POST['GroupZoomlevel'])) ? $_POST['GroupZoomlevel'] : '';
			$color      = (isset($_POST['GroupColor'])) ? $_POST['GroupColor'] : '';

			if ($coordinate != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'GPS',
						'value'   => $coordinate,
						'setId'   => 0,
						'setType' => 2,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}
			// Actually update the database with the zoomlevel passed on to the ItemID
			if ($zoom != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'ZoomLevel',
						'value'   => $zoom,
						'setId'   => 0,
						'setType' => 1,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}
			// Actually update the database with the color passed on to the ItemID
			if ($color != '') {
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'     => $id,
						'field'      => 'Color',
						'value'      => $color,
						'setId'      => 0,
						'setType'    => 2,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
			}

			$results['status']              = $status;
			$results['error']               = $error;
			$results['redirect']['view']    = 'mapv3.ShowMap';
			$results['redirect']['Mode']    = 'Pick';
			$results['redirect']['plugin']  = 'GroupCoord';
			$results['redirect']['groupid'] = $id;
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapGroupAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		$template->head('modules/mapv3/templates/AdminHeader.tpl');
		$template->head('modules/mapv3/includes/GoogleMap.css');

		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$form['formName'] = 'MapGroupAdmin';

		// Get the date/time format and today's date
		list($ret, $coreParams)  = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
		$dateFormat              = $coreParams['format.date'];
		$form['now']             = strftime($dateFormat, strtotime('now'));

		// Select items that have GPS coordinates filled out
		$storage              =& $gallery->getStorage();
		$query                = '
	SELECT
	  [mapv3Map::itemId],
	  [mapv3Map::value]
	FROM
	  [mapv3Map]
	WHERE
	  [mapv3Map::field] = \'GPS\'
	';
		list($ret, $results) = $gallery->search($query, array(), array());

		if (!$ret) {
			$result1 = $results->nextResult();
		}

		if (!empty($result1)) {
			list($ret, $results) = $gallery->search($query, array(), array());

			/*
			 * Builds a hash for each mappable entity from its ID to a hash of
			 * properties. Not strictly necessary, since it just gets flattened
			 * out later, but it could be useful at some point.
			 */
			$ids       = array();
			$pointHash = array();

			while ($result = $results->nextResult()) {
				$id = $result[0];

				$ids[]                 = $id;
				$pointHash[$id]        = array();
				$pointHash[$id]['id']  = $id;
				$pointHash[$id]['gps'] = $result[1];


				$tmp = array();

				if (substr($id, 0, 1) == 'G') {
					if ((array_key_exists('EditGroup', $form) and $form['EditGroup'] != '')
						and (array_key_exists('mode', $form) and $form['mode'] == 'AddGroup')
					) {
						$tmp = explode('`', $form['EditGroup']);

						if ($tmp[0] == substr($id, 1)) {
							unset($pointHash[$id]);
						}
					}

					if (array_key_exists($id, $pointHash)) {
						$pointHash[$id]['type'] = 'GoogleMapGroup';
						$groupid                = substr($id, 1);
						list($ret, $tmpparam)   = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');
						$groups                 = explode('|', $tmpparam);

						foreach ($groups as $group) {
							if (!empty($group)) {
								$groupinfo = explode('`', $group);

								if ($groupinfo[0] == $groupid and $groupid != '') {
									$thumbpic                      = $basegallerydir . 'modules/mapv3/images/groups/'
										. $groupinfo[4];
									$pointHash[$id]['title']       = $groupinfo[1];
									$pointHash[$id]['date']        = strftime($dateFormat, $groupinfo[5]);
									$pointHash[$id]['description'] = $groupinfo[3];
									$pointHash[$id]['thumb']       = GalleryUtilities::convertPathToUrl($thumbpic);
								}
							}
						}
					}
				}
			}

			// create an ID array without the groups in it
			$tmp = array();

			foreach ($ids as $id) {
				if (substr($id, 0, 1) != 'G') {
					$tmp[] = $id;
				}
			}
			$ids = $tmp;


			// Trim the list down to the items that the user is allowed to see
			list($ret, $perms) = GalleryCoreApi::fetchPermissionsForItems($ids, $gallery->getActiveUserId());

			for ($i = 0; $i < count($ids); $i++) {
				if (!array_key_exists($ids[$i], $perms)
					|| !array_key_exists('core.view', $perms[$ids[$i]])
				) {
					unset($pointHash[$ids[$i]], $ids[$i]);
				}
			}

			$ids = array_values($ids);
			// Retrieve data for each entity
			list($ret, $entities) = $storage->loadEntities($ids);
			list($ret, $thumbs)   = GalleryCoreApi::fetchThumbnailsByItemIds($ids);

			/*
			 * Fill out additional properties for each entity, taking care to
			 * format them so that they don't break the Javascript that they
			 * end up embedded within.
			 */
			foreach ($entities as $entity) {
				$pointHash[$entity->getId()]['type']        = $entity->getEntityType();
				$pointHash[$entity->getId()]['title']       = preg_replace(array('/\n/', '/\r/'), array('', ''), $entity->getTitle());
				$pointHash[$entity->getId()]['description'] = preg_replace(array('/\n/', '/\r/'), array('', ''), $entity->getDescription());
				$pointHash[$entity->getId()]['thumb']       = $urlGenerator->generateUrl(
					array(
						'view'         => 'core.DownloadItem',
						'itemId'       => $thumbs[$entity->getId()]->getId(),
						'serialNumber' => $thumbs[$entity->getId()]->getSerialNumber(),
					)
				);
				$pointHash[$entity->getId()]['date']        = strftime($dateFormat, $entity->getCreationTimestamp());
			}


			$GPSItem = array();

			foreach ($pointHash as $id => $point) {
				$GPSItem[] = $point;
			}

			$form['items'] = $GPSItem;
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		// Create the option list for Group Color (based on Photo selection)
		if (array_key_exists('useMarkerSet', $form)) {
			if (file_exists(
				$basegallerydir . 'modules/mapv3/images/markers/' . $form['useMarkerSet']
				. '/colorlist'
			)
			) {
				$handle = fopen(
					$basegallerydir . 'modules/mapv3/images/markers/'
					. $form['useMarkerSet'] . '/colorlist',
					'r'
				);
				$list   = explode(
					'*',
					fread(
						$handle,
						filesize(
							$basegallerydir
							. 'modules/mapv3/images/markers/'
							. $form['useMarkerSet']
							. '/colorlist'
						) - 1
					)
				);
				fclose($handle);
				$form['colorGoption'] = '';

				foreach ($list as $color) {
					$form['colorGoption'] .= '<option value="' . $color . '" ';

					if ((array_key_exists('EditGroupParam', $form)
						and is_array($form['EditGroupParam'])
						and array_key_exists('Color', $form['EditGroupParam'])
						and ($form['EditGroupParam']['Color'] == $color))
						or ((!array_key_exists('EditGroupParam', $form)
						or !is_array($form['EditGroupParam'])
						or !array_key_exists('Color', $form['EditGroupParam']))
						and array_key_exists('defaultphotocolor', $form)
						and ($form['defaultphotocolor'] == $color))
					) {
						$form['colorGoption'] .= 'selected';
					}
					$form['colorGoption'] .= '>' . $color . '</option>';
				}
			}
		}

		// Create the group array
		if (array_key_exists('MapGroups', $param) and $param['MapGroups'] != '') {
			$groups = explode('|', $param['MapGroups']);
			sort($groups);

			foreach ($groups as $group) {
				$tmp                 = explode('`', $group);
				$tmp[5]              = strftime($dateFormat, $tmp[5]);
				$pic                 = GalleryUtilities::convertPathToUrl(
					$basegallerydir
					. 'modules/mapv3/images/groups/' . $tmp[4]
				);
				$tmp[4]              = '<img width=80px src="' . $pic . '" />';
				$form['thegroups'][] = $tmp;
			}
		}

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapGroupAdmin');

		return array(
			null,
			array(
				'body' => 'modules/mapv3/templates/MapGroupAdmin.tpl',
			),
		);
	}
}
