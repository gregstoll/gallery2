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
 * Mount WebDAV resource using mime type.  RFC4609.
 * @package WebDav
 * @subpackage UserInterface
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 17580 $
 */
class DownloadDavMountView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	GalleryUtilities::setResponseHeader(
	    'Content-Type: application/davmount+xml; charset=UTF-8');
	GalleryUtilities::setResponseHeader(
	    'Content-Disposition: inline; filename="Gallery.davmount"');
	echo "<dm:mount xmlns:dm=\"http://purl.org/NET/webdav/mount\">\n";

	/*
	 * I guess URL shouldn't include session id because a query string may break some clients
	 * and reusing session id by another browser may be considdered session hijacking
	 */
	echo '  <dm:url>' . $urlGenerator->generateUrl(array('controller' => 'webdav.WebDav'),
	    array('forceFullUrl' => true, 'forceSessionId' => false)) . "</dm:url>\n";

	list ($itemId, $path) = GalleryUtilities::getRequestVariables('itemId', 'path');
	if (isset($itemId)) {
	    list ($ret, $item) =
		GalleryCoreApi::loadEntitiesById($itemId, 'GalleryFileSystemEntity');
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $path) = $item->fetchLogicalPath();
	    if ($ret) {
		return $ret;
	    }
	}

	$path = ltrim($path, '/');
	if (!empty($path)) {
	    echo '  <dm:open>' . $path . "</dm:open>\n";
	}

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return $ret;
	}

	if (!$isAnonymous) {
	    $user = $gallery->getActiveUser();
	    echo '  <dm:username>' . $user->getUserName() . "</dm:username>\n";
	}

	echo "</dm:mount>\n";
    }
}
?>
