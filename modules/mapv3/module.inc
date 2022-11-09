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
 * @package mapv3
 * @author Steven N. Severinghaus <sns@severinghaus.org>
 * @author Orestes Sanchez <miceno.atreides@gmail.com>
 * @version
 *
 *
 *//**
 * Map
 *
 * This module provides Google Map for Gallery items.
 *
 * @package map
 */
class mapv3Module extends GalleryModule {
	private static $_CONVERSION_MATRIX = array(
		'ThumbBarPos' => array(
			0 => 'hidden',
			1 => 'top',
			2 => 'bottom',
			3 => 'right',
			4 => 'left',
		),
	);

	public function __construct() {
		global $gallery;
		$this->setId('mapv3');
		$this->setName($gallery->i18n('Google Maps API v3 Module'));
		$this->setDescription($gallery->i18n('Show items on a dynamic Google Map'));
		$this->setVersion('0.6.1');
		$this->_templateVersion = 1;
		$this->setGroup('gallery', $gallery->i18n('Gallery'));
		$this->setCallbacks(
			'registerEventListeners|getSiteAdminViews|getItemSummaries'
				. '|getItemLinks'
		);
		$this->setRequiredCoreApi(array(7, 54));
		$this->setRequiredModuleApi(array(3, 0));
	}

	/**
	 * @see GalleryModule::registerEventListeners()
	 */
	public function registerEventListeners() {
		GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
		$mapv_3_helper = new mapv3Helper();
		GalleryCoreApi::registerEventListener('GalleryEntity::delete', $mapv_3_helper);
		GalleryCoreApi::registerEventListener('GalleryEntity::save', $mapv_3_helper);
	}

	public function needsConfiguration() {
		GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
		// This module requires a Google Map API key.
		list($ret, $profiles) = mapv3Helper::getAllKeyProfiles();

		if ($ret) {
			return array($ret, null);
		}

		return array(null, empty($profiles));
	}

