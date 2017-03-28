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
 * This controller will handle an administration request for a theme
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17674 $
 */
class AdminThemesController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = $errorMessage = array();
	$theme = null;
	$themeId = GalleryUtilities::getRequestVariables('themeId');

	if (isset($form['action']['saveDefaults'])) {
	    /* Do some basic input validation (by whitelisting) */
	    GalleryCoreApi::requireOnce('modules/core/classes/GallerySortInterface_1_2.class');
	    list ($ret, $orderByList, $presortList, $orderDirectionList) =
		GallerySortInterface_1_2::getAllSortOrders(false);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (isset($form['default']['orderBy'])
		    && !isset($orderByList[$value = $form['default']['orderBy']])) {
		$error = GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
		    "Unknown sort order '$value'");
		return array($error, null);
	    }
	    if (isset($form['default']['presort'])
		    && !isset($presortList[$value = $form['default']['presort']])) {
		$error = GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
		   "Unknown presort order '$value'");
		return array($error, null);
	    }
	    if (isset($form['default']['orderDirection'])
		    && !isset($orderDirectionList[$value = $form['default']['orderDirection']])) {
		$error = GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
		    "Unknown order direction '$value'");
		return array($error, null);
	    }
	    if (isset($form['default']['theme'])) {
		list ($ret, $defaultTheme) =
		    GalleryCoreApi::loadPlugin('theme', $form['default']['theme'], false, true);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    if (isset($form['default']['newAlbumsUseDefaults']) &&
		    !in_array($form['default']['newAlbumsUseDefaults'], array('true', 'false'))) {
		$error = GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
		    'Unknown value for newAlbumDefaults ' .
		    $form['default']['newAlbumsUseDefaults']);
		return array($error, null);
	    }

	    /* Save the values */
	    foreach (array('default.orderBy',
			   'default.orderDirection',
			   'default.theme',
			   'default.newAlbumsUseDefaults')
		     as $key) {
		list ($outer, $inner) = explode('.', $key);
		if (isset($form[$outer][$inner])) {
		    $value = $form[$outer][$inner];
		    if ($key == 'default.orderBy' && !empty($form['default']['presort'])) {
			$value = $form['default']['presort'] . '|' . $value;
		    }
		    $ret = GalleryCoreApi::setPluginParameter('module', 'core', $key, $value);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    /* Switch to display the theme settings for the default theme if it has changed */
	    if (!empty($form['default']['theme'])) {
		$themeId = $form['default']['theme'];
	    }

	    /* Redirect upon success */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminThemes';
	    $redirect['themeId'] = $themeId;
	    $status['savedDefaults'] = 1;
	} else {
	    if (!empty($themeId)) {
		list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $themeId, false, true);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_PLUGIN_VERSION_MISMATCH &&
			isset($form['action']['upgrade'])) {
			list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $themeId, true);
			if ($ret) {
			    return array($ret, null);
			}
		    } else {
			return array($ret, null);
		    }
		}
	    }

	    if ($theme && isset($form['action']['saveTheme'])) {
		list ($ret, $isActive) = $theme->isActive();
		if ($ret) {
		    return array($ret, null);
		}

		/* Get all the possible settings */
		list ($ret, $settings) = $theme->getSettings(null);
		if ($ret) {
		    return array($ret, null);
		}

		/* Validate the settings */
		$validationErrors = $theme->validateSettings($form['key']);
		if (empty($validationErrors)) {
		    /* Save any settings that have changed */
		    foreach ($settings as $setting) {
			if ($setting['type'] == 'checkbox') {
			    $value = isset($form['key'][$setting['key']]) ? 1 : 0;
			} else {
			    $value = $form['key'][$setting['key']];
			}

			if (!empty($setting['typeParams']['packType'])) {
			    list ($success, $value) = $theme->packSetting(
				$setting['typeParams']['packType'], $value);
			}

			if ($setting['value'] !== $value) {
			    $ret = $theme->setParameter($setting['key'], $value);
			    if ($ret) {
				return array($ret, null);
			    }
			}
		    }

		    if ($theme->isAdvancedSettings()) {
			list ($ret, $error, $statusKey) = $theme->handleSettingsRequest($form);
			if ($ret) {
			    return array($ret, null);
			}
			if (empty($error)) {
			    $status[$statusKey] = $theme->translate($theme->getName());
			}
		    }

		    if (empty($error)) {
			/* Redirect upon success */
			$redirect['view'] = 'core.SiteAdmin';
			$redirect['subView'] = 'core.AdminThemes';
			$redirect['themeId'] = $themeId;
			$status['savedTheme'] = $theme->translate($theme->getName());
		    }
		} else {
		    foreach ($validationErrors as $validationKey => $validationMessage) {
			$error[] = sprintf('form[error][key][%s][invalid]', $validationKey);
			$errorMessage[$validationKey] = $validationMessage;
		    }
		    GalleryUtilities::putRequestVariable('form[errorMessage]', $errorMessage);
		}
	    } else if ($theme && isset($form['action']['undoTheme'])) {
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminThemes';
		$redirect['themeId'] = $themeId;
		$status['restoredTheme'] = $theme->translate($theme->getName());
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminThemes';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show administration options for available themes
 */
class AdminThemesView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $defaultThemeId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'default.theme');
	if ($ret) {
	    return array($ret, null);
	}

	$themeId = GalleryUtilities::getRequestVariables('themeId');
	if (empty($themeId)) {
	    $themeId = $defaultThemeId;
	}

	/* Load the theme list */
	list ($ret, $themes) = GalleryCoreApi::fetchPluginStatus('theme', true);
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * It's possible that we have some out-of-date themes which haven't been deactivated yet,
	 * since the deactivation only occurs when we try to load the theme.  Load all the active
	 * themes now to force the version check, then reload the theme status when we're done to
	 * pick up any changes we trigger.
	 */
	foreach ($themes as $id => $status) {
	    if (!empty($status['active'])) {
		list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $id, false, true);
		if ($ret && !($ret->getErrorCode() & ERROR_PLUGIN_VERSION_MISMATCH)) {
		    return array($ret, null);
		}
	    }
	}

	/* Reload the theme list, which may now be updated because of obsolete modules. */
	list ($ret, $themes) = GalleryCoreApi::fetchPluginStatus('theme', true);
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * Figure out how many active themes we have (because we don't allow
	 * deactivating the last one.
	 */
	$activeCount = 0;
	foreach (array_keys($themes) as $id) {
	    $activeCount += !empty($themes[$id]['active']);
	}

	/* Transform it to include all the theme data that we want to display */
	$themeList = array();
	foreach ($themes as $id => $status) {

	    list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $id, true);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Detect and remove old themes which not even had a setId() in their constructor */
	    $getId = $theme->getId();
	    if (empty($getId)) {
		unset($themes[$id]);
		continue;
	    }

	    $entry = array();
	    $entry['name'] = $theme->translate($theme->getName());
	    $entry['active'] = !empty($status['active']);
	    $entry['l10Domain'] = $theme->getL10Domain();
	    $entry['action'] = array();
	    $coreApiCompatible = GalleryUtilities::isCompatibleWithApi(
		$theme->getRequiredCoreApi(), GalleryCoreApi::getApiVersion());
	    $themeApiCompatible = GalleryUtilities::isCompatibleWithApi(
		$theme->getRequiredThemeApi(), GalleryTheme::getApiVersion());
	    if ($coreApiCompatible && $themeApiCompatible) {
		if (empty($status['active'])) {
		    $entry['state'] = 'inactive';

		    /*
		     * If the database versions doesn't match the theme
		     * version, we need to get the user to install the theme.
		     */
		    if ($status['version'] != $theme->getVersion()) {
			if (empty($status['version'])) {
			    $entry['action'][] =
				array('text' => $module->translate('install'),
				      'params' => array('controller' => 'core.AdminThemes',
							'themeId' => $id,
							'form[action][install]' => 1));
			    $entry['state'] = 'install';
			} else {
			    $entry['action'][] =
				array('text' => $module->translate('upgrade'),
				      'params' => array('controller' => 'core.AdminThemes',
							'themeId' => $id,
							'form[action][upgrade]' => 1));
			    $entry['state'] = 'upgrade';
			}
		    } else {
			$entry['action'][] =
			    array('text' => $module->translate('activate'),
				  'params' => array('controller' => 'core.AdminThemes',
						    'themeId' => $id,
						    'form[action][activate]' => 1));
			$entry['action'][] =
			    array('text' => $module->translate('uninstall'),
				  'params' => array('controller' => 'core.AdminThemes',
						    'themeId' => $id,
						    'form[action][uninstall]' => 1));
		    }
		} else {
		    $entry['state'] = 'active';
		    if ($id != $defaultThemeId) {
			$entry['action'][] =
			    array('text' => $module->translate('deactivate'),
				  'params' => array('controller' => 'core.AdminThemes',
						    'themeId' => $id,
						    'form[action][deactivate]' => 1));
		    }
		    if ($status['version'] != $theme->getVersion()) {
			$entry['action'][] =
			    array('text' => $module->translate('upgrade'),
				  'params' => array('controller' => 'core.AdminThemes',
						    'themeId' => $id,
						    'form[action][upgrade]' => 1));
			$entry['state'] = 'upgrade';
		    }
		}
	    } else {
		$entry['state'] = 'incompatible';
		$entry['api']['required']['theme'] = join('.', $theme->getRequiredThemeApi());
		$entry['api']['required']['core'] = join('.', $theme->getRequiredCoreApi());
		$entry['api']['provided']['theme'] = join('.', GalleryTheme::getApiVersion());
		$entry['api']['provided']['core'] = join('.', GalleryCoreApi::getApiVersion());
	    }

	    $entry['description'] = $theme->getDescription();
	    $entry['version'] = $theme->getVersion();
	    $entry['installedVersion'] = isset($status['version']) ? $status['version'] : null;
	    $themes[$id] = $entry;

	    if (!empty($status['active'])) {
		$themeList[$id] = $theme->translate($theme->getName());
	    }
	}

	$AdminThemes = array();
	/* Load up the stored defaults */
	foreach (array('default.orderBy',
		       'default.orderDirection',
		       'default.theme',
		       'default.theme',
		       'default.newAlbumsUseDefaults')
		 as $key) {

	    list ($ret, $value) =
		GalleryCoreApi::getPluginParameter('module', 'core', $key);
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($key == 'default.orderBy') {
		$tmp = explode('|', $value, 2);
		if (count($tmp) == 2) {
		    $value = $tmp[1];
		    $form['default']['presort'] = $tmp[0];
		} else {
		    $form['default']['presort'] = '';
		}
	    }

	    list ($outer, $inner) = explode('.', $key);
	    $form[$outer][$inner] = $value;
	}

	$newAlbumsUseDefaultsList = array(
	    'false' => $module->translate('Inherit settings from parent album'),
	    'true' => $module->translate('Use these settings')
	    );

	/* Set up our sort order selection list */
	GalleryCoreApi::requireOnce(
	    'modules/core/classes/GallerySortInterface_1_2.class');
	list ($ret, $orderByList, $presortList, $orderDirectionList) =
	    GallerySortInterface_1_2::getAllSortOrders(false);
	if ($ret) {
	    return array($ret, null);
	}

	$AdminThemes['orderByList'] = $orderByList;
	$AdminThemes['presortList'] = $presortList;
	$AdminThemes['orderDirectionList'] = $orderDirectionList;
	$AdminThemes['themeList'] = $themeList;
	$AdminThemes['newAlbumsUseDefaultsList'] = $newAlbumsUseDefaultsList;

	/* Load the config settings from the active theme */
	list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $themeId, false, true);
	if ($ret) {
	    if ($ret->getErrorCode()
		    & (ERROR_PLUGIN_VERSION_MISMATCH | ERROR_BAD_PARAMETER | ERROR_BAD_PLUGIN)) {
		/* The default theme isn't available. Show a warning, don't load any settings. */
		$theme = null;
		$form['error']['themeUnavailable'] = true;
	    } else {
		return array($ret, null);
	    }
	}

	if ($theme) {
	    if ($theme->isAdvancedSettings()) {
		list ($ret, $tpl) = $theme->loadSettingsTemplate($template, $form);
		if ($ret) {
		    return array($ret, null);
		}
		$AdminThemes['customTemplate'] = $tpl;
	    }

	    list ($ret, $AdminThemes['settings']) = $theme->getSettings(null);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $AdminThemes['availableBlocks']) = GalleryCoreApi::loadAvailableBlocks();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminThemes') {
	    $form['formName'] = 'AdminThemes';

	    $settingTypes = array();
	    if ($theme && isset($AdminThemes['settings'])) {
		foreach ($AdminThemes['settings'] as $setting) {
		    $key = $setting['key'];
		    if (!empty($setting['typeParams']['packType'])) {
			list ($success, $form['key'][$key]) = $theme->unpackSetting(
			    $setting['typeParams']['packType'], $setting['value']);
		    } else {
			$form['key'][$key] = $setting['value'];
		    }

		    $settingTypes[$setting['type']] = 1;
		}
	    }
        }

	if ($theme && isset($AdminThemes['settings'])) {
	     foreach ($AdminThemes['settings'] as $setting) {
		if ($setting['type'] == 'block-list') {
		    /* Query param is "version".. bump if data format in js changes */
		    $template->javascript('lib/javascript/BlockSelectWidget.js?1');
		    break;
		}
	     }
	}

	$AdminThemes['themes'] = $themes;
	$AdminThemes['themeId'] = $themeId;

	$template->setVariable('AdminThemes', $AdminThemes);
	$template->setVariable('controller', 'core.AdminThemes');
	return array(null, array('body' => 'modules/core/templates/AdminThemes.tpl'));
    }
}
?>
