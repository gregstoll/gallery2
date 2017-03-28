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
 * Download PicLens code
 * @package Slideshow
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17785 $
 */
class DownloadPicLensView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::shouldSaveSession
     */
    function shouldSaveSession() {
	return false;
    }

    /**
     * @see GalleryView::autoCacheControl
     */
    function autoCacheControl() {
	return false;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;

	/* Figure out which file we're talking about */
	$file = GalleryUtilities::getRequestVariables('file');

	$codeDir = $gallery->getConfig('data.gallery.plugins_data') . 'modules/slideshow/';
	switch ($file) {
	case 'js':
	    return $this->_sendFile(array('path' => "${codeDir}piclens.js",
					  'mimeType' => 'application/x-javascript',
					  'pseudoFileName' => 'piclens.js'));

	case 'swf':
	    return $this->_sendFile(array('path' => "${codeDir}PicLensLite.swf",
					  'mimeType' => 'application/x-shockwave-flash',
					  'pseudoFileName' => 'PicLensLite.swf'));

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}
    }

    /**
     * @see GalleryView::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * Copied from modules/core/DownloadItem.inc
     * @todo refactor all the various _sendFile() implementations into an API call
     */
    function _sendFile($data) {
	global $gallery;
	$platform =& $gallery->getPlatform();
	$phpVm = $gallery->getPhpVm();

	/**
	 * Try to prevent Apache's mod_deflate from gzipping the output since it's likely already
	 * a binary file and broken versions of mod_deflate sometimes get the byte count wrong.
	 */
	if (function_exists('apache_setenv') && !@$gallery->getConfig('apacheSetenvBroken')) {
	    @apache_setenv('no-gzip', '1');
	}

	$requestMethod =
	    GalleryUtilities::strtolower(GalleryUtilities::getServerVar('REQUEST_METHOD'));

	if (!$platform->file_exists($data['path'])) {
	    return GalleryCoreApi::error(ERROR_MISSING_OBJECT, null, null, "$data[path] missing");
	}

	$stats = $platform->stat($data['path']);
	$phpVm->header('Content-type: ' . $data['mimeType']);
	$phpVm->header('Content-Disposition: inline; filename="' . $data['pseudoFileName'] . '"');
	$phpVm->header('Last-Modified: ' . GalleryUtilities::getHttpDate($stats[9]));
	$phpVm->header('Expires: ' . GalleryUtilities::getHttpDate(2147483647));
	$phpVm->header('Cache-Control: public');

	/* If the request method is HEAD, don't send back the body */
	if ($requestMethod == 'head') {
	    $phpVm->header('Content-length: 0');
	} else {
	    if ($stats[7] > 0) {
		$phpVm->header('Content-length: ' . $stats[7]);
	    }
	    /*
	     * Don't use readfile() because it buffers the entire file in memory.  Profiling shows
	     * that this approach is as efficient as fpassthru() but we get to call
	     * guaranteeTimeLimit which prevents it from failing on very large files
	     */
	    if ($fd = $platform->fopen($data['path'], 'rb')) {
		while (true) {
		    $bytes = $platform->fread($fd, 65535);
		    if (strlen($bytes) == 0) {
			break;
		    }
		    print $bytes;
		    $gallery->guaranteeTimeLimit(30);
		}
		$platform->fclose($fd);
	    }
	}

	return null;
    }
}
?>
