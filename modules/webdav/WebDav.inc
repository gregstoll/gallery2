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

GalleryCoreApi::requireOnce('modules/core/ItemAdd.inc');
GalleryCoreApi::requireOnce('modules/webdav/classes/WebDavHelper.class');

/**
 * The WebDAV controller handles all WebDAV requests.
 * @package WebDav
 * @subpackage UserInterface
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 17623 $
 */
class WebDavController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	/*
	 * Suppress generating HTML error pages for WebDAV clients.  Exploit the fact that event
	 * listeners aren't currently stored in the database, so this event listener is only
	 * registered for this request.  Since this is the WebDAV controller, we know it is a WebDAV
	 * request.
	 */
	$ret = GalleryCoreApi::registerFactoryImplementationForRequest('GalleryEventListener',
	    'WebDavModule', 'WebDavModule', 'modules/webdav/module.inc', 'webdav', 
	    array('Gallery::Error'));
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * Windows Web Folders does not respect HTTP auth requests in PUT responses, so the user
	 * will not be prompted to login.  To work around this, request HTTP auth in all responses
	 * to anonymous Windows Web Folders requests.  Consequently anonymous users can't connect to
	 * WebDAV with Windows Web Folders.
	 */
	$userAgent = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
	if (strpos($userAgent, 'Microsoft Data Access Internet Publishing Provider DAV') !== false
		|| strpos($userAgent, 'Microsoft-WebDAV-MiniRedir') !== false) {
	    list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	    if ($ret) {
		return array($ret, null);
 	    }
	    if ($isAnonymous) {
		return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED, __FILE__, __LINE__,
			'WebDAV: Anonymous request by MS Windows Webfolders WebDAV client detected.'
			. '  Usability workaround: Require auth on all requests.'), null);
	    }
	}

	/*
	 * GalleryCoreApi::addItemToAlbum normalizes path components but
	 * GalleryCoreApi::fetchItemIdByPath does not.  This is a problem for WebDAV because GET
	 * following a successful PUT at the same URL should succeed.
	 *
	 * An alternative is to consistantly normalize all requests.  While this shouldn't break any
	 * clients, PROPFIND following a successful PUT should strictly include the URL of the PUT.
	 *
	 * Consequently we reject an illegal path component.
	 *
	 * This condition may be relaxed in future if it's a problem.
	 */
	$path = GalleryUtilities::getRequestVariables('path');
	/**
 	 * @todo Refactor towards a cleaner approach, e.g. using a map for public paths.
 	 */
 	GalleryUtilities::putRequestVariable('originalPath', $path);
	$pathComponent = basename($path);

	/* Special case to suppress Mac '._' and '.DS_Store files.  Ignore but return success. */
	if (!strncmp($pathComponent, '._', 2) || $pathComponent == '.DS_Store') {
	    WebDavServer::setResponseStatus('200 OK');
	    $handle = WebDavServer::openRequestBody();
	    while (!feof($handle)) {
		fgets($handle);
	    }
	    return array(GalleryCoreApi::error(ERROR_BAD_PATH), null);
	}

	/* Silently legalize path component */
	$pathComponents = preg_split('/\//', $path, -1, PREG_SPLIT_NO_EMPTY);
	foreach ($pathComponents as $key => $pathComponent) {
	    GalleryUtilities::unsanitizeInputValues($pathComponent);
	    $pathComponents[$key] = $platform->legalizePathComponent($pathComponent);
	}
	$path = implode('/', $pathComponents);
	GalleryUtilities::putRequestVariable('path', $path);

	$requestMethod = GalleryUtilities::strToLower(
	    GalleryUtilities::getServerVar('REQUEST_METHOD'));
	if ($requestMethod == 'get' || $requestMethod == 'head') {
	    if (empty($path)) {
		list ($ret, $itemId) = GalleryCoreApi::getDefaultAlbumId();
		if ($ret) {
		    return array($ret, null);
		}
	    } else {
		list ($ret, $itemId) = GalleryCoreApi::fetchItemIdByPath($path);
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Until there's a better way to delegate to Show/DownloadItem */
	    GalleryUtilities::putRequestVariable('itemId', $itemId);
	    if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		return array(null, array('delegate' => array('view' => 'core.ShowItem'),
					 'status' => array(),
					 'error' => array()));
	    }

	    return array(null, array('delegate' => array('view' => 'core.DownloadItem'),
				     'status' => array(),
				     'error' => array()));
	} else if ($requestMethod == 'put') {
	    list ($ret, $parentId) = WebDavHelper::getParentItemIdByPath($path);
	    if ($ret) {
		if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		    /*
		     * A PUT that would result in the creation of a resource without an
		     * appropriately scoped parent collection MUST fail with a 409
		     * (Conflict).
		     */
		    WebDavServer::setResponseStatus('409 Conflict');
		}

		return array($ret, null);
	    }

	    /* Until there's a better way to delegate to ItemAdd */
	    GalleryUtilities::putRequestVariable('itemId', $parentId);
	    GalleryUtilities::putRequestVariable('addPlugin', 'ItemAddWebDav');
	    /* Let the ItemAddPlugin handle the replacement for the auth-token check */
	    list ($ret, $data) = ItemAddController::handleRequest($form);
	    if ($ret) {
		return array($ret, null);
	    }

	    /*
	     * Don't send HTTP redirects or HTML status/error back to DAV clients since we
	     * have already set our status code. Fall through to the standard empty return.
	     */
	}

	return array(null, array('delegate' => array('view' => 'webdav.WebDav'),
				 'status' => array(),
				 'error' => array()));
    }

    /**
     * @see GalleryController::omitAuthTokenCheck
     */
    function omitAuthTokenCheck() {
	return true;
    }

    /**
     * Overriding permissionCheck() since the WebDAV protocol needs to handle authentication
     * differently (redirects to a HTML based login view don't work here).
     * @see GalleryController::permissionCheck()
     */
    function permissionCheck($ret) {
	if ($ret) {
	    list ($ret2, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret2) {
		return array($ret, null);
	    }
	    if (!$isAdmin && ($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		$ret->addErrorCode(ERROR_PERMISSION_DENIED);
	    }
	}
	return array($ret, null);
    }
}

/**
 * The WebDAV view generates WebDAV responses.
 */
class WebDavView extends GalleryView {

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
	/* Although this view is controller-like, we return false since we can't add authTokens */
	return false;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	$requestMethod = GalleryUtilities::strToLower(
		GalleryUtilities::getServerVar('REQUEST_METHOD'));
	if (!in_array($requestMethod, array('options', 'propfind', 'proppatch', 'mkcol', 'delete',
					    'move', 'lock', 'unlock'))) {

	    /* If we just completed a PUT request and got delegated here, just silently return */
	    if ($requestMethod == 'put') {
		return null;
	    }

	    return GalleryCoreApi::error(ERROR_UNIMPLEMENTED);
	}

	$ret = WebDavHelper::$requestMethod();
	if ($ret) {
	    return $ret;
	}

	return null;
    }
}
?>
