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

GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');

/**
 * Additional upgrade/event handling code
 * @package Rewrite
 * @author Douglas Cau <douglas@cau.se>
 * @version $Revision: 20954 $
 * @static
 */
class RewriteModuleExtras {

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($module, $currentVersion) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	switch ($currentVersion) {
	case null:
	    /* Initial install */
	    foreach (array('accessList' => serialize(array()),
			   'allowEmptyReferer' => '1',
			   'shortUrls' => serialize(array()),
			   'activeRules' => serialize(array()),
			   'parserId' => '',
			   'modrewrite.galleryLocation' => '',
			   'modrewrite.status' => serialize(array()),
			   'modrewrite.embeddedLocation' => '',
			   'modrewrite.embeddedHtaccess' => '',
			   'isapirewrite.forced' => '0',
			   'isapirewrite.galleryLocation' => '',
			   'isapirewrite.embeddedLocation' => '',
			   'isapirewrite.httpdini' => '',
			   'pathinfo.forced' => '0',
			   'pathinfo.parser' => serialize(array()))
		     as $key => $value) {
		$ret = $module->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	    break;

	case '0.0.1':
	case '0.0.2':
	case '0.0.3':
	case '0.0.4':
	case '0.0.5':
	case '0.1.0':
	case '0.1.1':
	case '0.1.2':
	case '0.1.3':
	case '0.1.4':
	case '0.1.5':
	case '0.1.6':
	case '0.1.7':
	case '0.1.8':
	case '0.1.9':
	case '0.1.10':
	case '0.8.0':
	case '0.8.1':
	case '0.9.0':
	case '0.9.1':
	case '0.9.2':
	    /* Reset Gallery location, needs to be reconfigured */
	    foreach (array('galleryLocation' => '',
			   'embeddedLocation' => '',
			   'embeddedHtaccess' => '')
		     as $key => $value) {
		$ret = $module->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }

	    /* The module needs to be reconfigured */
	    list ($ret) = $module->deactivate();
	    if ($ret) {
		return $ret;
	    }

	case '0.9.3':
	case '0.9.4':
	    foreach (array('accessList' => serialize(array()))
		     as $key => $value) {
		$ret = $module->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }

	case '0.9.5':
	    /*
	     * Previously we changed all module:View names to module.View.  Since we no longer store
	     * view names in the active rules list, and the old table is deleted in version 1.1.0 we
	     * no longer need to do this.
	     */

	case '0.9.6':
	case '0.9.7':
	case '0.9.8':
	case '0.9.9':
	case '0.9.10':
	    foreach (array('status' => serialize(array()))
		     as $key => $value) {
		$ret = $module->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }

	case '0.9.11':
	case '0.9.12':
	case '0.9.13':
	case '0.9.14':
	case '0.9.15':
	case '0.9.16':
	case '0.9.17':
	case '0.9.18':
	case '1.0.0':
	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	case '1.0.4':
	    /* Get the custom rules and save it to a plugin param (RewriteMap depricated) */
	    $query = '
		SELECT
		    [RewriteMap::module], [RewriteMap::ruleId], [RewriteMap::pattern]
		FROM
		    [RewriteMap]
		ORDER BY
		    [RewriteMap::module] ASC, [RewriteMap::ruleId] ASC
	    ';

	    /*
	     * If this is an upgrade from beta 2 we dont have a RewriteMap.  Previous to 0.9.3 rules
	     * were static, therefore leave $activeRules empty.  It'll get populated with the
	     * default rules when activate() is called.
	     */
	    $activeRules = array();
	    if (version_compare($currentVersion, '0.9.3', '>=')) {
		list ($ret, $searchResults) = $gallery->search($query);
		if ($ret) {
		    return $ret;
		}

		while ($result = $searchResults->nextResult()) {
		    $activeRules[$result[0]][$result[1]] = $result[2];
		}
	    }

	    $params = array('shortUrls' => serialize(array()),
			    'activeRules' => serialize($activeRules),
			    'allowEmptyReferer' => '1',
			    'parserId' => 'modrewrite',
			    'pathinfo.parser' => serialize(array()),
			    'pathinfo.forced' => '0',
			    'isapirewrite.forced' => '0',
			    'isapirewrite.galleryLocation' => '',
			    'isapirewrite.embeddedLocation' => '',
			    'isapirewrite.httpdini' => '');

	    /* Rename mod_rewrite specific parameters */
	    foreach (array('galleryLocation',
			   'status',
			   'embeddedLocation',
			   'embeddedHtaccess')
		     as $key) {
		list ($ret, $value) = $module->getParameter($key);
		if ($ret) {
		    return $ret;
		}

		$params['modrewrite.' . $key] = $value;
	    }

	    list ($ret, $value) = $module->getParameter('accessList');
	    if ($ret) {
		return $ret;
	    }
	    $params['accessList'] = $value;

	    /* Cleanup: Remove all old params */
	    $ret = GalleryCoreApi::removeAllPluginParameters('module', $module->getId());
	    if ($ret) {
		return $ret;
	    }

	    /* Set the new params */
	    foreach ($params as $key => $value) {
		$ret = $module->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }

	case '1.1.0':
	    /* Major GalleryUrlGenerator change */
	case '1.1.1':
	    /* GalleryCoreApi 7.0 and GalleryModule 3.0 */
	case '1.1.2':
	case '1.1.3':
	case '1.1.4':
	case '1.1.5':
	    /*
	     * Support rewrite rules with 'match' => array('controller' => 'webdav.WebDav').  Check
	     * if 'match' is subset of URL params, instead of indexing by "$view|$subView".  Means
	     * adding 'match' to $shortUrls.
	     */

	case '1.1.6':
	    /* Add activateRewriteRulesForModule to rewrite API */

	case '1.1.7':
	    /* Support rewrite rules without 'pattern' */
	    GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
	    list ($ret, $activeRules) = $module->getParameter('activeRules');
	    if ($ret) {
		return $ret;
	    }
	    $activeRules = unserialize($activeRules);

	    list ($ret, $modules) = GalleryCoreApi::fetchPluginStatus('module');
	    if ($ret) {
		return $ret;
	    }

	    /*
	     * Convert active rules from array($moduleId => array($ruleId => $pattern)) to
	     * array($moduleId => array($ruleId => array('pattern' => $pattern)))
	     */
	    foreach (array_keys($activeRules) as $moduleId) {
		if (!isset($modules[$moduleId]) || empty($modules[$moduleId]['available'])) {
		    /*
		     * Archive rules of unavailable modules and remove them from the active rules.
		     * Use the old history format, it will get converted in the below loop.
		     */
		    $ret = $module->setParameter(
			'history.' . $moduleId, serialize($activeRules[$moduleId]));
		    if ($ret) {
			return $ret;
		    }
		    unset($activeRules[$moduleId]);
		    continue;
		}

		list ($ret, $tmpModule) = GalleryCoreApi::loadPlugin('module', $moduleId, true);
		if ($ret) {
		    return $ret;
		}
		$rules = $tmpModule->getRewriteRules();

		foreach ($activeRules[$moduleId] as $ruleId => $pattern) {
		    $activeRules[$moduleId][$ruleId] = array();
		    /*
		     * Only save old pattern if the rule defines a pattern.  Saving a pattern for a
		     * rule which doesn't define a pattern is invalid.
		     */
		    if (isset($rules[$ruleId]['pattern'])) {
			$activeRules[$moduleId][$ruleId]['pattern'] = $pattern;
		    }
		}
	    }

	    /*
	     * Save active rules.  There should be no change in the interpretation of active rules,
	     * only the representation, so don't call $rewriteParser->saveActiveRules.
	     */
	    $ret = $module->setParameter('activeRules', serialize($activeRules));
	    if ($ret) {
		return $ret;
	    }

	    /*
	     * Convert history parameters from array($moduleId => array($ruleId => $pattern)) to
	     * array($moduleId => array($ruleId => array('pattern' => $pattern)))
	     */
	    list ($ret, $parameters) = $module->fetchParameters();
	    if ($ret) {
		return $ret;
	    }

	    foreach ($parameters as $key => $value) {
		if (strncmp($key, 'history.', 8) !== 0) {
		    continue;
		}
		$history = unserialize($value);
		$moduleId = substr($key, 8);

		$moduleAvailable = true;
		if (!isset($modules[$moduleId]) || empty($modules[$moduleId]['available'])) {
		    $moduleAvailable = false;
		} else {
		    list ($ret, $tmpModule) =
			GalleryCoreApi::loadPlugin('module', $moduleId, true);
		    if ($ret) {
			return $ret;
		    }
		    $rules = $tmpModule->getRewriteRules();
		}

		foreach ($history as $ruleId => $pattern) {
		    $history[$ruleId] = array();
		    /*
		     * Only save old pattern if the rule defines a pattern.  Saving a pattern for a
		     * rule which doesn't define a pattern is invalid.
		     */
		    if (!$moduleAvailable || isset($rules[$ruleId]['pattern'])) {
			$history[$ruleId]['pattern'] = $pattern;
		    }
		}

		$ret = $module->setParameter($key, serialize($history));
		if ($ret) {
		    return $ret;
		}
	    }

	case '1.1.8':
	    /* Remove L from mandatory flags, remove pattern for restrict rules */
	case '1.1.9':
	    /* Update required module API version for getRewriteRules change from 1.1.8 */

	case '1.1.10':
	    /* Update regex rules structure to reuse code from GalleryUrlGenerator */

	case '1.1.11':
	    /* Add generateUrl options and %{REQUEST_URI} handling to rewrite rule format */

	    /**
	     * Clear the tpl cache since our .htaccess and isapi templates have changed and the file
	     * modification timestamp is not guaranteed to be newer than the compiled files.
	     * @todo: Revisit when we have per module .tpl versioning.
	     */
	    $gallery->guaranteeTimeLimit(60);
	    $dir = $gallery->getConfig('data.smarty.templates_c');
	    if ($platform->file_exists($dir)) {
		$platform->recursiveRmdir($dir);
	    }

	case '1.1.12':
	case '1.1.13':
	case '1.1.13.1':
	case '1.1.13.2':
	    /* .mo file migration */
	case '1.1.13.3':
	    /* Gallery 2.2.4 security release */
	case '1.1.13.4':
	    /* Gallery 2.2.5 security release */
	case '1.1.14':
	case '1.1.15':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.1.16':
	case '1.1.17':
	case '1.1.18':

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					 sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::handleEvent
     */
    function handleDeactivatePlugin($module, $pluginId) {
	list ($ret, $activeRules) = $module->getParameter('activeRules');
	if ($ret) {
	    return $ret;
	}
	$activeRules = unserialize($activeRules);

	/* Don't update the rules if not necissary */
	if (isset($activeRules[$pluginId])) {
	    list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
	    if ($ret) {
		return $ret;
	    }

	    /* Save the old patterns for later use if module gets reactivated */
	    $ret = RewriteHelper::setHistoryForModule($pluginId, $activeRules[$pluginId]);
	    if ($ret) {
		return $ret;
	    }
	    unset($activeRules[$pluginId]);

	    /* Try to activate the new rules */
	    list ($ret) = $rewriteParser->saveActiveRules($activeRules, $module);
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::handleEvent
     */
    function handleUninstallPlugin($module, $pluginId) {
	$ret = RewriteHelper::setHistoryForModule($pluginId, array());
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::handleEvent
     */
    function handleActivatePlugin($module, $pluginId) {
	/* Get saved patterns */
	list ($ret, $plugin) = GalleryCoreApi::loadPlugin('module', $pluginId);
	if ($ret) {
	    return $ret;
	}
	$rules = $plugin->getRewriteRules();

	list ($ret, $history) = RewriteHelper::getHistoryForModule($pluginId);
	if ($ret) {
	    return $ret;
	}
	$ret = RewriteHelper::setHistoryForModule($pluginId, array());
	if ($ret) {
	    return $ret;
	}

	if (!empty($history)) {
	    list ($ret, $activeRules) = $module->getParameter('activeRules');
	    if ($ret) {
		return $ret;
	    }
	    $activeRules = unserialize($activeRules);
	    $originalSize = count($activeRules);

	    $seen = array();
	    foreach (array_keys($activeRules) as $moduleId) {
		foreach ($activeRules[$moduleId] as $ruleId => $rule) {
		    if (isset($rule['pattern'])) {
			$seen[$rule['pattern']] = true;
		    }
		}
	    }

	    foreach ($history as $ruleId => $rule) {
		if (isset($rules[$ruleId])) {
		    if (isset($rules[$ruleId]['pattern']) && isset($rule['pattern'])) {
			if (isset($seen[$rule['pattern']])) {
			    continue;
			}
			$seen[$rule['pattern']] = true;
		    } else {
			$rule = array();
		    }
		    $activeRules[$pluginId][$ruleId] = $rule;
		}
	    }

	    /* Only update the active rules if we added something */
	    if (count($activeRules) > $originalSize) {
		list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
		if ($ret) {
		    return $ret;
		}

		list ($ret) = $rewriteParser->saveActiveRules($activeRules, $module);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }
}
?>
