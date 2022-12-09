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
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');


/**
 * @package map
 * @subpackage UserInterface
 */
class MapLegendAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		// Cancel hit, return to the default Legend Admin Page
		if (isset($form['action']['cancel'])) {
			$results['status']              = array();
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapLegendAdmin';
		}

		// Save Legend information
		if (isset($form['action']['save'])) {
			$form['AlbumLegend'] = false;
			$form['PhotoLegend'] = false;
			$form['GroupLegend'] = false;

			if (array_key_exists('usealbumlegend', $_POST) and $_POST['usealbumlegend']) {
				$form['AlbumLegend'] = true;
			}

			if (array_key_exists('usephotolegend', $_POST) and $_POST['usephotolegend']) {
				$form['PhotoLegend'] = true;
			}

			if (array_key_exists('UseGroupLegend', $_POST) and $_POST['UseGroupLegend']) {
				$form['GroupLegend'] = true;
			}

			$form['ExpandAlbumLegend'] = false;
			$form['ExpandPhotoLegend'] = false;
			$form['ExpandGroupLegend'] = false;

			if (array_key_exists('ExpandAlbumLegend', $_POST) and $_POST['ExpandAlbumLegend']) {
				$form['ExpandAlbumLegend'] = true;
			}

			if (array_key_exists('ExpandPhotoLegend', $_POST) and $_POST['ExpandPhotoLegend']) {
				$form['ExpandPhotoLegend'] = true;
			}

			if (array_key_exists('ExpandGroupLegend', $_POST) and $_POST['ExpandGroupLegend']) {
				$form['ExpandGroupLegend'] = true;
			}

			$albumlegenditem = '';
			$photolegenditem = '';
			$grouplegenditem = '';

			for ($i = 0; $i < 10; $i++) {
				if ($form['AlbumLegend']) {
					$albumlegenditem .= $_POST['A' . $i] . '|';
				}

				if ($form['PhotoLegend']) {
					$photolegenditem .= $_POST['P' . $i] . '|';
				}

				if ($form['GroupLegend']) {
					$grouplegenditem .= $_POST['G' . $i] . '|';
				}
			}
			$form['PhotoLegends'] = $photolegenditem;
			$form['AlbumLegends'] = $albumlegenditem;
			$form['GroupLegends'] = $grouplegenditem;

			switch ($form['LegendPos']) {
				case 'left':
					// Legend is on the left, add the mapv3.Legend Block
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
									$value                 = str_replace('[mapv3.Legend]', '', $value);
									$value                 = '[mapv3.Legend] ' . $value;
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
									$value                  = str_replace('[mapv3.Legend]', '', $value);
									$value                  = '[mapv3.Legend] ' . $value;
									list($ret, $newvalue)   = $deftheme->packSetting('block-list', $value);
									$ret                    = $deftheme->setParameter('sidebarBlocks', $newvalue);
								}
							}
						}
					}

					break;

				case 'top': // Legend on the top   -> continue
				case 'bottom': // Legend on the bottom -> continue
				case 'hide': // No Legend to show -> continue
				case 'right': // Legend on the right, remove the mapv3.Legend block from the sidebar
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
									$value                 = str_replace('[mapv3.Legend]', '', $value);
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
									$value                  = str_replace('[mapv3.Legend]', '', $value);
									list($ret, $newvalue)   = $deftheme->packSetting('block-list', $value);
									$ret                    = $deftheme->setParameter('sidebarBlocks', $newvalue);
								}
							}
						}
					}
			}

			foreach (array(
				'AlbumLegends',
				'PhotoLegends',
				'GroupLegends',
				'LegendPos',
			) as $setting) {
				GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
			}

			foreach (array(
				'AlbumLegend',
				'PhotoLegend',
				'GroupLegend',
				'ExpandAlbumLegend',
				'ExpandGroupLegend',
				'ExpandPhotoLegend',
			) as $setting) {
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
			$results['redirect']['subView'] = 'mapv3.MapLegendAdmin';
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapLegendAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$template->head('modules/mapv3/includes/GoogleMap.css');
		$form['formName'] = 'MapLegendAdmin';

		// Set default values for undefined plugin parameters
		if (!array_key_exists('AlbumLegend', $form)) {
			$form['AlbumLegend'] = false;
		}

		if (!array_key_exists('PhotoLegend', $form)) {
			$form['PhotoLegend'] = false;
		}

		if (!array_key_exists('AlbumLegends', $form)) {
			$form['AlbumLegends'] = '';
		}

		if (!array_key_exists('GroupLegend', $form)) {
			$form['GroupLegend'] = false;
		}

		if (!array_key_exists('GroupLegends', $form)) {
			$form['GroupLegends'] = '';
		}

		if (!array_key_exists('PhotoLegends', $form)) {
			$form['PhotoLegends'] = '';
		}

		if (!array_key_exists('ExpandAlbumLegend', $form)) {
			$form['ExpandAlbumLegend'] = true;
		}

		if (!array_key_exists('ExpandPhotoLegend', $form)) {
			$form['ExpandPhotoLegend'] = true;
		}

		if (!array_key_exists('ExpandGroupLegend', $form)) {
			$form['ExpandGroupLegend'] = true;
		}

		if (!array_key_exists('LegendPos', $form)) {
			$form['LegendPos'] = 'right'; // right by default
		}

		// Explode the list of information in the legends if available
		if ($form['PhotoLegends'] != '') {
			$photolegends         = explode('|', $form['PhotoLegends']);
			$form['PhotoLegends'] = $photolegends;
		}

		if ($form['AlbumLegends'] != '') {
			$albumlegends         = explode('|', $form['AlbumLegends']);
			$form['AlbumLegends'] = $albumlegends;
		}

		if ($form['GroupLegends'] != '') {
			$grouplegends         = explode('|', $form['GroupLegends']);
			$form['GroupLegends'] = $grouplegends;
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		$form['allmarkers'] = GoogleMapUtilities::AllMarkers();

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapLegendAdmin');

		return array(
			null,
			array(
				'body' => 'modules/mapv3/templates/MapLegendAdmin.tpl',
			),
		);
	}
}
