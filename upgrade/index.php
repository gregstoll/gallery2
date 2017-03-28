<?php
/*
 * ATTENTION:
 *
 * If you're seeing this in your browser, and are trying to upgrade Gallery,
 * you either do not have PHP installed, or if it is installed, it is not
 * properly enabled. Please visit the following page for assistance:
 *
 *    http://gallery.sourceforge.net/
 *
 * ----------------------------------------------------------------------------
 *
 * $Id: index.php 20960 2009-12-16 06:54:53Z mindless $
 *
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
 * Gallery Upgrader
 * @package Upgrade
 */

/* Show all errors. */
@ini_set('display_errors', 1);

/*
 * Disable magic_quotes runtime -- it causes problems with legitimate quotes
 * in our SQL, as well as reading/writing the config.php
 */
@ini_set('magic_quotes_runtime', 0);

$g2Base = dirname(dirname(__FILE__)) . '/';
require_once($g2Base . 'upgrade/UpgradeStep.class');
require_once($g2Base . 'upgrade/StatusTemplate.class');
require_once($g2Base . 'bootstrap.inc');
require_once($g2Base . 'modules/core/classes/GalleryUtilities.class');
require_once($g2Base . 'lib/support/GallerySetupUtilities.class');


/*
 * If gettext isn't enabled, subvert the _() text translation function
 * and just pass the string on through in English
 */
if (!function_exists('_')) {
    function _($s) {
	return $s;
    }
}

$error = false;

/* Our install steps, in order */
$stepOrder = array();
$stepOrder[] = 'Welcome';
$stepOrder[] = 'Authenticate';
$stepOrder[] = 'SystemChecks';
$stepOrder[] = 'DatabaseBackup';
$stepOrder[] = 'UpgradeCoreModule';
$stepOrder[] = 'UpgradeOtherModules';
$stepOrder[] = 'CleanCache';
$stepOrder[] = 'Finished';

foreach ($stepOrder as $stepName) {
    $className = $stepName . 'Step';
    require("steps/$className.class");
}

GallerySetupUtilities::startSession();

require_once(dirname(__FILE__) . '/../init.inc');
/* Check if config.php is ok */
$storageConfig = @$gallery->getConfig('storage.config');
if (!empty($storageConfig)) {
    /* We want to avoid using the cache */
    GalleryDataCache::setFileCachingEnabled(false);

    $ret = GalleryInitFirstPass(array('debug' => 'buffered', 'noDatabase' => 1));
    if ($ret) {
	print $ret->getAsHtml();
	return;
    }

    $translator =& $gallery->getTranslator();
    if (!$translator->canTranslate()) {
	unset($translator);
    } else {
	if (empty($_SESSION['language'])) {
	    $_SESSION['language'] = GalleryTranslator::getLanguageCodeFromRequest();
	}
	$translator->init($_SESSION['language'], true);
	/* Select domain for translation */
	bindtextdomain('gallery2_upgrade', dirname(dirname(__FILE__)) . '/locale');
	textdomain('gallery2_upgrade');
	if (function_exists('bind_textdomain_codeset')) {
	    bind_textdomain_codeset('gallery2_upgrade', 'UTF-8');
	}
	/* Set the appropriate charset in our HTTP header */
	if (!headers_sent()) {
	    header('Content-Type: text/html; charset=UTF-8');
	}
    }

    /* Preallocate at least 5 minutes for the upgrade */
    $gallery->guaranteeTimeLimit(300);

    /* Check to see if we have a database.  If we don't, then go to the installer */
    $storage =& $gallery->getStorage();
    list ($ret, $isInstalled) = $storage->isInstalled();
    if ($ret || !$isInstalled) {
	$error = true;
    }
} else {
    $error = true;
}

/* If we don't have our steps in our session, initialize them now. */
if (!isset($_GET['startOver']) && !empty($_SESSION['upgrade_steps'])) {
    $steps = unserialize($_SESSION['upgrade_steps']);
}

if (empty($steps) || !is_array($steps)) {
    $steps = array();
    for ($i = 0; $i < sizeof($stepOrder); $i++) {
	$className = $stepOrder[$i] . 'Step';
	$step = new $className();
	if ($step->isRelevant()) {
	    $step->setIsLastStep(false);
	    $step->setStepNumber($i);
	    $step->setInError(false);
	    $step->setComplete(false);
	    $steps[] = $step;
	}
    }

    /* Don't do this in the loop, since not all steps are relevant */
    $steps[sizeof($steps)-1]->setIsLastStep(true);
}