	/**
	 * @see GalleryModule::autoConfigure
	 */
	public function autoConfigure() {
		list($ret, $needsConfiguration) = $this->needsConfiguration();

		if ($ret) {
			return array($ret, false);
		}

		if (!$needsConfiguration) {
			return array(null, true);
		}

		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'mapWidth', 600);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'mapHeight', 400);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'centerLongLat', '0,0');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'zoomLevel', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'zoomInLevel', 5);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'mapType', 'roadmap');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'showZoomScale', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'showMapType', 1);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'showZoomLinks', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'showItemSummaries', 1);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'showItemDescriptions', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'regroupAlbums', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'regroupItems', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'regroupDist', 15);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'regroupIcon', 'zoom-in-trans');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'useMarkerSet', 'smallpushpins');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'useAlbumMarkerSet', 'smallpushpins');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'useGroupMarkerSet', 'smallpushpins');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'defaultphotocolor', 'GREEN');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'defaultalbumcolor', 'YELLOW');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'defaultgroupcolor', 'RED');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'useParentColor', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'GxMagnifier', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'GxMagnifier2', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapControlType', 'Small');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapControlPos', '0');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapControlPosOffX', '10');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapControlPosOffY', '15');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'AutoCenterZoom', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ShowExportGELink', 1);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ExpandAlbumLegend', 1);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ExpandPhotoLegend', 1);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ShowFilters', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'LegendPos', 'right');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'GxMagnifierFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ThemeFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MarkerFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'LegendFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'FilterFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'GroupFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'RouteFeature', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ThumbBarPos', 'hidden');
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'ThumbHeight', 60);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'linktype', 0);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'AdminHelp', 1);
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'UserHelp', 1);
		list($ret, $exifModule) = GalleryCoreApi::loadPlugin('module', 'exif', false);

		if ($ret) {
			return array(null, false);
		}

		if (!version_compare('1.0.1', $exifModule->getVersion(), '<=')) {
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'oldexifversion', 1);
		} else {
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'oldexifversion', 0);
		}

		list($ret, $imagemagickModule) = GalleryCoreApi::loadPlugin('module', 'imagemagick', false);

		if ($ret) {
			return array(null, false);
		}

		if (empty($imagemagickModule)) {
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'noimagemagick', 1);
		} else {
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'noimagemagick', 0);
		}

		return array(null, false);
	}

	public function activate($postActivationEvent = true) {
		// Register the Map Edit Permission, no error checking
		$ret = GalleryCoreApi::registerPermission(
			$this->getId(),
			'mapv3.EditMap',
			$this->_translate('[Google Map] Allow Coordinate Editing')
		);

		// By default admin can edit everything
		list($ret, $rootalbum)  = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
		list($ret, $siteadmins) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.adminGroup');
		$ret                    = GalleryCoreApi::addGroupPermission($rootalbum, $siteadmins, 'mapv3.EditMap', true);

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null);
		}

		list($ret, $redirect) = parent::activate($postActivationEvent);

		if ($ret) {
			return array($ret, null);
		}

		return array(null, $redirect);
	}

	public function getConfigurationView() {
		return 'mapv3.MapSiteAdmin';
	}

	/**
	 * @see GalleryModule::getSiteAdminViews()
	 */
	public function getSiteAdminViews() {
		global $gallery;

		return array(
			null,
			array(
				array(
					'name' => $this->_translate('Google Map v3'),
					'view' => 'mapv3.MapSiteAdmin',
				),
			),
		);
	}

	/**
	 * @see GalleryModule::getItemSummaries()
	 */
	public function getItemSummaries($items, $permissions, &$template) {
		GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
		GalleryCoreApi::requireOnce('lib/smarty_plugins/modifier.markup.php');
		$summaries = $data = array();

		list($ret, $data) = mapv3Helper::fetchFieldValues($items, 'summary');

		if ($ret) {
			return array($ret, null);
		}

		foreach ($data as $itemId => $fields) {
			$list = array();

			foreach ($fields as $field => $value) {
				$list[] = $field . ': ' . smarty_modifier_markup($value);
			}

			if (!empty($list)) {
				$summaries[$itemId] = implode("<br/>\n", $list);
			}
		}

		return array(null, $summaries);
	}

	/**
	 * @see GalleryModule::getRewriteRules()
	 */
	public function getRewriteRules() {
		global $gallery;
		$rules = array();

		$rule             = array();
		$rule['comment']  = 'View Google Map Group';
		$rule['match']    = array(
			'view' => 'mapv3.ShowMap',
		);
		$rule['pattern']  = 'Map/%Group%';
		$rule['keywords'] = array(
			'Group' => array(
				'pattern' => '(G{1}[0-9]+)*',
				'help'    => $this->_translate(
					'The ID of the group to show, '
					. 'No group will show the default Map'
				),
			),
		);
		$rules[0]         = $rule;

		return $rules;
	}

	/**
	 * @see GalleryModule::performFactoryRegistrations()
	 */
	public function performFactoryRegistrations() {
		// Register our item edit plugin
		$ret = GalleryCoreApi::registerFactoryImplementation(
			'ItemEditPlugin',
			'mapItemEdit',
			'mapItemEdit',
			'modules/mapv3/mapItemEdit.inc',
			'mapv3',
			null
		);

		if ($ret) {
			return $ret;
		}

		// Register interface implementation
		$ret = GalleryCoreApi::registerFactoryImplementation(
			'mapv3Interface_1_0',
			'mapv3Helper',
			'mapv3',
			'modules/mapv3/classes/mapv3Helper.class',
			'mapv3',
			null
		);

		if ($ret) {
			return $ret;
		}

		// Register search implementation
		$ret = GalleryCoreApi::registerFactoryImplementation(
			'GallerySearchInterface_1_0',
			'mapv3Search',
			'mapv3',
			'modules/mapv3/classes/mapv3Search.class',
			'mapv3',
			null
		);

		if ($ret) {
			return $ret;
		}

		$ret = GalleryCoreApi::registerFactoryImplementation(
			'MaintenanceTask',
			'PopulateGPSEXIFInfos',
			'PopulateGPSEXIFInfos',
			'modules/mapv3/classes/PopulateGPSEXIFInfos.class',
			'mapv3',
			null
		);

		if ($ret) {
			return $ret;
		}

		$ret = GalleryCoreApi::registerFactoryImplementation(
			'MaintenanceTask',
			'WriteExifHeaderToPictures',
			'WriteExifHeaderToPictures',
			'modules/mapv3/classes/WriteExifHeaderToPictures.class',
			'mapv3',
			null
		);

		if ($ret) {
			return $ret;
		}

		return null;
	}

	/**
	 * @see GalleryModule::getItemLinks()
	 */
	public function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId = null) {
		GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
		$links = array();

		list($ret, $linktype) = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'linktype');

		if (empty($linktype)) {
			$linktype = 0;
		}
		list($ret, $ExportGELink) = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'ShowExportGELink');

		foreach ($items as $item) {
			$itemId = $item->getId();
			$type   = $item->getEntityType();

			if ($linktype != 3) {
				if ($type == 'GalleryPhotoItem') {
					$NitemId = $item->getParentId();
				} else {
					$NitemId = $itemId;
				}
				list($ret, $item) = GalleryCoreApi::loadEntitiesById($NitemId);
				$name             = $item->getTitle();

				$goodalbum = false;

				if ($linktype == 2) {
					$goodalbum = true; // Always filter on current album
				}

				if ($linktype == 1) {
					// Dynamic link -> search for item in the album/subalbum with GPS coords
					list($ret, $count) = mapv3Helper::getCountItemWithCoords($item->getId());

					if ($ret) {
						return array($ret, null);
					}

					if ($count != 0) {
						$goodalbum = true;
					}
				}

				if (isset($wantsDetailedLinks[$itemId])) {
					// The View in Google Earth Link (only if enabled in the Panel)
					if ($ExportGELink) {
						$links[$itemId][] = array(
							'text'   => $this->_translate('View in Google Earth'),
							'params' => array(
								'view'   => 'mapv3.GoogleEarth',
								'itemId' => $itemId,
							),
						);
					}
					// The Google Map link
					if ($goodalbum) {
						$links[$itemId][] = array(
							'text'   => $this->_translate('View Album on a Map'),
							'params' => array(
								'view'  => 'mapv3.ShowMap',
								'Mode'  => 'Normal',
								'Group' => '',
								'album' => $name,
							),
						);
					} else {
						/*
						 * The current album (or parent of the current photo) does not have any
						 * GPS item, check from the root
						 */
						list($ret, $count) = mapv3Helper::getCountItemWithCoords();

						if ($ret) {
							return array($ret, null);
						}

						if ($count) {
							// There are some items to show in the Gallery Display the Map Link
							$links[$itemId][] = array(
								'text'   => $this->_translate('Show a Google Map'),
								'params' => array(
									'view' => 'mapv3.ShowMap',
								),
							);
						}
					}
				}
			}
		}

		return array(null, $links);
	}

	public function upgrade($currentVersion, $x1 = null) {
		global $gallery;
		$storage =& $gallery->getStorage();

		list($ret, $exifModule) = GalleryCoreApi::loadPlugin('module', 'exif', false);

		if ($ret) {
			/**
			 * @todo This doesn't seem right. The upgrade shouldn't bail out just because this
			 * plugin couldn't be loaded. It's not required for core functionality of this module.
			 */
			return $ret;
		}
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'oldexifversion', 0);

		if (!version_compare('1.0.1', $exifModule->getVersion(), '<=')) {
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'oldexifversion', 1);
		}

		list($ret, $imagemagickModule) = GalleryCoreApi::loadPlugin('module', 'imagemagick', false);

		if ($ret) {
			/*
			 * @todo This doesn't seem right. The upgrade shouldn't bail out just because this
			 * plugin couldn't be loaded. It's not required for core functionality of this module.
			 */
			return $ret;
		}
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'noimagemagick', 0);



		if (!isset($currentVersion)) {
			$currentVersion = '0';
		}

		switch ($currentVersion) {
			case '0':
				// Initial install
				break;

			case '0.1.0':
			case '0.1.1':
			case '0.1.2':
			case '0.1.3':
			case '0.2':
			case '0.3beta1':
			case '0.3.0':
			case '0.3.1':
			case '0.3.1b':
			case '0.3.2':
			case '0.3.2b':
				// Upgrade from when we were using CustomField

				// get CustomfieldMap table fields related to Map
				$query               = '
	    SELECT *
	    FROM
	      [GalleryCustomFieldMap]
	    WHERE
	      [GalleryCustomFieldMap::field] = \'GPS\'
	      OR
	      [GalleryCustomFieldMap::field] = \'Colors\'
	      OR
              [GalleryCustomFieldMap::field] = \'Color\'
	      OR
	      [GalleryCustomFieldMap::field] = \'ZoomLevel\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if ($result[1] == 'Color') {
							$result[1] = 'Colors'; // Replace the "color" field for "colors"
						}

						$insertquery = '
		    INSERT INTO
		    [mapv3Map]
		    VALUES (\'' . $result[0] . '\',\'' . $result[1] . '\',\'' . $result[2]
									   . '\',\'' . $result[3] . '\',\'' . $result[4] . '\')
		    ';

						$ret = $storage->execute($insertquery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// Fall Through
			case '0.4.0':
			case '0.4.1':
			case '0.4.2':
			case '0.4.2b':
			case '0.4.2c':
			case '0.4.3':
			case '0.4.4':
			case '0.4.4b':
			case '0.4.4c':
			case '0.4.4e':
				// Upgrade from before the routes were able to be filters
				// Get the Routes field
				$routes    = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Routes');
				$newroutes = array();

				// Update with new parameters set to no
				if (!empty($routes[1])) {  // There is some routes, upgrading
					$allroutes = explode('|', $routes[1]);

					foreach ($allroutes as $route) {
						$usingnewroutes = strpos($route, '`Yes`No')
						    ? 1
						    : (0 + strpos($route, '`Yes`Yes') ? 1 : (0 + strpos($route, '`No`Yes') ? 1 : (0 + strpos($route, '`No`No') ? 1 : 0)));
						$posyes         = strpos($route, '`Yes');
						$posno          = strpos($route, '`No');

						if ($posyes and $usingnewroutes == 0) {
							$newroutes[] = substr($route, 0, $posyes + 4) . '`No'
										   . substr($route, $posyes + 4) . '<br>';
						} elseif ($posno and $usingnewroutes == 0) {
							$newroutes[] = substr($route, 0, $posno + 3) . '`No'
										   . substr($route, $posno + 3) . '<br>';
						}
					}

					if ($newroutes != '1') {
						$theroutes = implode('|', $newroutes);
						// Set the new routes in the database
						GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Routes', $theroutes);
					}
				}

				// copy markers to the module directory (we don't care if that fails)
				/**
				 * @todo The substr() method is fragile. Change to use
				 * GalleryCoreApi::getPluginBaseDir() for now, then getCodeBasePath() for Gallery 2.3+
				 */
				//	    $basegallerydir = substr(dirname(__FILE__), 0, -11);
				$basegallerydir = GalleryCoreApi::getCodeBasePath();
				/**
				 * @todo This is the only place CopyDir is used. If we can move that functionality
				 * here, we can eliminate filecopy.class altogether.
				 */
				GalleryCoreApi::requireOnce('modules/mapv3/classes/filecopy.class');
				@CopyDir(
					$basegallerydir . '../images/map',
					$basegallerydir . 'mapv3/images/markers',
					0777,
					false
				);
				@rm($basegallerydir . 'images/map');

				// Upgrade routes to be pointing to Item instead of coordinates of items
				// Get current routes
				list($ret, $routes) = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Routes');
				$newroutes          = '';

				if (!empty($routes)) { // there is some route, fix them :-)
					$allroutes = explode('|', $routes);

					foreach ($allroutes as $route) { // go through each route
						$routeinfos = explode('`', $route);
						$theroute   = $routeinfos[0] . '`' . $routeinfos[1] . '`' . $routeinfos[2]
									  . '`' . $routeinfos[3] . '`' . $routeinfos[4] . '`' . $routeinfos[5]
									  . '`' . $routeinfos[6];

						for ($i = 7; $i < count($routeinfos); $i++) {
							// Search for the itemid from the coordinate and replace
							$query               = '
			SELECT
			  [mapv3Map::itemId]
			FROM
			  [mapv3Map]
			WHERE
			  [mapv3Map::value] = \'' . $routeinfos[$i] . '\'
			';
							list($ret, $results) = $storage->search($query);

							if ($results->resultCount > 0) {
								$result    = $results->nextResult();
								$theroute .= '`' . $result[0];
							} else {
								$theroute .= '`' . $routeinfos[$i];
							}
						}

						if ($newroutes == '') {
							$newroutes = $theroute;
						} else {
							$newroutes .= '|' . $theroute;
						}
					}
					// Replace route with new infos
					GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Routes', $newroutes);
				}
				// Fall Through
			case '0.4.5':
			case '0.4.5c':
			case '0.4.5d':
			case '0.4.6':
			case '0.4.6b':
			case '0.4.6e':
			case '0.4.7c':
			case '0.4.7d':
				// attempt to change the database itemId from int to varchar

				$query = '
	    ALTER TABLE
	      [mapv3Map]
	    CHANGE
	      [mapv3Map::itemId]
	      [mapv3Map::itemId] VARCHAR(11) DEFAULT \'0\' NOT NULL
	    ';

				$ret = $storage->execute($query); // run the command
				if ($ret) {
					return $ret;
				}
				// Fall Through
			case '0.4.8':
			case '0.4.8b':
			case '0.4.8c':
			case '0.4.8d':
			case '0.4.8e':
			case '0.4.9':
			case '0.4.9b':
			case '0.4.9c':
			case '0.4.10':
			case '0.4.10b':
			case '0.4.10c':
			case '0.4.10d':
			case '0.4.10e':
			case '0.4.10f':
			case '0.4.11a':
			case '0.4.11b':
			case '0.4.11c':
			case '0.4.11d':
				$ret = $storage->configureStore($this->getId(), array('mapv3Map:1.2'));

				if ($ret) {
					return $ret;
				}
				// Fall Through
			case '0.5.0':
			case '0.5.0b':
			case '0.5.0c':
			case '0.5.0d':
			case '0.5.0e':
			case '0.5.0f':
			case '0.5.0g':
				// Reverse the existing 'GPS' coordinates in the DB
				$query               = '
	    SELECT *
	    FROM
	      [mapv3Map]
	    WHERE
	      [mapv3Map::field] = \'GPS\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[2])) {
							$tempcoord = explode(',', $result[2]);
							$result[2] = "$tempcoord[1],$tempcoord[0]";
						}

						$updatequery = '
		    UPDATE
		      [mapv3Map]
		    SET
		      [mapv3Map::value] = \'' . $result[2] . '\'
		    WHERE
		      [mapv3Map::itemId] = \'' . $result[0] . '\'
		      AND
		      [mapv3Map::field] = \'' . $result[1] . '\'
		      AND
		      [mapv3Map::setId] = \'' . $result[3] . '\'
		      AND
		      [mapv3Map::setType] = \'' . $result[4] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End reversing GPS coordinates in DB

				// Update map type
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND
	      [GalleryPluginParameterMap::parameterName] = \'mapType\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) {
							if ($result[4] == 'G_MAP_TYPE') {
								$result[4] = 'roadmap';
							}

							if ($result[4] == 'satellite') {
								$result[4] = 'satellite';
							}

							if ($result[4] == 'G_HYBRID_TYPE') {
								$result[4] = 'hybrid';
							}
						}

						$updatequery = '
		    UPDATE
		      [PluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End updating the Map Type in the DB

				// Reverse the Map Center parameter
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND
	      [GalleryPluginParameterMap::parameterName] = \'centerLongLat\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) {
							$tempcoord = explode(',', $result[4]);
							$result[4] = "$tempcoord[1],$tempcoord[0]";
						}

						$updatequery = '
		    UPDATE
		      [GalleryPluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End reversing the coordinates in the Map Center Paramater

				// Reverse the Default Map Center parameter
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND [GalleryPluginParameterMap::parameterName] = \'GVMCenter\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) {
							$tempcoord = explode(',', $result[4]);
							$result[4] = "$tempcoord[1],$tempcoord[0]";
						}

						$updatequery = '
		    UPDATE
		      [GalleryPluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End reversing the coordinates in the Default Map Center Paramater

				// Updating the Zoom Level
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND
	      [GalleryPluginParameterMap::parameterName] = \'zoomLevel\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) {
							$result[4] = 17 - $result[4];
						}

						$updatequery = '
		    UPDATE
		      [GalleryPluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End updating the Map Zoom

				// Updating the Default Zoom Level
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND
	      [GalleryPluginParameterMap::parameterName] = \'GVMZoom\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) {
							$result[4] = 17 - $result[4];
						}

						$updatequery = '
		    UPDATE
		      [GalleryPluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End updating the Default Map Zoom

				// Updating the Default ZoomIn Level
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND
	      [GalleryPluginParameterMap::parameterName] = \'zoomInLevel\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) {
							$result[4] = 17 - $result[4];
						}

						$updatequery = '
		    UPDATE
		      [GalleryPluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End updating the Default Map ZoomIn

				// Updating the Manual Filter information
				$query               = '
	    SELECT *
	    FROM
	      [GalleryPluginParameterMap]
	    WHERE
	      [GalleryPluginParameterMap::pluginType] = \'module\'
	      AND
	      [GalleryPluginParameterMap::pluginId] = \'map\'
	      AND
	      [GalleryPluginParameterMap::parameterName] = \'Filters\'
	    ';
				list($ret, $results) = $gallery->search($query, array(), array());

				if ($results->resultCount() > 0) { // Verify that it isn't empty
					while ($result = $results->nextResult()) {
						if (!empty($result[4])) { // we do have some filters, start the upgrade
							$filterarray = explode('|', $result[4]);

							foreach ($filterarray as $key => $filterinfos) {
								$infos      = explode('`', $filterinfos);
								$tempcenter = explode(',', $infos[3]);
								// invert the coordinates for this filter
								$infos[3] = $tempcenter[1] . ',' . $tempcenter[0];
								$infos[2] = 17 - $infos[2];  // change the zoom for this filter
								// put the new infos back into the original array
								$filterarray[$key] = implode('`', $infos);
							}
							// put all the fixed filter into the result
							$result[4] = implode('|', $filterarray);
						}

						$updatequery = '
		    UPDATE
		      [GalleryPluginParameterMap]
		    SET
		      [GalleryPluginParameterMap::parameterValue] = \'' . $result[4] . '\'
		    WHERE
		      [GalleryPluginParameterMap::pluginType] = \'' . $result[0] . '\'
		      AND
		      [GalleryPluginParameterMap::pluginId] = \'' . $result[1] . '\'
		      AND
		      [GalleryPluginParameterMap::itemId] = \'' . $result[2] . '\'
		      AND
		      [GalleryPluginParameterMap::parameterName] = \'' . $result[3] . '\'
		    ';

						$ret = $storage->execute($updatequery); // put the values in Mapmap
						if ($ret) {
							return $ret;
						}
					}
				}
				// End updating the Manual Filter Information

				// Fall Through
			case '0.5.1':
			case '0.5.1a':
			case '0.5.1b':
			case '0.5.1c':
			case '0.5.1d':
			case '0.5.2':
			case '0.5.2a':
				// Get Information for the "Colors", "AlbumColor" and "PhotoColor" fields and move all of it to "Color"
				$updatequery = '
	    UPDATE
	      [mapv3Map]
	    SET
	      [mapv3Map::field] = \'Color\'
	    WHERE
	      [mapv3Map::field] = \'Colors\'
	      OR
	      [mapv3Map::field] = \'AlbumColor\'
	      OR
	      [mapv3Map::field] = \'PhotoColor\'
	    ';
				$ret         = $storage->execute($updatequery); // put the values in Mapmap
				if ($ret) {
					return $ret;
				}
				// Fall Through
			case '0.5.3':
			case '0.5.4':
			case '0.5.5':
			case '0.5.6':
				// Change stored map keys to new style
				$newKeys             = array();
				list($ret, $mapKeys) = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapKeys');
				/* Format of old keys as serialized in the database:
				 * 1`P1Name`P1Host`P1Port`P1ApiKey|2`P2Name`P2Host`P2Port`P2ApiKey
				 * Format of new keys serialized in the database:
				 * array(
				 *   array('name' => P1Name,
				 *         'url' => http://P1Host:P1Port, (or https where port=443)
				 *         'apiKey' => P1ApiKey),
				 *   array('name' => P2Name,
				 *         'url' => http://P2Host:P2Port, (or https where port=443)
				 *         'apiKey' => P2ApiKey)
				 * )
				 */
				if (empty($ret)) {
					$allkeysets = explode('|', $mapKeys);

					foreach ($allkeysets as $keyset) {
						$temp       = explode('`', $keyset);
						$protocol   = ($temp[3] == '443') ? 'https://' : 'http://';
						$portSuffix = ($temp[3] == '443' || $temp[3] == '80') ? '' : ':' . $temp[3];
						$url        = $protocol . $temp[2] . $portSuffix;
						$newKeys[]  = array(
							'name'   => $temp[1],
							'url'    => $url,
							'apiKey' => $temp[4],
						);
					}
					// remove it first, otherwise mysql will throw a name collision, even though
					// the case is different
					GalleryCoreApi::removePluginParameter('module', 'mapv3', 'MapKeys');
					$ret = GalleryCoreApi::setPluginParameter(
						'module',
						'mapv3',
						'mapKeys',
						serialize($newKeys)
					);

					if ($ret) {
						// Attempt to put the old value back
						GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapKeys', $mapKeys);

						return $ret;
					}
					GalleryCoreApi::removePluginParameter('module', 'mapv3', 'EditKey');
				}
				// Fall Through
			case '0.6':
				// Change names of plugin parameters from numbers to text to better read them.
				list($ret, $thumbBarPosition) = GalleryCoreApi::getPluginParameter(
					'module',
					'mapv3',
					'ThumbBarPos'
				);

				if (is_numeric($thumbBarPosition)) {
					$thumbBarPosition = mapv3Module::paramNumberToText('ThumbBarPos', $thumbBarPosition);
					$ret              = GalleryCoreApi::setPluginParameter(
						'module',
						'mapv3',
						'ThumbBarPos',
						$thumbBarPosition
					);

					if ($ret) {
						return $ret;
					}
				}

				// Fall Through
			case 'end of upgrade path':
				/*
				 * Leave this bogus case at the end of the legitimate case statements so that we always
				 * properly terminate our upgrade path with a break
				 */
				break;
		}

		return null;
	}

	/**
	 * Returns the new value of a parameter.
	 *
	 * @param $paramName
	 * @param $oldValue
	 *
	 * @static
	 */
	private static function paramNumberToText($paramName, $oldValue) {
		if (array_key_exists($paramName, mapv3Module::$_CONVERSION_MATRIX)) {
			$c = mapv3Module::$_CONVERSION_MATRIX[$paramName];

			if (array_key_exists($oldValue, $c)) {
				return $c[$oldValue];
			}
		}

		return null;
	}
}
