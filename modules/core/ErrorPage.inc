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
 * This view prepares all the information the theme requires to render an error page.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17829 $
 */
class ErrorPageView extends GalleryView {

    function setError($error) {
	$this->_error = $error;
    }

    /**
     * @see GalleryView::getViewType
     */
    function getViewType() {
	return VIEW_TYPE_ERROR;
    }

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$theme =& $template->getVariableByReference('theme');

	list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	$ErrorPage = array('isAdmin' => !$ret && $isAdmin);

	$showDetails = $ErrorPage['isAdmin'] || $gallery->getDebug();
	if (empty($this->_error)) {
	    $session =& $gallery->getSession();
	    $errorCode = $session->get('core.error.code');
	    $errorTrace = $session->get('core.error.trace');
	} else {
	    $errorCode = $this->_error->getErrorCode();
	    $errorTrace = $this->_error->getAsHtml($showDetails);
	}

	if ($showDetails) {
	    $ErrorPage['stackTrace'] = $errorTrace;
	}

	/* Landing page for errors */
	if ($errorCode & ERROR_OBSOLETE_DATA) {
	    $ErrorPage['code']['obsoleteData'] = true;
	}
	if ($errorCode & (ERROR_PERMISSION_DENIED | ERROR_BAD_PARAMETER)) {
	    $ErrorPage['code']['securityViolation'] = true;
	}
	if ($errorCode & ERROR_STORAGE_FAILURE) {
	    $ErrorPage['code']['storageFailure'] = true;
	}
	if ($errorCode & ERROR_PLATFORM_FAILURE) {
	    $ErrorPage['code']['platformFailure'] = true;
	}
	if ($errorCode & ERROR_MISSING_OBJECT) {
	    $ErrorPage['code']['missingObject'] = true;
	}
	if ($errorCode & ERROR_REQUEST_FORGED) {
	    $ErrorPage['code']['requestAuthenticationFailure'] = true;
	}

	if ($ErrorPage['isAdmin']) {
	    $ErrorPage['phpversion'] = phpversion();
	    $ErrorPage['php_uname'] = php_uname();
	    $ErrorPage['php_sapi_name'] = php_sapi_name();
	    $ErrorPage['webserver'] = GalleryUtilities::getServerVar('SERVER_SOFTWARE');
	    $ErrorPage['browser'] = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
	    if ($gallery->isStorageInitialized()) {
		$storage =& $gallery->getStorage();
		$ErrorPage['dbType'] = $storage->getAdoDbType();
		$ErrorPage['dbVersion'] = @$storage->getVersion();

		list ($ret, $list) = GalleryCoreApi::getToolkitOperationMimeTypes('thumbnail');
		if (!$ret) {
		    $toolkitList = array();
		    foreach ($list as $tmp) {
			$toolkitList = array_merge($toolkitList, $tmp);
		    }
		    $ErrorPage['toolkits'] = implode(', ', array_unique($toolkitList));
		}
	    }

	    GalleryCoreApi::requireOnce('modules/core/module.inc');
	    $installedVersions = CoreModule::getInstalledVersions();
	    $ErrorPage['version'] = $installedVersions['gallery'];
	}

	/*
	 * Normally this would be called by GalleryTheme::loadTemplate, but error processing could
	 * bypass this and call this view directly, so we will set it now.
	 */
	$theme['pageType'] = 'error';