if (isset($_GET['step'])) {
    $stepNumber = (int)$_GET['step'];
} else {
    $stepNumber = 0;
}

/* Make sure all steps up to the current one are ok */
for ($i = 0; $i < $stepNumber; $i++) {
    if (!$steps[$i]->isComplete() && ! $steps[$i]->isOptional()) {
	$stepNumber = $i;
	break;
    }
}

if (!$error) {
    $currentStep =& $steps[$stepNumber];
} else {
    require_once(dirname(__FILE__) . '/steps/RedirectToInstallerStep.class');
    $currentStep = new RedirectToInstallerStep();
}

if (!empty($_GET['doOver'])) {
    $currentStep->setComplete(false);
}

/* If the current step is incomplete, the rest of the steps can't be complete either */
if (!$currentStep->isComplete()) {
    for ($i = $stepNumber+1; $i < sizeof($steps); $i++) {
	$steps[$i]->setComplete(false);
	$steps[$i]->setInError(false);
    }
}

if ($currentStep->processRequest()) {
    /* Load up template data from the current step */
    $templateData = array();

    /* Round percentage to the nearest 5 */
    $templateData['errors'] = array();
    $currentStep->loadTemplateData($templateData);

    /* Render the output */
    $template = new StatusTemplate();
    $template->renderHeaderBodyAndFooter($templateData);
}

/**
 * Find admin user and set as active user
 * @param bool $fallback (optional) whether we should try to fall back if the
 *             API to load the admin user object fails
 * @return GalleryStatus a status code
 */
function selectAdminUser($fallback=false) {
    global $gallery;

    list ($ret, $siteAdminGroupId) =
	GalleryCoreApi::getPluginParameter('module', 'core', 'id.adminGroup');
    if ($ret) {
	return $ret;
    }
    list ($ret, $adminUserInfo) = GalleryCoreApi::fetchUsersForGroup($siteAdminGroupId, 1);
    if ($ret) {
	return $ret;
    }
    if (empty($adminUserInfo)) {
	return GalleryCoreApi::error(ERROR_MISSING_VALUE);
    }
    /* Fetch the first admin from list */
    list ($userId, $userName) = each($adminUserInfo);
    list ($ret, $adminUser) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
    if ($ret) {
	if ($fallback) {
	    /* Initialize a GalleryUser with the id of a real admin */
	    $gallery->debug('Unable to load admin user. Using in-memory user object as fallback');
	    GalleryCoreApi::requireOnce('modules/core/classes/GalleryUser.class');
	    $adminUser = new GalleryUser();
	    $adminUser->setId((int)$userId);
	    $adminUser->setUserName($userName);
	} else {
	    return $ret;
	}
    }

    $gallery->setActiveUser($adminUser);
    $session =& $gallery->getSession();
    $session->put('isUpgrade', true);
    return null;
}

/**
 * Mini url generator for upgrader
 */
function generateUrl($uri, $print=true) {
    if (strncmp($uri, 'index.php', 9) && strncmp($uri, '../' . GALLERY_MAIN_PHP, 11)) {
	/* upgrade/images/*, upgrade/styles/*, ... URLs */
	global $gallery;
	/* Add @ here in case we haven't yet upgraded config.php to include galleryBaseUrl */
	$baseUrl = @$gallery->getConfig('galleryBaseUrl');
	if (!empty($baseUrl)) {
	     $uri = $baseUrl . 'upgrade/' . $uri;
	}
    } else if (!strncmp($uri, 'index.php', 9)) {
	/* If session.use_trans_sid is on then it will add the session id. */
	if (!GallerySetupUtilities::areCookiesSupported() && !ini_get('session.use_trans_sid')) {
	    /*
	     * Don't use SID since it's a constant and we change (regenerate) the session id
	     * in the request
	     */
	    $sid = session_name() . '=' . session_id();
	    $uri .= !strpos($uri, '?') ? '?' : '&amp;';
	    $uri .= $sid;
	}
    }

    if ($print) {
	print $uri;
    }
    return $uri;
}

/*
 * We don't store the steps in the session in raw form because that
 * will break in environments where session.auto_start is on since
 * it will try to instantiate the classes before they've been defined
 */
$_SESSION['upgrade_steps'] = serialize($steps);
?>
