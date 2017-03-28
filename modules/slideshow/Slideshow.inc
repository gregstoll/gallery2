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

GalleryCoreApi::requireOnce('modules/slideshow/classes/SlideshowHelper.class');

/**
 * Display the slideshow
 * @package Slideshow
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17625 $
 */
class SlideshowView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$Slideshow = array();

	list ($itemListString, $startId, $offset) =
	    GalleryUtilities::getRequestVariables('itemList', 'startId', 'offset');
	if (isset($startId)) {
	    $startId = (int)$startId;
	}
	if (isset($offset)) {
	    $offset = (int)$offset;
	}

	/**
	 * @todo Remove the redundancy of calling buildAlbumSlideshow() twice (here and in
	 *       SlideshowMediaRss), e.g. by building the lytebox slideshow via AJAX/DOM.
	 */
	if (!empty($itemListString)) {
	    foreach (explode('|', $itemListString) as $id) {
		$itemIds[] = (int)$id;
	    }
	    list ($ret, $items, $start) = SlideshowHelper::buildItemsSlideshow($itemIds, $startId);
	    if ($ret) {
		return array($ret, null);
	    }
	} else {
	    /* Build slideshow for an album */
	    list ($ret, $item) = $this->getItem();
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $items, $start) =
		SlideshowHelper::buildAlbumSlideshow($item, $startId, $offset);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'slideshow');
	if ($ret) {
	    return array($ret, null);
	}
	if (!empty($Slideshow['item']['title'])) {
	    $template->title($module->translate(array('text' => 'Slideshow for %s',
						      'arg1' => $Slideshow['item']['title'])));
	} else {
	    $template->title($module->translate('Slideshow'));
	}

	list ($ret, $picLensVersion) = $module->getParameter('piclens.version');
	if ($ret) {
	    return array($ret, null);
	}

	if ($picLensVersion) {
	    $Slideshow['piclensVersion'] = $picLensVersion;

	    $template->javascript($urlGenerator->generateUrl(
		array('view' => 'slideshow.DownloadPicLens',
		      'file' => 'js',
		      'v' => $picLensVersion),
		array('forceDirect' => true, 'forceFullUrl' => true)));

	    $jsUrlParams = array('forJavascript' => true, 'forceFullUrl' => true,
				 'htmlEntities' => false);
	    $jsUrlParamsDirect = array_merge($jsUrlParams, array('forceDirect' => true));
	    $Slideshow['piclensSwfUrl'] = $urlGenerator->generateUrl(
		array('view' => 'slideshow.DownloadPicLens',
		      'file' => 'swf',
		      'v' => $picLensVersion),
		$jsUrlParamsDirect);

	    if (empty($itemListString)) {
		$Slideshow['mediaRssUrl'] = $urlGenerator->generateUrl(
		    array('view' => 'slideshow.SlideshowMediaRss',
			  'itemId' => $item->getId()),
		    $jsUrlParams);
	    } else {
		$Slideshow['mediaRssUrl'] = $urlGenerator->generateUrl(
		    array('view' => 'slideshow.SlideshowMediaRss',
			  'itemList' => $itemListString),
		    $jsUrlParams);
	    }

	    /* Set the start item */
	    if (!empty($items)) {
		$Slideshow['startItemId'] = $items[$start]['item']['id'];
	    } else {
		$Slideshow['startItemId'] = $item->getId();
	    }

	    if (GalleryUtilities::hasRequestVariable('return')) {
		/* Remove HTML entities */
		$Slideshow['returnUrl'] =
		    html_entity_decode(GalleryUtilities::getRequestVariables('return'));
	    }

	    if (empty($Slideshow['returnUrl'])) {
		$Slideshow['returnUrl'] = $urlGenerator->generateUrl(
		    array('view' => 'core.ShowItem', 'itemId' => $Slideshow['startItemId']),
		    $jsUrlParams);
	    }
	    $Slideshow['returnUrl'] = $urlGenerator->makeAbsoluteUrl($Slideshow['returnUrl']);
	}

	$Slideshow['items'] = $items;
	$Slideshow['count'] = count($items);
	$Slideshow['start'] = $start;
	$template->setVariable('Slideshow', $Slideshow);

	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('modules/slideshow/data/lytebox.js');
	$template->link(array('rel' => 'stylesheet',
			      'href' => 'modules/slideshow/data/lytebox.css',
			      'type' => 'text/css',
			      'media' => 'screen'));
	return array(null, array('body' => 'modules/slideshow/templates/Slideshow.tpl',
				 'useFullScreen' => true));
    }
}
?>
