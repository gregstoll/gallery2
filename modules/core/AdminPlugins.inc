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

GalleryCoreApi::requireOnce('modules/core/classes/GalleryRepository.class');

/**
 * This view will show administration options for available plugins
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminPluginsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}
	$themeGroup = array('group' => 'theme', 'groupLabel' => $core->translate('Themes'));

	if ($form['formName'] != 'AdminPlugins') {
	    /* We have no form variables */
	    $form['formName'] = 'AdminPlugins';
	}

	foreach (array('module', 'theme') as $type) {
	    /* Load the module list */
	    list ($ret, $pluginStatus[$type]) = GalleryCoreApi::fetchPluginStatus($type, true);
	    if ($ret) {
		return array($ret, null);
	    }

	    /*
	     * It's possible that we have some out-of-date plugins which haven't been deactivated
	     * yet, since the deactivation only occurs when we try to load the module.  Load all
	     * the active plugins now to force the version check, then reload the plugin status
	     * when we're done to pick up any changes we trigger.
	     */
	    foreach ($pluginStatus[$type] as $pluginId => $status) {
		if (!empty($status['active'])) {
		    list ($ret, $module) =
			GalleryCoreApi::loadPlugin($type, $pluginId, false, true);
		    if ($ret && !($ret->getErrorCode() & ERROR_PLUGIN_VERSION_MISMATCH)) {
			return array($ret, null);
		    }
		}
	    }

	    /* Reload the plugin list, which may now be updated because of obsolete modules. */
	    list ($ret, $pluginStatus[$type]) = GalleryCoreApi::fetchPluginStatus($type, true);
	    if ($ret) {
		return array($ret, null);
	    }

	    $platform =& $gallery->getPlatform();
	    $g2Dir = dirname(dirname(dirname(__FILE__))) . '/';
	    GalleryCoreApi::requireOnce('modules/core/PluginCallback.inc');
	    foreach ($pluginStatus[$type] as $pluginId => $status) {
		list ($ret, $plugin) = GalleryCoreApi::loadPlugin($type, $pluginId, true);
		if ($ret) {
		    return array($ret, null);
		}

		$entry = array();
		$entry['type'] = $type;
		$entry['name'] = $plugin->translate($plugin->getName());
		$entry['id'] = $plugin->getId();
		$entry['description'] = $plugin->translate($plugin->getDescription());
		$entry['version'] = $plugin->getVersion();
		$entry['installedVersion'] = isset($status['version']) ? $status['version'] : null;
		$entry['deletable'] =
		    ($platform->is_writeable("$g2Dir{$type}s/$pluginId") &&
		     $platform->is_writeable("$g2Dir{$type}s/$pluginId/$type.inc")) ? 1 : 0;

		list ($ret, $entry['state']) =
		    PluginCallbackView::getPluginState($type, $plugin, $status);
		if ($ret) {
		    return array($ret, null);
		}

		$screenshot = "{$type}s/$pluginId/images/screenshot.png";
		if ($platform->file_exists($g2Dir . $screenshot)) {
		    $entry['screenshot'] = $screenshot;
		}

		if ($entry['state'] == 'incompatible') {
		    $entry['api']['required']['plugin'] =
			join('.', $type == 'theme' ?
			     $plugin->getRequiredThemeApi() : $plugin->getRequiredModuleApi());
		    $entry['api']['provided']['plugin'] = join('.', GalleryModule::getApiVersion());
		    $entry['api']['required']['core'] = join('.', $plugin->getRequiredCoreApi());
		    $entry['api']['provided']['core'] = join('.', GalleryCoreApi::getApiVersion());
		}

		if ($type == 'theme') {
		    $plugins[] = array_merge($entry, $themeGroup);
		} else {
		    $plugins[] = array_merge($entry, $plugin->getGroup());
		}
	    }
	}
	usort($plugins, array($this, 'pluginSort'));

	$AdminPlugins = array();
	$AdminPlugins['plugins'] = $plugins;
	list ($ret, $AdminPlugins['defaultTheme']) = GalleryCoreApi::getPluginParameter(
	    'module', 'core', 'default.theme');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $repositories, $repositoryInitErrorCount) =
	    GalleryRepository::getRepositories();
	if ($ret) {
	    return array($ret, null);
	}

	$AdminPlugins['showGetMorePluginsTip'] = 1;
	foreach ($repositories as $source => $repository) {
	    if ($repository->localIndexExists()) {
		$AdminPlugins['showGetMorePluginsTip'] = 0;
		break;
	    }
	}

	/* Only the codebase install can delete plugins, not multisites */
	$AdminPlugins['canDeletePlugins'] =
	    GALLERY_CONFIG_DIR == dirname(dirname(dirname(__FILE__)));

	$template->setVariable('AdminPlugins', $AdminPlugins);
	$template->setVariable('controller', 'core.AdminPlugins');
	$template->javascript('lib/yui/utilities.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('modules/core/templates/AdminPlugins.js');

	return array(null, array('body' => 'modules/core/templates/AdminPlugins.tpl'));
    }

    function pluginSort($a, $b) {
	static $groupOrder, $stateOrder;
	if (!isset($stateOrder)) {
	    /*
	     * unupgraded first, unconfigured second, incompatible third, inactive forth,
	     * active fifth, uninstalled last
	     */
	    $stateOrder = array('unupgraded' => 1, 'unconfigured' => 2, 'incompatible' => 3,
				'inactive' => 4, 'active' => 5, 'uninstalled' => 6);
	}
	if (!isset($groupOrder)) {
	    /* gallery first, toolkits second, other last */
	    $groupOrder = array('gallery' => 1, 'toolkits' => 2, '' => 3, 'other' => 4);
	}
	$ag = $a['group'];
	$bg = $b['group'];
	if ($ag != $bg) {
	    $ao = isset($groupOrder[$ag]) ? $groupOrder[$ag] : $groupOrder[''];
	    $bo = isset($groupOrder[$bg]) ? $groupOrder[$bg] : $groupOrder[''];
	    if ($ao != $bo) {
		return ($ao > $bo) ? 1 : -1;
	    }
	    $ag = isset($a['groupLabel']) ? $a['groupLabel'] : $ag;
	    $bg = isset($b['groupLabel']) ? $b['groupLabel'] : $bg;
	    return ($ag > $bg) ? 1 : -1;
	} else {
	    $as = $a['state'];
	    $bs = $b['state'];
	    $aso = $stateOrder[$as];
	    $bso = $stateOrder[$bs];
	    if($aso == $bso) {
	        $an = strtolower($a['name']);
	        $bn = strtolower($b['name']);
	        if ($an == $bn) {
	    	    return 0;
		} else {
		    return ($an > $bn) ? 1 : -1;
		}
	    } else {
		return ($aso > $bso) ? 1 : -1;
	    }
	}
    }
}
?>
