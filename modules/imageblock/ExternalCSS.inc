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
 * Generate CSS required if image frames are used with external image block.
 * @package ImageBlock
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ExternalCSSController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	list ($ret, $imageframe) = GalleryCoreApi::newFactoryInstance('ImageFrameInterface_1_1');
	if ($ret) {
	    return array($ret, null);
	}
	if (isset($imageframe)) {
	    $frames = GalleryUtilities::getRequestVariables('frames');
	    if (empty($frames)) {
		$frames = array();
		foreach (array('albumFrame', 'itemFrame') as $key) {
		    list ($ret, $frame) =
			GalleryCoreApi::getPluginParameter('module', 'imageblock', $key);
		    if ($ret) {
			return array($ret, null);
		    }
		    if (!empty($frame)) {
			$frames[] = $frame;
		    }
		}
		if (!empty($frames)) {
		    GalleryUtilities::putRequestVariable('frames', implode('|', $frames));
		}
	    }
	}
	return array(null,
		     array('delegate' => array('view' =>
				empty($frames) ? 'imageblock.ExternalCSS' : 'imageframe.CSS'),
			   'status' => array(), 'error' => array()));
    }

    /**
     * @see GalleryController::omitAuthTokenCheck
     */
    function omitAuthTokenCheck() {
	/*
	 * The sole purpose of this controller is to delegate dynamically to different views.
	 * It does not do any controller-like action, thus there is no need for an auth-token check.
	 */
	return true;
    }

    /**
     * @see GalleryController::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }
}

/**
 * Generate CSS when no frames are selected
 */
class ExternalCSSView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	header('Content-type: text/html');
	header('Expires: ' . GalleryUtilities::getHttpDate(time() + 2592000));
	print '<style type="text/css"> img.giThumbnail { border-style: none; } </style>';
	print "\n";
	return null;
    }
}
?>
