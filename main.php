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
 * Main handler for all Gallery pages/requests.
 * @package Gallery
 */

$gallerySetErrorHandler = false;
include(dirname(__FILE__) . '/bootstrap.inc');

/*
 * If they don't have a setup password, we assume that the config.php is empty and this is an
 * initial install
 */
if (!@$gallery->getConfig('setup.password')) {
    /* May be invalid if a multisite install lost its config.php; galleryBaseUrl unknown */
    header('Location: install/');
    return;
}

if ($gallery->isEmbedded()) {
    require_once(dirname(__FILE__) . '/init.inc');
} else {
    /* If this is a request for a public data file, give it to the user immediately */
    $unsanitizedView = isset($_GET[GALLERY_FORM_VARIABLE_PREFIX . 'view']) ?
	$_GET[GALLERY_FORM_VARIABLE_PREFIX . 'view'] : null;
    $itemId = (int)(isset($_GET[GALLERY_FORM_VARIABLE_PREFIX . 'itemId']) ?
		    $_GET[GALLERY_FORM_VARIABLE_PREFIX . 'itemId'] : null);
    if ($unsanitizedView == 'core.DownloadItem' && !empty($itemId)) {
	/*
	 * Our URLs are immutable because they have the serial numbers embedded.  If the browser
	 * presents us with an If-Modified-Since then it has the latest version of the file already.
	 */
	if (isset($_SERVER['HTTP_IF_MODIFIED_SINCE'])
		|| (function_exists('getallheaders')
		    && ($headers = getallheaders())
		    && (isset($headers['If-Modified-Since'])
			|| isset($headers['If-modified-since'])))) {
	    header('HTTP/1.0 304 Not Modified');
	    return;
	}

	/*
	 * Fast download depends on having data.gallery.cache set, so set it now.  If for some
	 * reason we fail, we'll reset it in init.inc (but that's OK).
	 */
	$gallery->setConfig(
	    'data.gallery.cache', $gallery->getConfig('data.gallery.base') . 'cache/');

	$path = GalleryDataCache::getCachePath(
	    array('type' => 'fast-download', 'itemId' => $itemId));
	/* We don't have a platform yet so we have to use the raw file_exists */
	/* Disable fast-download in maintenance mode, admins still get via core.DownloadItem */
	if (file_exists($path) && !$gallery->getConfig('mode.maintenance')) {
	    include($path);
	    if (GalleryFastDownload()) {
		return;
	    }
	}
    }

    /* Otherwise, proceed with our regular process */
    require_once(dirname(__FILE__) . '/init.inc');
    $ret = GalleryInitFirstPass();
    if ($ret) {
	_GalleryMain_errorHandler($ret, null);
	return;
    }

    /* Process the request */
    GalleryMain();
}

if (!empty($gallerySetErrorHandler)) {
    restore_error_handler();
    $gallerySetErrorHandler = false;
}

/**
 * Main handler for all Gallery pages/requests.
 * @return array
 */
function GalleryMain($embedded=false) {
    global $gallery;

    /* Process the request */
    list ($ret, $g2Data) = _GalleryMain($embedded);
    if ($ret) {
	_GalleryMain_errorHandler($ret, $g2Data);

	/**
	 * @todo security question: should we be resetting the auth code (below) even if the
	 * storage is not initialized here?
	 */
	if ($gallery->isStorageInitialized()) {
	    /* Nuke our transaction, too */
	    $storage =& $gallery->getStorage();
	    $storage->rollbackTransaction();

	    if ($ret->getErrorCode() & ERROR_REQUEST_FORGED) {
		/*
		 * The auth token was automatically reset as a side-effect when we determined that
		 * this request was forged, so save the session now.
		 */
		$session =& $gallery->getSession();
		$ret2 = $session->save(true);
		if ($ret2) {
		    GalleryCoreApi::addEventLogEntry(
			'Gallery Error', 'Unable to reset the auth token', $ret2->getAsText());
		}
	    }
	}
    } else {
	$gallery->performShutdownActions();

	/* Write out our session data */
	$session =& $gallery->getSession();
	$ret = $session->save();
    }

    /* Complete our transaction */
    if (!$ret && $gallery->isStorageInitialized()) {
	$storage =& $gallery->getStorage();
	$ret = $storage->commitTransaction();
    }

    if ($ret) {
	$g2Data['isDone'] = true;
    } else if (isset($g2Data['redirectUrl'])) {
	/* If we're in debug mode, show a redirect page */
	print '<h1> Debug Redirect </h1> ' .
	    'Not automatically redirecting you to the next page because we\'re in debug mode<br/>';
	printf('<a href="%s">Continue to the next page</a>', $g2Data['redirectUrl']);
	print '<hr/>';
	print '<pre>';
	print $gallery->getDebugBuffer();
	print '</pre>';
    }

    return $g2Data;
}