	$template->javascript('lib/javascript/BlockToggle.js');
	$template->setVariable('ErrorPage', $ErrorPage);
	return array(null, array('body' => 'modules/core/templates/ErrorPage.tpl'));
    }

    /**
     * Entry point from main.php
     *
     * @param GalleryStatus $error
     * @param mixed $g2Data (optional) GalleryMain result, if available
     * @param boolean $initOk @deprecated
     * @return boolean true if the error was handled at the last moment and should therefore
     *                 not be considered an error.
     * @static
     */
    function errorHandler($error, $g2Data=null, $initOk=null) {
	global $gallery;
	$failsafe = false;

	/* Post Gallery::Error event */
	$event = GalleryCoreApi::newEvent('Gallery::Error');
	$event->setData(array('error' => $error));
	list ($ret, $eventResults) = GalleryCoreApi::postEvent($event);
	if ($ret) {
	    $failsafe = true;
	    $eventResults = array();
	}

	/* Return HTTP 404 status for ERROR_MISSING_OBJECT */
	if ($error->getErrorCode() & ERROR_MISSING_OBJECT) {
	    GalleryUtilities::setResponseHeader('HTTP/1.0 404 Not Found', false);
	} else {
	    GalleryUtilities::setResponseHeader('HTTP/1.0 500 Internal Server Error', false);
	}

	$suppressBody = $errorHandled = false;
	foreach ($eventResults as $eventResult) {
	    $suppressBody |= !empty($eventResult['suppressBody']);
	    $errorHandled |= !empty($eventResult['errorHandled']);
	}
	if ($suppressBody) {
	    /* One of our error handlers has dealt with the output */
	    return $errorHandled;
	}

	if (!$failsafe) {
	    $translator =& $gallery->getTranslator();
	    /**
	     * @todo Add a fallback mode to the translator to just output untranslated text.
	     */
	    if (!isset($translator)) {
		$ret = $gallery->initTranslator();
		if ($ret) {
		    $failsafe = true;
		}
	    }
	}

	if (!$failsafe) {
	    $session =& $gallery->getSession();
	    if (!isset($session)) {
		$gallery->initEmptySession();
	    }
	}

    	if (!$failsafe) {
	    $urlGenerator =& $gallery->getUrlGenerator();
	    if (!isset($urlGenerator)) {
		GalleryCoreApi::requireOnce('modules/core/classes/GalleryUrlGenerator.class');
		$urlGenerator = new GalleryUrlGenerator();
		$ret = $urlGenerator->init();
		if ($ret) {
		    $failsafe = true;
		} else {
		    $gallery->setUrlGenerator($urlGenerator);
		}
	    }
	}

	if (!$failsafe) {
	    list ($ret, $themeId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'default.theme');
	    if ($ret) {
		$failsafe = true;
	    }
	}

	if (!$failsafe) {
	    list ($ret, $theme) = GalleryCoreApi::loadPlugin('theme', $themeId);
	    if ($ret) {
		$failsafe = true;
	    }
	}

	if (!$failsafe) {
	    $templateAdapter =& $gallery->getTemplateAdapter();
	    $templateAdapter->setTheme($theme);
	    list ($ret, $view) = GalleryView::loadView('core.ErrorPage');
	    if ($ret) {
		$failsafe = true;
	    }
	}

	if (!$failsafe) {
	    $dummyForm = array();
	    GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	    $template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	    $view->setError($error);
	    list ($ret, $results) = $view->loadTemplate($template, $dummyForm);
	    if ($ret) {
		$failsafe = true;
	    }
	}

	if (!$failsafe) {
	    $t =& $template->getVariableByReference('theme');
	    $t['errorTemplate'] = $results['body'];
	    $template->setVariable('l10Domain', 'modules_core');
	    list ($ret, $templatePath) = $theme->showErrorPage($template);
	    if ($ret) {
		$failsafe = true;
	    }
	}

	if (!$failsafe) {
	    $template->setVariable('l10Domain', 'themes_' . $themeId);
	    $ret = $template->display("themes/$themeId/templates/$templatePath");
	    if ($ret) {
		$failsafe = true;
	    }
	}

	if ($failsafe) {
	    /* A catastrophic failure has occurred so just dump the error out to the browser */
	    print '<h2>Error</h2>';
	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($gallery->getDebug() || (!$ret && $isAdmin)) {
		print $error->getAsHtml();
	    }
	    if ($gallery->getDebug() == 'buffered') {
		print '<h3>Debug Output</h3><pre>' . $gallery->getDebugBuffer() . '</pre>';
	    }
	}

	return $errorHandled;
    }
}
?>
