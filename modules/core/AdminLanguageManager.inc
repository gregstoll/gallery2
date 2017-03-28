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
 * This controller will handle an administration request to add, update or remove a language.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Tim Almdal <tnalmdal@shaw.ca>
 * @version $Revision: 17768 $
 */
class AdminLanguageManagerController extends AdminRepositoryDownloadAndInstallController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$templateAdapter =& $gallery->getTemplateAdapter();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();

	if (!empty($form['save'])) {
	    /* Remove the reset languageActions */
	    if (!empty($form['languageAction'])) {
		foreach ($form['languageAction'] as $language => $action) {
		    if ($action == 'reset') {
			unset($form['languageAction'][$language]);
		    }
		}
	    }
	    /*
	     * We can update the general settings from 1 of two places: either here if there 
	     * were no local repositories; or as part of the progressBar call back.
	     */
	    if (!empty($form['languageAction'])) {
		$templateAdapter->registerTrailerCallback(array($this, 'updateLanguageSettings'),
		    array($form));
		$delegate['view'] = 'core.ProgressBar';
	    } else {
		list ($ret, $status['languageSettingsSaved']) = 
		    $this->_updateGeneralSettings($form);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	if (empty($delegate)) {
	    $results['redirect']['view'] = 'core.SiteAdmin';
	    $results['redirect']['subView'] = 'core.AdminLanguageManager';
	} else {
	    $results['delegate'] = $delegate;
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    /**
     * Callback method for updating the language settings while reporting progress.
     * @param array $form the form values
     * @return GalleryStatus a status code
     */
    function updateLanguageSettings($form) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$templateAdapter =& $gallery->getTemplateAdapter();

	$status = array('error' => array());

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return $ret;
	}
	$heading = $module->translate('Updating Language Settings');

	list ($ret, $status['languageSettingsSaved']) = $this->_updateGeneralSettings($form);
	if ($ret) {
	    return $ret;
	}

	list ($ret, $repositories, $repositoryInitErrorCount) = 
		GalleryRepository::getRepositories();
	if ($ret) {
	    return $ret;
	}

	if ($repositoryInitErrorCount) {
	    $status =& $template->getVariableByReference('status');
	    $status['error']['repositoryInitErrorCount'] = $repositoryInitErrorCount;
	}

	list ($ret, $installLanguages, $deleteLanguages) = 
	    $this->_getFormLanguages($form, $repositories);
	if ($ret) {
	    return $ret;
	}

	$sourceFiles = array();
	foreach ($repositories as $source => $repository) {
	    list ($ret, $sourceFiles[$source]) = 
		    $repository->getLanguagePackageFiles($installLanguages);
	    if ($ret) {
		return $ret;
	    }
	}

	$installLanguagesCount = $this->_countInstallActions($sourceFiles);

	/* Add one for the actual download */
	$totalActions = $installLanguagesCount + count($deleteLanguages) + 1;

	/* If actions > 0, then we have some language packages to work with */
	if ($totalActions > 0) {
	    $templateAdapter->updateProgressBar($heading, '', 0);

	    $callback = array('method' => array($templateAdapter, 'updateProgressBar'),
		'title' => $heading, 'current' => 0, 'total' => $totalActions);

	    if ($installLanguagesCount > 0) {
		list ($ret, $downloadStatus, $count) = $this->_downloadAndInstallLanguagePackages(
		    $sourceFiles, $repositories, $callback);
		if ($ret) {
		    return $ret;
		}
		if (!empty($downloadStatus)) {
		    $status['error'] = array_merge($status['error'], $downloadStatus);
		}

		$status['languagePacksInstalled'] = $count;
	    }

	    /* Delete old language packs */
	    list ($ret, $languagePacksDeleted, $failedToDeleteLanguage) = 
		$this->_deleteLanguagePackages($deleteLanguages, $callback);
	    if ($ret) {
		return $ret;
	    }
	    $status['languagePacksDeleted'] = $languagePacksDeleted;
	    if (!empty($failedToDeleteLanguage)) {
		$status['error']['failedToDeleteLanguage'] = $failedToDeleteLanguage;
	    }

	    $ret = $this->_synchronizeLanguagesByLocale($installLanguages, $callback);
	    if ($ret) {
		return $ret;
	    }
	} else {
	    $status['allLanguagesCurrent'] = 1;
	}

	/* Update progress bar. */
	if (!empty($status['error'])) {
	    $message = $module->translate('Update completed with errors.');
	} else {
	    $message = $module->translate('Update complete.');
	}
	$templateAdapter->updateProgressBar($heading, $message, 1);

	$redirect['view'] = 'core.SiteAdmin';
	$redirect['subView'] = 'core.AdminLanguageManager';

	$session =& $gallery->getSession();
	$session->putStatus($status);

	$urlGenerator =& $gallery->getUrlGenerator();
	$templateAdapter->completeProgressBar($urlGenerator->generateUrl($redirect), true);
	return $ret;
    }

    /**
     * Update the general settings (UseBrowserPref and default language)
     * @param array $form the form values
     * @return array GalleryStatus a status code
     *		     bool flag indicating that the settings were updated
     */
    function _updateGeneralSettings($form) {
	/* Handle general language settings. */
	$form['language']['useBrowserPref'] = !empty($form['language']['useBrowserPref']);

	list ($ret, $coreParameters) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	$updatedParms = false;
	foreach (array('default.language', 'language.useBrowserPref') as $key) {
	    list ($outer, $inner) = explode('.', $key);
	    if (isset($form[$outer][$inner])) {
		$value = $coreParameters[$key];

		if ($value == $form[$outer][$inner]) {
		    continue;
		}
		$ret = GalleryCoreApi::setPluginParameter('module', 'core', 
							  $key, $form[$outer][$inner]);
		if ($ret) {
		    return array($ret, null);
		}
		$updatedParms = true;
	    }
	}
	return array(null, $updatedParms);
    }

    /**
     * Parse the form and determine the locales that need to be installed or removed
     * @param array $form the form values
     * @param array $repositories an array of GalleryRepositories
     * @return array GalleryStatus a status code
     *		     array list of language packages to install
     * 		     array list of language packages to remove
     */
    function _getFormLanguages($form, $repositories) {
	global $gallery;
	$translator =& $gallery->getTranslator();

	list ($languageData) = $translator->getLanguageData();
	$locales = array('install' => array(), 'delete' => array());
	foreach ($form['languageAction'] as $language => $action) {
	    $actionType  = $action == 'remove' ? 'delete' : 'install';
	    if (in_array($language, array('en_GB', 'zh_CN', 'zh_TW'))) {
		$locales[$actionType][$language] = 1;
	    } else if (isset($languageData[$language])) {
		foreach(array_keys($languageData[$language]) as $country) {
		    list ($ret, $locale) = 
			$translator->getLanguageCodeFromLocale($language . '_' . $country);
		    if ($ret) {
			return array($ret, null, null);
		    }
		    $locales[$actionType][$locale] = 1;
		}
	    } else {
		/* Ignore bad language codes */
	    }
	}

	return array(null, array_keys($locales['install']), array_keys($locales['delete']));
    }

    /**
     * Delete the requested language packages
     * @param array $deleteLanguages list of packages to be deleted
     * @param array $callback progress notification callback
     * @return array GalleryStatus a status code
     * 		     int number of language packages deleted
     *		     array string status messages indicating problems encounterd
     */
    function _deleteLanguagePackages($deleteLanguages, $callback) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null, null);
	}

	$deleteText = $module->translate('Deleting Language Packs');
	$localeDir = $gallery->getConfig('data.gallery.locale');

	$languagesToDelete = array();
	$languagePacksDeleted = 0;

	$status = array();
	foreach ($deleteLanguages as $language) {
	    $dir = "$localeDir/$language";
	    if ($platform->file_exists($dir)) {
		$pattern = "$localeDir/$language/LC_MESSAGES/*.mo";
		$count = count($platform->glob($pattern));
		$languagePacksDeleted += $count;
		$callback['current'] += $count;
		$percentage = $callback['current'] / $callback['total'];
		call_user_func($callback['method'], $callback['title'], $deleteText, $percentage);

		if (!$platform->recursiveRmdir($dir)) {
		    $status[] = $language;
		}
	    }
	}
	return array(null, $languagePacksDeleted, $status);
    }

    /**
     * Download and install the language packages
     * @param array $sourceFiles list of files that were downloaded
     * @param array $repositories an array of GalleryRepositories
     * @param array $callback progress notification callback
     * @return array GalleryStatus a status code
     *		     array string status messages indicating problems encounterd
     * 		     int count of packages installed
     */
    function _downloadAndInstallLanguagePackages($sourceFiles, $repositories, &$callback) {
	$status = array();
	$count = 0;
	foreach ($sourceFiles as $source => $files) {
	    $repository = $repositories[$source];
	    $filesToDownload = array();
	    foreach ($files as $pluginType => $plugins) {
		foreach ($plugins as $pluginId => $pluginDownloadData) {
		    foreach ($pluginDownloadData['files'] as $key => $file) {
			$filesToDownload[] = $file;
		    }
		}
	    }

	    list($ret, $downloadStatus, $fileCount) = 
		$repository->downloadAndUnpackPackages($filesToDownload, $callback);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $count += $fileCount;
	    $status = array_merge($status, $downloadStatus);
	}

	return array(null, $status, $count);
    }

    /**
     * Synchronize the active language packages with the newly downloaded packages
     * @param array $installLanguages an array of locales that were installed.
     * @return GalleryStatus a status code
     */
    function _synchronizeLanguagesByLocale($installLanguages, &$callback) {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return $ret;
	}

	$synchronizeText = $module->translate('Synchronizing Language Packs');

	foreach ($installLanguages as $language) {
	    $percentage = ++$callback['current'] / $callback['total'];
	    call_user_func($callback['method'], $callback['title'], $synchronizeText, 
		$percentage);

	    $ret = GalleryCoreApi::installTranslationsForLocale($language);
	    if ($ret) {
		return $ret;
	    }
	}
	return null;
    }
}

