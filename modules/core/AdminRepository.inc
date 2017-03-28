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
GalleryCoreApi::requireOnce(
    'modules/core/classes/AdminRepositoryDownloadAndInstallController.class');

/**
 * This controller will handle an administration request for a module
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Jozef Selesi <selesi at gmail dot com>
 * @version $Revision: 17580 $
 */
class AdminRepositoryController extends AdminRepositoryDownloadAndInstallController {

    /**
     * Get the repositories, but allow tests to inject their own version.
     * @see GalleryRepository::getRepositories
     * @access private
     */
    function _getRepositories() {
	if (isset($this->_repositories)) {
	    return array(null, $this->_repositories);
	}
	return GalleryRepository::getRepositories();
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['update'])) {
	    /**
	     * @todo refactor this repository update such that it can be used from Language
	     * manager.
	     */
	    foreach (array('module', 'theme') as $pluginType) {
		list ($ret, $pluginStatus[$pluginType]) =
		    GalleryCoreApi::fetchPluginStatus($pluginType, true);
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    if (empty($form['repositories'])) {
		$form['repositories'] = array('released' => 1);
	    }

	    /* filter the form values through our list of valid keys to avoid exploits */
	    $data = array();
	    foreach (array('released', 'experimental', 'community') as $key) {
		if (isset($form['repositories'][$key])) {
		    $data[$key] = 1;
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'core', 'core.repositories', serialize($data));
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Remove the default repositories from the data cache */
	    GalleryRepository::clearRepositoryCache();
	    list ($ret, $repositories) = $this->_getRepositories();
	    if ($ret) {
		return array($ret, null);
	    }

	    $templateAdapter =& $gallery->getTemplateAdapter();

	    $templateAdapter->registerTrailerCallback(
		array($this, 'updatePluginStatus'),
		array($pluginStatus, $repositories, 
		      array('view' => 'core.SiteAdmin', 'subView' => 'core.AdminRepository')));
	    $delegate['view'] = 'core.ProgressBar';
	} else if (isset($form['action']['upgradeAll'])) {
	    list ($ret, $repositories) = $this->_getRepositories();
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Get list of upgradeable packages in repository */
	    foreach ($repositories as $source => $repository) {
		list ($ret, $tmp) = $repository->getAllUpgradeablePackages();
		if ($ret) {
		    return array($ret, null);
		}

		if (!empty($tmp)) {
		    $packages[$source] = $tmp;
		}
	    }

	    if (empty($packages)) {
		$status['noUpgradeAvailable'] = 1;
	    } else {
		/* Start upgrade process and show progress bar. */
		GalleryCoreApi::requireOnce('modules/core/AdminRepositoryDownload.inc');
		$templateAdapter =& $gallery->getTemplateAdapter();
		$templateAdapter->registerTrailerCallback(
		    array($this, 'performDownloadAndInstallation'),
		    array($packages, $repositories));
		$delegate['view'] = 'core.ProgressBar';
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    if (empty($delegate)) {
		$results['delegate']['view'] = 'core.SiteAdmin';
		$results['delegate']['subView'] = 'core.AdminRepository';
	    } else {
		$results['delegate'] = $delegate;
	    }
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

}

/**
 * This view will show all repository-related features.
 */
class AdminRepositoryView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$session =& $gallery->getSession();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$AdminRepository = array();

	/* Init repository. */
	list ($ret, $repositories, $repositoryInitErrorCount) =
	    GalleryRepository::getRepositories();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminRepository') {
	    /* Set some defaults */
	    $form['formName'] = 'AdminRepository';
	    list ($ret, $sources) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'core.repositories');
	    if ($ret) {
		return array($ret, null);
	    }
	    @$form['repositories'] = unserialize($sources);
	}
	if (empty($form['repositories'])) {
	    $form['repositories'] = array('released' => 1);
	}

	/* Check if incompatible plugins should be shown. */
	$coreApis = array();
	$showIncompatible = GalleryUtilities::getRequestVariables('showIncompatible');
	if ($showIncompatible == 'true') {
	    list ($coreApiVersion, $themeApiVersion, $moduleApiVersion) =
		GalleryUtilities::getRequestVariables('coreApi', 'themeApi', 'moduleApi');
	    if (empty($coreApiVersion) || empty($themeApiVersion) || empty($moduleApiVersion)) {
		return array(GalleryCoreApi::error(
				 ERROR_BAD_PARAMETER, __FILE__, __LINE__,
				 "[$coreApiVersion] [$themeApiVersion] [$moduleApiVersion]"),
			     null);
	    }
	    $coreApis['core'] = explode('.', $coreApiVersion);
	    $coreApis['module'] = explode('.', $moduleApiVersion);
	    $coreApis['theme'] = explode('.', $themeApiVersion);
	    $AdminRepository['showIncompatible'] = 1;
	}

	$AdminRepository['OS'] = strncasecmp(PHP_OS, 'win', 3) ? 'unix' : 'winnt';

	$AdminRepository['isCoreUpgradeAvailable'] = false;
	$AdminRepository['upgradesAvailable'] = false;
	foreach ($repositories as $source => $repository) {
	    if (!$repository->localIndexExists()) {
		break;
	    }

	    /**
	     * Note: this merges modules and themes instead of treating themes separately.
	     * @todo: separate themes into their own group, like we do elsewhere
	     */
	    foreach (array('module', 'theme') as $pluginType) {
		/* Get list of plugins to show in the repository browser. */
		list ($ret, $browseData[$pluginType]) = $repository->getRepositoryPluginList(
		    $pluginType, $showIncompatible, $coreApis);
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($browseData[$pluginType] as $pluginId => $browseInfo) {
		    if (empty($AdminRepository['browseData'][$pluginId]) ||
			(empty($AdminRepository['browseData'][$pluginId]['action']) &&
			 !empty($browseInfo['action'])) ||
			version_compare(
			    $browseInfo['repositoryVersion'],
			    $AdminRepository['browseData'][$pluginId]['repositoryVersion'],
			    '>')) {
			$AdminRepository['browseData'][$pluginId] = $browseInfo;
		    }
		}
	    }

	    /* Get local index meta data. */
	    list ($ret, $indexMetaData) = $repository->getIndexMetaData();
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $message) = GalleryRepository::translateRepositoryName($source);
	    if ($ret) {
		return array($ret, null);
	    }

	    $AdminRepository['indexMetaData'][$source] =
		array_merge(array('name' => $message), $indexMetaData);

	    list ($ret, $tmp) = $repository->getAllUpgradeablePackages();
	    if ($ret) {
		return array($ret, null);
	    }

	    if (!empty($tmp)) {
		$AdminRepository['upgradesAvailable'] = true;
	    }

	    /* Get core upgrade info. */
	    if ($source == 'released') {
		list ($ret, $AdminRepository['isCoreUpgradeAvailable'], $apiVersions) =
		    $repository->isCoreUpgradeAvailable();
		if ($ret) {
		    return array($ret, null);
		}
		if ($AdminRepository['isCoreUpgradeAvailable']) {
		    $AdminRepository['latestCoreApiVersion'] = $apiVersions['core'];
		    $AdminRepository['latestThemeApiVersion'] = $apiVersions['theme'];
		    $AdminRepository['latestModuleApiVersion'] = $apiVersions['module'];
		}
	    }
	}

	if (!empty($AdminRepository['browseData'])) {
	    GalleryCoreApi::requireOnce('modules/core/AdminPlugins.inc');
	    uasort($AdminRepository['browseData'], array($this, 'pluginSort'));
	}

	$AdminRepository['basePath'] = dirname(dirname(dirname(__FILE__)));
	$AdminRepository['writeable'] = array(
	    'modules' => $platform->is_writeable($AdminRepository['basePath'] . '/modules'),
	    'themes' => $platform->is_writeable($AdminRepository['basePath'] .'/themes'));
	$AdminRepository['authToken'] = $session->getAuthToken();

	/* Render the HTML body */
	$template->setVariable('AdminRepository', $AdminRepository);
	$template->setVariable('controller', 'core.AdminRepository');

	if ($repositoryInitErrorCount) {
	    $status =& $template->getVariableByReference('status');
	    $status['error']['repositoryInitErrorCount'] = $repositoryInitErrorCount;
	}

	$template->javascript('lib/yui/utilities.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('modules/core/templates/AdminRepository.js');
	return array(null, array('body' => 'modules/core/templates/AdminRepository.tpl'));
    }

    /**
     * @todo: combine this with AdminPluginsView::pluginSort
     */
    function pluginSort($a, $b) {
	if (($cmp = strcmp($a['groupLabel'], $b['groupLabel'])) == 0) {
	    $cmp = strcmp($a['name'], $b['name']);
	}
	return $cmp;
    }
}
?>
