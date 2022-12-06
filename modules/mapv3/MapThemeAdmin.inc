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
class MapThemeAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		// Save the theme settings
		if (isset($form['action']['save'])) {
			// Check for errors
			if (array_key_exists('MapControlPosOffX', $form)
				&& (!is_numeric($form['MapControlPosOffX'])
				|| (intval($form['MapControlPosOffX']) != $form['MapControlPosOffX']))
			) {
				$error[] = 'form[error][MapControlPosOffX]';
			} elseif (array_key_exists('MapControlPosOffY', $form)
				&& (!is_numeric($form['MapControlPosOffY'])
				|| (intval($form['MapControlPosOffY']) != $form['MapControlPosOffY']))
			) {
				$error[] = 'form[error][MapControlPosOffY]';
			} elseif (!is_numeric($form['zoomInLevel'])
				|| (intval($form['zoomInLevel']) != $form['zoomInLevel'])
				|| ($form['zoomInLevel'] < 0) || ($form['zoomInLevel'] > 17)
			) {
				$error[] = 'form[error][zoomInLevel]';
			} elseif (!is_numeric($form['ThumbHeight'])
				|| (intval($form['ThumbHeight']) != $form['ThumbHeight'])
			) {
				$error[] = 'form[error][ThumbHeight]';
			} else {
				// Save parameters if there have been no errors
				foreach (array(
					'MapControlType',
					'MapControlPos',
					'zoomInLevel',
					'MapControlPosOffX',
					'MapControlPosOffY',
					'MapWindowType',
					'ThumbBarPos',
					'ThumbHeight',
				) as $setting) {
					GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
				}

				foreach (array(
					'showMapType',
					'showZoomLinks',
					'showItemSummaries',
					'showItemDescriptions',
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
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapThemeAdmin';
		}

		// Cancel hit, return to the default adminpage
		if (isset($form['action']['cancel'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapThemeAdmin';
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapThemeAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$form['formName'] = 'MapThemeAdmin';
		$template->head('modules/mapv3/includes/GoogleMap.css');
		$template->javascript('modules/mapv3/GoogleMap.js');

		$BASE_PATH_STRING = 'modules/mapv3';
		$basegallerydir   = substr(dirname(__FILE__), 0, -strlen($BASE_PATH_STRING));
		// setup the directory listing for the custom controls
		if (!array_key_exists('MapControlType', $form)) {
			$form['MapControlType'] = 'Small';
		}

		if (!array_key_exists('MapControlPos', $form)) {
			$form['MapControlPos'] = '3';
		}

		if (!array_key_exists('MapControlPosOffX', $form)) {
			$form['MapControlPosOffX'] = '10';
		}

		if (!array_key_exists('MapControlPosOffY', $form)) {
			$form['MapControlPosOffY'] = '15';
		}

		$form['controllist'] = GoogleMapUtilities::OptionFromFolder(
			$basegallerydir . 'modules/mapv3/templates/controls/',
			$form['MapControlType']
		);

		if (!$form['controllist']) {
			$form['nocontrols'] = 'There is no control available';
		}

		// Set default values for undefined plugin parameters
		if (!array_key_exists('zoomInLevel', $form)) {
			$form['zoomInLevel'] = 5;
		}

		if (!array_key_exists('showZoomLinks', $form)) {
			$form['showZoomLinks'] = false;
		}

		if (!array_key_exists('showItemSummaries', $form)) {
			$form['showItemSummaries'] = true;
		}

		if (!array_key_exists('showItemDescriptions', $form)) {
			$form['showItemDescriptions'] = false;
		}

		if (!array_key_exists('MapControlType', $form)) {
			$form['MapControlType'] = 'Small';
		}

		if (!array_key_exists('MapControlPos', $form)) {
			$form['MapControlPos'] = '3';
		}

		if (!array_key_exists('MapControlPosOffX', $form)) {
			$form['MapControlPosOffX'] = '10';
		}

		if (!array_key_exists('MapControlPosOffY', $form)) {
			$form['MapControlPosOffY'] = '15';
		}

		if (!array_key_exists('MapWindowType', $form)) {
			$form['MapWindowType'] = 'default';
		}

		if (!array_key_exists('ThumbBarPos', $form)) {
			$form['ThumbBarPos'] = 'hidden';
		}

		if (!array_key_exists('ThumbHeight', $form)) {
			$form['ThumbHeight'] = 60;
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		// setup the list of InfoWindow templates
		$mydir              = dir($basegallerydir . 'modules/mapv3/templates/infowindow/');
		$form['windowlist'] = '';
		// Loop through all file and folder in the directory
		$infowindowFiles = array();

		while ($file = $mydir->read()) {
			// if we have a file then proceed with it
			if (GoogleMapUtilities::GoodFile(
				$basegallerydir . 'modules/mapv3/templates/infowindow/',
				$file
			)
			) {
				$infowindowFiles[] = $file;
			}
		}
		$mydir->close();
		asort($infowindowFiles);

		foreach ($infowindowFiles as $file) {
			$form['windowlist'] .= '<option value="' . substr($file, 0, -4) . '" ';

			if ((array_key_exists('MapWindowType', $form))
				and ($form['MapWindowType'] == substr($file, 0, -4))
			) {
				$form['windowlist'] .= 'selected';
			}
			$form['windowlist'] .= '>' . substr($file, 0, -4) . "</option>\n\t";
		}
		/*
		 * Just make sure anything got created, if, not return an error that there is no control
		 * available
		 */
		if ($form['windowlist'] == '') {
			$form['nowindows'] = 'There is no InfoWindow Template available';
		}

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapThemeAdmin');

		return array(
			null,
			array(
				'body' => 'modules/mapv3/templates/MapThemeAdmin.tpl',
			),
		);
	}
}
