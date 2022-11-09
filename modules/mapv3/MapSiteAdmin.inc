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
 * @version $Revision: 1576 $
 *
 * This is some of the ugliest code I've ever written. Please do not
 * judge me by it. :)
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');

/**
 * @package map
 * @subpackage UserInterface
 */
class MapSiteAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		$status = array();

		// Cancel hit, return to the default Admin Page
		if (isset($form['action']['cancel'])) {
			$results['status']   = $status;
			$results['error']    = array();
			$results['redirect'] = array(
				'view'                 => 'core.SiteAdmin',
				'subView'              => 'mapv3.MapSiteAdmin',
				'mode'                 => 'General',
			);
		}

		// Save general settings
		if (isset($form['action']['save'])) {
			// Check for errors
			if (!is_numeric($form['mapWidth']) || intval($form['mapWidth']) != $form['mapWidth']) {
				$error[] = 'form[error][mapWidth]';
			} elseif (!is_numeric($form['mapHeight'])
				|| intval($form['mapHeight']) != $form['mapHeight']
			) {
				$error[] = 'form[error][mapHeight]';
			} elseif (!isset($form['AutoCenterZoom']) && (empty($form['centerLongLat'])
				|| !preg_match(
					'/^([-+]?(([0-9]{1,3}(\.[0-9]{0,16})?)|(\.[0-9]{1,16}))(\040)*,'
					. '(\040)*[-+]?(([0-9]{1,3}(\.[0-9]{0,16})?)|(\.[0-9]{1,16})))?$/',
					$form['centerLongLat']
				))
			) {
				$error[] = 'form[error][centerLongLat]';
			} elseif (!isset($form['AutoCenterZoom'])
				&& (!is_numeric($form['zoomLevel'])
				|| (intval($form['zoomLevel']) != $form['zoomLevel'])
				|| ($form['zoomLevel'] < 0) || ($form['zoomLevel'] > 19))
			) {
				$error[] = 'form[error][zoomLevel]';
			} elseif (!empty($form['GVMCenter'])
				&& !preg_match(
					'/^([-+]?(([0-9]{1,3}(\.[0-9]{0,16})?)|(\.[0-9]{1,16}))(\040)*,'
					 . '(\040)*[-+]?(([0-9]{1,3}(\.[0-9]{0,16})?)|(\.[0-9]{1,16})))?$/',
					$form['GVMCenter']
				)
			) {
				$error[] = 'form[error][GVMCenter]';
			} elseif (!empty($form['GVMZoom'])
				&& (!is_numeric($form['GVMZoom'])
				|| (intval($form['GVMZoom']) != $form['GVMZoom'])
				|| ($form['GVMZoom'] < 0) || ($form['GVMZoom'] > 19))
			) {
				$error[] = 'form[error][GVMZoom]';
			} else {
				if (!array_key_exists('showScale', $form)) {
					$form['showScale'] = false;
				}

				if (!array_key_exists('centerLongLat', $form)) {
					$form['centerLongLat'] = '0,0';
				}

				if (!array_key_exists('zoomLevel', $form)) {
					$form['zoomLevel'] = '0';
				}

				// Cleanup coordinates of any spaces
				if ($form['centerLongLat'] != '') {
					$form['centerLongLat'] = str_replace(' ', '', trim($form['centerLongLat']));
				}

				// Save parameters if there have been no errors
				foreach (array(
					'GVMZoom',
					'GVMCenter',
					'mapWidth',
					'linktype',
					'mapHeight',
					'centerLongLat',
					'zoomLevel',
					'mapType',
					'MapWidthFormat',
					'MapHeightFormat',
				) as $setting) {
					GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
				}

				foreach (array(
					'showScale',
					'AutoCenterZoom',
					'ShowExportGELink',
					'GoogleOverviewFeature',
					'ThemeFeature',
					'MarkerFeature',
					'LegendFeature',
					'AdminHelp',
					'UserHelp',
					'FilterFeature',
					'GroupFeature',
					'RouteFeature',
					'GZoomFeature',
				) as $setting) {
					GalleryCoreApi::setPluginParameter(
						'module',
						'mapv3',
						$setting,
						(isset($form[$setting]) && $form[$setting]) ? 1 : 0
					);
				}

				$status['saved'] = 1;
			}

			// Figure out the method we will use
			$method            = empty($error) ? 'redirect' : 'delegate';
			$results['status'] = $status;
			$results['error']  = $error;
			$results[$method]  = array(
				'view'             => 'core.SiteAdmin',
				'subView'          => 'mapv3.MapSiteAdmin',
				'mode'             => 'General',
			);
		}

		if (isset($form['action']['editKey'])) {
			$url = $form['mapKeys'];

			$results['status']   = '';
			$results['error']    = '';
			$results['redirect'] = array(
				'view'                 => 'core.SiteAdmin',
				'subView'              => 'mapv3.MapSiteAdmin',
				'mode'                 => 'mapKey',
				'keyUrl'               => $url,
			);
		}

		if (isset($form['action']['addKey'])) {
			// Don't supply a keyUrl param when adding a new key
			$results['status']   = array();
			$results['error']    = array();
			$results['redirect'] = array(
				'view'                 => 'core.SiteAdmin',
				'subView'              => 'mapv3.MapSiteAdmin',
				'mode'                 => 'mapKey',
			);
		}

		if (isset($form['action']['delKey'])) {
			$url = $form['mapKeys'];

			list($ret, $success) = mapv3Helper::deleteKeyProfile($url);

			$status['profileDeleted'] = $success;

			$results['status']   = $status;
			$results['error']    = array();
			$results['redirect'] = array(
				'view'                 => 'core.SiteAdmin',
				'subView'              => 'mapv3.MapSiteAdmin',
				'mode'                 => 'General',
			);
		}

		if (isset($form['action']['saveProfile'])) {
			// error checking
			if (!empty($form['oldProfileUrl'])) {
				// Check if the profile was modified by someone else simultaneously
				list($ret, $oldProfile) = mapv3Helper::fetchKeyProfileForUrl($form['oldProfileUrl']);

				if ($ret) {
					return $ret;
				}

				if (empty($oldProfile)) {
					$error[] = 'form[error][profileDeleted]';
				} elseif ($oldProfile['name'] != $form['oldProfileName']
					|| $oldProfile['url'] != $form['oldProfileUrl']
					|| $oldProfile['apiKey'] != $form['oldProfileApiKey']
				) {
					$error[] = 'form[error][profileModified]';
				}
			}

			if (trim($form['editProfile']['name']) == '') {
				$error[] = 'form[error][editProfile][name][missing]';
			}

			if (trim($form['editProfile']['url']) == '') {
				$error[] = 'form[error][editProfile][url][missing]';
			}

			if (trim($form['editProfile']['apiKey']) == '') {
				$error[] = 'form[error][editProfile][apiKey][missing]';
			}

			if (empty($error)) {
				mapv3Helper::saveKeyProfile(
					trim($form['editProfile']['name']),
					trim($form['editProfile']['url']),
					trim($form['editProfile']['apiKey']),
					$form['oldProfileUrl']
				);

				$status['profilesaved'] = 1;

				list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mapv3');

				if ($ret) {
					return array($ret, null);
				}
				list($ret, $isActive) = $module->isActive();

				if ($ret) {
					return array($ret, null);
				}
				$results['redirect']['view'] = 'core.SiteAdmin';

				if ($isActive) {
					$results['status']              = $status;
					$results['error']               = $error;
					$results['redirect']['subView'] = 'mapv3.MapSiteAdmin';
					$results['redirect']['mode']    = 'General';
				} else {
					$status['configured']           = 'mapv3';
					$results['status']              = $status;
					$results['error']               = $error;
					$results['redirect']['subView'] = 'core.AdminModules';
				}
			} else {
				$results['status']   = $status;
				$results['error']    = $error;
				$results['delegate'] = array(
					'view'             => 'core.SiteAdmin',
					'subView'          => 'mapv3.MapSiteAdmin',
					'mode'             => 'mapKey',
					'keyUrl'           => $form['oldProfileUrl'],
				);
			}
		}

		if (isset($form['action']['getviamap'])) {
			$results['status']   = '';
			$results['error']    = array();
			$results['redirect'] = array(
				'view'             => 'mapv3.ShowMap',
				'Mode'             => 'Pick',
				'plugin'           => 'AdminCenter',
			);
		}

		if (isset($form['action']['getviamap2'])) {
			$results['status']   = '';
			$results['error']    = array();
			$results['redirect'] = array(
				'view'             => 'mapv3.ShowMap',
				'Mode'             => 'Pick',
				'plugin'           => 'GVMDefault',
			);
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class mapSiteAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		global $gallery;

		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mapv3');

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null);
		}

		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$form['formName'] = 'mapSiteAdmin';
		$template->head('modules/mapv3/includes/GoogleMap.css');
		$template->javascript('modules/mapv3/GoogleMap.js');

		// Set default values for undefined plugin parameters
		if (!array_key_exists('mapWidth', $form)) {
			$form['mapWidth'] = 600;
		}

		if (!array_key_exists('mapHeight', $form)) {
			$form['mapHeight'] = 400;
		}

		if (!array_key_exists('centerLongLat', $form)) {
			$form['centerLongLat'] = '0,0';
		}

		if (!array_key_exists('zoomLevel', $form)) {
			$form['zoomLevel'] = 17;
		}

		if (!array_key_exists('mapType', $form)) {
			$form['mapType'] = 'satellite';
		}

		if (!array_key_exists('showScale', $form)) {
			$form['showScale'] = false;
		}

		if (!array_key_exists('AutoCenterZoom', $form)) {
			$form['AutoCenterZoom'] = false;
		}

		if (!array_key_exists('ShowExportGELink', $form)) {
			$form['ShowExportGELink'] = true;
		}

		if (!array_key_exists('GVMCenter', $form)) {
			$form['GVMCenter'] = '0,0';
		}

		if (!array_key_exists('GVMZoom', $form)) {
			$form['GVMZoom'] = 17;
		}

		if (!array_key_exists('MapWidthFormat', $form)) {
			$form['MapWidthFormat'] = 1; // pixels by default
		}

		if (!array_key_exists('MapHeightFormat', $form)) {
			$form['MapHeightFormat'] = 1; // pixels by default
		}

		if (!array_key_exists('GoogleOverviewFeature', $form)) {
			$form['GoogleOverviewFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('GZoomFeature', $form)) {
			$form['GZoomFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('ThemeFeature', $form)) {
			$form['ThemeFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('MarkerFeature', $form)) {
			$form['MarkerFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('LegendFeature', $form)) {
			$form['LegendFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('FilterFeature', $form)) {
			$form['FilterFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('GroupFeature', $form)) {
			$form['GroupFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('RouteFeature', $form)) {
			$form['RouteFeature'] = 0; // Disabled by default
		}

		if (!array_key_exists('linktype', $form)) {
			$form['linktype'] = 0; // Default link
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		// Supply the current URL to the form to fill in default key profile values
		$urlGen          = $gallery->getUrlGenerator();
		$form['baseUrl'] = $urlGen->getCurrentUrlDir();

		list($ret, $form['arrayMapKeys']) = mapv3Helper::getAllKeyProfiles();

		if ($ret) {
			return $ret;
		}

		$editUrl              = GalleryUtilities::getRequestVariables('keyUrl');
		list($ret, $profile)  = mapv3Helper::fetchKeyProfileForUrl($editUrl);

		if ($ret) {
			return $ret;
		}
		$form['editProfile'] = $profile;

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}
		$template->setVariable('controller', 'mapv3.MapSiteAdmin');

		return array(
			null,
			array(
				'body' => 'modules/mapv3/templates/MapSiteAdmin.tpl',
			),
		);
	}
}
