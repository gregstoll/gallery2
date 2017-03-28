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
 * ItemAdd plugin to show the upload applet
 * @package UploadApplet
 * @author Pierre-Luc Paour <gallery@paour.com>
 * @version $Revision: 17580 $
 */
class ItemAddUploadApplet extends ItemAddPlugin {

    /**
     * @see ItemAdd:loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	global $gallery;

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'uploadapplet');
	if ($ret) {
	    return array($ret, null, null);
	}

	list ($ret, $modules) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
	    return array($ret, null, null);
	}

	$ItemAddUploadApplet = array();
	if (!isset($modules['remote']['active']) || !$modules['remote']['active']) {
	    $ItemAddUploadApplet['NoProtocolError'] = 1;
	} else {
	    $session =& $gallery->getSession();
	    $urlGenerator =& $gallery->getUrlGenerator();
	    /*
	     * Force the use of the G2 base for the url and the cookie path as the applet will
	     * communicate to G2 directly, even when G2 is embedded
	     */
	    $ItemAddUploadApplet['g2BaseUrl'] = $urlGenerator->getCurrentUrlDir(true);
	    list ($ret, $cookiePath) = $urlGenerator->getCookiePath(true);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $cookieDomain) = $session->getCookieDomain();
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ItemAddUploadApplet['NoProtocolError'] = 0;
	    $ItemAddUploadApplet['cookieName'] = $session->getKey();
	    $ItemAddUploadApplet['cookieValue'] = $session->getId();
	    $ItemAddUploadApplet['cookieDomain'] = $cookieDomain;
	    $ItemAddUploadApplet['cookiePath'] = $cookiePath;
	    $ItemAddUploadApplet['album'] = $item->getId();
	    $ItemAddUploadApplet['userAgent'] = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
	    $ItemAddUploadApplet['galleryVersion'] = 2;
	    $ItemAddUploadApplet['code'] = 'com.gallery.GalleryRemote.ItemAddUploadAppletMini';

	    list ($ret, $locale) = $gallery->getActiveLanguageCode();
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ItemAddUploadApplet['locale'] = $locale;

	    list ($ret, $params) =
		GalleryCoreApi::fetchAllPluginParameters('module', 'uploadapplet');
	    if ($ret) {
		return array($ret, null, null);
	    }
	    foreach (array('default', 'override') as $type) {
		$ItemAddUploadApplet[$type] = array();
		if (!empty($params['upload' . $type . 'Variables'])) {
		    $variablesArray = explode('|', $params['upload' . $type . 'Variables']);
		    foreach ($variablesArray as $variable) {
			list ($name, $value) = explode('=', $variable);
			$ItemAddUploadApplet[$type][$name] = $value;
		    }
		}
	    }
	}

	$template->setVariable('ItemAddUploadApplet', $ItemAddUploadApplet);
	return array(null,
		     'modules/uploadapplet/templates/ItemAddUploadApplet.tpl',
		     'modules_uploadapplet');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'uploadapplet');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Upload Applet'));
    }
}
?>
