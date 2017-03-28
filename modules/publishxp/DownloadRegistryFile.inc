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
 * Generates the Windows registry file for web publishing.
 * This is an immediate view which generates the registry file for Windows.  The information in
 * the registry file informs Windows that there is a web service which supports uploading media.
 *
 * @package PublishXp
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class DownloadRegistryFileView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * Renders the Windows registry file.
     * Generates the registry file to be downloaded and installed on Windows systems.
     * The gallery name and hostname are used as part of the web service for publishing.
     *
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;

	/* Get the domain name */
	$urlGenerator =& $gallery->getUrlGenerator();
	$domain = $urlGenerator->getHostName();

	/* Get the name of the gallery */
	list ($ret, $rootId) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return $ret;
	}
	list ($ret, $hasPermission) = GalleryCoreApi::hasItemPermission($rootId, 'core.view');
	if ($ret) {
	    return $ret;
	}
	if (!$hasPermission) {
	    /* Avoid information disclosure, use a default title */
	    $title = 'Gallery';
	} else {
	    list ($ret, $root) = GalleryCoreApi::loadEntitiesById($rootId, 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }
	    $title = $root->getTitle();
	}

	/* Setup the headers for the Registry file */
	header("Cache-control: private");
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: filename=install_registry.reg");

	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	$template = new GalleryTemplate(dirname(__FILE__) . '/../..');
	$template->setVariable('l10Domain', 'modules_publishxp');

	$vistaVersion = GalleryUtilities::getRequestVariables('vistaVersion');
	$DownloadRegistryFile['vistaVersion'] = !empty($vistaVersion);
	$DownloadRegistryFile['title'] = $title;
	$DownloadRegistryFile['domain'] = $domain;
	$template->setVariable('DownloadRegistryFile', $DownloadRegistryFile);
	list ($ret, $registryFileContents) =
	    $template->fetch('gallery:modules/publishxp/templates/DownloadRegistryFile.tpl');
	if ($ret) {
	    return $ret;
	}

	/* Make sure we use Windows line endings */
	$registryFileContents = preg_replace("/(?<!\r)\n/", "\r\n", $registryFileContents);

	/* Convert the text to the Windows character set corresponding to the requested locale */
	$clientWindowsLocale = $this->_getCharsetFromRequest();
	print GalleryCoreApi::convertFromUtf8($registryFileContents, $clientWindowsLocale);

	return null;
    }

    /**
     * Maps the locale of the HTTP request to a corresponding Windows character set.
     * To detect the locale, the following precedence is applied:
     *  1) Preferred locale of the user (check session locale) unless it is equal to the default
     *  2) HTTP_ACCEPT_LANGUAGE header (what does the system of the client accept)
     *  3) Gallery site-wide default locale (might not be supported by system of the client)
     *
     * @return string A Microsoft Windows character set
     */
    function _getCharsetFromRequest() {
	global $gallery;

	list ($ret, $languageCode) = $gallery->getActiveLanguageCode();
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('publishxp.DownloadRegistryFile, failed to get the session ' .
				'language. Error: ' . $ret->getAsText());
	    }
	    $languageCode = '';
	}

	list ($ret, $defaultLanguageCode) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'default.language');
	if ($ret) {
	    if ($gallery->getDebug()) {
		$gallery->debug('publishxp.DownloadRegistryFile, failed to get the default ' .
				'language. Error: ' . $ret->getAsText());
	    }
	    $defaultLanguageCode = 'en_US';
	}
	$languageCode = empty($languageCode) ? $defaultLanguageCode : $languageCode;

	if ($languageCode == $defaultLanguageCode) {
	    $httpAcceptLanguage = GalleryUtilities::getServerVar('HTTP_ACCEPT_LANGUAGE');
	    if (!empty($httpAcceptLanguage)) {
		$codes = explode(',', $httpAcceptLanguage);
		$languageCode = $codes[0];
	    }
	}

	list ($language, $country) = preg_split('/[-_]/', $languageCode . '_');

	/*
	 * Lookup table: language code to Windows Code Page. Source:
	 * http://www.microsoft.com/globaldev/reference/oslocversion.mspx
	 */
	switch (trim(GalleryUtilities::strToLower((string)$language))) {
	    case 'th': /* Thai */
		$codePage = 874;
		break;
	    case 'ja': /* Japanese */
		$codePage = 932;
		break;
	    case 'zh': /* Chinese */
		$country = trim(GalleryUtilities::strToLower((string)$country));
		if (in_array($country, array('hans', 'cn'))) {
		    $codePage = 936; /* Chinese (Simplified) */
		} else {
		    $codePage = 950; /* Chinese (Traditional) */
		}
		break; 
	    case 'ko': /* Korean */
		$codePage = 949;
		break;
	    case 'hr': /* Croatian */
	    case 'cs': /* Czech */
	    case 'hu': /* Hungarian */
	    case 'po': /* Polish */
	    case 'ro': /* Romanian */
	    case 'sk': /* Slovak */
	    case 'sl': /* Slovenian */
		$codePage = 1250;
		break;
	    case 'bg': /* Bulgarian */
	    case 'ru': /* Russian */
		$codePage = 1251;
		break;
	    case 'ca': /* Catalan */
	    case 'da': /* Danish */
	    case 'nl': /* Dutch */
	    case 'en': /* English */
	    case 'fi': /* Finnish */
	    case 'fr': /* French */
	    case 'de': /* German */
	    case 'it': /* Italian */
	    case 'no': /* Norwegian */
	    case 'pt': /* Portuguese */
	    case 'es': /* Spanish */
	    case 'sv': /* Swedish */
		$codePage = 1252;
		break;
	    case 'el': /* Greek */
		$codePage = 1253;
		break;
	    case 'tr': /* Turkish */
		$codePage = 1254;
		break;
	    case 'he': /* Hebrew */
	    case 'iw': /* Hebrew (legacy) */
		$codePage = 1255;
		break;
	    case 'ar': /* Arabic */
		$codePage = 1256;
		break;
	    case 'et': /* Estonian */
	    case 'lv': /* Latvian */
	    case 'lt': /* Lithuanian */
		$codePage = 1257;
		break;
	    default: $codePage = 1252;
	}
	return 'CP' . $codePage;
    }
}
?>
