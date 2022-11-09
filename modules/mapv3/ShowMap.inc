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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/**
 * @package map
 * @author Steven N. Severinghaus <sns@severinghaus.org>
 * @version $Revision: 1576 $
 *
 * This implements the Google map view
 * This is some of the ugliest code I've ever written. Please do not
 * judge me by it. :)
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');

class ShowMapController extends GalleryController {
	public function handleRequest($form) {
		global $gallery;

		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mapv3');

		if ($ret) {
			return $ret->wrap(__FILE__, __LINE__);
		}

		// When requested to save the settings and populate the GPS coordinate
		if (isset($form['cancel'])) {
			if ($form['plugin'] == 'quickEdit') {
				$results['status']             = '';
				$results['error']              = array();
				$results['redirect']['itemId'] = $form['id'];
			}

			if ($form['plugin'] == 'ItemEdit') {
				$results['status']                 = '';
				$results['error']                  = array();
				$results['redirect']['view']       = 'core.ItemAdmin';
				$results['redirect']['subView']    = 'core.ItemEdit';
				$results['redirect']['itemId']     = $form['id'];
				$results['redirect']['editPlugin'] = 'mapItemEdit';
			}

			if ($form['plugin'] == 'GroupCoord') {
				// return to the page "Edit group"
				$results['status']              = '';
				$results['error']               = array();
				$results['redirect']['view']    = 'core.SiteAdmin';
				$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
				$results['redirect']['mode']    = 'AddGroup';
			}

			if ($form['plugin'] == 'AdminCenter' or $form['plugin'] == 'GVMDefault') {
				// return to the page "AdminPanel"
				$results['status']              = '';
				$results['error']               = array();
				$results['redirect']['view']    = 'core.SiteAdmin';
				$results['redirect']['subView'] = 'mapv3.MapSiteAdmin';
			}

			if ($form['plugin'] == 'Filters') {
				// return to the page "Legend Admin Panel"
				$results['status']              = '';
				$results['error']               = array();
				$results['redirect']['view']    = 'core.SiteAdmin';
				$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
				$results['redirect']['mode']    = 'AddFilter';
			}
		}

		if (isset($form['save'])) {
			if ($form['plugin'] == 'ItemEdit' or $form['plugin'] == 'GroupCoord' or $form['plugin'] == 'quickEdit') {
				$groupid = GalleryUtilities::getRequestVariables('groupid');

				if (!empty($groupid)) {
					$id = $groupid;
				} else {
					$id = $form['id'];
				}
				$coordinate = $form['coord'];
				$zoom       = $form['zoom'];

				$ret = GalleryCoreApi::removeMapEntry(
					'mapv3Map',
					array(
						'itemId' => $id,
						'field'  => 'GPS',
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
				// Actually update the database with the coordinated passed on to the ItemID
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'GPS',
						'value'   => $coordinate,
						'setId'   => 0,
						'setType' => 1,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
				$ret = GalleryCoreApi::removeMapEntry(
					'mapv3Map',
					array(
						'itemId' => $id,
						'field'  => 'ZoomLevel',
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
				// Actually update the database with the zoomlevel passed on to the ItemID
				$ret = GalleryCoreApi::addMapEntry(
					'mapv3Map',
					array(
						'itemId'  => $id,
						'field'   => 'ZoomLevel',
						'value'   => $zoom,
						'setId'   => 0,
						'setType' => 3,
					)
				);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}

				if ($form['plugin'] == 'GroupCoord') {
					// Get the coordinates and such in the map database
					$query              = '
		    SELECT
		      [mapv3Map::field],
		      [mapv3Map::value]
		    FROM
		      [mapv3Map]
		    WHERE
		      [mapv3Map::itemId] = \'' . $id . '\'
		    ';
					list($ret, $result) = $gallery->search($query, array(), array());

					$params = '';

					while ($param = $result->nextResult()) {
						if ($params != '') {
							$params = $params . '|' . $param[0] . ':' . $param[1];
						} else {
							$params = $param[0] . ':' . $param[1];
						}
					}

					GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroupParam', $params);
				}

				$results['status'] = '';
				$results['error']  = array();

				if ($form['plugin'] == 'ItemEdit') { // return to the page "ItemEdit"
					$results['redirect']['view']       = 'core.ItemAdmin';
					$results['redirect']['subView']    = 'core.ItemEdit';
					$results['redirect']['editPlugin'] = 'mapItemEdit';
					$results['redirect']['itemId']     = $id;
				} elseif ($form['plugin'] == 'GroupCoord') { // return to the page "Edit Group"
					$results['redirect']['view']    = 'core.SiteAdmin';
					$results['redirect']['subView'] = 'mapv3.MapGroupAdmin';
					$results['redirect']['mode']    = 'AddGroup';
				} elseif ($form['plugin'] == 'quickEdit') { // return to the page "quickEdit"
					$results['redirect']['itemId'] = $id;
				}
			}

			if ($form['plugin'] == 'AdminCenter') {
				GalleryCoreApi::setPluginParameter(
					'module',
					'mapv3',
					'centerLongLat',
					$form['coord']
				);
				GalleryCoreApi::setPluginParameter('module', 'mapv3', 'zoomLevel', $form['zoom']);

				// return to the page "AdminPanel"
				$results['status']              = '';
				$results['error']               = array();
				$results['redirect']['view']    = 'core.SiteAdmin';
				$results['redirect']['subView'] = 'mapv3.MapSiteAdmin';
			}

			if ($form['plugin'] == 'GVMDefault') {
				GalleryCoreApi::setPluginParameter('module', 'mapv3', 'GVMCenter', $form['coord']);
				GalleryCoreApi::setPluginParameter('module', 'mapv3', 'GVMZoom', $form['zoom']);

				// return to the page "AdminPanel"
				$results['status']              = '';
				$results['error']               = array();
				$results['redirect']['view']    = 'core.SiteAdmin';
				$results['redirect']['subView'] = 'mapv3.MapSiteAdmin';
			}

			if ($form['plugin'] == 'Filters') {
				$oldeditfilter = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'EditFilter');

				if (!empty($oldeditfilter[1])) {
					$oldfilter    = explode('`', $oldeditfilter[1]);
					$oldfilter[2] = $form['zoom'];
					$oldfilter[3] = $form['coord'];

					$neweditfilter = implode('`', $oldfilter);
				} else {
					$oldfilters = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Filters');

					if (!empty($oldfilters[1])) {
						$currentfilters = explode('|', $oldfilters[1]);

						foreach ($currentfilters as $filterinfos) {
							$infos = explode('`', $filterinfos);
							$ids[] = $infos[0];
						}
						$newid         = max($ids) + 1;
						$neweditfilter = $newid . '``' . $form['zoom'] . '`' . $form['coord'];
					} else {
						$neweditfilter = '1``' . $form['zoom'] . '`' . $form['coord'];
					}
				}

				GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditFilter', $neweditfilter);

				// return to the page "Legend Admin Panel"
				$results['status']              = '';
				$results['error']               = array();
				$results['redirect']['view']    = 'core.SiteAdmin';
				$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
				$results['redirect']['mode']    = 'AddFilter';
			}
		}

		return array(null, $results);
	}
}

class ShowMapView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		global $gallery;
		$platform = $gallery->getPlatform();

		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mapv3');

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null, null);
		}

		// Get the real path for the map module installation
		/** @todo for gallery 2.3 and later change this to use getCodeBasePath() */
		// $mapModBaseDir = GalleryCoreApi::getPluginBaseDir('module', 'mapv3') . 'modules/mapv3/';
		$mapModBaseDir = dirname(__FILE__) . '/';

		// Include header that links to the Google Maps script

		$template->head('modules/mapv3/templates/MapHeader.tpl');

		list($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'mapv3');

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null);
		}

		// creates the array that will be later passed on to ShowMap.inc
		$map = array();

		foreach ($param as $key => $value) {
			$map[$key] = $param[$key];
		}

		/** @todo Aren't these already covered by the foreach() above? */
		if (array_key_exists('ThumbBarPos', $param)) {
			$map['ThumbBarPos'] = $param['ThumbBarPos'];
		}

		if (array_key_exists('ThumbHeight', $param)) {
			$map['ThumbHeight'] = $param['ThumbHeight'];
		}

		$map['useMarkerSet']      = 'none';
		$map['useAlbumMarkerSet'] = 'none';
		$map['useGroupMarkerSet'] = 'none';

		if (array_key_exists('useMarkerSet', $param)
			and $platform->is_dir(
				$mapModBaseDir . 'images/markers/'
				   . $param['useMarkerSet']
			)
		) {
			$map['useMarkerSet'] = $param['useMarkerSet'];
		}

		if (array_key_exists('useGroupMarkerSet', $param)
			&& $platform->is_dir(
				$mapModBaseDir . 'images/markers/'
					 . $param['useGroupMarkerSet']
			)
		) {
			$map['useGroupMarkerSet'] = $param['useGroupMarkerSet'];
		} else {
			$map['useGroupMarkerSet'] = $param['useMarkerSet'];
		}

		if (array_key_exists('useAlbumMarkerSet', $param)
			&& $platform->is_dir(
				$mapModBaseDir . 'images/markers/'
					 . $param['useAlbumMarkerSet']
			)
		) {
			$map['useAlbumMarkerSet'] = $param['useAlbumMarkerSet'];
		} else {
			$map['useAlbumMarkerSet'] = $map['useMarkerSet'];
		}

		// Get the API key for this URL
		list($ret, $map['googleMapKey']) = mapv3Helper::fetchApiKey();

		if ($ret) {
			return $ret;
		}

		$map['WidthFormat']  = (isset($param['MapWidthFormat']) and $param['MapWidthFormat'] == '2') ? '%' : 'px';
		$map['HeightFormat'] = (isset($param['MapHeightFormat']) and $param['MapHeightFormat'] == '2') ? '%' : 'px';
		$map['mapWidth']     = isset($param['mapWidth']) ? $param['mapWidth'] : '600';
		$map['mapHeight']    = isset($param['mapHeight']) ? $param['mapHeight'] : '400';
		$map['mapType']      = isset($param['mapType']) ? $param['mapType'] : 'satellite';

		$map['mode'] = GalleryUtilities::getRequestVariables('Mode');
		$fullScreen  = GalleryUtilities::getRequestVariables('fullScreen');

		if (empty($map['mode'])) {
			$map['mode'] = 'Normal';
		}

		// "Normal" Mode to display the map

		$storage =& $gallery->getStorage();
		$gurl    =& $gallery->getUrlGenerator();

		$map['gallerybase'] = $gurl->generateUrl(
			array(
				'href' => '',
			),
			array(
				'forceFullUrl' => true,
			)
		);

		unset($map['zoomLevel']);

		if ($map['mode'] == 'Normal') {
			$map['EnableRouteNumber'] = isset($param['EnableRouteNumber']) ? $param['EnableRouteNumber'] : 0;

			$color = mapv3Helper::getItemColor(177);

			// Get the items with GPS coordinates
			list($ret, $gpsitems) = mapv3Helper::getGPSItems();

			if ($ret) {
				return array($ret, null);
			}

			if (count($gpsitems) > 0) {
				$map['nogpscoords'] = false;

				// Get the parent sequences
				$LevelFilterRoot = isset($param['LevelFilterRoot']) ? $param['LevelFilterRoot'] : 0;
				$LevelFilterAll  = isset($param['LevelFilterAll']) ? $param['LevelFilterRoot'] : 0;
				$albumfilter     = html_entity_decode(GalleryUtilities::getRequestVariables('album'));

				list($ret, $parentsequences) = mapv3Helper::getParentSequence($gpsitems, $LevelFilterRoot, $LevelFilterAll, $albumfilter);

				if ($ret) {
					return array($ret, null);
				}

				// Get the groups
				list($ret, $groups) = mapv3Helper::getGPSGroups($map['ThumbHeight']);

				if ($ret) {
					return array($ret, null);
				}

				// make the point Hash array
				$pointHash = $gpsitems + $groups;
				$ids       = array_keys($gpsitems);

				$isgroup = GalleryUtilities::getRequestVariables('Group');

				if (!empty($isgroup)) {
					/** todo add this to the function that gets items */
					// We're dealing with a group display, removing all item not in the group
					// Get the group
					if (array_key_exists('MapGroups', $param) and $param['MapGroups'] != '') {
						// we have some groups, let's check
						$allgroups   = explode('|', $param['MapGroups']);
						$theonegroup = array();

						foreach ($allgroups as $agroup) {
							$tmp = explode('`', $agroup);

							if ('G' . $tmp[0] == $isgroup) {
								$theonegroup = $tmp;
							}
						}

						if (!empty($theonegroup)) {
							$map['Filter'] = 'Group: ' . $theonegroup[1] . '  <a href='
											. $gurl->generateUrl(
												array(
													'view'  => 'mapv3.ShowMap',
													'Group' => '',
												),
												array(
													'forceFullUrl' => true,
												)
											)
											 . '>Show All</a>';
							$NumberOfIds   = count($ids);

							for ($i = 0; $i < $NumberOfIds; $i++) {
								$inGroup = false;

								for ($j = 6; $j < count($theonegroup); $j++) {
									if ($theonegroup[$j] == $ids[$i]) {
										$inGroup = true;
									}
								}

								if (!$inGroup) {
									unset($pointHash[$ids[$i]], $ids[$i]);
								}
							}
						} else { // No group found corresponding, joke ?
							$map['GroupHackingError'] = 1;
						}
					} else { // No groups at all ...
						$map['GroupHackingError'] = 1;
					}
					$ids = array_values($ids);
				}

				if (count($ids) > 0) {
					$map['noitemperms'] = false;

					// Get the list of album with GPS Item in them
					$map['listalbums'] = $parentsequences;
					$temp              = array();

					foreach ($map['listalbums'] as $tmp) {
						$temp[] = implode('`', $tmp);
					}
					$listofalbums = implode('`', array_unique($temp));
					$temp         = explode('`', $listofalbums);
					$listofalbums = implode('`', array_unique($temp));
					GalleryCoreApi::setPluginParameter('module', 'mapv3', 'albumlist', $listofalbums);

					// Trim the list if the "Album" filter is ON
					// $albumfilter = GalleryUtilities::getRequestVariables('album');
					list($ret, $TheRoot) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');

					if ($ret) {
						return array($ret, null);
					}

					$map['noiteminalbum'] = false;

					if (!empty($albumfilter)) {
						/** @todo: Include a marker for the album if it has coordinates */
						$map['Filter'] = 'Album: ' . $albumfilter;
						$NumberOfIds   = count($ids);

						for ($i = 0; $i < $NumberOfIds; $i++) {
							$pos     = $ids[$i];
							$InAlbum = false;

							if ($pos != $TheRoot) {
								for ($j = 0; $j < count($parentsequences[$pos]); $j++) {
									if (strtolower($parentsequences[$pos][$j]) == strtolower($albumfilter)
									) {
										$InAlbum = true;

										break;
									}
								}
							}

							if (!$InAlbum) {
								unset($pointHash[$pos], $ids[$i]);
							}
						}
					}

					$ids = array_values($ids);

					if (count($ids) > 0) {
						// prepare for regrouping
						foreach ($ids as $id) {
							$pointHash[$id]['regroupShowHigh'] = 0;
							$pointHash[$id]['regroupShowLow']  = 19;
							$parentId                          = $pointHash[$id]['parentId'];

							if (array_key_exists($pointHash[$id]['parentId'], $pointHash)
								and (array_key_exists('regroupAlbums', $param)
								and array_key_exists('regroupDist', $param)
								and ($param['regroupAlbums'] == 1))
							) {
								$pointHash[$id]['parentDist']     = 1;
								$pointHash[$parentId]['hasChild'] = true;
							} else {
								$pointHash[$id]['parentDist']     = -1;
								$pointHash[$id]['hasHiddenChild'] = false;
							}
							$pointHash[$id]['group']   = -1;
							$pointHash[$id]['display'] = true;
						}

						// Album Regroup
						if (array_key_exists('regroupAlbums', $param) and array_key_exists('regroupDist', $param)
							and $param['regroupAlbums'] == 1
						) {
							$map['regroupAlbums'] = true;
							$pointHash            = GoogleMapUtilities::updateArrayForAlbumRegroup($pointHash, $param['regroupDist']);
						} else {
							$map['regroupAlbums'] = false;
						}

						$mapPoints = $pointHash;

						// item Regroup
						if (array_key_exists('regroupItems', $param)
							and $param['regroupItems']
						) {
							$map['regroupItems']       = true;
							$map['regroupIcon']        = $param['regroupIcon'];
							$iconsize                  = $platform->getimagesize(
								$mapModBaseDir . 'images/multi/'
								. $param['regroupIcon'] . '.png'
							);
							$map['ReplaceMarkerSizeX'] = $iconsize[0];
							$map['ReplaceMarkerSizeY'] = $iconsize[1];

							if ($param['regroupIcon'] == 'zoom-in-trans') {
								$map['replaceAnchorPos'] = '' . ($iconsize[0] / 2) . ', '
														   . ($iconsize[1] / 2);
							} else {
								$map['replaceAnchorPos'] = '' . ($iconsize[0] / 2) . ', '
														   . $iconsize[1];
							}

							$regroupDist    = $param['regroupDist'];
							$showItems      = array();
							$showGroups     = array();
							$groups         = array();
							$nbgroups       = 0;
							$distances      = array();
							$groupItemDists = array();
							$groupDists     = array();
							$nbpoints       = count($mapPoints);

							$DEBUG_REGROUP = false; // set true to output ugly regroup feature debug info instead of a nice map
							for ($z = 19; $z >= 0; $z--) {
								if ($DEBUG_REGROUP) {
									echo " -------------- ZOOMLEVEL $z -----------------\n";
								}
								/*
								 * unset points that are not visible and set new points
								 * (album based)
								 */
								foreach ($mapPoints as $id => $point) {
									if ($z < $point['regroupShowHigh']) {
										// item no longer visible because of album regroup
										$mapPoints[$id]['display'] = false;

										if (isset($showItems[$id])) {
											unset($showItems[$id]);
										}
									} elseif (($z <= $point['regroupShowLow'])
										&& ($z >= $point['regroupShowHigh'])
									) {
										// album item popped in at this level
										if (!isset($showItems[$id])) {
											$showItems[$id] = true;
										}
										$mapPoints[$id]['display'] = true;
									}
								}
								// check first if any items can be added to an existing group
								foreach ($showItems as $id => $true2) {
									foreach ($showGroups as $groupId => $true) {
										if ($mapPoints[$id]['display']) {
											// if not, they are already in a previous group
											$groupItemDists[$groupId][$id] = mapv3Helper::PixelDistance(
												$mapPoints[$id]['gps'],
												$groups[$groupId]['gps'],
												$z
											);
											$pixdist                       = $groupItemDists[$groupId][$id];

											if ($pixdist <= $regroupDist) {
												if ($mapPoints[$id]['regroupShowLow'] == $z) {
													if ($DEBUG_REGROUP) {
														echo " ITEM $id NEVER SHOWS UP\n";
													}
													/*
													 * in this case, we don't add the marker to
													 * the group because it was never showed.
													 */

													// do nothing
													$mapPoints[$id]['display']         = false;
													$mapPoints[$id]['regroupShowHigh'] = $z + 1;
												} else { // add marker to group
													if ($DEBUG_REGROUP) {
														echo " Adding Item  $id to groups $groupId\n";
													}
													$groups[$groupId]['items']++;
													$groups[$groupId]['directItems']++;
													$mapPoints[$id]['group']           = $groupId;
													$mapPoints[$id]['display']         = false;
													$mapPoints[$id]['regroupShowHigh'] = $z + 1;
													// group item only into one group!
													continue 1;
												}
											}
										}
									}
								}

								// undisplay grouped items
								foreach ($mapPoints as $id => $point) {
									if (!$point['display'] && isset($showItems[$id])) {
										unset($showItems[$id]);
									}
								}

								// create new groups with close items
								foreach ($showItems as $id1 => $true1) {
									foreach ($showItems as $id2 => $true2) {
										if ($id1 != $id2) {
											$dist                  = mapv3Helper::PixelDistance(
												$mapPoints[$id1]['gps'],
												$mapPoints[$id2]['gps'],
												$z
											);
											$distances[$id1][$id2] = $dist;
											$pixdist               = $distances[$id1][$id2];

											if ($pixdist <= $regroupDist) {
												if ($mapPoints[$id1]['regroupShowLow'] == $z) {
													/*
													 * in this case, we don't add the marker to
													 * any group because it was never showed.
													 */

													/* do nothing. skip item 1, it disappears
													 * before showing up.
													 */
													if ($DEBUG_REGROUP) {
														echo " ITEMS $id1 and $id2 close but $id1 never shows up => skip $id1 and $id2\n";
													}
													$mapPoints[$id1]['display']         = false;
													$mapPoints[$id1]['regroupShowHigh'] = $z + 1;

													continue 1;
												}

												if ($mapPoints[$id2]['regroupShowLow'] == $z) {
													/*
													 * in this case, we don't add the marker to
													 * any group because it was never showed.
													 */

													// do nothing, continue with next item 2
													if ($DEBUG_REGROUP) {
														echo " ITEMS $id1 and $id2 close but $id2 never shows up => skip $id2\n";
													}
													$mapPoints[$id2]['display']         = false;
													$mapPoints[$id2]['regroupShowHigh'] = $z + 1;

													continue;
												}
												// check if these items have been grouped
												$g1 = $mapPoints[$id1]['group'];
												$g2 = $mapPoints[$id2]['group'];

												if ($g1 < 0 and $g2 < 0) {
													if ($DEBUG_REGROUP) {
														echo " regrouping ITEMS $id1 and $id2, CREATING NEW GROUP $nbgroups !!!\n";
													}
													// make a new group merging two elements
													$groups[$nbgroups]['regroupShowLow'] = $z;
													$groups[$nbgroups]['maxx']           = max(
														$mapPoints[$id1]['posx'],
														$mapPoints[$id2]['posx']
													);
													$groups[$nbgroups]['minx']           = min(
														$mapPoints[$id1]['posx'],
														$mapPoints[$id2]['posx']
													);
													$groups[$nbgroups]['maxy']           = max(
														$mapPoints[$id1]['posy'],
														$mapPoints[$id2]['posy']
													);
													$groups[$nbgroups]['miny']           = min(
														$mapPoints[$id1]['posy'],
														$mapPoints[$id2]['posy']
													);
													$groups[$nbgroups]['group']          = -1;
													$groups[$nbgroups]['groups']         = 0;
													$groups[$nbgroups]['display']        = true;
													$groups[$nbgroups]['items']          = 2;
													$groups[$nbgroups]['directItems']    = 2;

													$mapPoints[$id1]['group']           = $nbgroups;
													$mapPoints[$id2]['group']           = $nbgroups;
													$mapPoints[$id1]['regroupShowHigh'] = $z + 1;
													$mapPoints[$id2]['regroupShowHigh'] = $z + 1;
													$mapPoints[$id1]['display']         = false;
													$mapPoints[$id2]['display']         = false;
													$nbgroups++;
												} elseif ($g1 < 0) {
													if ($DEBUG_REGROUP) {
														echo " regrouping ITEMS $id1 and $id2, putting $id1 into $g2\n";
													}
													// item1 not yet grouped, put item1 in g2
													$groups[$g2]['maxx'] = max(
														$groups[$g2]['maxx'],
														$mapPoints[$id1]['posx']
													);
													$groups[$g2]['minx'] = min(
														$groups[$g2]['minx'],
														$mapPoints[$id1]['posx']
													);
													$groups[$g2]['maxy'] = max(
														$groups[$g2]['maxy'],
														$mapPoints[$id1]['posy']
													);
													$groups[$g2]['miny'] = min(
														$groups[$g2]['miny'],
														$mapPoints[$id1]['posy']
													);
													$groups[$g2]['items']++;
													$groups[$g2]['directItems']++;

													$mapPoints[$id1]['group']           = $g2;
													$mapPoints[$id1]['regroupShowHigh'] = $z + 1;
													$mapPoints[$id1]['display']         = false;
												} else {
													if ($g1 == $g2) {
														/*
														 * both already grouped in same group,
														 * do nothing!
														 */
														continue;

														if ($DEBUG_REGROUP) {
															echo " ITEMS $id1 and $id2 are already in $g1, skipping $id2\n";
														}
													}

													if ($DEBUG_REGROUP) {
														echo " regrouping ITEMS $id1 and $id2, putting $id2 into $g1\n";
													}
													// put item2 in g1
													/*
													 * if old item2 was already in a new group,
													 * this will be merged afterwards
													 */
													$groups[$g1]['maxx'] = max(
														$groups[$g1]['maxx'],
														$mapPoints[$id2]['posx']
													);
													$groups[$g1]['minx'] = min(
														$groups[$g1]['minx'],
														$mapPoints[$id2]['posx']
													);
													$groups[$g1]['maxy'] = max(
														$groups[$g1]['maxy'],
														$mapPoints[$id2]['posy']
													);
													$groups[$g1]['miny'] = min(
														$groups[$g1]['miny'],
														$mapPoints[$id2]['posy']
													);
													$groups[$g1]['items']++;
													$groups[$g1]['directItems']++;

													$mapPoints[$id2]['group']           = $g1;
													$mapPoints[$id2]['regroupShowHigh'] = $z + 1;
													$mapPoints[$id2]['display']         = false;

													if ($g2 > -1) {
														if ($DEBUG_REGROUP) {
															echo " regrouping ITEMS $id1 and $id2, both were in a group ($g1 and $g2). Moving elements of $g2 into $g1\n";
														}
														// item2 was already in a new group.
														/*
														 * move other elements of this new group
														 * into this one, and kill other new group
														 */
														foreach ($showItems as $id3 => $true3) {
															if ($id3 != $id1 and $id3 != $id2) {
																if ($mapPoints[$id3]['group'] == $g2
																	&& $mapPoints[$id3]['group'] > -1
																) {
																	if ($DEBUG_REGROUP) {
																		echo " moving ITEM $id3 from {$mapPoints[$id3]['group']} to $g1\n";
																	}
																	// now this icon3 goes also in NEW g1
																	$groups[$g1]['maxx'] = max(
																		$groups[$g1]['maxx'],
																		$mapPoints[$id3]['posx']
																	);
																	$groups[$g1]['minx'] = min(
																		$groups[$g1]['minx'],
																		$mapPoints[$id3]['posx']
																	);
																	$groups[$g1]['maxy'] = max(
																		$groups[$g1]['maxy'],
																		$mapPoints[$id3]['posy']
																	);
																	$groups[$g1]['miny'] = min(
																		$groups[$g1]['miny'],
																		$mapPoints[$id3]['posy']
																	);
																	$groups[$g1]['items']++;
																	$groups[$g1]['directItems']++;

																	$mapPoints[$id3]['group']           = $g1;
																	$mapPoints[$id3]['regroupShowHigh'] = $z - 1;
																	$mapPoints[$id3]['display']         = false;
																}
															}
														}

														if ($DEBUG_REGROUP) {
															echo " DELETING new group $g2\n";
														}
														unset($groups[$g2]);

														if (isset($showGroups[$g2])) {
															unset($showGroups[$g2]); // die("noo 1");
														}
													}
												}
											}
										}
									}
								}

								// setup and display new groups
								foreach ($groups as $id => $group) {
									if ($group['display'] && !isset($showGroups[$id])) {
										// new group in array. show and calculate marker pos
										$showGroups[$id]     = true;
										$groups[$id]['posx'] = ($group['maxx'] + $group['minx']) / 2;
										$groups[$id]['posy'] = ($group['maxy'] + $group['miny']) / 2;
										$groups[$id]['gps']  = number_format($groups[$id]['posx'], 10, '.', '')
											. ','
											. number_format($groups[$id]['posy'], 10, '.', '');
									}
								}

								// undisplay grouped items
								foreach ($mapPoints as $id => $point) {
									if (!$point['display'] and isset($showItems[$id])) {
										unset($showItems[$id]);
									}
								}

								// check if any groups can be merged
								foreach ($showGroups as $gid1 => $true) {
									foreach ($showGroups as $gid2 => $true2) {
										if ($gid1 < $gid2) {
											$groupDists[$gid1][$gid2] = mapv3Helper::PixelDistance(
												$groups[$gid1]['gps'],
												$groups[$gid2]['gps'],
												$z
											);
											$pixdist                  = $groupDists[$gid1][$gid2];

											if ($pixdist <= $regroupDist) {
												if ($DEBUG_REGROUP) {
													echo " groups $gid1 and $gid2 have dist $pixdist\n";
												}
												// check if these groups have been regrouped
												$g1 = $groups[$gid1]['group'];
												$g2 = $groups[$gid2]['group'];

												if ($g1 < 0 && $g2 < 0) {
													if ($DEBUG_REGROUP) {
														echo " regrouping groups $gid1 and $gid2, CREATING NEW GROUP $nbgroups !!!\n";
													}
													// make a new group merging two (old) groups
													$groups[$nbgroups]['regroupShowLow'] = $z;
													$groups[$nbgroups]['maxx']           = max(
														$groups[$gid1]['maxx'],
														$groups[$gid2]['maxx']
													);
													$groups[$nbgroups]['minx']           = min(
														$groups[$gid1]['minx'],
														$groups[$gid2]['minx']
													);
													$groups[$nbgroups]['maxy']           = max(
														$groups[$gid1]['maxy'],
														$groups[$gid2]['maxy']
													);
													$groups[$nbgroups]['miny']           = min(
														$groups[$gid1]['miny'],
														$groups[$gid2]['miny']
													);
													$groups[$nbgroups]['group']          = -1;
													$groups[$nbgroups]['groups']         = 2;
													$groups[$nbgroups]['display']        = true;
													$groups[$nbgroups]['items']          = $groups[$gid1]['items'] + $groups[$gid2]['items'];
													$groups[$nbgroups]['directItems']    = 0;

													$groups[$gid1]['group']           = $nbgroups;
													$groups[$gid2]['group']           = $nbgroups;
													$groups[$gid1]['regroupShowHigh'] = $z + 1;
													$groups[$gid2]['regroupShowHigh'] = $z + 1;
													$groups[$gid1]['display']         = false;
													$groups[$gid2]['display']         = false;
													$nbgroups++;
												} elseif ($g1 < 0) {
													if ($DEBUG_REGROUP) {
														echo " adding group1 $gid1 (group2 = $gid2) to NEW group $g2\n";
													}
													// put (old) group1 in NEW g2
													$groups[$g2]['maxx'] = max(
														$groups[$g2]['maxx'],
														$groups[$gid1]['maxx']
													);
													$groups[$g2]['minx'] = min(
														$groups[$g2]['minx'],
														$groups[$gid1]['minx']
													);
													$groups[$g2]['maxy'] = max(
														$groups[$g2]['maxy'],
														$groups[$gid1]['maxy']
													);
													$groups[$g2]['miny'] = min(
														$groups[$g2]['miny'],
														$groups[$gid1]['miny']
													);
													$groups[$g2]['groups']++;
													$groups[$g2]['items'] += $groups[$gid1]['items'];

													$groups[$gid1]['group']           = $g2;
													$groups[$gid1]['regroupShowHigh'] = $z + 1;
													$groups[$gid1]['display']         = false;
												} else {
													if ($g1 == $g2) {
														/*
														 * both already grouped in same group,
														 * do nothing!
														 */
														if ($DEBUG_REGROUP) {
															echo " group1 $gid1 and group2 $gid2 are both already in NEW group $g2. COOL!\n";
														}

														continue;
													}

													if ($DEBUG_REGROUP) {
														echo " adding group2 $gid2 (group1 = $gid1) to NEW group $g1\n";
													}
													// put (old) group2 in NEW g1
													/*
													 * if old group2 was already in a new group,
													 * this will be merged afterwards
													 */

													$groups[$g1]['maxx'] = max(
														$groups[$g1]['maxx'],
														$groups[$gid2]['maxx']
													);
													$groups[$g1]['minx'] = min(
														$groups[$g1]['minx'],
														$groups[$gid2]['minx']
													);
													$groups[$g1]['maxy'] = max(
														$groups[$g1]['maxy'],
														$groups[$gid2]['maxy']
													);
													$groups[$g1]['miny'] = min(
														$groups[$g1]['miny'],
														$groups[$gid2]['miny']
													);
													$groups[$g1]['groups']++;
													$groups[$g1]['items'] += $groups[$gid2]['items'];

													$groups[$gid2]['group']           = $g1;
													$groups[$gid2]['regroupShowHigh'] = $z + 1;
													$groups[$gid2]['display']         = false;

													if ($g2 > -1) {
														if ($DEBUG_REGROUP) {
															echo " group2 $gid2 was in NEW group $g2\n";
														}
														// group2 was already in a new group.
														/*
														 * move other elements of this new group
														 * in this one, and kill other new group
														 */
														foreach ($showGroups as $gid3 => $true3) {
															if ($gid3 != $gid1 and $gid3 != $gid2) {
																if ($groups[$gid3]['group'] == $g2
																	&& $groups[$gid3]['group'] > -1
																) {
																	if ($DEBUG_REGROUP) {
																		echo " adding group3 $gid3 to NEW group $g1\n";
																	}
																	// now this (old) group3 comes also in NEW g1
																	$groups[$g1]['maxx'] = max(
																		$groups[$g1]['maxx'],
																		$groups[$gid3]['maxx']
																	);
																	$groups[$g1]['minx'] = min(
																		$groups[$g1]['minx'],
																		$groups[$gid3]['minx']
																	);
																	$groups[$g1]['maxy'] = max(
																		$groups[$g1]['maxy'],
																		$groups[$gid3]['maxy']
																	);
																	$groups[$g1]['miny'] = min(
																		$groups[$g1]['miny'],
																		$groups[$gid3]['miny']
																	);
																	$groups[$g1]['groups']++;
																	$groups[$g1]['items'] += $groups[$gid3]['items'];

																	$groups[$gid3]['group']           = $g1;
																	$groups[$gid3]['regroupShowHigh'] = $z + 1;
																	$groups[$gid3]['display']         = false;
																}
															}
														}

														if ($DEBUG_REGROUP) {
															echo " DELETING new group $g2\n";
														}
														unset($groups[$g2]);

														if (isset($showGroups[$g2])) {
															unset($showGroups[$g2]);
														}
													}
												}
											}
										}
									}
								}

								// undisplay merged groups, and prepare new groups
								foreach ($groups as $id => $group) {
									// undisplay merged groups
									if (!$group['display'] && isset($showGroups[$id])) {
										// old group gone out
										unset($showGroups[$id]);
									} elseif ($group['display'] && !isset($showGroups[$id])) {
										// new group in array. show and calculate marker pos
										$showGroups[$id]     = true;
										$groups[$id]['posx'] = ($group['maxx'] + $group['minx']) / 2;
										$groups[$id]['posy'] = ($group['maxy'] + $group['miny']) / 2;
										$groups[$id]['gps']  = number_format($groups[$id]['posx'], 10, '.', '')
											. ', '
											. number_format($groups[$id]['posy'], 10, '.', '');
									}
								}
								// ok, done for this zoomlevel. next zoomlevel please!
							}

							// set showHigh to 0 for groups which are still showed at the end.
							foreach ($showGroups as $id => $true) {
								$groups[$id]['regroupShowHigh'] = 0;
							}

							// unset albums that never show up
							foreach ($mapPoints as $id => $point) {
								if ($point['regroupShowHigh'] > $point['regroupShowLow']) {
									/*
									 * this special case is an album that is hidden (grouped)
									 * because of grouping before it is actually showed.
									 */

									// we can remove it from the list.
									unset($mapPoints[$id]);
								}
							}

							$map['regroupPoints'] = $groups;
						} else {
							$map['regroupItems'] = false;
						}
					} else {
						$map['noiteminalbum'] = true; // No item in the selected album
					}
				} else {
					/*
					 * There is no marker because no item with GPS coordinate have correct
					 * permission
					 */
					$map['nogpscoords'] = true;
				}
			} else {
				// There is no marker because no item have GPS coordinate
				$map['noitemperms'] = true;
			}

			/** END OF MAKING THE ARRAY OF ITEMS WITH GPS **/

			// Set various parameters needed by ShowMap.tpl, accessible as {$map.mapWidth}, etc.
			$map['mapPoints']    = (isset($mapPoints)) ? $mapPoints : array();
			$map['RegroupItems'] = (isset($groups)) ? $groups : array();

			if (!array_key_exists('MapWindowType', $map) or $map['MapWindowType'] == '') {
				$map['MapWindowType'] = 'default';
			}

			include $mapModBaseDir . 'templates/infowindow/' . $map['MapWindowType'] . '.inc';

			$filter = GalleryUtilities::getRequestVariables('filter');

			if (!empty($filter)) {
				if (!array_key_exists('Filters', $param) and substr($filter, 0, 5) != 'Route') {
					$map['filterhackingerror'] = 1;
				} else {
					$currentfilters = $param['Filters'];

					if ($currentfilters != '') {
						$filterarray = explode('|', $currentfilters);

						foreach ($filterarray as $filterinfos) {
							$infos = explode('`', $filterinfos);

							if ($infos[1] == $filter) {
								$map['centerLongLat'] = $infos[3];
								$map['zoomLevel']     = $infos[2];
								$map['Filter']        = $infos[1];

								break;
							}
						}
					}

					if (substr($filter, 0, 5) == 'Route') {
						// filtering on a route, create the values in the javascript
						$map['centerLongLat'] = '0,0';
						$map['zoomLevel']     = '5';
						$map['Filter']        = $filter;
					}

					if (!array_key_exists('centerLongLat', $map)) {
						$map['filterhackingerror'] = 1;
					}
				}
			} else {
				if ($map['mode'] == 'Normal') {
					$map['AutoCenterZoom'] = $param['AutoCenterZoom'];
					$map['centerLongLat']  = $param['centerLongLat'];
					$map['zoomLevel']      = $param['zoomLevel'];
				}
			}
			$map['zoomInLevel']          = (array_key_exists('zoomInLevel', $param)) ? $param['zoomInLevel'] : '0';
			$map['showMapType']          = (array_key_exists('showMapType', $param)) ? $param['showMapType'] : '0';
			$map['showScale']            = (array_key_exists('showScale', $param)) ? $param['showScale'] : '0';
			$map['showZoomScale']        = (array_key_exists('showZoomScale', $param)) ? $param['showZoomScale'] : '0';
			$map['showZoomLinks']        = (array_key_exists('showZoomLinks', $param)) ? $param['showZoomLinks'] : '0';
			$map['showItemSummaries']    = (array_key_exists('showItemSummaries', $param)) ? $param['showItemSummaries'] : '0';
			$map['showItemDescriptions'] = (array_key_exists('showItemDescriptions', $param)) ? $param['showItemDescriptions'] : '0';
			$map['MapControlType']       = (array_key_exists('MapControlType', $param)) ? $param['MapControlType'] : 'Small';
			$map['MapControlPos']        = (array_key_exists('MapControlPos', $param)) ? $param['MapControlPos'] : '0';
			$map['MapControlPosOffY']    = (array_key_exists('MapControlPosOffY', $param)) ? $param['MapControlPosOffY'] : '30';
			$map['MapControlPosOffX']    = (array_key_exists('MapControlPosOffX', $param)) ? $param['MapControlPosOffX'] : '30';
			$map['defaultalbumcolor']    = (array_key_exists('defaultalbumcolor', $param)) ? $param['defaultalbumcolor'] : '';
			$map['defaultphotocolor']    = (array_key_exists('defaultphotocolor', $param)) ? $param['defaultphotocolor'] : '';
			$map['defaultgroupcolor']    = (array_key_exists('defaultgroupcolor', $param)) ? $param['defaultgroupcolor'] : '';
			$map['GoogleOverview']       = (array_key_exists('GoogleOverview', $param)) ? $param['GoogleOverview'] : false;

			$map['GZoom'] = (array_key_exists('GZoom', $param)) ? $param['GZoom'] : false;

			if ($map['GZoom']) {
				$map['GZPos']     = $param['GZPos'];
				$map['GZPosOffX'] = isset($param['GZPosOffX']) ? $param['GZPosOffX'] : 7;
				$map['GZPosOffY'] = isset($param['GZPosOffY']) ? $param['GZPosOffY'] : 30;
			}

			if ($map['GoogleOverview']) {
				$map['GOSizeX']   = isset($param['GOSizeX']) ? $param['GOSizeX'] : 200;
				$map['GOSizeY']   = isset($param['GOSizeY']) ? $param['GOSizeY'] : 200;
				$map['GOPos']     = isset($param['GOPos']) ? $param['GOPos'] : 3;
				$map['GOType']    = isset($param['GOType']) ? $param['GOType'] : 'satellite';
				$map['GOPosOffX'] = isset($param['GOPosOffX']) ? $param['GOPosOffX'] : 0;
				$map['GOPosOffY'] = isset($param['GOPosOffY']) ? $param['GOPosOffY'] : 0;
			}

			// preload IMG for IE
			$map['preload'] = '';

			// Preload Photo Icons
			list($Sizes, $map['preload']) = GoogleMapUtilities::setPreloadFromFolder($map['preload'], $map['useMarkerSet']);
			// Preload Album Icons
			list($SizesA, $map['preload']) = GoogleMapUtilities::setPreloadFromFolder($map['preload'], $map['useAlbumMarkerSet']);
			// Preload Group Icons
			list($SizesG, $map['preload']) = GoogleMapUtilities::setPreloadFromFolder($map['preload'], $map['useGroupMarkerSet']);

			$map['MarkerSizeX']      = $Sizes[0];
			$map['MarkerSizeY']      = $Sizes[1];
			$map['AlbumMarkerSizeX'] = $SizesA[0];
			$map['AlbumMarkerSizeY'] = $SizesA[1];
			$map['GroupMarkerSizeX'] = $SizesG[0];
			$map['GroupMarkerSizeY'] = $SizesG[1];
			$map['routeitem']        = array();

			$MarkerSets    = array(
				'GalleryPhotoItem' => $map['useMarkerSet'],
				'GalleryAlbumItem' => $map['useAlbumMarkerSet'],
				'GoogleMapGroup'   => $map['useGroupMarkerSet'],
			);
			$DefaultColors = array(
				'GalleryPhotoItem' => $map['defaultphotocolor'],
				'GalleryAlbumItem' => $map['defaultalbumcolor'],
				'GoogleMapGroup'   => $map['defaultgroupcolor'],
			);


			if ((array_key_exists('Routes', $param)) and ($param['Routes'] != '')) {
				list($ret, $map['Routes'], $map['routeitem']) = mapv3Helper::getRoutes($mapPoints, $map['EnableRouteNumber'], $MarkerSets, $DefaultColors);

				if ($ret) {
					return array($ret, null);
				}
			}

			$map['MapWindowType']     = (array_key_exists('MapWindowType', $param)) ? $param['MapWindowType'] : 'default';
			$map['AlbumLegend']       = (array_key_exists('AlbumLegend', $param)) ? $param['AlbumLegend'] : 0;
			$map['PhotoLegend']       = (array_key_exists('PhotoLegend', $param)) ? $param['PhotoLegend'] : 0;
			$map['ExpandAlbumLegend'] = (array_key_exists('ExpandAlbumLegend', $param)) ? $param['ExpandAlbumLegend'] : 0;
			$map['ExpandPhotoLegend'] = (array_key_exists('ExpandPhotoLegend', $param)) ? $param['ExpandPhotoLegend'] : 0;
			$map['AlbumLegends']      = (array_key_exists('AlbumLegends', $param) && ($param['AlbumLegends'] != '')) ? explode('|', $param['AlbumLegends']) : '';
			$map['PhotoLegends']      = (array_key_exists('PhotoLegends', $param) && ($param['PhotoLegends'] != '')) ? explode('|', $param['PhotoLegends']) : '';
			$map['LegendPos']         = (!array_key_exists('LegendPos', $param)) ? 'right' : $param['LegendPos'];
			$map['allmarkers']        = GoogleMapUtilities::AllMarkers();


			/**
			 * @todo replace this with a nice function to get the background color and check
			 * for existence of a sidebar
			 */
			// Test for the existance of a sidebar on the google map view
			$map['sidebar'] = 1;

			$absolutegallerydir = dirname(dirname(dirname(__FILE__)));

			// Grab the Default theme value (used for the google map)
			list($ret, $themeId) = GalleryCoreApi::getPluginParameter('module', 'core', 'default.theme');

			if ($ret) {
				return array($ret->wrap(__FILE__, __LINE__), null);
			}
			// Grab the Theme Default settings
			list($ret, $deftheme) = GalleryCoreApi::loadPlugin('theme', $themeId);

			if ($ret) {
				return array($ret->wrap(__FILE__, __LINE__), null);
			}
			$settings  = $deftheme->getSettings(null);
			$bodycolor = '';

			if (is_array($settings[1])) {
				foreach ($settings[1] as $num => $arrayinfo) {
					if (($settings[1][$num]['key'] == 'colorpack')
						and ($settings[1][$num]['value'] != '')
					) {
						$colorpack    = $settings[1][$num]['value'];
						$filename     = $absolutegallerydir . 'modules/colorpack/packs/'
									 . $colorpack . '/color.css';
						@$filepointer = fopen($filename, 'r');
						$file         = '';

						if ($filepointer) {
							while (!feof($filepointer)) {
								$buffer = fgets($filepointer, 4096);
								$file  .= $buffer;
							}
							fclose($filepointer);
						}
						preg_match('/body.gallery { background-color: (.*) }/', $file, $match);
						$result    = explode(' ', $match[1]);
						$bodycolor = $result[0];
					}
				}

				if ($bodycolor == '') {
					$bodycolor = 'white';
				}
			}
			$map['bodycolor'] = $bodycolor;

			if ($deftheme->getName() == 'Floatrix') {
				$map['sidebar'] = 0;
			}

			if ($deftheme->getName() == 'Ajaxian') {
				$map['WidthFormat'] = 'px';
				$map['mapWidth']    = 650;
				$map['mapHeight']   = $map['mapHeight'] - 20;
			}

			if ($deftheme->getName() == 'Siriux') {
				$map['sidebar']     = 0;
				$map['WidthFormat'] = 'px';
				$map['mapWidth']    = $settings[1][3]['value'];
			}

			if (array_key_exists('2', $settings)) {
				$settings = $settings[2];

				foreach ($settings as $key => $value) {
					if ($key == 'sidebarBlocks') {
						list($success, $sidebarvalue) = $deftheme->unpackSetting('block-list', $value);

						if ($sidebarvalue == '') {
							$map['sidebar'] = 0;
						}
					}
				}
			} elseif (is_array($settings[1])) {
				foreach ($settings[1] as $num => $arrayvalue) {
					if ($arrayvalue['key'] == 'sidebarBlocks') {
						list($success, $sidebarvalue) = $deftheme->unpackSetting(
							'block-list',
							$settings[1][$num]['value']
						);

						if ($sidebarvalue == '') {
							$map['sidebar'] = 0;
						}
					}
				}
			} else {
				$map['sidebar'] = 0;
			}

			if (!array_key_exists('ShowFilters', $param)) {
				$param['ShowFilters'] = '0';
			}
			$map['ShowFilters'] = $param['ShowFilters'];
		}

		// If the "Pick" Mode is required ...
		if ($map['mode'] == 'Pick') {
			// Get the itemId to be able later to populate the coordinated picked out.
			list($form['id'], $map['plugin']) = GalleryUtilities::getRequestVariables('itemId', 'plugin');
			$form['formName']                 = 'ShowMap';

			// Get the thumb to display
			list($ret, $thumb) = GalleryCoreApi::fetchThumbnailsByItemIds(array($form['id']));

			if ($ret) {
				return array($ret, null);
			}

			if (array_key_exists($form['id'], $thumb)) {
				$form['itemthumb'] = $gurl->generateUrl(
					array(
						'view'         => 'core.DownloadItem',
						'itemId'       => $thumb[$form['id']]->getId(),
						'serialNumber' => $thumb[$form['id']]->getSerialNumber(),
					)
				);
			}

			// get data for item edit
			if ($map['plugin'] == 'ItemEdit' or $map['plugin'] == 'GroupCoord' or $map['plugin'] == 'quickEdit') {
				$groupid = GalleryUtilities::getRequestVariables('groupid');

				if (!empty($groupid)) {
					$form['id'] = $groupid;
				}

				// Get coordinates for this item
				$query               = '
		SELECT
		  [mapv3Map::field],
		  [mapv3Map::value]
		FROM
		  [mapv3Map]
		WHERE
		  [mapv3Map::itemId] = \'' . $form['id'] . '\'
		';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) {
					while ($result = $results->nextResult()) {
						if ($result[0] == 'GPS') {
							$map['coordinates'] = $result[1];
						}

						if ($result[0] == 'ZoomLevel') {
							$map['zoomLevel'] = $result[1];
						}

						if ($result[0] == 'Colors') {
							$map['colors'] = $result[1];
						}
					}
				}

				if ($map['plugin'] != 'GroupCoord') {
					list($ret, $item) = $this->_getItem();
					$set              = GalleryUtilities::isA($item, 'GalleryAlbumItem') ? 'album' : ((GalleryUtilities::isA($item, 'GalleryPhotoItem')) ? 'photo' : 'common');
					$map['type']      = $set;
				}
			}

			if ($map['plugin'] == 'AdminCenter' and !$map['AutoCenterZoom']) {
				if (isset($param['centerLongLat'])) {
					$map['centerLongLat'] = $param['centerLongLat'];
				}

				if (isset($param['zoomLevel'])) {
					$map['zoomLevel'] = $param['zoomLevel'];
				}

				$map['colors'] = '';
				$map['type']   = 'common';
			}

			if ($map['plugin'] == 'Filters') {
				$map['filter'] = true;

				$center = GalleryUtilities::getRequestVariables('center');
				$zoom   = GalleryUtilities::getRequestVariables('zoom');

				if (!empty($center)) {
					$map['coordinates'] = $center;
				}

				if (!empty($zoom)) {
					$map['zoomLevel'] = $zoom;
				}
			}


			// make sure parameters are set.
			if (!isset($map['coordinates'])) {
				$map['coordinates'] = (isset($map['GVMCenter']) && $map['GVMCenter'] != '') ? $map['GVMCenter'] : '33,-20';
			}
			$map['centerLongLat'] = $map['coordinates'];

			if (!isset($map['zoomLevel']) or $map['zoomLevel'] == '') {
				$map['zoomLevel'] = (isset($map['GVMZoom']) and $map['GVMZoom'] != '') ? $map['GVMZoom'] : '2';
			}

			if (!isset($map['colors'])) {
				$map['colors'] = '';
			}

			if (!isset($map['type'])) {
				$map['type'] = 'common';
			}

			if (!isset($map['mapType'])) {
				$map['mapType'] = 'G_MAP_TYPE';
			}
			// Feature set transfert
			if (array_key_exists('LegendFeature', $param)) {
				$map['LegendFeature'] = $param['LegendFeature'];
			}

			$template->setVariable('controller', 'mapv3.ShowMap');
		}

		$map['hasadminrights'] = false;
		$ret                   = GalleryCoreApi::assertUserIsSiteAdministrator();

		if (!$ret) {
			$map['hasadminrights'] = true;
		}

		$map['fullScreen'] = $fullScreen;

		$template->setVariable('mapv3', $map);

		// Render the actual template with the map display
		return array(
			null,
			array(
				'body'          => 'modules/mapv3/templates/ShowMap.tpl',
				'useFullScreen' => $fullScreen,
			),
		);
	}
}
