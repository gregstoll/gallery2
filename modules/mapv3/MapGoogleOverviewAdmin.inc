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
 * @author Eric Daniel <ehdaniel@gmail.com>
 * @version $Revision: 1264 $
 *
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');

/**
 * @package map
 * @subpackage UserInterface
 */
class MapGoogleOverviewAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		$status = array();
		// Cancel hit, return to the default Google Overview Admin Page
		if (isset($form['action']['cancel'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGoogleOverviewAdmin';
		}

		// Save the Google Overview settings
		if (isset($form['action']['save'])) {
			// Defaults
			if (!array_key_exists('GOPos', $form)) {
				$form['GOPos'] = '3';
			}

			if (!array_key_exists('GOSizeX', $form)) {
				$form['GOSizeX'] = '100';
			}

			if (!array_key_exists('GOSizeY', $form)) {
				$form['GOSizeY'] = '100';
			}

			if (!array_key_exists('GOPosOffY', $form) || $form['GOPosOffY'] == '') {
				$form['GOPosOffY'] = '0';
			}

			if (!array_key_exists('GOPosOffX', $form) || $form['GOPosOffX'] == '') {
				$form['GOPosOffX'] = '0';
			}

			// error checking
			if (($form['GOSizeY'] == '') || ($form['GOSizeX'] == '')
				|| (!is_numeric($form['GOSizeY'])) || !is_numeric($form['GOSizeX'])
			) {
				$error[] = 'form[error][noGOSize]';
			} else {
				foreach (array('GOPos', 'GOSizeX', 'GOSizeY', 'GOPosOffX', 'GOPosOffY') as $setting) {
					GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
				}

				foreach (array('GoogleOverview') as $setting) {
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
			$results[$method]['subView'] = 'mapv3.MapGoogleOverviewAdmin';
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapGoogleOverviewAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		$base_path = 'modules/mapv3/';

		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$form['formName'] = 'MapGoogleOverviewAdmin';
		$template->head($base_path . 'includes/GoogleMap.css');
		$template->javascript($base_path . 'GoogleMap.js');

		if (!array_key_exists('GoogleOverview', $form)) {
			$form['GoogleOverview'] = false;
		}

		if (!array_key_exists('GOSizeX', $form)) {
			$form['GOSizeX'] = '200';
		}

		if (!array_key_exists('GOSizeY', $form)) {
			$form['GOSizeY'] = '200';
		}

		if (!array_key_exists('GOPos', $form)) {
			$form['GOPos'] = '3';
		}

		if (!array_key_exists('GOPosOffX', $form)) {
			$form['GOPosOffX'] = '0';
		}

		if (!array_key_exists('GOPosOffY', $form)) {
			$form['GOPosOffY'] = '0';
		}

		if (!array_key_exists('GOType', $form)) {
			$form['GOType'] = 'satellite';
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapGoogleOverviewAdmin');

		return array(
			null,
			array(
				'body' => $base_path . 'templates/MapGoogleOverviewAdmin.tpl',
			),
		);
	}
}
