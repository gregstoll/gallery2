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
	$BASE_PATH_STRING = 'modules/mapv3/includes';
	$basegallerydir   = substr(dirname(__FILE__), 0, -strlen($BASE_PATH_STRING));

	global $gallery;
	$urlGenerator  =& $gallery->getUrlGenerator();
	$urlComponents = parse_url($urlGenerator->getCurrentUrlDir(true));
	$base          = $urlComponents['path'];

	// fix for multisite installs
	$baseURL = $gallery->getConfig('galleryBaseUrl');

if (!empty($baseURL)) {
	$base = $baseURL;
}

	// Make sure the user has permission to do this operation
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();

if ($ret) {
	return $ret->wrap(__FILE__, __LINE__);
}

	$status  = array();
	$results = array();
	$error   = array();
