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

GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
GalleryCoreApi::requireOnce('modules/slideshow/classes/SlideshowHelper.class');

/**
 * Display the slideshow
 * @package Slideshow
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17785 $
 */
class SlideshowMediaRssView extends GalleryView {

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
     * @see GalleryView::loadTemplate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$phpVm = $gallery->getPhpVm();

	$SlideShowMediaRss = array();

	$itemId = $prevOffset = $nextOffset = $startId = null;

	list ($itemListString, $startId, $offset) =
	    GalleryUtilities::getRequestVariables('itemList', 'startId', 'offset');
        if (isset($startId)) {
            $startId = (int)$startId;                
        }
        if (isset($offset)) {
            $offset = (int)$offset;
        }

	if (!empty($itemListString)) {
	    foreach (explode('|', $itemListString) as $id) {
		$itemIds[] = (int)$id;
	    }
	    list ($ret, $items, $start) = SlideshowHelper::buildItemsSlideshow($itemIds);
	    if ($ret) {
		return $ret;
	    }
	} else {
	    list ($ret, $item) = $this->getItem();
	    if ($ret) {
		return $ret;
	    }

	    $itemId = $item->getId();

	    list ($ret, $items, $start, $prevOffset, $nextOffset) =
		SlideshowHelper::buildAlbumSlideshow($item, $startId, $offset);
	    if ($ret) {
		return $ret;
	    }
	}

	if (!$phpVm->headers_sent()) {
	    $phpVm->header('Content-Type: text/xml; charset=UTF-8');
	}
	$template = new GalleryTemplate(GalleryCoreApi::getCodeBasePath());
	$template->setVariable('SlideshowMediaRss', array('items' => $items,
	    'itemId' => $itemId, 'prevOffset' => $prevOffset, 'nextOffset' => $nextOffset));
	return $template->display('modules/slideshow/templates/SlideshowMediaRss.tpl');
    }
}
?>
