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
 * @todo Change the map filters to derive from GalleryEntity and use that framework.
 */
$base_path = 'modules/mapv3/';

GalleryCoreApi::requireOnce($base_path . 'classes/mapv3Helper.class');
GalleryCoreApi::requireOnce($base_path . 'classes/GoogleMapUtilities.class');

/**
 * @package map
 * @subpackage UserInterface
 */
class MapFilterAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		$status = array();
		// Cancel hit, return to the default Filter adminpage
		if (isset($form['action']['cancel'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
		}

		if (isset($form['action']['add'])) {
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditFilter', '');

			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
			$results['redirect']['mode']    = 'AddFilter';
		}

		if (isset($form['action']['delete'])) {
			$id = $form['filter'];

			$tmp            = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Filters');
			$currentfilters = explode('|', $tmp[1]);

			$newfilter = '';

			foreach ($currentfilters as $filter) {
				$temp = explode('`', $filter);

				if ($temp[0] != $id) {
					if ($newfilter == '') {
						$newfilter = $filter;
					} else {
						$newfilter = $newfilter . '|' . $filter;
					}
				}
			}

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Filters', $newfilter);

			$status['filterdeleted'] = 1;

			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
			$results['redirect']['mode']    = 'Filter';
		}

		if (isset($form['action']['edit'])) {
			$id = $form['filter'];

			$tmp            = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Filters');
			$currentfilters = explode('|', $tmp[1]);

			$newfilter = '';

			foreach ($currentfilters as $filter) {
				$temp = explode('`', $filter);

				if ($temp[0] != $id) {
					if ($newfilter == '') {
						$newfilter = $filter;
					} else {
						$newfilter = $newfilter . '|' . $filter;
					}
				} else {
					$editfilter = $filter;
				}
			}

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Filters', $newfilter);
			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditFilter', $editfilter);

			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
			$results['redirect']['mode']    = 'AddFilter';
		}

		if (isset($form['action']['save'])) {
			// Some error checking
			if (empty($_POST['FilterName'])) {
				$error[] = 'form[error][filtername]';
			} elseif (empty($_POST['FilterZoom']) || (!is_numeric($_POST['FilterZoom']))
				|| ($_POST['FilterZoom'] < 0) || ($_POST['FilterZoom'] > 17)
			) {
				$error[] = 'form[error][filterzoom]';
			} elseif (empty($_POST['FilterCenter'])
				|| (!preg_match(
					'/^([-+]?(([0-9]{1,3}(\.[0-9]{0,16})?)|(\.[0-9]{1,16}))(\040)*,'
					. '(\040)*[-+]?(([0-9]{1,3}(\.[0-9]{0,16})?)|(\.[0-9]{1,16})))?$/',
					$_POST['FilterCenter']
				))
			) {
				$error[] = 'form[error][filtercenter]';
			} else {
				$oldfilter = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Filters');

				$FilterName   = $_POST['FilterName'];
				$FilterZoom   = $_POST['FilterZoom'];
				$FilterCenter = $_POST['FilterCenter'];

				if ($oldfilter[1] != '') {
					$temp = explode('|', $oldfilter[1]);

					if (!isset($_POST['oldid'])) {
						// @todo This code will not get the maximum id. Fix similar to Routes
						$maxfilter = explode('`', max($temp));
						$num       = $maxfilter[0] + 1;
					} else {
						$num = $_POST['oldid'];
					}
					$form['Filters'] = $oldfilter[1]
						   . '|' . $num
						   . '`' . $FilterName
						   . '`' . $FilterZoom
						   . '`' . $FilterCenter;
				} else {
					$form['Filters'] = '1`' . $FilterName
						  . '`' . $FilterZoom
						  . '`' . $FilterCenter;
				}

				GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Filters', $form['Filters']);

				$status['filtersaved'] = 1;
			}

			GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditFilter', '');

			$method                      = empty($error) ? 'redirect' : 'delegate';
			$mode                        = empty($error) ? 'Filter' : 'AddFilter';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapFilterAdmin';
			$results[$method]['mode']    = $mode;
		}

		if (isset($form['action']['savesettings'])) {
			$form['ShowAlbumFilters'] = false;
			$form['ShowFilters']      = 0;

			if (array_key_exists('showalbumfilters', $_POST) and $_POST['showalbumfilters']) {
				$form['ShowAlbumFilters'] = true;
			}

			if (array_key_exists('ShowFilters', $_POST) and $_POST['ShowFilters']) {
				$form['ShowFilters'] = $_POST['ShowFilters'];
			}

			switch ($form['ShowFilters']) {
				case 1: // Show the filter block on the left
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
					$settings                       = $deftheme->getSettings(null);
					list($thememajor, $thememinor)  = $deftheme->getApiVersion();
					/*
					 * This block manipulation logic only works for earlier block encoding,
					 * so skip it for later Theme API versions to avoid corrupting the
					 * block list.
					 */
					if ($thememajor == 2 && $thememinor <= 3) {
						if (array_key_exists('2', $settings)) {
							$settings = $settings[2];

							foreach ($settings as $key => $values) {
								if ($key == 'sidebarBlocks') {
									list($ret, $value)     = $deftheme->unpackSetting('block-list', $values);
									$value                 = str_replace('[map.map]', '', $value);
									$value                 = '[map.map] ' . $value;
									list($ret, $newvalue)  = $deftheme->packSetting('block-list', $value);
									$ret                   = $deftheme->setParameter('sidebarBlocks', $newvalue);
								}
							}
						} elseif (is_array($settings[1])) {
							foreach ($settings[1] as $num => $arrayvalue) {
								if ($arrayvalue['key'] == 'sidebarBlocks') {
									list($success, $value) = $deftheme->unpackSetting(
										'block-list',
										$settings[1][$num]['value']
									);
									$value                  = str_replace('[map.map]', '', $value);
									$value                  = '[map.map] ' . $value;
									list($ret, $newvalue)   = $deftheme->packSetting('block-list', $value);
									$ret                    = $deftheme->setParameter('sidebarBlocks', $newvalue);
								}
							}
						}
					}

					break;

				case 2: // Show on the right, no sidebar block -> continue
				case 3: // Show on the top, no sidebar block -> continue
				case 4: // Show on the bottom, no sidebar block -> continue
				case 0: // Don't show the filter block in the sidebar
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
					$settings                       = $deftheme->getSettings(null);
					list($thememajor, $thememinor)  = $deftheme->getApiVersion();
					/*
					 * This block manipulation logic only works for earlier block encoding,
					 * so skip it for later Theme API versions to avoid corrupting the
					 * block list.
					 */
					if ($thememajor == 2 && $thememinor <= 3) {
						if (array_key_exists('2', $settings)) {
							$settings = $settings[2];

							foreach ($settings as $key => $values) {
								if ($key == 'sidebarBlocks') {
									list($ret, $value)     = $deftheme->unpackSetting('block-list', $values);
									$value                 = str_replace('[map.map]', '', $value);
									list($ret, $newvalue)  = $deftheme->packSetting('block-list', $value);
									$ret                   = $deftheme->setParameter('sidebarBlocks', $newvalue);
								}
							}
						} elseif (is_array($settings[1])) {
							foreach ($settings[1] as $num => $arrayvalue) {
								if ($arrayvalue['key'] == 'sidebarBlocks') {
									list($success, $value) = $deftheme->unpackSetting(
										'block-list',
										$settings[1][$num]['value']
									);
									$value                  = str_replace('[map.map]', '', $value);
									list($ret, $newvalue)   = $deftheme->packSetting('block-list', $value);
									$ret                    = $deftheme->setParameter('sidebarBlocks', $newvalue);
								}
							}
						}
					}
			}

			foreach (array('ShowFilters', 'LevelFilterAll') as $setting) {
				GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
			}

			foreach (array('ShowAlbumFilters', 'LevelFilterRoot') as $setting) {
				GalleryCoreApi::setPluginParameter(
					'module',
					'mapv3',
					$setting,
					(isset($form[$setting]) && $form[$setting]) ? 1 : 0
				);
			}

			$status['saved'] = 1;

			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapFilterAdmin';
			$results['redirect']['mode']    = 'Filter';
		}

		if (isset($form['action']['getviamap'])) {
			$results['status']             = '';
			$results['error']              = array();
			$results['redirect']['view']   = 'mapv3.ShowMap';
			$results['redirect']['Mode']   = 'Pick';
			$results['redirect']['plugin'] = 'Filters';
			$results['redirect']['center'] = $_POST['center'];
			$results['redirect']['zoom']   = $_POST['zoom'];
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapFilterAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$base_path = 'modules/mapv3/';
		$template->head($base_path . 'includes/GoogleMap.css');

		$form['formName'] = 'MapFilterAdmin';

		// Set default values for undefined plugin parameters
		if (!array_key_exists('ShowFilters', $form)) {
			$form['ShowFilters'] = 0; // hide the block by default
		}

		if (!array_key_exists('ShowAlbumFilters', $form)) {
			$form['ShowAlbumFilters'] = false;
		}

		if (!array_key_exists('LevelFilterAll', $form)) {
			$form['LevelFilterAll'] = 0;
		}

		if (!array_key_exists('LevelFilterRoot', $form)) {
			$form['LevelFilterRoot'] = false;
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		// Create the filter array
		if (array_key_exists('Filters', $form) and $form['Filters'] != '') {
			$separatedfilters = explode('|', $form['Filters']);
			sort($separatedfilters);

			foreach ($separatedfilters as $alloptions) {
				$options              = explode('`', $alloptions);
				$form['filterlist'][] = array(
					'name' => $options[1],
					'id'   => $options[0],
				);
			}
		}

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapFilterAdmin');

		return array(
			null,
			array(
				'body' => $base_path . 'templates/MapFilterAdmin.tpl',
			),
		);
	}
}
