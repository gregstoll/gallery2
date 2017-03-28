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
 * This view lets you make very simple callbacks to the framework to get very specific data.
 * Eventually this will probably get refactored into a much more sophisticated framework.
 *
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class SimpleCallbackView extends GalleryView {

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
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	list ($command, $prefix) = GalleryUtilities::getRequestVariables('command', 'prefix');
	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return $ret;
	}
	if (!headers_sent()) {
	    header("Content-type: text/plain; charset=UTF-8");
	}

	switch($command) {
	case 'lookupUsername':
	    if (!$isAnonymous) {
		list ($ret, $usernames) = GalleryCoreApi::fetchUsernames(10, null, $prefix);
		if (!$ret) {
		    print implode("\n", $usernames);
		}
	    }
	    break;

	case 'lookupGroupname':
	    if (!$isAnonymous) {
		list ($ret, $groupNames) = GalleryCoreApi::fetchGroupNames(10, null, $prefix);
		if (!$ret) {
		    print implode("\n", $groupNames);
		}
	    }
	    break;

	case 'lookupDirectories':
	    list ($ret, $isSiteAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if (!$ret && $isSiteAdmin) {
		GalleryUtilities::unsanitizeInputValues($prefix, false);
		$dirs = implode("\n", $this->_getDirectoryListing($prefix, false));
		GalleryUtilities::sanitizeInputValues($dirs, false);
		print $dirs;
	    }
	    break;

	case 'lookupFiles':
	    list ($ret, $isSiteAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if (!$ret && $isSiteAdmin) {
		GalleryUtilities::unsanitizeInputValues($prefix, false);
		$files = implode("\n", $this->_getDirectoryListing($prefix));
		GalleryUtilities::sanitizeInputValues($files, false);
		print $files;
	    }
	    break;
	}

	return null;
    }

    function _getDirectoryListing($path, $getFiles=true) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	/* $path is UTF-8, we need it in the system charset for filesystem interactions */
	$path = GalleryCoreApi::convertFromUtf8($path);

	if ($platform->is_dir($path)) {
	    $match = '';
	} else {
	    $match = basename($path);
	    $matchLength = strlen($match);
	    $path = dirname($path);
	    if (!$platform->is_dir($path)) {
		return array();
	    }
	}

	$dirList = $fileList = array();
	if ($dir = $platform->opendir($path)) {
	    $slash = $platform->getDirectorySeparator();
	    if ($path{strlen($path)-1} != $slash) {
		$path .= $slash;
	    }
	    while (($file = $platform->readdir($dir)) !== false) {
		if ($file == '.' || $file == '..'
			|| ($match && strncmp($file, $match, $matchLength))) {
		    continue;
		}
		$file = $path . $file;

		if ($platform->is_dir($file)) {
		    /* Filesystem charset -> UTF-8 conversion required */
		    $dirList[] = GalleryCoreApi::convertToUtf8($file);
		} else if ($getFiles && $platform->is_file($file)) {
		    $fileList[] = GalleryCoreApi::convertToUtf8($file);
		}
	    }
	    $platform->closedir($dir);
	    sort($dirList);
	    sort($fileList);
	}

	return empty($dirList) ? $fileList : array_merge($dirList, $fileList);
    }
}
?>
