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

GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');

/**
 * @package map
 * @author Eric H Daniel <ehdaniel@gmail.com>
 * @version $Revision: 1576 $
 *
 * This is some of the ugliest code I've ever written. Please do not
 * judge me by it. :)
 *
 * based on some code form CustomField by Alan Harder <alan.harder@sun.com>
 */
class mapv3Callbacks {
	public function callback($params, &$smarty, $callback, $userId = null) {
		global $gallery;
		$platform            = $gallery->getPlatform();
		static $blockNum     = 0;
		list($ret, $module)  = GalleryCoreApi::loadPlugin('module', 'mapv3');

		if ($ret) {
			return $ret->wrap(__FILE__, __LINE__);
		}

		switch ($callback) {
			case 'Loadmaps':
				list($ret, $item) = GalleryCoreApi::loadEntitiesById($params['itemId']);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
				list($ret, $fields) = mapv3Helper::fetchFieldValues(array($item), 'detail');

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}

				return null;

			case 'LoadFilters':
				if (GalleryUtilities::getRequestVariables('view') == 'mapv3.ShowMap') {
					list($ret, $filters) = GalleryCoreApi::getPluginParameter(
						'module',
						'mapv3',
						'Filters'
					);

					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					list($ret, $routes) = GalleryCoreApi::getPluginParameter(
						'module',
						'mapv3',
						'Routes'
					);

					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					$routefilters = array();

					if (!empty($routes)) {
						// we have some routes, check if there is a need to create a filter for them
						$routearray = explode('|', $routes);
						$i          = 0;
						$routeword  = $module->translate('Route') . ':';

						foreach ($routearray as $route) {
							$routeinfos = explode('`', $route);

							if ($routeinfos[6] == 'Yes') {  // yep, need a filter for that route :-)
								$routefilters[$i]['name']   = $routeword . ' ' . $routeinfos[1];
								$routefilters[$i]['params'] = array(
									'view'     => 'mapv3.ShowMap',
									'Group'    => '',
									'filter'   => 'Route: ' . $routeinfos[1],
								);
								$i++;
							}
						}
					}

					list($ret, $showalbumfilters) = GalleryCoreApi::getPluginParameter(
						'module',
						'mapv3',
						'ShowAlbumFilters'
					);

					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					if (!empty($showalbumfilters) and $showalbumfilters == '1') {
						list($ret, $albumlist) = GalleryCoreApi::getPluginParameter(
							'module',
							'mapv3',
							'albumlist'
						);

						if ($ret) {
							return $ret->wrap(__FILE__, __LINE__);
						}

						$albumfilters = array();

						if (!empty($albumlist)) {
							$i            = 0;
							$listofalbums = explode('`', $albumlist);
							$albumword    = $module->translate('Album') . ':';

							foreach ($listofalbums as $album) {
								$albumfilters[$i]['name']   = $albumword . ' ' . $album;
								$albumfilters[$i]['params'] = array(
									'view'     => 'mapv3.ShowMap',
									'Group'    => '',
									'album'    => $album,
								);
								$i++;
							}
						}
					}

					$filteroptions = array();

					if ((!empty($filters) and $filters != '') or !empty($albumfilters)
						or !empty($routefilters)
					) {
						$filteroptions[0]['name']   = $module->translate('Default');
						$filteroptions[0]['params'] = array(
							'view'  => 'mapv3.ShowMap',
							'Group' => '',
						);
					}

					if (!empty($filters) and $filters != '') {
						$separatedfilters = explode('|', $filters);
						$i                = 1;

						foreach ($separatedfilters as $alloptions) {
							$options                     = explode('`', $alloptions);
							$filteroptions[$i]['name']   = $options[1];
							$filteroptions[$i]['params'] = array(
								'view'   => 'mapv3.ShowMap',
								'Group'  => '',
								'filter' => $options[1],
							);
							$i++;
						}
					} elseif (empty($routefilters) and empty($albumfilters)) {
						$filteroptions[0]['name']   = '&laquo;' . $module->translate('No Filters')
						. '&raquo;';
						$filteroptions[0]['params'] = '';
						$filteroptions[1]['name']   = $module->translate('Define...');
						$filteroptions[1]['params'] = array(
							'view'    => 'core.SiteAdmin',
							'subView' => 'mapv3.MapFilterAdmin',
						);
					}

					if (!empty($routefilters)) {
						foreach ($routefilters as $routefilter) {
							// add the Route filters at the end of the filter list
							$filteroptions[] = $routefilter;
						}
					}

					if (!empty($albumfilters)) {
						foreach ($albumfilters as $albumfilter) {
							// add the Album filters at the end of the filter list
							$filteroptions[] = $albumfilter;
						}
					}
					$filter                                    = GalleryUtilities::getRequestVariables('filter');
					$album                                     = GalleryUtilities::getRequestVariables('album');
					$filterOn                                  = ($filter == '' or empty($filter)) ? $albumword . ' ' . $album : $filter;
					$block                                     =& $smarty->_tpl_vars['block'];
					$block['mapv3']['LoadFilters']['filters']  = $filteroptions;
					$block['mapv3']['LoadFilters']['filterOn'] = $filterOn;
				}

				return null;

			case 'ItemCoords':
				if (GalleryUtilities::getRequestVariables('view') != 'mapv3.ShowMap') {
					// Get the item type
					list($ret, $item) = GalleryCoreApi::loadEntitiesById($params['itemId']);

					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					if ($item->getCanContainChildren()) {
						$ItemType = $module->translate('Album');
					} elseif (GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
						$ItemType = $module->translate('Photo');
					} else { // Handle other cases (data items)
						$ItemType = $module->translate('Item');
					}
					// Grab the field data for the item
					list($ret, $data) = mapv3Helper::fetchFieldValues(array($item));

					if ($ret) {
						return array($ret->wrap(__FILE__, __LINE__), null);
					}
					$lat      = array();
					$lng      = array();
					$dir      = array(); // directions
					$dir['N'] = $module->translate(
						array(
							'text' => 'N',
							'hint' => 'North abbreviation',
						)
					);
					$dir['S'] = $module->translate(
						array(
							'text' => 'S',
							'hint' => 'South abbreviation',
						)
					);
					$dir['E'] = $module->translate(
						array(
							'text' => 'E',
							'hint' => 'East abbreviation',
						)
					);
					$dir['W'] = $module->translate(
						array(
							'text' => 'W',
							'hint' => 'West abbreviation',
						)
					);

					if (array_key_exists('GPS', $data[$item->getId()])) {
						$coords = explode(',', $data[$item->getId()]['GPS']);

						switch ($params['coordStyle']) {
							case 1: // Decimal degrees
								$lat['deg'] = sprintf('%.6F', $coords[0]);
								$lng['deg'] = sprintf('%.6F', $coords[1]);

								break;

							case 2: // Degrees decimal minutes
								$lat['deg'] = floor(abs($coords[0]));
								$lat['min'] = sprintf('%.4F', fmod(abs($coords[0]), 1) * 60);
								$lng['deg'] = floor(abs($coords[1]));
								$lng['min'] = sprintf('%.4F', fmod(abs($coords[1]), 1) * 60);

								break;

							case 3: // Degrees minutes seconds
								$lat['deg'] = floor(abs($coords[0]));
								$lat['min'] = floor(fmod(abs($coords[0]), 1) * 60);
								$lat['sec'] = sprintf('%.3F', fmod(fmod(abs($coords[0]), 1) * 60, 1) * 60);
								$lng['deg'] = floor(abs($coords[1]));
								$lng['min'] = floor(fmod(abs($coords[1]), 1) * 60);
								$lng['sec'] = sprintf('%.3F', fmod(fmod(abs($coords[1]), 1) * 60, 1) * 60);

								break;
						}

						if ($params['coordStyle'] == 2 or $params['coordStyle'] == 3) {
							$lat['dir'] = $coords[0] > 0 ? $dir['N'] : $dir['S'];
							$lng['dir'] = $coords[1] > 0 ? $dir['E'] : $dir['W'];
						}
					}
					// Send variable to the template for processing
					$block =& $smarty->_tpl_vars['block'];

					$block['mapv3']['ItemCoords']['ItemType'] = $ItemType;
					$block['mapv3']['ItemCoords']['lat']      = $lat;
					$block['mapv3']['ItemCoords']['lng']      = $lng;
				}

				return null;

			case 'MiniMap':
				if (GalleryUtilities::getRequestVariables('view') != 'mapv3.ShowMap') {
					// Get the item type
					list($ret, $item) = GalleryCoreApi::loadEntitiesById($params['itemId']);

					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					if ($item->getCanContainChildren()) {
						$ItemType = $module->translate('Album');
					} elseif (GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
						$ItemType = $module->translate('Photo');
					} else { // Handle other cases (data items)
						$ItemType = $module->translate('Item');
					}

					// Get all the module information
					list($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'mapv3');

					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					// Get the API key for the current URL
					list($ret, $APIKey) = mapv3Helper::fetchApiKey();

					if ($ret) {
						return $ret;
					}

					// Get the real path for the map module installation
					/** @todo for gallery 2.3 and later change this to use getCodeBasePath() */
					$mapModBaseDir = GalleryCoreApi::getPluginBaseDir('module', 'mapv3') . 'modules/mapv3/';

					// Get the Default MarkerSet & Color
					$DefaultPhotoMarker = 'smallpushpins';
					$DefaultPhotoColor  = 'GREEN';
					$DefaultAlbumMarker = 'bigdots';
					$DefaultAlbumColor  = 'RED';

					if (array_key_exists('useAlbumMarkerSet', $param)
						&& $platform->is_dir(
							$mapModBaseDir . 'images/markers/'
							. $param['useAlbumMarkerSet']
						)
					) {
						$DefaultAlbumMarker = $param['useAlbumMarkerSet'];
					}

					if (array_key_exists('defaultalbumcolor', $param)) {
						$DefaultAlbumColor = $param['defaultalbumcolor'];
					}

					if (array_key_exists('useMarkerSet', $param)
						&& $platform->is_dir(
							$mapModBaseDir . 'images/markers/'
							. $param['useMarkerSet']
						)
					) {
						$DefaultPhotoMarker = $param['useMarkerSet'];
					}

					if (array_key_exists('defaultphotocolor', $param)) {
						$DefaultPhotoColor = $param['defaultphotocolor'];
					}

					// Get the site default zoom level, or default to zoom level 5
					$mapZoom       = (array_key_exists('zoomInLevel', $param)) ? $param['zoomInLevel'] : 5;
					$mapCenter     = ''; // Default blank mapCenter
					$markerItemIds = array();

					// If it's an album, check settings and possibly fetch descendants
					if ($item->getCanContainChildren()) {
						/*
						 * Note: Both fetchChildItemIds() and fetchDescendentItemIds() by default
						 * retrieve only items for which the current user has 'core.view' permission.
						 */
						if ($params['albumItems'] == 2) { // show items in current level only
							list($ret, $markerItemIds) = GalleryCoreApi::fetchChildItemIds($item);
						} elseif ($params['albumItems'] == 3) { // show all subitems
							list($ret, $markerItemIds) = GalleryCoreApi::fetchDescendentItemIds($item);
						}
						// Get the site default zoom level
						if (array_key_exists('zoomInLevel', $param)) {
							$mapZoom = $param['zoomInLevel'];
						}
					}
					// Add parent sequence to items to fetch from database.
					if ($params['useParentCoords']) {
						list($ret, $itemParents)  = GalleryCoreApi::fetchParentSequence($item->getId());
						$markerItemIds            = array_merge($markerItemIds, $itemParents);
					}
					/*
					 * Now add the main item to the markerItemIds array so it will grab its data.
					 * The item may be removed from the markerParams later as block settings dictate.
					 */
					$markerItemIds[] = $params['itemId'];

					/** @todo Add a mapv3Helper function for this */
					// Get the Item data
					$query = '
		SELECT
		  [mapv3Map::itemId],
		  [mapv3Map::field],
		  [mapv3Map::value]
		FROM
		  [mapv3Map]
		WHERE
		  [mapv3Map::field] IN ( \'GPS\', \'ZoomLevel\', \'Color\' )
		  AND
		  [mapv3Map::itemId] IN ('
					. GalleryUtilities::makeMarkers(count($markerItemIds)) . ')
		';

					list($ret, $results) = $gallery->search($query, $markerItemIds, array());
					// $results is a series of rows with columns: itemId, field, value
					if ($ret) {
						return $ret->wrap(__FILE__, __LINE__);
					}

					$markerParams = array();
					$itemCoords   = null;

					// Convert rows to array tree keyed by itemId
					while ($myResults = $results->nextResult()) {
						$markerParams[$myResults[0]][$myResults[1]] = $myResults[2];
						// extract the map center and zoom if we encounter them
						if ($myResults[0] == $params['itemId']) {
							if ($myResults[1] == 'GPS') {
								$mapCenter  = $myResults[2];
								$itemCoords = $myResults[2];
							} elseif ($myResults[1] == 'ZoomLevel') {
								$mapZoom = $myResults[2];
							}
						}
					}

					// Fall back to parent album coordinates if item has no coordinates set
					if ($params['useParentCoords']) {
						foreach (array_reverse($itemParents) as $parentId) {
							if (empty($mapCenter) && array_key_exists($parentId, $markerParams)
								&& array_key_exists('GPS', $markerParams[$parentId])
								&& true
							) { // replace && true with coordinate validation function
								$mapCenter = $markerParams[$parentId]['GPS'];
								$mapZoom   = (array_key_exists('ZoomLevel', $markerParams[$parentId])) ? $markerParams[$parentId]['ZoomLevel'] : $mapZoom;
							}
							// Now that we have the data, remove the parent items
							unset($markerParams[$parentId]);
						}
					}

					if ($item->getCanContainChildren() && !$params['albumMarker']) {
						// Don't show a marker for the album
						unset($markerParams[$params['itemId']]);
					}

					// Load the items which have parameters and determine which are albums, etc.
					if (!empty($markerParams)) {
						list($ret, $markerItems) = GalleryCoreApi::loadEntitiesById(array_keys($markerParams));
					} else {
						$markerItems = array();
					}

					// Get the gallery URL generator to properly create links to images
					$gurl = &$gallery->getUrlGenerator();

					// The smarty markup modifier is required to strip out markup code in titles
					GalleryCoreApi::requireOnce('lib/smarty_plugins/modifier.markup.php');

					$markerIcons = array();

					foreach ($markerItems as $markerItem) {
						// First make sure it has coordinates and that they're valid
						/** @todo replace '&& true' with a new MapHelper function to validate coordinates */
						if (!(array_key_exists('GPS', $markerParams[$markerItem->getId()]) && true)) {
							// No coordinates; don't show this marker.
							unset($markerParams[$markerItem->getId()]);

							continue;
						}
						$ItemColor  = (array_key_exists('Color', $markerParams[$markerItem->getId()])) ? $markerParams[$markerItem->getId()]['Color'] : (($markerItem->getCanContainChildren()) ? $DefaultAlbumColor : $DefaultPhotoColor);
						$ItemMarker = ($markerItem->getCanContainChildren()) ? $DefaultAlbumMarker : $DefaultPhotoMarker;

						if (!array_key_exists("{$ItemMarker}_{$ItemColor}", $markerIcons)) {
							$markerIcons["{$ItemMarker}_{$ItemColor}"]           = array();
							$markerIcons["{$ItemMarker}_{$ItemColor}"]['imgUrl'] = $gurl->generateUrl(
								array(
									'href' => 'modules/mapv3/images/markers/'
									. $ItemMarker . '/marker_' . $ItemColor . '.png',
								)
							);
							/** @todo Each icon type should have it's own shadow */
							$markerIcons["{$ItemMarker}_{$ItemColor}"]['shadowUrl'] = $gurl->generateUrl(
								array(
									'href' => 'modules/mapv3/images/marker_shadow.png',
								)
							);

							/** @todo Figure out error handling if image file doesn't exist */
							// Get the Marker Size
							$imageSizes = $platform->getimagesize(
								$mapModBaseDir . 'images/markers/'
								. $ItemMarker . '/marker_' . $ItemColor . '.png'
							);
							$markerIcons["{$ItemMarker}_{$ItemColor}"]['width']  = $imageSizes[0];
							$markerIcons["{$ItemMarker}_{$ItemColor}"]['height'] = $imageSizes[1];
						}
						// Now associate the marker icon to the marker itself (allows for sharing)
						$markerParams[$markerItem->getId()]['icon']  = "{$ItemMarker}_{$ItemColor}";
						$markerParams[$markerItem->getId()]['title'] = html_entity_decode(
							smarty_modifier_markup($markerItem->getTitle(), 'strip')
						);
					}

					// Send variable to the template for processing
					$block =& $smarty->_tpl_vars['block'];

					/*
					 * Append a serial number to the div id, otherwise only one map will be
					 * displayed if the block is used in more than one spot on the page (i.e., sidebar
					 * and photo)
					 */
					$block['mapv3']['MiniMap']['blockNum']    = ++$blockNum;
					$block['mapv3']['MiniMap']['ItemType']    = $ItemType;
					$block['mapv3']['MiniMap']['APIKey']      = $APIKey;
					$block['mapv3']['MiniMap']['mapCenter']   = $mapCenter;
					$block['mapv3']['MiniMap']['mapZoom']     = $mapZoom;
					$block['mapv3']['MiniMap']['markerIcons'] = $markerIcons;
					$block['mapv3']['MiniMap']['markers']     = $markerParams;
				}

				return null;
		}

		return GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__);
	}
}