/**
 * Process our request.
 * @return array GalleryStatus a status code
 *               array
 */
function _GalleryMain($embedded=false, $template=null) {
    global $gallery;
    $urlGenerator =& $gallery->getUrlGenerator();

    /* Figure out the target view/controller */
    list ($controllerName, $viewName) = GalleryUtilities::getRequestVariables('controller', 'view');
    $controllerName = is_string($controllerName) ? $controllerName : null;
    $viewName = is_string($viewName) ? $viewName : null;
    $gallery->debug("controller $controllerName, view $viewName");

    /* Check if core module needs upgrading */
    list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core', true);
    if ($ret) {
	return array($ret, null);
    }
    $installedVersions = $core->getInstalledVersions();
    if ($installedVersions['core'] != $core->getVersion()) {
	if ($redirectUrl = @$gallery->getConfig('mode.maintenance')) {
	    /* Maintenance mode - redirect if given URL, else simple message */
	    if ($redirectUrl === true) {
		header('Content-Type: text/html; charset=UTF-8');
		print $core->translate('Site is temporarily down for maintenance.');
		exit;
	    }
	} else {
	    $gallery->debug('Redirect to the upgrade wizard, core module version is out of date');
	    $redirectUrl = $urlGenerator->getCurrentUrlDir(true) . 'upgrade/index.php';
	}
	list ($ignored, $results) = _GalleryMain_doRedirect($redirectUrl, null, null, true);
	return array(null, $results);
    }

    $ret = GalleryInitSecondPass();
    if ($ret) {
	return array($ret, null);
    }

    /* Load and run the appropriate controller */
    $results = array();
    if (!empty($controllerName)) {
	GalleryCoreApi::requireOnce('modules/core/classes/GalleryController.class');
	list ($ret, $controller) = GalleryController::loadController($controllerName);
	if ($ret) {
	    return array($ret, null);
	}

	if (!$embedded && $gallery->getConfig('mode.embed.only')
		&& !$controller->isAllowedInEmbedOnly()) {
	    /* Lock out direct access when embed-only is set */
	    if (($redirectUrl = $gallery->getConfig('mode.embed.only')) === true) {
		return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	    }
	    list ($ignored, $results) = _GalleryMain_doRedirect($redirectUrl, null, null, true);
	    return array(null, $results);
	}

	if ($gallery->getConfig('mode.maintenance') && !$controller->isAllowedInMaintenance()) {
	    /* Maintenance mode - allow admins, else redirect to given or standard URL */
	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret) {
		return array($ret, null);
	    }

	    if (!$isAdmin) {
		if (($redirectUrl = $gallery->getConfig('mode.maintenance')) === true) {
		    $redirectUrl = $urlGenerator->generateUrl(
			array('view' => 'core.MaintenanceMode'), array('forceFullUrl' => true));
		}
		list ($ignored, $results) = _GalleryMain_doRedirect($redirectUrl, null, null, true);
		return array(null, $results);
	    }
	}

	/* Get our form and return variables */
	$form = GalleryUtilities::getFormVariables('form');

	/* Verify the genuineness of the request */
	if (!$controller->omitAuthTokenCheck()) {
	    $ret = GalleryController::assertIsGenuineRequest();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Let the controller handle the input */
	list ($ret, $results) = $controller->handleRequest($form);
	if ($ret) {
	    list ($ret, $results) = $controller->permissionCheck($ret);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Check to make sure we got back everything we want */
	if (!isset($results['status'])
		|| !isset($results['error'])
		|| (!isset($results['redirect'])
		    && !isset($results['delegate'])
		    && !isset($results['return']))) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
		'Controller results are missing status, error, (redirect, delegate, return)'),
		null);
	}

	/* Try to return if the controller instructs it */
	if (!empty($results['return'])) {
	    $redirectUrl = GalleryUtilities::getRequestVariables('return');
	    if (empty($redirectUrl)) {
		$redirectUrl = GalleryUtilities::getRequestVariables('formUrl');
	    }
	}

	/* Failing that, redirect if so instructed */
	if (empty($redirectUrl) && !empty($results['redirect'])) {
	    /* If we have a status, store its data in the session */
	    if (!empty($results['status'])) {
		$session =& $gallery->getSession();
		$session->putStatus($results['status']);
	    }

	    $urlToGenerate = $results['redirect'];
	    $redirectUrl = $urlGenerator->generateUrl($urlToGenerate,
						      array('forceFullUrl' => true));
	}

	/* If we have a redirect URL use it */
	if (!empty($redirectUrl)) {
	    return _GalleryMain_doRedirect($redirectUrl, null, $controllerName);
	}

	/* Let the controller specify the next view */
	if (!empty($results['delegate'])) {
	    /* Load any errors into the request */
	    if (!empty($results['error'])) {
		foreach ($results['error'] as $error) {
		    GalleryUtilities::putRequestVariable($error, 1);
		}
	    }

	    /* Save the view name, put the rest into the request so the view can get it */
	    foreach ($results['delegate'] as $key => $value) {
		switch($key) {
		case 'view':
		    $viewName = $value;
		    break;

		default:
		    GalleryUtilities::putRequestVariable($key, $value);
		    break;
		}
	    }
	}
    }

    /* Load and run the appropriate view */
    if (empty($viewName)) {
	$viewName = GALLERY_DEFAULT_VIEW;
	GalleryUtilities::putRequestVariable('view', $viewName);
    }

    list ($ret, $view) = GalleryView::loadView($viewName);
    if ($ret) {
	return array($ret, null);
    }

    if ($gallery->getConfig('mode.maintenance') && !$view->isAllowedInMaintenance()) {
	/* Maintenance mode - allow admins, else redirect to given url or show standard view */
	list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	if (!$isAdmin) {
	    if (($redirectUrl = $gallery->getConfig('mode.maintenance')) !== true) {
		list ($ignored, $results) = _GalleryMain_doRedirect($redirectUrl, null, null, true);
		return array(null, $results);
	    }

	    $viewName = 'core.MaintenanceMode';
	    list ($ret, $view) = GalleryView::loadView($viewName);
	    if ($ret) {
		return array($ret, null);
	    }
	}
    }

    if (!$embedded && $gallery->getConfig('mode.embed.only') && !$view->isAllowedInEmbedOnly()) {
	/* Lock out direct access when embed-only is set */
	if (($redirectUrl = $gallery->getConfig('mode.embed.only')) === true) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}
	return _GalleryMain_doRedirect($redirectUrl);
    }

    /* Check if the page is cached and return the cached version, else generate the page */
    list ($ret, $shouldCache) = GalleryDataCache::shouldCache('read', 'full');
    if ($ret) {
	return array($ret, null);
    }

    $html = '';
    if ($shouldCache) {
	$session =& $gallery->getSession();
	list ($ret, $html) = GalleryDataCache::getPageData(
	    'page', $urlGenerator->getCacheableUrl());
	if ($ret) {
	    return array($ret, null);
	}

	if (!empty($html) && $embedded) {
	     /* Also get the theme data */
	    list ($ret, $themeData) = GalleryDataCache::getPageData(
		'theme', $urlGenerator->getCacheableUrl());
	    if ($ret) {
		return array($ret, null);
	    }
	}
    }

    if (!empty($html) && (!$embedded || !empty($themeData))) {
	/* TODO: If we cache all the headers and replay them here, we could send a 304 back */
	$session =& $gallery->getSession();

	if (!$embedded) {
	    /* Set the appropriate charset in our HTTP header */
	    if (!headers_sent()) {
		header('Content-Type: text/html; charset=UTF-8');
	    }
	    print $session->replaceTempSessionIdIfNecessary($html);
	    $data['isDone'] = true;
	} else {
	    $html = unserialize($html);
	    $themeData = unserialize($themeData);

	    $data = $session->replaceSessionIdInData($html);
	    $data['themeData'] = $session->replaceSessionIdInData($themeData);

	    $data['isDone'] = false;
	}
    } else {
	/* Initialize our container for template data */
	$gallery->setCurrentView($viewName);

	if ($view->isControllerLike()) {
	    /* Verify the genuineness of the request */
	    $ret = GalleryController::assertIsGenuineRequest();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* If we render directly to the browser, we need a session before, or no session at all */
	if ($view->isImmediate() || $viewName == 'core.ProgressBar') {
	    /*
	     * Session: Find out whether we need to send a cookie & get a new sessionId and save it
	     * (make sure there's a sessionId before starting to render, but only if we need a
	     * session)
	     */
	    $session =& $gallery->getSession();
	    $ret = $session->start();
	    if ($ret) {
		return array($ret, null);
	    }
	    /* From now on, don't add sessionId to URLs if there's no persistent session */
	    $session->doNotUseTempId();
	}

	/*
	 * If this is an immediate view, it will send its own output directly.  This is used in the
	 * situation where we want to send back data that's not controlled by the layout.  That's
	 * usually something that's not user-visible like a binary file.
	 */
	$data = array();
	if ($view->isImmediate()) {
	    if ($view->autoCacheControl()) {
		/* r17660 removed the default on the $template parameter */
		$null = null;
		$ret = $view->setCacheControl($null);
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    $status = isset($results['status']) ? $results['status'] : array();
	    $error = isset($results['error']) ? $results['error'] : array();
	    $ret = $view->renderImmediate($status, $error);
	    if ($ret) {
		list ($ret2, $inGroup) = GalleryCoreApi::isUserInSiteAdminGroup();
		if ($ret->getErrorCode() & ERROR_MISSING_OBJECT && ($ret2 || !$inGroup)) {
		    /* Normalize error to GalleryView::_permissionCheck() */
		    $ret->addErrorCode(ERROR_PERMISSION_DENIED);
		}
		return array($ret, null);
	    }
	    $data['isDone'] = true;
	} else {
	    if (!isset($template)) {
		GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
		$template = new GalleryTemplate(dirname(__FILE__));
	    }
	    list ($ret, $results, $theme) = $view->doLoadTemplate($template);
	    if ($ret) {
		list ($ret, $results) = $view->_permissionCheck($ret);
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    if (isset($results['redirect']) || isset($results['redirectUrl'])) {
		if (isset($results['redirectUrl'])) {
		    $redirectUrl = $results['redirectUrl'];
		} else {
		    $redirectUrl = $urlGenerator->generateUrl($results['redirect'],
							      array('forceFullUrl' => true));
		}

		return _GalleryMain_doRedirect($redirectUrl, $template);
	    }

	    if (empty($results['body'])) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
						   'View results are missing body file'), null);
	    }

	    $templatePath = 'gallery:' . $results['body'];
	    $template->setVariable('l10Domain', $theme->getL10Domain());
	    $template->setVariable('isEmbedded', $embedded);

	    if ($viewName == 'core.ProgressBar') {
		@ini_set('output_buffering', '0');

		/**
		 * Try to prevent Apache's mod_deflate from gzipping the output since that
		 * can interfere with streamed output.
		 */
		if (function_exists('apache_setenv')
		        && !@$gallery->getConfig('apacheSetenvBroken')) {
		    @apache_setenv('no-gzip', '1');
		}

		/* Render progress bar pages immediately so that the user sees the bar moving */
		$ret = $template->display($templatePath);
		if ($ret) {
		    return array($ret, null);
		}
		$data['isDone'] = true;
	    } else {
		$event = GalleryCoreApi::newEvent('Gallery::BeforeDisplay');
		$event->setEntity($template);
		$event->setData(array('templatePath' => $templatePath,
				      'view' => $view));
		list ($ret, $ignored) = GalleryCoreApi::postEvent($event);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $html) = $template->fetch($templatePath);
		if ($ret) {
		    return array($ret, null);
		}

		/*
		 * Session: Find out whether we need to send a cookie & need a new session (only if
		 * we don't have one yet)
		 */
		$session =& $gallery->getSession();
		$ret = $session->start();
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $shouldCache) = GalleryDataCache::shouldCache('write', 'full');
		if ($ret) {
		    return array($ret, null);
		}

		if ($embedded) {
		    $html = $theme->splitHtml($html, $results);
		}

		if ($shouldCache && $results['cacheable']) {
		    $htmlForCache = $html;
		    if ($embedded) {
			$themeDataForCache = $template->getVariable('theme');
		    }
		}

		if ($embedded) {
		    $data = $session->replaceSessionIdInData($html);

		    $data['themeData'] =& $template->getVariableByReference('theme');
		    $data['themeData'] = $session->replaceSessionIdInData($data['themeData']);
		    $data['isDone'] = false;
		} else {
		    /* Set the appropriate charset in our HTTP header */
		    if (!headers_sent()) {
			header('Content-Type: text/html; charset=UTF-8');
		    }
		    print $session->replaceTempSessionIdIfNecessary($html);

		    $data['isDone'] = true;
		}

		if ($shouldCache && $results['cacheable']) {
		    $session =& $gallery->getSession();
		    $cacheKey = $urlGenerator->getCacheableUrl();
		    $sessionId = $session->getId();

		    if (!empty($sessionId) && $sessionId != SESSION_TEMP_ID) {
			$htmlForCache = $session->replaceSessionIdInData(
				$htmlForCache, $sessionId, SESSION_TEMP_ID);

			if ($embedded) {
			    $data['themeData'] = $session->replaceSessionIdInData(
				    $data['themeData'], $sessionId, SESSION_TEMP_ID);
			}
		    }

		    if ($embedded) {
			$htmlForCache = serialize($htmlForCache);

			$ret = GalleryDataCache::putPageData('theme', $results['cacheable'],
				$cacheKey, serialize($data['themeData']));
			if ($ret) {
			    return array($ret, null);
			}
		    }

		    $ret = GalleryDataCache::putPageData('page', $results['cacheable'],
			$cacheKey, $htmlForCache);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	}
    }

    return array(null, $data);
}

