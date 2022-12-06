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
 * @version $Revision: 1253 $
 *
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');

/**
 * @package map
 * @subpackage UserInterface
 */
class MapGZoomAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		$status = array();

		// Cancel hit, return to the default GZoom Admin Page
		if (isset($form['action']['cancel'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapGZoomAdmin';
		}

		// Save the Gzoom settings
		if (isset($form['action']['save'])) {
			foreach (array('GZPos', 'GZPosOffX', 'GZPosOffY') as $setting) {
				GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
			}

			foreach (array('GZoom') as $setting) {
				GalleryCoreApi::setPluginParameter(
					'module',
					'mapv3',
					$setting,
					(isset($form[$setting]) && $form[$setting]) ? 1 : 0
				);
			}

			$status['saved'] = 1;

			// Figure out the method we will use
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapGZoomAdmin';
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapGZoomAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';
		$form['formName'] = 'MapGZoomAdmin';

		$template->head('modules/mapv3/includes/GoogleMap.css');
		$template->javascript('modules/mapv3/GoogleMap.js');

		if (!array_key_exists('GZoom', $form)) {
			$form['GZoom'] = false;
		}

		if (!array_key_exists('GZPos', $form)) {
			$form['GZPos'] = '1';
		}

		if (!array_key_exists('GZPosOffX', $form)) {
			$form['GZPosOffX'] = '7';
		}

		if (!array_key_exists('GZPosOffY', $form)) {
			$form['GZPosOffY'] = '30';
		}

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapGZoomAdmin');

		return array(
			null,
			array(
				'body' => 'modules/mapv3/templates/MapGZoomAdmin.tpl',
			),
		);
	}
}
