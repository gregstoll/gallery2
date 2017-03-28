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
 * Settings for Ratings
 * @package Rating
 * @subpackage UserInterface
 * @author  Don Seiler <don@seiler.us>
 * @version $Revision: 17580 $
 */
class RatingSiteAdminController extends GalleryController {

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
	    list ($ret, $themeSettingsId) =
		GalleryCoreApi::getPluginParameter('module', 'rating', 'themeSettingsId');
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $error, $status) = GalleryCoreApi::handleThemeSettingsRequest(
		    $form['currentThemeId'], $themeSettingsId, $form);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status = empty($status) ? array() : array('statusMessage' => $status);

	    if (!isset($form['minLimit'])
		    || !is_numeric($form['minLimit'] = str_replace(',', '.', $form['minLimit']))) {
		$error[] = 'form[error][minLimit]';
	    }
	}

	if (isset($form['action']['save']) && empty($error)) {
	    foreach (array('allowAlbumRating' => empty($form['allowAlbumRating']) ? 0 : 1,
			   'themeId' => $form['themeId'],
			   'orderBy' => (empty($form['presort']) ? '' : $form['presort'] . '|')
					. $form['orderBy'],
			   'orderDirection' => $form['orderDirection'],
			   'minLimit' => $form['minLimit'],
			   'description' => $form['description'])
		    as $key => $value) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'rating', $key, $value);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['saved'] = 1;
	}

	$method = empty($error) ? 'redirect' : 'delegate';
	$results = array($method => array('view' => 'core.SiteAdmin',
					  'subView' => 'rating.RatingSiteAdmin'),
			 'status' => $status,
			 'error' => $error);

	return array(null, $results);
    }
}

/**
 * Settings for Ratings
 */
class RatingSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/rating/classes/RatingHelper.class');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'RatingSiteAdmin') {
	    foreach (array('allowAlbumRating', 'themeId', 'orderBy', 'orderDirection', 'minLimit',
			   'description') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'rating', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    /* Localize by casting to string */
	    $form['minLimit'] = (string)GalleryUtilities::castToFloat($form['minLimit']);
	    $tmp = explode('|', $form['orderBy'], 2);
	    if (count($tmp) < 2) {
		$form['presort'] = '';
	    } else {
		$form['orderBy'] = $tmp[1];
		$form['presort'] = $tmp[0];
	    }
	} else {
	    /* Reset theme selection on validation error for theme settings */
	    $form['themeId'] = $form['currentThemeId'];
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'rating');
	if ($ret) {
	    return array($ret, null);
	}

	/* Set up sort order selection list */
	GalleryCoreApi::requireOnce('modules/core/classes/GallerySortInterface_1_2.class');
	list ($ret, $orderByList, $presortList, $orderDirectionList) =
	    GallerySortInterface_1_2::getAllSortOrders();
	if ($ret) {
	    return array($ret, null);
	}

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

	$RatingSiteAdmin = array(
		'orderByList' => $orderByList,
		'presortList' => $presortList,
		'orderDirectionList' => $orderDirectionList,
		'themeList' => $themeList);

	/* Set up theme settings */
	list ($ret, $themeSettingsId) =
	    GalleryCoreApi::getPluginParameter('module', 'rating', 'themeSettingsId');
	if ($ret) {
	    return array($ret, null);
	}
	$ret = GalleryCoreApi::loadThemeSettingsForm($form['themeId'], $themeSettingsId,
						     $template, $form);
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'RatingSiteAdmin') {
	    $form['formName'] = 'RatingSiteAdmin';
	}

	$template->setVariable('RatingSiteAdmin', $RatingSiteAdmin);
	$template->setVariable('controller', 'rating.RatingSiteAdmin');
	return array(null, array('body' => 'modules/rating/templates/RatingSiteAdmin.tpl'));
    }
}
?>
