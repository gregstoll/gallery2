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
GalleryCoreApi::requireOnce('modules/core/classes/GalleryRepositoryUtilities.class');
GalleryCoreApi::requireOnce(
    'modules/core/classes/AdminRepositoryDownloadAndInstallController.class');

/**
 * This controller will handle an administration request for a module
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Jozef Selesi <selesi at gmail dot com>
 * @version $Revision: 17580 $
 */
class AdminRepositoryDownloadController extends AdminRepositoryDownloadAndInstallController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$translator =& $gallery->getTranslator();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();

	if (!isset($form['pluginType']) || !isset($form['pluginId'])) {
	    return array(GalleryCoreApi::error(
			     ERROR_BAD_PARAMETER, __FILE__, __LINE__,
			     "Plugin type or ID not set [$pluginType] [$pluginId]"),
			 null);
	}
	$pluginType = $form['pluginType'];
	$pluginId = $form['pluginId'];

	if (!in_array($pluginType, array('theme', 'module'))) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
					       "Invalid plugin type [$pluginType]"),
			 null);
	}

	/* Handle cancel action. */
	if (isset($form['action']['cancel'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminRepository';
	} else if (isset($form['action']['download'])) {
	    /* Create package list. */
	    $installPackages = array();

	    $utils = new GalleryRepositoryUtilities();
	    list ($ret, $pluginPackages) = $utils->getPluginPackages($pluginType, $pluginId);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (!empty($form['base'])) {
		list ($languages, $defaultCountry) = $translator->getLanguageData();
		list($baseSource, $baseNewBuild) = explode(':', $form['base']);
		$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
		if (empty($pluginPackages['base']['build']) ||
			$pluginPackages['base']['build'] != $baseNewBuild) {
		    $installPackages[$baseSource][$pluginType][$pluginId]['base'] = 1;
		    foreach (array_keys($pluginPackages) as $packageName) {
			if (preg_match('/^lang-(\w+)$/', $packageName, $matches)) {
			    list ($ret, $language, $country) =
				$translator->getLanguageAndCountryFromLanguageCode($matches[1]);
			    if (!empty($supportedLanguages[$language][$country])) {
				$installPackages[$baseSource][$pluginType][$pluginId][$packageName]
				     = 1;
			    }
			}
		    }
		}
	    }

	    /* Show error message if no packages have been selected for download. */
	    if (empty($installPackages)) {
		$delegate['view'] = 'core.SiteAdmin';
		$delegate['subView'] = 'core.AdminRepositoryDownload';
		$error[] = 'form[error][nothingSelected]';

		/** @todo: Do we need to put these vars back into the request? */
		GalleryUtilities::putRequestVariable('pluginId', $pluginId);
		GalleryUtilities::putRequestVariable('pluginType', $pluginType);
	    } else {
		list ($ret, $repositories) = GalleryRepository::getRepositories();
		if ($ret) {
		    return array(null, $ret);
		}

		$templateAdapter =& $gallery->getTemplateAdapter();
		$templateAdapter->registerTrailerCallback(
		    array($this, 'performDownloadAndInstallation'),
		    array($installPackages, $repositories));
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
class AdminRepositoryDownloadView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$translator =& $gallery->getTranslator();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminRepositoryDownload') {
	    $form['formName'] = 'AdminRepositoryDownload';
	}

	/* Init repository. */
	list ($ret, $repositories) = GalleryRepository::getRepositories();
	if ($ret) {
	    return array($ret, null);
	}

	list ($pluginType, $pluginId) =
	    GalleryUtilities::getRequestVariables('pluginType', 'pluginId');

	$AdminRepositoryDownload = array('pluginId' => $pluginId, 'pluginType' => $pluginType);

	$utils = new GalleryRepositoryUtilities();
	list ($ret, $pluginPackages) = $utils->getPluginPackages($pluginType, $pluginId);
	if ($ret) {
	    return array($ret, null);
	}

	list ($languages, $defaultCountry) = $translator->getLanguageData();

	foreach ($repositories as $source => $repository) {
	    if (!$repository->pluginExistsInIndex($pluginType, $pluginId)) {
		continue;
	    }

	    /* Downloading and upgrading plugins are only different in the UI */
	    list ($ret, $upgradeData) = $repository->getPluginUpgradeInfo($pluginType, $pluginId);
	    if ($ret) {
		return array($ret, null);
	    }

            if (!$upgradeData['base']['isCompatible']) {
		continue;
	    }

	    list ($ret, $upgradeData['pluginName']) =
		$repository->getPluginName($pluginType, $pluginId);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Treat all variations of a language as one */
	    $condensedData = array();
	    foreach ($upgradeData['languages'] as $langCode => $languageData) {
	    	if (in_array($langCode, array('zh_CN', 'zh_TW', 'en_GB'))) {
		    $condensedData[$langCode] = $languageData;
	    	} else {
		    list ($ret, $language) = 
			$translator->getLanguageAndCountryFromLanguageCode($langCode);
		    if ($ret) {
			return array($ret, null);
		    }
		    if (empty($condensedData[$language])) {
			$condensedData[$language] = $languageData;
		    } else {
			if ($condensedData[$language]['newBuild'] < $languageData['newBuild']) {
			    $condensedData[$language]['newBuild'] = $languageData['newBuild'];
			    $condensedData[$language]['relation'] = $languageData['relation'];
			}
		    }
		}
	    }
	    $upgradeData['languages'] = $condensedData;
	    $upgradeData['languageCount'] = count($upgradeData['languages']);
	    $upgradeData['repository'] = $source;
	    list ($ret, $upgradeData['repositoryName']) =
		GalleryRepository::translateRepositoryName($source);
	    if ($ret) {
		return array($ret, null);
	    }

	    $AdminRepositoryDownload['upgradeData'][] = $upgradeData;

	    if (empty($AdminRepositoryDownload['pluginName'])) {
		$AdminRepositoryDownload['pluginName'] = $upgradeData['pluginName'];
	    }
	}
	usort($AdminRepositoryDownload['upgradeData'], array($this, 'versionComparator'));

	if (!empty($AdminRepositoryDownload['upgradeData'][0]['base']['currentVersion'])) {
	    for ($i = 0; $i < count($AdminRepositoryDownload['upgradeData']); $i++) {
		if ($AdminRepositoryDownload['upgradeData'][$i]['base']['relation'] == 'equal') {
		    break;
		}
		if ($AdminRepositoryDownload['upgradeData'][$i]['base']['relation'] == 'older') {
		    $newEntry['pluginName'] = $AdminRepositoryDownload['pluginName'];
		    $newEntry['base'] = array(
			'newVersion' => $pluginPackages['base']['version'],
			'newBuild' => $pluginPackages['base']['build'],
			'currentVersion' => $pluginPackages['base']['version'],
			'currentBuild' => $pluginPackages['base']['build'],
			'relation' => 'equal');
		    $newEntry['repository'] = 'installed';
		    $newEntry['languages'] = array();
		    $newEntry['languageCount'] = 0;
		    array_splice($AdminRepositoryDownload['upgradeData'], $i, 0, array($newEntry));
		    break;
		}
	    }
	}

	$template->setVariable('AdminRepositoryDownload', $AdminRepositoryDownload);
	$template->setVariable('controller', 'core.AdminRepositoryDownload');
	$template->javascript('modules/core/templates/AdminRepositoryDownload.js');

	return array(null, array('body' => 'modules/core/templates/AdminRepositoryDownload.tpl'));
    }

    function versionComparator($a, $b) {
	$result = version_compare($a['base']['newVersion'], $b['base']['newVersion']);
	if (!$result) {
	    $result = version_compare($a['base']['newBuild'], $b['base']['newBuild']);
	}
	return $result;
    }
}
?>
