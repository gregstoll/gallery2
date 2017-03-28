<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
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
 * Settings for Dynamic Albums
 * @package DynamicAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class DynamicAlbumSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    foreach (array('size_date', 'size_views', 'size_random') as $key) {
		if (!isset($form[$key]) || !is_numeric($form[$key])) {
		    $error[] = 'form[error][' . $key . ']';
		}
	    }
	}

	if (isset($form['action']['save']) && empty($error)) {
	    list ($ret, $themeSettingsId) =
		GalleryCoreApi::getPluginParameter('module', 'dynamicalbum', 'themeSettingsId');
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $error, $status) = GalleryCoreApi::handleThemeSettingsRequest(
		    $form['currentThemeId'], $themeSettingsId, $form);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status = empty($status) ? array() : array('statusMessage' => $status);
	}

	if (isset($form['action']['save']) && empty($error)) {
	    foreach (array('themeId', 'size.date', 'size.views', 'size.random',
			   'type.date', 'type.views', 'type.random',
			   'description.date', 'description.views', 'description.random')
		    as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'dynamicalbum',
							  $key, $form[strtr($key, '.', '_')]);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    foreach (array('itemlink.date', 'itemlink.views', 'itemlink.random') as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'dynamicalbum',
			$key, empty($form[strtr($key, '.', '_')]) ? 0 : 1);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['saved'] = 1;
	}

	$method = empty($error) ? 'redirect' : 'delegate';
	$results = array($method => array('view' => 'core.SiteAdmin',
					  'subView' => 'dynamicalbum.DynamicAlbumSiteAdmin'),
			 'status' => $status,
			 'error' => $error);

	return array(null, $results);
    }
}

/**
 * Settings for Dynamic Albums
 */
class DynamicAlbumSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'DynamicAlbumSiteAdmin') {
	    foreach (array('themeId', 'size.date', 'size.views', 'size.random',
			   'type.date', 'type.views', 'type.random',
			   'description.date', 'description.views', 'description.random',
			   'itemlink.date', 'itemlink.views', 'itemlink.random')
		    as $key) {
		list ($ret, $form[strtr($key, '.', '_')]) =
		    GalleryCoreApi::getPluginParameter('module', 'dynamicalbum', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	} else {
	    /* Reset theme selection on validation error for theme settings */
	    $form['themeId'] = $form['currentThemeId'];
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'dynamicalbum');
	if ($ret) {
	    return array($ret, null);
	}
	$defaultList = array(
		'data' => $module->translate('Show data items only'),
		'all' => $module->translate('Show all items'),
		'album' => $module->translate('Show albums only'));

	/* Set up theme selection list */
	$themeList = array('' => $module->translate('&laquo; default theme &raquo;'));
	list ($ret, $themeStatus) = GalleryCoreApi::fetchPluginStatus('theme');
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($themeStatus as $id => $status) {
	    if (!empty($status['active'])) {
		list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $id);
		if ($ret) {
		    return array($ret, null);
		}
		$themeList[$id] = $theme->translate($theme->getName());
	    }
	}

	/* Set up theme settings */
	list ($ret, $themeSettingsId) =
	    GalleryCoreApi::getPluginParameter('module', 'dynamicalbum', 'themeSettingsId');
	if ($ret) {
	    return array($ret, null);
	}
	$ret = GalleryCoreApi::loadThemeSettingsForm($form['themeId'], $themeSettingsId,
						     $template, $form);
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'DynamicAlbumSiteAdmin') {
	    $form['formName'] = 'DynamicAlbumSiteAdmin';
	}

	$template->setVariable('DynamicAlbumSiteAdmin',
		array('themeList' => $themeList, 'defaultList' => $defaultList));
	$template->setVariable('controller', 'dynamicalbum.DynamicAlbumSiteAdmin');
	return array(null,
		     array('body' => 'modules/dynamicalbum/templates/DynamicAlbumSiteAdmin.tpl'));
    }
}
?>
