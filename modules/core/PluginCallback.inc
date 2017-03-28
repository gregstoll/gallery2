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
 * This view lets you make very simple callbacks to the framework to get very specific data.
 * Eventually this will probably get refactored into a much more sophisticated framework.
 *
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class PluginCallbackView extends GalleryView {
    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isControllerLike
     */
    function isControllerLike() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$session =& $gallery->getSession();
	$storage =& $gallery->getStorage();

	$command = GalleryUtilities::getRequestVariables('command');
	if (!headers_sent()) {
	    header("Content-type: text/plain; charset=UTF-8");
	}

	$result = array();
	list ($ret, $beforeStates) = $this->getPluginStates();
	if ($ret) {
	    $result['status'] = 'error';
	    $storage->rollbackTransaction();  /* ignore errors here */
	    $ret->putInSession();
	}

	if (!$ret) {
	    $ret = $this->handleCallback($command, $result);
	    if ($ret) {
		$result['status'] = 'error';
		$storage->rollbackTransaction();  /* ignore errors here */
		$ret->putInSession();
	    } else {
		/* Make sure this change is isolated from any potential failures we get below. */
		$ret = $storage->checkPoint();
		if ($ret) {
		    $result['status'] = 'error';
		    $ret->putInSession();
		}
	    }

	    if ($result['status'] == 'redirect') {
		$urlGenerator =& $gallery->getUrlGenerator();
		$result['redirect'] =
		    $urlGenerator->generateUrl($result['redirect'],
			array('htmlEntities' => 0, 'forceServerRelativeUrl' => 1));
	    }
	}

	if (!$ret) {
	    list ($ret, $afterStates) = $this->getPluginStates();
	    if ($ret) {
		$result['status'] = 'error';
		$storage->rollbackTransaction();  /* ignore errors here */
		$ret->putInSession();
	    } else {
		$result = array_merge(
		    $result, $this->calculateStateChanges($beforeStates, $afterStates));
	    }
	}

	GalleryCoreApi::requireOnce('lib/JSON/JSON.php');
	$json = new Services_JSON();
	print $json->encode($result);
	return null;
    }

    /**
     * Given two sets of states, figure out what's changed from before to after.
     *
     * @param array $beforeStates (moduleId => state, ...)
     * @param array $afterStates (moduleId => state, ...)
     * @return array changed states (moduleId => state, ...)
     * @static
     */
    function calculateStateChanges($beforeStates, $afterStates) {
	$states = array();
	$deleted = array();
	foreach (array('module', 'theme') as $type) {
	    foreach ($beforeStates[$type] as $moduleId => $state) {
		if (!isset($afterStates[$type][$moduleId])) {
		    $deleted[$type][$moduleId] = 1;
		} else if ($afterStates[$type][$moduleId] != $state) {
		    $states[$type][$moduleId] = $afterStates[$type][$moduleId];
		}
	    }
	}
	return array('states' => $states, 'deleted' => $deleted);
    }

    /**
     * Handle the specific callback, and store its result in the given output array.
     *
     * @param string $command (eg. "installModule")
     * @param array $result the location for result data to be sent back to the browser
     * @return GalleryStatus a status code
     * @static
     */
    function handleCallback($command, &$result) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return $ret;
	}

	$result = array();
	list ($pluginType, $pluginId) =
	    GalleryUtilities::getRequestVariables('pluginType', 'pluginId');

	list ($ret, $plugin) = GalleryCoreApi::loadPlugin($pluginType, $pluginId, true);
	if ($ret) {
	    return $ret;
	}

	list ($ret, $isActive) = $plugin->isActive();
	if ($ret) {
	    return $ret;
	}

	switch($command) {
	case 'activate':

	    if ($pluginType == 'module') {
		list ($ret, $needsConfiguration) = $plugin->needsConfiguration();
		if ($ret) {
		    return $ret;
		}
	    } else {
		/* Themes don't need configuration */
		$needsConfiguration = false;
	    }

	    if ($isActive || $needsConfiguration) {
		/* UI shouldn't let us come here anyway */
		$result['status'] = 'invalid';
		return null;
	    }

	    list ($ret, $redirect) = $plugin->activate();
	    if ($ret) {
		return $ret;
	    }

	    if ($redirect) {
		$result['status'] = 'redirect';
		$result['redirect'] = $redirect;
	    } else {
		$result['status'] = 'success';
	    }
	    break;

	case 'deactivate':
	    if (!$isActive) {
		/* UI shouldn't let us come here anyway */
		$result['status'] = 'invalid';
		return null;
	    }

	    if ($pluginType == 'theme') {
		list ($ret, $defaultThemeId) = GalleryCoreApi::getPluginParameter(
		    'module', 'core', 'default.theme');
		if ($ret) {
		    return $ret;
		}

		if ($plugin->getId() == $defaultThemeId) {
		    /* UI shouldn't let us come here anyway */
		    $result['status'] = 'invalid';
		}
	    }

	    if (empty($result['status'])) {
		list ($ret, $redirect) = $plugin->deactivate();
		if ($ret) {
		    return $ret;
		}

		if ($redirect) {
		    $result['status'] = 'redirect';
		    $result['redirect'] = $redirect;
		} else {
		    $result['status'] = 'success';
		}
	    }
	    break;

	case 'upgrade':
	case 'install':
	    $ret = $plugin->installOrUpgrade();
	    if ($ret) {
		return $ret;
	    }

	    if ($pluginType == 'module') {
		list ($ret, $autoConfigured) = $plugin->autoConfigure();
		if ($ret) {
		    return $ret;
		}
	    } else {
		/* Themes don't need this step */
		$autoConfigured = true;
	    }


	    if ($autoConfigured) {
		list ($ret, $redirect) = $plugin->activate();
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_CONFIGURATION_REQUIRED) {
			/*
			 * Some modules don't override autoConfigure which defaults to success.
			 * Show the "Modules needs configuration" message.
			 */
		    } else {
			return $ret;
		    }
		}

		if ($redirect) {
		    $result['status'] = 'redirect';
		    $result['redirect'] = $redirect;
		} else {
		    $result['status'] = 'success';
		}
	    } else {
		$result['status'] = 'success';
	    }

	    break;

	case 'uninstall':
	    if ($isActive) {
		list ($ret, $redirect) = $plugin->deactivate();
		if ($ret) {
		    return $ret;
		}
	    } else {
		$redirect = false;
	    }

	    if ($redirect) {
		$result['status'] = 'redirect';
		$results['redirect'] = $redirect;
	    } else {
		$ret = $plugin->uninstall();
		if ($ret) {
		    return $ret;
		}
		$result['status'] = 'success';
	    }
	    break;

	case 'delete':
	    if ($isActive) {
		list ($ret, $redirect) = $plugin->deactivate();
		if ($ret) {
		    return $ret;
		}
	    } else {
		$redirect = false;
	    }

	    if ($redirect) {
		$result['status'] = 'redirect';
		$results['redirect'] = $redirect;
	    } else {
		$ret = $plugin->uninstall();
		if ($ret) {
		    return $ret;
		}

		$path = sprintf(
		    "%s%ss/%s", GalleryCoreApi::getCodeBasePath(), $pluginType, $pluginId);
		$success = @$platform->recursiveRmdir($path);
		if (!$success) {
		    $result['status'] = 'fail';
		} else {
		    $ret = GalleryCoreApi::removeMapEntry(
			'GalleryPluginPackageMap',
			array('pluginType' => $pluginType, 'pluginId' => $pluginId));
		    if ($ret) {
			return $ret;
		    }

		    $result['status'] = 'success';
		}
	    }
	    break;

	case 'configure':
	    $result['status'] = 'redirect';
	    $result['redirect'] = array('view' => 'core.SiteAdmin',
					'subView' => $plugin->getConfigurationView());
	    break;
	}

	return null;
    }

    /**
     * Get the state ('active', 'inactive', 'uninstalled', etc) of all modules
     *
     * @return array GalleryStatus a status code
     *               array(moduleId => state, ...)
     * @static
     */
    function getPluginStates() {
	$states = array();

	foreach (array('module', 'theme') as $type) {
	    list ($ret, $pluginStatus) = GalleryCoreApi::fetchPluginStatus($type, true);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($pluginStatus as $pluginId => $status) {
		list ($ret, $plugin) = GalleryCoreApi::loadPlugin($type, $pluginId, true);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $states[$type][$pluginId]) =
		    $this->getPluginState($type, $plugin, $status);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	return array(null, $states);
    }

    /**
     * Get the state ('active', 'inactive', 'uninstalled', etc) of a given module
     *
     * @param string $type ('module' or 'theme')
     * @param GalleryPlugin $plugin
     * @param array $status status of the plugin (from GalleryCoreApi::fetchPluginStatus)
     * @return array GalleryStatus a status code
     *               string a state
     * @static
     */
    function getPluginState($type, $plugin, $status) {
	if ($type == 'module' && $plugin->getId() == 'core') {
	    return array(null, 'active');
	}

	$coreApiCompatible = GalleryUtilities::isCompatibleWithApi(
	    $plugin->getRequiredCoreApi(), GalleryCoreApi::getApiVersion());

	/* TODO: refactor this into type specific wrapper methods around getPluginState() */
	switch ($type) {
	case 'module':
	    $pluginApiCompatible = GalleryUtilities::isCompatibleWithApi(
		$plugin->getRequiredModuleApi(), GalleryModule::getApiVersion());
	    break;

	case 'theme':
	    $pluginApiCompatible = GalleryUtilities::isCompatibleWithApi(
		$plugin->getRequiredThemeApi(), GalleryTheme::getApiVersion());
	    break;
	}

	if ($coreApiCompatible && $pluginApiCompatible) {
	    if (empty($status['active'])) {
		$version = $status['version'];
		$state = 'inactive';

		/*
		 * If the database versions doesn't match the module
		 * version, we need to get the user to install the module.
		 */
		if ($version != $plugin->getVersion()) {
		    if (empty($version)) {
			$state = 'uninstalled';
		    } else {
			$state = 'unupgraded';
		    }
		} else {
		    if ($type == 'module') {
			/*
			 * The versions match, but the module can still demand
			 * to be configured before being activated.
			 */
			list ($ret, $needsConfig) = $plugin->needsConfiguration();
			if ($ret) {
			    return array($ret, null);
			}
		    } else {
			$needsConfig = false;
		    }

		    if ($needsConfig) {
			$state = 'unconfigured';
		    } else {
			$state = 'inactive';
		    }
		}
	    } else {
		$state = 'active';
	    }
	} else {
	    $state = 'incompatible';
	}

	return array(null, $state);
    }
}
?>
