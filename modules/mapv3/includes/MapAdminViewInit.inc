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
 * @version $Revision: 1253 $
 *
 */
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	// Fetch the real path of the gallery installation for file operation
	$ref_string     = 'modules/mapv3/includes';
	$basegallerydir = substr(dirname(__FILE__), 0, -strlen($ref_string));

	$base = GalleryUtilities::convertPathToUrl($basegallerydir);

	// Fetch module parameters and save them in form variable
	list($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'mapv3');

foreach ($param as $key => $value) {
	$form[$key] = $value;
}

	// Add the google map fields (we don't care about errors)
	list($ret, $added) = mapv3Helper::addField('GPS', 'common', 0);
	list($ret, $added) = mapv3Helper::addField('ZoomLevel', 'common', 0);
	list($ret, $added) = mapv3Helper::addField('GELookAt', 'common', 0);

	// Save the Color Names into the Custom Field
	$handle    = fopen(
		$basegallerydir . 'modules/mapv3/images/markers/' . $param['useMarkerSet']
				. '/colorlist',
		'r'
	);
	$photolist = explode(
		'*',
		fread(
			$handle,
			filesize(
				$basegallerydir . 'modules/mapv3/images/markers/' . $param['useMarkerSet']
				. '/colorlist'
			) - 1
		)
	);
	fclose($handle);

	$handle    = fopen(
		$basegallerydir . 'modules/mapv3/images/markers/' . $param['useAlbumMarkerSet']
				. '/colorlist',
		'r'
	);
	$albumlist = explode(
		'*',
		fread(
			$handle,
			filesize(
				$basegallerydir . 'modules/mapv3/images/markers/' . $param['useAlbumMarkerSet']
				. '/colorlist'
			) - 1
		)
	);
	fclose($handle);

	list($ret, $params) = mapv3Helper::loadParameters(0, false);

	$newparam['common'] = $params['common'];

	// Add in the Photo colors into the custom field
	$newparam['photo'][] = array(
		'field'               => 'Color',
		'summary'             => '',
		'detail'              => 1,
		'choices'             => $photolist,
	);

	// Add in the Album colors into the custom field
	$newparam['album'][] = array(
		'field'               => 'Color',
		'summary'             => '',
		'detail'              => 1,
		'choices'             => $albumlist,
	);

	$ret = mapv3Helper::saveParameters($newparam, 0);

	// Add additional fields
	$form['mode']    = GalleryUtilities::getRequestVariables('mode');
	$form['picbase'] = $base . 'modules/mapv3/images/';
	$msie            = '/msie\s(5\.[5-9]|[6-9]\.[0-9]*).*(win)/i';

	if (!isset($_SERVER['HTTP_USER_AGENT']) || !preg_match($msie, $_SERVER['HTTP_USER_AGENT'])
		|| preg_match('/opera/i', $_SERVER['HTTP_USER_AGENT'])
	) {
		$form['IE'] = false;
	} else {
		$form['IE'] = true;
	}

	if (!array_key_exists('AdminHelp', $form)) {
		$form['AdminHelp'] = 1;
	}

	if (!array_key_exists('UserHelp', $form)) {
		$form['UserHelp'] = 1;
	}

	$template->head('modules/mapv3/templates/AdminHeader.tpl');
