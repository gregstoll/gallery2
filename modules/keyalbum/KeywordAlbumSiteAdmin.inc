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
 * Settings for Keyword Albums
 * @package KeyAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class KeywordAlbumSiteAdminController extends GalleryController {

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
	    if (empty($form['split'])) {
		$error[] = 'form[error][split]';
	    }
	}
	if (isset($form['action']['save']) && empty($error)) {
	    list ($ret, $themeSettingsId) =
		GalleryCoreApi::getPluginParameter('module', 'keyalbum', 'themeSettingsId');
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
	    $split = '';
	    $map = array('semicolon' => ';', 'comma' => ',', 'space' => '\\s');
	    foreach ($form['split'] as $key => $unused) {
		if (isset($map[$key])) {
		    $split .= $map[$key];
		}
	    }
	    foreach (array('themeId' => $form['themeId'],
			   'orderBy' => (empty($form['presort']) ? '' : $form['presort'] . '|')
					. $form['orderBy'],
			    'orderDirection' => !isset($form['orderDirection']) ? ''
						: $form['orderDirection'],
			    'summaryLinks' => $form['summaryLinks'],
			    'description' => $form['description'],
			    'split' => $split)
		    as $key => $value) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'keyalbum', $key, $value);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['saved'] = 1;
	}

	$method = empty($error) ? 'redirect' : 'delegate';
	$results = array($method => array('view' => 'core.SiteAdmin',
					  'subView' => 'keyalbum.KeywordAlbumSiteAdmin'),
			 'status' => $status,
			 'error' => $error);

	return array(null, $results);
    }
}

/**
 * Settings for Keyword Albums
 */
class KeywordAlbumSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'KeywordAlbumSiteAdmin') {
	    foreach (array('themeId', 'orderBy', 'orderDirection', 'summaryLinks', 'description',
			   'split') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'keyalbum', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $tmp = explode('|', $form['orderBy'], 2);
	    if (count($tmp) < 2) {
		$form['presort'] = '';
	    } else {
		$form['orderBy'] = $tmp[1];
		$form['presort'] = $tmp[0];
	    }
	    $map = array(';' => 'semicolon', ',' => 'comma', 's' => 'space');
	    $split = $form['split'];
	    $form['split'] = array();
	    for ($i = 0; $i < strlen($split); $i++) {
		if (isset($map[$split{$i}])) {
		    $form['split'][$map[$split{$i}]] = 1;
		}
	    }
	} else {
	    /* Reset theme selection on validation error for theme settings */
	    $form['themeId'] = $form['currentThemeId'];
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'keyalbum');
	if ($ret) {
	    return array($ret, null);
	}

	$summaryList = array(
		'all' => $module->translate('All album views'),
		'album' => $module->translate('Standard albums'),
		'key' => $module->translate('Keyword albums'),
		'none' => $module->translate('None'));

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

	$KeywordAlbumSiteAdmin = array(
		'summaryList' => $summaryList,
		'orderByList' => $orderByList,
		'presortList' => $presortList,
		'orderDirectionList' => $orderDirectionList,
		'themeList' => $themeList);

	/* Set up theme settings */
	list ($ret, $themeSettingsId) =
	    GalleryCoreApi::getPluginParameter('module', 'keyalbum', 'themeSettingsId');
	if ($ret) {
	    return array($ret, null);
	}
	$ret = GalleryCoreApi::loadThemeSettingsForm($form['themeId'], $themeSettingsId,
						     $template, $form);
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'KeywordAlbumSiteAdmin') {
	    $form['formName'] = 'KeywordAlbumSiteAdmin';
	}

	$template->setVariable('KeywordAlbumSiteAdmin', $KeywordAlbumSiteAdmin);
	$template->setVariable('controller', 'keyalbum.KeywordAlbumSiteAdmin');
	return array(null, array('body' => 'modules/keyalbum/templates/KeywordAlbumSiteAdmin.tpl'));
    }
}
?>
