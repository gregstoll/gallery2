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
if (!defined('G2_SUPPORT')) {
    return;
}

/**
 * Gallery Database Import
 * @package Support
 */

$g2Base = dirname(dirname(dirname(__FILE__))) . '/';
require_once($g2Base . 'modules/core/classes/GalleryCoreApi.class');
require_once($g2Base . 'modules/core/classes/GalleryStorage.class');
require_once($g2Base . 'modules/core/classes/GalleryUtilities.class');
require_once($g2Base . 'lib/support/SupportStatusTemplate.class');

$templateData = array();

$template = new SupportStatusTemplate('Database Import');

$configFilePath =
    (defined('GALLERY_CONFIG_DIR') ? GALLERY_CONFIG_DIR . '/' : $g2Base) . 'config.php';

require_once(dirname(__FILE__) . '/../../embed.php');

$templateData = array();
$templateData['bodyFile'] = 'ImportRequest.html';
$renderFullPage = true;

$ret = GalleryEmbed::init(array('fullInit' => false));
if ($ret) {
    $templateData['errors'][] = $ret->getAsHtml();
} else {
    $platform =& $gallery->getPlatform();
    $storage =& $gallery->getStorage();

    $templateData['warnings'] = array();
    if (isset($_REQUEST['importDatabase'])) {
	$importFile = $_REQUEST['importFile'];
	/* Sanitize the input */
	GalleryUtilities::sanitizeInputValues($importFile);

	if (!$platform->file_exists($importFile)) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER, null, null,
				     'The file "' . $importFile . '" does not exist.');
	}

	$verifiedFile = $_REQUEST['verifiedFile'];
	/* Sanitize the input */
	GalleryUtilities::sanitizeInputValues($verifiedFile);

	$doImportFlag = true;
	if ($verifiedFile != $importFile) {
	    $templateData['verifiedFile'] = $importFile;
	    $verifiedFile = $importFile;
	    $doImportFlag = verifyVersions($templateData, $importFile);
	}

	if ($doImportFlag) {
	    $template->renderHeader(true);
	    $template->renderStatusMessage('Restoring Gallery Database', '', 0);

	    /* Do the database import */
	    $importer = $storage->getDatabaseImporter();
	    list ($ret, $errors) = $importer->importToDb($verifiedFile, 'importProgressCallback');
	    if ($ret) {
		$templateData['errors'][] = $ret->getAsHtml();
	    } else if ($errors != null) {
		if (!is_array($errors)) {
		    $templateData['errors'][] = $errors->getAsHtml();
		} else {
		    foreach ($errors as $status) {
			$templateData['errors'][] = $status->getAsHtml();
		    }
		}
	    }

	    /* The import processing sets Gallery into maintenance mode, undo that now */
	    $ret = GalleryCoreApi::setMaintenanceMode(false);
	    if ($ret) {
		$templateData['errors'][] = $ret->getAsHtml();
	    }

	    $templateData['bodyFile'] = 'ImportFinished.html';
	    $templateData['hideStatusBlock'] = 1;
	    $renderFullPage = false;
	}
    } else {
	getBackupFiles($templateData);

	/* Render the output */
	$templateData['bodyFile'] = 'ImportRequest.html';
    }
}

if (!$ret) {
    $ret = GalleryEmbed::done();
    if ($ret) {
	$templateData['errors'][] = $ret->getAsHtml();
    }
}

if ($renderFullPage) {
    $template->renderHeaderBodyAndFooter($templateData);
} else {
    $template->renderBodyAndFooter($templateData);
}

/**
 * Verify the version compatibility and identify any modules that are incompatible with the
 * existing installation.
 * @param array $templateData The information to be displayed on the import page.
 * @param string $importFile The file to be verified.
 * @return boolean true if there are no verification messages to display.
 */
function verifyVersions(&$templateData, $importFile) {
    global $gallery;
    global $template;
    $storage =& $gallery->getStorage();

    $importer = $storage->getDatabaseImporter();
    $errors = $importer->verifyVersions($importFile);

    if (!empty($errors['warnings'])) {
	$templateData['versionWarnings'] = $errors['warnings'];
    } else {
	$templateData['versionWarnings'] = null;
    }

    if (!empty($errors['errors'])) {
	$templateData['errors'] = $errors['errors'];
    } else {
	$templateData['errors'] = null;
    }

    $verificationMessages = !empty($errors['warnings']) || !empty($errors['errors']);
    if ($verificationMessages) {
	getBackupFiles($templateData);

	/* Render the output */
	$templateData['bodyFile'] = 'ImportRequest.html';
	$templateData['hideStatusBlock'] = 1;
    }

    return !$verificationMessages;
}

/**
 * Retrieve the list of available backup files from the backup directory
 * @param array $templateData The information to be displayed on the import page.
 */
function getBackupFiles(&$templateData) {
    global $gallery;
    $platform =& $gallery->getPlatform();

    $backupFiles = $gallery->getConfig('data.gallery.backup') . '*.xml';

    $files = array();
    foreach ($platform->glob($backupFiles) as $fileName) {
       $files[filectime($fileName) . $fileName] = $fileName;
    }
    krsort($files);

    $templateData['backupFiles'] = $files;
    if (count($files) == 0) {
        $templateData['errors'][] =
	    'There are no backups found. You will probably have to reinstall.';
    }
}

/**
 * Update the progress bar with the current percent completion
 * @param float $percentage The current completion percentage.
 */
function importProgressCallback($percentage) {
    global $template;
    $template->renderStatusMessage('Importing Gallery Database', '', $percentage);
}
?>