/**
 * This view will show all repository-related features.
 */
class AdminLanguageManagerView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$translator =& $gallery->getTranslator();
	$session =& $gallery->getSession();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load some standard form parameters */
	if ($form['formName'] != 'AdminLanguages') {
	    /* Load up our form data */
	    foreach (array('default.language',
			   'language.useBrowserPref') as $key) {

		list ($ret, $value) =
		    GalleryCoreApi::getPluginParameter('module', 'core', $key);
		if ($ret) {
		    return array($ret, null);
		}

		list ($outer, $inner) = explode('.', $key);
		$form[$outer][$inner] = $value;
	    }

	    $form['formName'] = 'AdminLanguages';
	}

	/* Set up check boxes. */
	$form['language']['useBrowserPref'] = (int)!empty($form['language']['useBrowserPref']);

	$AdminLanguages = $languageList = array();

	$AdminLanguages['OS'] = strncasecmp(PHP_OS, 'win', 3) ? 'unix' : 'winnt';
	$AdminLanguages['basePath'] = GalleryCoreApi::getCodeBasePath();
	$AdminLanguages['writeable'] = array(
	    'modules' => $platform->is_writeable($AdminLanguages['basePath'] . '/modules'),
	    'themes' => $platform->is_writeable($AdminLanguages['basePath'] .'/themes'));

	list ($ret, $repositories, $repositoryInitErrorCount) = 
	    GalleryRepository::getRepositories();
	if ($ret) {
	    return array($ret, null);
	}

	/* Get list of upgradeable packages in repository */
	$upgradeInfo = $languagePackages = array();
	$enableAllFlags = array('upgrade' => false, 'remove' => false, 'download' => false);
	foreach ($repositories as $source => $repository) {
	    if (!$repository->localIndexExists()) {
		continue;
	    }

	    $AdminLanguages['indexMetaData'][$source] = 1;

	    list ($ret, $languagePackages) = $this->_getAllLanguages($repository, $enableAllFlags);
	    if ($ret) {
		return array($ret, null);
	    }
	    $upgradeInfo = array_merge($upgradeInfo, $languagePackages);
	}

	$AdminLanguages['canTranslate'] = $translator->canTranslate();
	$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	foreach ($supportedLanguages as $lang => $list) {
	    foreach ($list as $country => $languageData) {
		$locale = $lang . '_' . $country;
		$languageList[$locale] = $languageData['description'];
	    }
	}

	$AdminLanguages['languageUpgradeInfo'] = $languagePackages;
	$AdminLanguages['languageList'] = $languageList;
	$AdminLanguages['enableSelectAll'] = $enableAllFlags;
	$template->setVariable('AdminLanguages', $AdminLanguages);

	if ($repositoryInitErrorCount) {
	    $status =& $template->getVariableByReference('status');
	    $status['error']['repositoryInitErrorCount'] = $repositoryInitErrorCount;
	}

	/* Render the HTML body */
	$template->setVariable('controller', 'core.AdminLanguageManager');
	$template->javascript('lib/yui/utilities.js');
	return array(null, array('body' => 'modules/core/templates/AdminLanguageManager.tpl'));
    }

    /**
     * Creates a list of all upgradeable packages.
     *
     * It goes through all installed plugins and checks if an update is available to any of their
     * packages and adds it to a list if it is.
     * @param GalleryRepository $repository 
     * @param array $enableAllFlags flags indicating if the select all flag should be be enabled
     * @return array GalleryStatus a status code
     *		     array package list
      */
    function _getAllLanguages($repository, &$enableAllFlags) {
	global $gallery;
	$translator =& $gallery->getTranslator();

	list ($languageData, $defaultCountry) = $translator->getLanguageData();
	$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	$packages = array();

	foreach (array('module', 'theme') as $pluginType) {
	    list ($ret, $plugins) = GalleryCoreApi::getAllPluginIds($pluginType);
	    if ($ret) {
		return array($ret, null);
	    }

	    foreach ($plugins as $pluginId) {
		list ($ret, $upgradeInfo) = 
		    $repository->getPluginUpgradeInfo($pluginType, $pluginId);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_BAD_PARAMETER) {
			continue;
		    }
		    return array($ret, null);
		}
		$isCompatible = $upgradeInfo['base']['isCompatible'];
		foreach ($upgradeInfo['languages'] as $code => $pack) {
		    $gallery->guaranteeTimeLimit(60);
		    list ($ret, $language, $country) = 
			$translator->getLanguageAndCountryFromLanguageCode($code);
		    if ($ret) {
			return array($ret, null);
		    }
		    $installed = !empty($supportedLanguages[$language][$country]);
		    $enableAllFlags['remove'] = $enableAllFlags['remove'] || $installed;
		    $upgradeable = $isCompatible && $pack['relation'] == 'older';
		    if (in_array($code, array('zh_CN', 'zh_TW', 'en_GB'))) {
			$packages[$code] =  array('upgrade' => $upgradeable, 
			    'installed' => $installed,
			    'description' => $languageData[$language][$country]['description']);
			$language = $code;
		    } else {
			if (empty($packages[$language])) {
			    $country = $defaultCountry[$language];
			    $packages[$language] = array('upgrade' => $upgradeable,
				'installed' => $installed,
				'description' => $languageData[$language][$country]['description']);
			} else {
			    if ($packages[$language]['installed'] && !$installed) {
				$packages[$language]['upgrade'] = $isCompatible;
			    } else {
				$packages[$language]['upgrade'] = 
				    $packages[$language]['upgrade'] || $upgradeable;
			    }
			}
		    }
		    $enableAllFlags['upgrade'] = $enableAllFlags['upgrade'] 
			|| $packages[$language]['upgrade'];
		}
	    }
	}

	/* Add in the unistalled languages so the list is complete */
	foreach ($languageData as $language => $countries) {
	    $gallery->guaranteeTimeLimit(30);

	    if ($language == 'zh') {
		foreach ($countries as $country => $countryData) {
		    $locale = $language .'_' . $country;
		    if (empty($packages[$locale])) {
			$packages[$locale] = array('upgrade' => false, 
			    'installed' => false, 'description' => $countryData['description']);
		    }
		}
	    } else if ($language == 'en') {
		foreach ($countries as $country => $countryData) {
		    $locale = $language .'_' . $country;
		    if ($country != 'US') {
			if (empty($packages[$locale])) {
			    $packages[$locale] = array('upgrade' => false, 'installed' => false,
				'description' => $countryData['description']);
			}
		    }
		}
	    } else {
		if (empty($supportedLanguages[$language])) {
		    $packages[$language] = array('upgrade' => false, 'installed' => false,
			'description' => $countries[$defaultCountry[$language]]['description']);
		    $enableAllFlags['download'] = true;
		}
	    }
	}

	if (!empty($packages['en'])) {  /* don't show English(US) as a managed language */
	    unset($packages['en']);
	}
	ksort($packages);

	return array(null, $packages);
    }

    /**
     * Count the number of packages available for download from this repository
     * @param GalleryRepository $repository
     * @param array $langCodes to count the packages (should be all related i.e. es, es_MX, es_AR)
     * @return array GalleryStatus a status code
     *               int package count
     */
    function _determinePackageAvailability($repository, $langCodes) {
	global $gallery;
	$translator =& $gallery->getTranslator();

	list ($ret, $files) = $repository->getLanguagePackageFiles($langCodes);
	if ($ret) {
	    return array($ret, null);
	}
	$packageCount = 0;
	foreach ($files as $plugins) {
	    foreach ($plugins as $indexData) {
		$packageCount += count($indexData['files']);
		if (!empty($indexData['files']['descriptor'])) {
		    $packageCount--; /* don't include the descriptor in the count */
		}
	    }
	}
	return array(null, $packageCount);
    }
}
?>
