<?php
/*
 * ATTENTION:
 *
 * If you're seeing this in your browser, and are trying to install Gallery,
 * you either do not have PHP installed, or if it is installed, it is not
 * properly enabled. Please visit the following page for assistance:
 *
 *    http://gallery.sourceforge.net/
 *
 * ----------------------------------------------------------------------------
 *
 * $Id: index.php 17580 2008-04-13 00:38:13Z tnalmdal $
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
 * Gallery Installer
 * @package Install
 */

/* Show all errors. */
@ini_set('display_errors', 1);

/*
 * Disable magic_quotes runtime -- it causes problems with legitimate quotes
 * in our SQL, as well as reading/writing the config.php
 */
@ini_set('magic_quotes_runtime', 0);

$g2Base = dirname(dirname(__FILE__)) . '/';
require_once($g2Base . 'install/GalleryStub.class');
require_once($g2Base . 'install/InstallStep.class');
require_once($g2Base . 'install/StatusTemplate.class');
require_once($g2Base . 'modules/core/classes/GalleryUtilities.class');
require_once($g2Base . 'modules/core/classes/GalleryDataCache.class');
require_once($g2Base . 'lib/support/GallerySetupUtilities.class');
define('INDEX_PHP', basename(__FILE__));

/*
 * If gettext isn't enabled, subvert the _() text translation function
 * and just pass the string on through in English
 */
if (!function_exists('_')) {
    function _($s) {
	return $s;
    }
}

/* Our install steps, in order */
$stepOrder = array();
$stepOrder[] = 'Welcome';
$stepOrder[] = 'Authenticate';
$stepOrder[] = 'SystemChecks';
$stepOrder[] = 'Multisite';
$stepOrder[] = 'StorageSetup';
$stepOrder[] = 'DatabaseSetup';
$stepOrder[] = 'AdminUserSetup';
$stepOrder[] = 'CreateConfigFile';
$stepOrder[] = 'InstallCoreModule';
$stepOrder[] = 'InstallOtherModules';
$stepOrder[] = 'Secure';
$stepOrder[] = 'Finished';

foreach ($stepOrder as $stepName) {
    $className = $stepName . 'Step';
    require("steps/$className.class");
}

GallerySetupUtilities::startSession();

require_once($g2Base . 'modules/core/classes/GalleryStatus.class');
require_once($g2Base . 'modules/core/classes/GalleryTranslator.class');
if (empty($_SESSION['language'])) {
    /* Select language based on preferences sent from browser */
    $_SESSION['language'] = GalleryTranslator::getLanguageCodeFromRequest();
}
if (function_exists('dgettext')) {
    $gallery = new GalleryStub();
    $translator = new GalleryTranslator();
    $translator->init($_SESSION['language'], true);
    unset($gallery);
    bindtextdomain('gallery2_install', dirname(dirname(__FILE__)) . '/locale');
    textdomain('gallery2_install');
    if (function_exists('bind_textdomain_codeset')) {
	bind_textdomain_codeset('gallery2_install', 'UTF-8');
    }
    /* Set the appropriate charset in our HTTP header */
    if (!headers_sent()) {
	header('Content-Type: text/html; charset=UTF-8');
    }
}

/*
 * If register_globals is on then a global $galleryStub may have already been created.
 * Clear it here and initialize ourselves.
 */
unset($galleryStub);

if (!isset($_GET['startOver']) && !empty($_SESSION['install_steps'])) {
    $steps = unserialize($_SESSION['install_steps']);
    if (isset($_SESSION['galleryStub'])) {
	$galleryStub = unserialize($_SESSION['galleryStub']);
    }
}

