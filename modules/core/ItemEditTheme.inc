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
 * This plugin will handle the changes users make to theme settings for an album.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemEditTheme extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	if (isset($form['action']['chooseTheme'])) {
	    $error = array();
	    if (!empty($form['theme'])) {
		$themeId = $form['theme'];
		list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $themeId);
		if ($ret) {
		    return array($ret, null, null, null);
		}
	    } else {
		$themeId = null;
	    }

	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($item->getId());
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    list ($ret, $item) = $item->refresh();
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    $item->setSerialNumber($form['serialNumber']);
	    $item->setTheme($themeId);
	    $ret = $item->save();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null, null);
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    if (isset($form['changeInDescendents'])) {
		list ($ret, $subAlbumIds) =
		    GalleryCoreApi::fetchDescendentAlbumItemIds($item);
		if ($ret) {
		    return array($ret, null, null, null);
		}

		if (!empty($subAlbumIds)) {
		    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($subAlbumIds);
		    if ($ret) {
			return array($ret, null, null, null);
		    }

		    list ($ret, $subAlbums) =
			GalleryCoreApi::loadEntitiesById($subAlbumIds, 'GalleryAlbumItem');
		    if ($ret) {
			return array($ret, null, null, null);
		    }

		    foreach ($subAlbums as $album) {
			if (isset($form['changeInDescendents']['theme'])) {
			    $album->setTheme($form['theme']);
			}
			$ret = $album->save();
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return array($ret, null, null, null);
			}
		    }
		    $ret = GalleryCoreApi::releaseLocks($lockId);
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}
	    }

	    $status = $module->translate('Successfully changed theme');
	} else {
	    list ($ret, $themeId) = GalleryCoreApi::fetchThemeId($item);
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    list ($ret, $error, $status) = GalleryCoreApi::handleThemeSettingsRequest(
		$themeId, $item->getId(), $form);
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	$themeId = $themeSettingsThemeId = $item->getTheme();
	if (empty($themeSettingsThemeId)) {
	    list ($ret, $themeSettingsThemeId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'default.theme');
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	list ($ret, $theme) =
	    GalleryCoreApi::loadPlugin('theme', $themeSettingsThemeId, false, true);
	if (!$ret) {
	    $ret = GalleryCoreApi::loadThemeSettingsForm(
		$themeSettingsThemeId, $item->getId(), $template, $form);
	    if ($ret) {
		return array($ret, null, null);
	    }
	} else if ($ret->getErrorCode() & (ERROR_BAD_PARAMETER | ERROR_PLUGIN_VERSION_MISMATCH)) {
	    /* Ignore incompatible / missing themes.  Just show a warning. */
	} else {
	    return array($ret, null, null);
	}

	if ($form['formName'] != 'ItemEditTheme') {
	    $form['formName'] = 'ItemEditTheme';
	    $form['theme'] = $themeId;
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Set up our theme selection list */
	list ($ret, $themeStatusList) = GalleryCoreApi::fetchPluginStatus('theme');
	if ($ret) {
	    return array($ret, null, null);
	}
	$themeList = array('' => $module->translate('&laquo; default theme &raquo;'));
	foreach ($themeStatusList as $id => $status) {
	    if (empty($status['active'])) {
		continue;
	    }

	    list ($ret, $tmp) = GalleryCoreApi::loadPlugin('theme', $id, false, true);
	    if ($ret) {
		
		if ($ret->getErrorCode() & (ERROR_BAD_PARAMETER | ERROR_PLUGIN_VERSION_MISMATCH)) {
		    /* Ignore incompatible themes (but don't list them either) */
		    continue;
		}
	    	return array($ret, null, null);
	    }
	    $themeList[$id] = $tmp->translate($tmp->getName());
	}
	$ItemEditTheme['themeList'] = $themeList;
	$ItemEditTheme['theme'] = $themeId;

	$template->setVariable('controller', 'core.ItemEditTheme');
	$template->setVariable('ItemEditTheme', $ItemEditTheme);
	return array(null, 'modules/core/templates/ItemEditTheme.tpl', 'modules_core');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	return (GalleryUtilities::isA($item, 'GalleryAlbumItem'));
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Theme'));
    }
}
?>
