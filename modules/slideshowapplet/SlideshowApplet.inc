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
 * Show the slideshow applet
 * @package SlideshowApplet
 * @subpackage UserInterface
 * @author Pierre-Luc Paour <gallery@paour.com>
 * @version $Revision: 17580 $
 */
class SlideshowAppletView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'slideshowapplet');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $modules) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
	    return array($ret, null);
	}

	$startId = null;

	/* Build slideshow for an album.. */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	if (!$item->getCanContainChildren()) {
	    /* Photo: do slideshow of parent album, start at this photo */
	    $startId = $item->getId();
	    list ($ret, $item) =
		GalleryCoreApi::loadEntitiesById($item->getParentId(), 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $canView) = GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!$canView) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }
	}

	$SlideshowApplet = array();
	$SlideshowApplet['extra'] = array();

	if (!isset($modules['remote']['active']) || !$modules['remote']['active']) {
	    $SlideshowApplet['NoProtocolError'] = 1;
	} else {
	    $urlGenerator =& $gallery->getUrlGenerator();
	    $session =& $gallery->getSession();
	    /*
	     * Force the use of the G2 base for the url and the cookie path as the applet will
	     * communicate to G2 directly, even when G2 is embedded
	     */
	    $SlideshowApplet['g2BaseUrl'] = $urlGenerator->getCurrentUrlDir(true);
	    list ($ret, $cookiePath) = $urlGenerator->getCookiePath(true);
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $cookieDomain) = $session->getCookieDomain();
	    if ($ret) {
		return array($ret, null);
	    }
	    $SlideshowApplet['NoProtocolError'] = 0;
	    $SlideshowApplet['cookieName'] = $session->getKey();
	    $SlideshowApplet['cookieValue'] = $session->getId();
	    $SlideshowApplet['cookieDomain'] = $cookieDomain;
	    $SlideshowApplet['cookiePath'] = $cookiePath;
	    $SlideshowApplet['album'] = $item->getId();
	    $SlideshowApplet['userAgent'] = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
	    $SlideshowApplet['galleryVersion'] = 2;
	    $SlideshowApplet['code'] = 'com.gallery.GalleryRemote.GRAppletSlideshow';

	    list ($ret, $locale) = $gallery->getActiveLanguageCode();
	    if ($ret) {
		return array($ret, null);
	    }
	    $SlideshowApplet['locale'] = $locale;

	    if (isset($startId)) {
		/* format the start_from as album_picture, that's what GR expects */
		$SlideshowApplet['extra']['gr_slideshow_start_from'] = $item->getId().'_'.$startId;
	    }

	    list ($ret, $params) =
		GalleryCoreApi::fetchAllPluginParameters('module', 'slideshowapplet');
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach (array('default', 'override') as $type) {
		$SlideshowApplet[$type] = array();
		if (!empty($params['slideshow' . $type . 'Variables'])) {
		    $variablesArray = explode('|', $params['slideshow' . $type . 'Variables']);
		    foreach ($variablesArray as $variable) {
			list ($name, $value) = explode('=', $variable);
			$SlideshowApplet[$type][$name] = $value;
		    }
		}
	    }
	}

	$form['formName'] = '';
	$template->setVariable('controller', '');
	$template->setVariable('SlideshowApplet', $SlideshowApplet);
	return array(null,
		     array('body' => 'modules/slideshowapplet/templates/SlideshowApplet.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'slideshowapplet');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('slideshow (fullscreen)'));
    }
}
?>