/* If we don't have our steps in our session, initialize them now. */
if (empty($steps) || !is_array($steps)) {
    $steps = array();
    for ($i = 0; $i < count($stepOrder); $i++) {
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
    $steps[count($steps)-1]->setIsLastStep(true);
}

$stepNumber = isset($_GET['step']) ? (int)$_GET['step'] : 0;

/* Make sure all steps up to the current one are ok */
for ($i = 0; $i < $stepNumber; $i++) {
    if (!$steps[$i]->isComplete() && !$steps[$i]->isOptional()) {
	$stepNumber = $i;
	break;
    }
}
$currentStep =& $steps[$stepNumber];

if (!empty($_GET['doOver'])) {
    $currentStep->setComplete(false);
}

/* If the current step is incomplete, the rest of the steps can't be complete either */
if (!$currentStep->isComplete()) {
    for ($i = $stepNumber+1; $i < count($steps); $i++) {
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

function processAutoCompleteRequest() {
    $path = !empty($_GET['path']) ? $_GET['path'] : '';
    /* Undo the damage caused by magic_quotes */
    if (get_magic_quotes_gpc()) {
	$path = stripslashes($path);
    }

    if (is_dir($path)) {
	$match = '';
    } else {
	$match = basename($path);
	$matchLength = strlen($match);
	$path = dirname($path);
	if (!is_dir($path)) {
	    return;
	}
    }

    $dirList = array();
    if ($dir = opendir($path)) {
	if ($path{strlen($path)-1} != DIRECTORY_SEPARATOR) {
	    $path .= DIRECTORY_SEPARATOR;
	}
	while (($file = readdir($dir)) !== false) {
	    if ($file == '.' || $file == '..' || ($match && strncmp($file, $match, $matchLength))) {
		continue;
	    }
	    $file = $path . $file;
	    if (is_dir($file)) {
		$dirList[] = $file;
	    }
	}
	closedir($dir);
	sort($dirList);
    }

    header("Content-Type: text/plain");
    print implode("\n", $dirList);
}


/**
 * (Re-) Create the gallery filesystem data structure
 *
 * @param string $dataBase absolute filesystem path of the storage directory
 * @return boolean success whether the structure was created successfully
 */
function populateDataDirectory($dataBase) {
    /* Use non-restrictive umask to create directories with lax permissions */
    umask(0);

    if ($dataBase{strlen($dataBase)-1} != DIRECTORY_SEPARATOR) {
	$dataBase .= DIRECTORY_SEPARATOR;
    }

    /* Create the sub directories, if necessary */
    foreach (array('albums',
		   'cache',
		   'locks',
		   'tmp',
		   'plugins_data',
		   'plugins_data/modules',
		   'plugins_data/themes',
		   'smarty',
		   'smarty/templates_c') as $key) {
	$dir = $dataBase . $key;

	if (file_exists($dir) && !is_dir($dir)) {
	    return false;
	}

	if (!file_exists($dir)) {
	    if (!@mkdir($dir, 0755)) {
		return false;
	    }
	}

	if (!is_writeable($dir)) {
	    return false;
	}
    }

    return secureStorageFolder($dataBase);
}

/**
 * Secure the storage folder from attempts to access it directly via the web by adding a
 * .htaccess with a "Deny from all" directive. This won't have any effect on webservers other
 * than Apache 1.2+ though.
 * Since we can't reliably tell whether the storage folder is web-accessible or not,
 * we add this in all cases. It doesn't hurt.
 * @param string $dataBase absolute filesystem path to the storage folder
 * @return boolean true if the .htaccess file has been created successfully
 */
function secureStorageFolder($dataBase) {
    $htaccessPath = $dataBase . '.htaccess';
    $fh = @fopen($htaccessPath, 'w');
    if ($fh) {
	$htaccessContents = "DirectoryIndex .htaccess\n" .
			    "SetHandler Gallery_Security_Do_Not_Remove\n" .
			    "Options None\n" .
			    "<IfModule mod_rewrite.c>\n" .
			    "RewriteEngine off\n" .
			    "</IfModule>\n" .
			    "Order allow,deny\n" .
			    "Deny from all\n";
	fwrite($fh, $htaccessContents);
	fclose($fh);
    }

    return file_exists($htaccessPath);
}

/* Returns something like https://example.com */
function getBaseUrl() {
    /* Can't use GalleryUrlGenerator::makeUrl since it's an object method */
    if (!($hostName = GalleryUtilities::getServerVar('HTTP_X_FORWARDED_HOST'))) {
	$hostName = GalleryUtilities::getServerVar('HTTP_HOST');
    }
    $protocol = (GalleryUtilities::getServerVar('HTTPS') == 'on') ? 'https' : 'http';

    return sprintf('%s://%s', $protocol, $hostName);
}

/** Returns the URL to the G2 folder, e.g. http://example.com/gallery2/. */
function getGalleryDirUrl() {
    global $g2Base;

    require_once($g2Base . 'modules/core/classes/GalleryUrlGenerator.class');
    $urlPath = preg_replace('|^(.*/)install/index.php(?:\?.*)?$|s', '$1',
			    GalleryUrlGenerator::getCurrentRequestUri());

    return getBaseUrl() . $urlPath;
}

/**
 * Mini url generator for the installer
 */
function generateUrl($uri, $print=true) {
    if (!strncmp($uri, 'index.php', 9)) {
	/* Cookieless browsing: If session.use_trans_sid is on then it will add the session id. */
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
$_SESSION['install_steps'] = serialize($steps);
if (isset($galleryStub)) {
    $_SESSION['galleryStub'] = serialize($galleryStub);
}
?>
