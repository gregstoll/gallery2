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
 * This File is included in all the Admin Pages and prepare Edit item as well as recover "lost" ones
 *
 */
if (array_key_exists('EditRoute', $param) and $param['EditRoute'] != '') {
	if (array_key_exists('mode', $form) and $form['mode'] == 'AddRoute') {
		$form['EditRoute'] = explode('`', $param['EditRoute']);
		$form['OldRoute']  = $param['EditRoute'];
		$routevalues       = array();

		for ($i = 7; $i < count($form['EditRoute']); $i++) {
			$num = $i - 6;

			if (!array_key_exists($form['EditRoute'][$i], $routevalues)) {
				$routevalues[$form['EditRoute'][$i]] = $num;
			} else {
				$routevalues[$form['EditRoute'][$i]] .= ',' . $num;
			}
		}
		$form['routevalues'] = $routevalues;
	} else { // We have a route in the edit state that didn't get saved ... restoring
		$oldroutes = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Routes');

		if ($oldroutes[1] != '') {
			$newroutes = $oldroutes[1] . '|' . $form['EditRoute'];
		} else {
			$newroutes = $form['EditRoute'];
		}
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Routes', $newroutes);
		$tmp             = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Routes');
		$param['Routes'] = $tmp[1];
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditRoute', '');
	}
}

if (array_key_exists('EditFilter', $form) and $form['EditFilter'] != '') {
	if (array_key_exists('mode', $form) and $form['mode'] == 'AddFilter') {
		$form['EditFilter'] = explode('`', $form['EditFilter']);
		$form['OldFilter']  = $param['EditFilter'];
	} else { // We have a filter in the Edit state that didn't get saved ... restoring
		$oldfilters = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Filters');

		if ($oldfilters[1] != '') {
			$newfilters = $oldfilters[1] . '|' . $form['EditFilter'];
		} else {
			$newfilters = $form['EditFilter'];
		}
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'Filters', $newfilters);
		$tmp              = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'Filters');
		$param['Filters'] = $tmp[1];
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditFilter', '');
	}
}

if (array_key_exists('EditGroup', $form) and $form['EditGroup'] != '') {
	if (array_key_exists('mode', $form) and $form['mode'] == 'AddGroup') {
		$form['EditGroup']    = explode('`', $form['EditGroup']);
		$pic                  = GalleryUtilities::convertPathToUrl(
			$basegallerydir . 'modules/mapv3/images/groups/'
							  . $form['EditGroup'][4]
		);
		$form['EditGroup'][4] = '"' . $pic . '"';
		$form['EditGroup'][5] = strftime($dateFormat, $form['EditGroup'][5]);
		$editparams           = array();

		if (isset($param['EditGroupParam'])) {
			$tmp = array();
			$tmp = explode('|', $param['EditGroupParam']);

			foreach ($tmp as $editparam) {
				$tmparray                 = explode(':', $editparam);
				$editparams[$tmparray[0]] = $tmparray[1];
			}
		}
		$form['EditGroupParam'] = $editparams;
		$form['OldGroup']       = $param['EditGroup'];
	} else { // We have a Group in the Edit state that did not get saved .... restoring
		$oldgroups = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');

		if ($oldgroups[1] != '') {
			$newgroups = $oldgroups[1] . '|' . $form['EditGroup'];
		} else {
			$newgroups = $form['EditGroup'];
		}
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'MapGroups', $newgroups);
		$tmp                = GalleryCoreApi::getPluginParameter('module', 'mapv3', 'MapGroups');
		$param['MapGroups'] = $tmp[1];
		GalleryCoreApi::setPluginParameter('module', 'mapv3', 'EditGroup', '');
	}
}