function _GalleryMain_doRedirect($redirectUrl, $template=null, $controller=null,
				 $ignoreErrors=false) {
    global $gallery;
    $session =& $gallery->getSession();
    $urlGenerator =& $gallery->getUrlGenerator();

    /* Create a valid sessionId for guests, if required */
    $ret = $session->start();
    if ($ret) {
	if ($ignoreErrors) {
	    $gallery->debug("_GalleryMain_doRedirect: Failed to start the session, error stack:\n"
		. $ret->getAsHtml);
	} else {
	    return array($ret, null);
	}
    }
    $redirectUrl = $session->replaceTempSessionIdIfNecessary($redirectUrl);
    $session->doNotUseTempId();

    /*
     * UserLogin returnUrls don't have a sessionId in the URL to replace, make sure there's a
     * sessionId in the redirectUrl for users that don't use cookies
     */
    if (!$session->isUsingCookies() && $session->isPersistent()
	    && strpos($redirectUrl, $session->getKey()) === false) {
	$redirectUrl = GalleryUrlGenerator::appendParamsToUrl($redirectUrl,
	    array($session->getKey() => $session->getId()));
    }

    if ($gallery->getDebug() == false || $gallery->getDebug() == 'logged') {
	/*
	 * The URL generator makes HTML 4.01 compliant URLs using &amp; but we don't want those in
	 * our Location: header
	 */
	$redirectUrl = str_replace('&amp;', '&', $redirectUrl);
	$redirectUrl = rtrim($redirectUrl, '&? ');

	$redirectUrl = $urlGenerator->makeAbsoluteUrl($redirectUrl);

	/*
	 * IIS 3.0 - 5.0 webservers will ignore all other headers if the location header is set.  It
	 * will simply not send other headers, eg. the set-cookie header, which is important for us
	 * in the login and logout requests / redirects.  See:
	 * http://support.microsoft.com/kb/q176113/ Our solution: detect IIS version and append
	 * GALLERYSID to the Location URL if necessary
	 */
	if (in_array($controller, array('core.Logout', 'core.UserLogin', 'publishxp.Login'))) {
	    /* Check if it's IIS and if the version is < 6.0 */
	    $webserver = GalleryUtilities::getServerVar('SERVER_SOFTWARE');
	    if (!empty($webserver)
		    && preg_match('|^Microsoft-IIS/(\d)\.\d$|', trim($webserver), $matches)
		    && $matches[1] < 6) {
		/*
		 * It is IIS and it's a version with this bug, check if GALLERYSID is already in the
		 * URL, else append it
		 */
		$session =& $gallery->getSession();
		$sessionParamString =
		    GalleryUtilities::prefixFormVariable(urlencode($session->getKey())) . '='
		    . urlencode($session->getId());
		if ($session->isPersistent() && !strstr($redirectUrl, $sessionParamString)) {
		    $redirectUrl .= (strpos($redirectUrl, '?') === false) ? '?' : '&';
		    $redirectUrl .= $sessionParamString;
		}
	    }
	}

	GalleryUtilities::setResponseHeader("Location: $redirectUrl");
	return array(null, array('isDone' => true));
    }

    return array(null, array('isDone' => true, 'redirectUrl' => $redirectUrl,
			     'template' => $template));
}

/**
 * Handle an error condition that happened somewhere in our main request processing code.  If the
 * error cannot be handled, then add an error in the event log.
 * @param GalleryStatus a status code
 * @param array $g2Data the results from _GalleryMain
 */
function _GalleryMain_errorHandler($error, $g2Data=null) {
    global $gallery;

    GalleryCoreApi::requireOnce('modules/core/ErrorPage.inc');
    $handledError = ErrorPageView::errorHandler($error, $g2Data);
    if (!$handledError) {
	$summary = $error->getErrorMessage();
	if (empty($summary)) {
	    $summary = join(', ', $error->getErrorCodeConstants($error->getErrorCode()));
	}
	GalleryCoreApi::addEventLogEntry('Gallery Error', $summary, $error->getAsText());
    }
}
?>
