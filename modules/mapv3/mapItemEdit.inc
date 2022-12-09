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
 * @author Eric H Daniel <ehdaniel@gmail.com>
 * @version $Revision: 1576 $
 *
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');

/**
 * This controller will handle assigning values to custom fields
 *
 * @package map
 * @subpackage UserInterface
 *
 */
class mapItemEdit extends ItemEditPlugin {
	/**
	 * @see ItemEditPlugin::handleRequest
	 */
	public function handleRequest($form, &$item, &$preferred) {
		global $gallery;
		$gurl =& $gallery->getUrlGenerator();

		$status              = null;
		$error               = array();
		list($ret, $module)  = GalleryCoreApi::loadPlugin('module', 'mapv3');

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null, null, null);
		}

		if (isset($form['action']['save'])) {
			// error control
			/*
			 * heading can be +/- 180 degrees
			 * tilt can be 0-90 degrees
			 * range can be 0-63710000 m
			 */
			$headingregex = '[-+]?((180(\.0*)?)|((1[0-7]|[0-9])?'
					. '[0-9](\.[0-9]{0,20})?)|(\.[0-9]{1,19}))';
			$tiltregex    = '((90(\.0*)?)|([0-8]?[0-9](\.[0-9]{0,20})?)|(\.[0-9]{1,19}))?';
			$rangeregex   = '([0-9]{0,8})';
			// latitude can be +/- 90 degrees, longitude can be +/- 180 degrees
			$latregex = '[-+]?' . $tiltregex;
			$lonregex = $headingregex;

			if (!preg_match(
				'/^(' . $latregex . '(\040)*,(\040)*' . $lonregex
				. ')?$/',
				trim($form['fields']['GPS'])
			)
			) {
				$status = '<span class="giError">'
				  . $module->translate('Bad coordinates, please verify') . '</span>';
			} elseif (!preg_match(
				'/^(' . $headingregex . '((\040)*,(\040)*'
				. $tiltregex . '((\040)*,(\040)*' . $rangeregex . ')?)?)?$/',
				trim($form['fields']['GELookAt'])
			)
			) {
				$status = '<span class="giError">'
				. $module->translate('Bad LookAt parameters, please verify') . '</span>';
			} else {
				// cleanup the coordinates of any spaces
				if ($form['fields']['GPS'] != '') {
					$form['fields']['GPS'] = str_replace(' ', '', trim($form['fields']['GPS']));
				}

				if ($form['fields']['GELookAt'] != '') {
					$form['fields']['GELookAt'] = str_replace(' ', '', trim($form['fields']['GELookAt']));
				}

				if (($form['fields']['ZoomLevel'] != '') and ($form['fields']['ZoomLevel'] == 0)) {
					$form['fields']['ZoomLevel'] = '0.0';
				}

				$ret = mapv3Helper::saveFieldValues($item, $form['fields']);

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}
				$status = $module->translate('Changes saved successfully');
			}
		} elseif (isset($form['action']['getexif'])) {
			$form['fields']['GPS'] = $form['exif'];
			$ret                   = mapv3Helper::saveFieldValues($item, $form['fields']);

			if ($ret) {
				return array($ret->wrap(__FILE__, __LINE__), null, null, null);
			}
			list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mapv3');

			if ($ret) {
				return array($ret->wrap(__FILE__, __LINE__), null, null, null);
			}

			$status = $module->translate('GPS coordinates successfully entered and saved');
		} elseif (isset($form['action']['setexif'])) {
			// Write the GPS coordinates to the EXIF header
			// Get the coordinates from the field
			if ($form['fields']['GPS'] != '') {
				$coordinates = $form['fields']['GPS'];

				list($ret, $error, $status, $other) = GoogleMapUtilities::setGPSCoordsInExif($item, $coordinates, true);

				if ($ret) {
					return array($ret->wrap(__FILE__, __LINE__), null, null, null);
				}
			} else {
				$error[] = 'form[error][gps][missingGPSCoordinates]';
			}
		}

		return array(null, $error, $status, false);
	}

	/**
	 * @see ItemEditPlugin::loadTemplate
	 */
	public function loadTemplate(&$template, &$form, $item, $thumbnail) {
		if ($form['formName'] != 'mapItemEdit') {
			$form['formName'] = 'mapItemEdit';
			$form['itemId']   = $item->getId();
		}

		list($ret, $form['exif']) = GoogleMapUtilities::getGPSCoordsFromExif($item);

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null, null);
		}

		if ($form['exif'] == null) {
			$form['noexif'] = 1;
		}

		// Get the API key for the current URL
		list($ret, $form['apiKey']) = mapv3Helper::fetchApiKey();

		if ($ret) {
			return array($ret, null, null);
		}

		$type = GalleryUtilities::isA($item, 'GalleryAlbumItem') ? 'AlbumLegend' : 'PhotoLegend';

		list($ret, $LegendEnable) = GalleryCoreApi::getPluginParameter('module', 'mapv3', $type);

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null, null);
		}

		if ($LegendEnable) {
			list($ret, $Legends) = GalleryCoreApi::getPluginParameter('module', 'mapv3', $type . 's');
		}

		$set                                           = (GalleryUtilities::isA($item, 'GalleryAlbumItem')) ? 'album' : ((GalleryUtilities::isA($item, 'GalleryPhotoItem')) ? 'photo' : 'common');
		list($ret, $tmp, $param, $isContainerSettings) = mapv3Helper::fetchFieldValues(array($item), null, $set);

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null, null);
		}
		$form['fields'] = $tmp[$item->getId()];

		foreach (array(
			'common' => 1,
			$set     => 1,
		) as $s => $tmp) {
			foreach ($param[$item->getId()][$s] as $it) {
				if (!empty($it['choices'])) {
					$form['choices'][$it['field']] = $it['choices'];
				}
			}
		}

		if ($set == 'album') {
			$form['adminFlag'] = $isContainerSettings[$item->getId()] ? 1 : 2;
		}
		$tpl = 'modules/mapv3/templates/mapItemEdit.tpl';
		$template->setVariable('controller', 'mapv3.mapItemEdit');

		// Add stuff for the help system
		$form['picbase']              = GalleryUtilities::convertPathToUrl(substr(dirname(__FILE__), 0, -strlen('modules/mapv3'))) . 'modules/mapv3/images/';
		list($ret, $form['UserHelp']) = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'UserHelp');
		$msie                         = '/msie\s(5\.[5-9]|[6-9]\.[0-9]*).*(win)/i';

		if (!isset($_SERVER['HTTP_USER_AGENT']) || !preg_match($msie, $_SERVER['HTTP_USER_AGENT'])
			|| preg_match('/opera/i', $_SERVER['HTTP_USER_AGENT'])
		) {
			$form['IE'] = false;
		} else {
			$form['IE'] = true;
		}

		return array(null, $tpl, 'modules_map');
	}

	/**
	 * @see ItemEditPlugin::isSupported
	 */
	public function isSupported($item, $thumbnail) {

		// Make sure we have permission do edit the google map option of this item
		$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'mapv3.EditMap');

		if ($ret) {
			return false;
		}

		if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
			return true;
		}
		list($ret, $param) = mapv3Helper::loadParameters($item->getParentId());

		if ($ret) {
			return false;
		}

		if (!empty($param['common'])
			|| (GalleryUtilities::isA($item, 'GalleryPhotoItem') && !empty($param['photo']))
		) {
			return true;
		}

		return false;
	}

	/**
	 * @see ItemEditPlugin::getTitle
	 */
	public function getTitle() {
		global $gallery;
		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'mapv3');

		if ($ret) {
			return $ret->wrap(__FILE__, __LINE__);
		}

		return array(null, $module->translate('Google Map'));
	}
}
