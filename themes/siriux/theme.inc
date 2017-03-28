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
 * Siriux.net Gallery theme
 * @package Themes
 * @subpackage Siriux
 * @author Nico Kaiser <nico@siriux.net>
 * @version $Revision: 18172 $
 */
class SiriuxTheme extends GalleryTheme {

    function SiriuxTheme() {
	global $gallery;

	$this->setId('siriux');
	$this->setName($gallery->i18n('Siriux'));
	$this->setDescription($gallery->i18n('Siriux.net Gallery theme'));
	$this->setVersion('1.1.5');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredThemeApi(array(2, 4));
	$this->setStandardSettings(
	    array('perPage' => 9,
		  'contentWidth' => 700,
		  'albumBlocks' => serialize(array(
			array('comment.ViewComments', array()))),
		  'photoBlocks' => serialize(array(
			array('exif.ExifInfo', array()),
			array('comment.ViewComments', array()))),
		  'contentWidth' => 700,
		  'groupByYear' => 0,
		  'enableImageMap' => 1));
    }

    /**
     * @see GalleryTheme::showAlbumPage
     */
    function showAlbumPage(&$template, $item, $params, $childIds) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params,
	    array('owner', 'viewCount', 'childCount', 'descendentCount', 'parents',
		  'systemLinks', 'itemLinks', 'itemSummaries', 'permissions',
		  'thumbnails', 'pageNavigator', 'jumpRange'),
	     $childIds);
	if ($ret) {
	    return array($ret, null);
	}

	/* Override some navigator settings */
	$theme =& $template->getVariableByReference('theme');
	if (isset($theme['navigator']['first'])) {
	    unset($theme['navigator']['first']);
	}
	if (isset($theme['navigator']['last'])) {
	    unset($theme['navigator']['last']);
	}

	/* Find the thumbnail size for this album */
	$theme['params']['thumbnailSize'] = 150;
	list ($ret, $preferences) =
	    GalleryCoreApi::fetchDerivativePreferencesForItem($item->getId());
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($preferences as $preference) {
	    if ($preference['derivativeType'] == DERIVATIVE_TYPE_IMAGE_THUMBNAIL &&
		  preg_match('/thumbnail\|(\d+)/', $preference['derivativeOperations'], $matches)) {
		$theme['params']['thumbnailSize'] =  $matches[1];
		break;
	    }
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	$ret = $this->loadCommonTemplateData($template, $item, $params,
	    array('owner', 'parents', 'systemLinks', 'itemLinks', 'permissions',
		  'itemLinksDetailed', 'itemNavigator', 'imageViews'));
	if ($ret) {
	    return array($ret, null);
	}

	/* Override some navigator settings */
	$theme =& $template->getVariableByReference('theme');
	if (isset($theme['navigator']['first'])) {
	    unset($theme['navigator']['first']);
	}
	if (isset($theme['navigator']['last'])) {
	    unset($theme['navigator']['last']);
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showModulePage
     */
    function showModulePage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}

	/* Add in our extra stuff */
	$theme =& $template->getVariableByReference('theme');
	$theme['moduleTemplate'] = $templateFile;

	/* Add our header and styles */
	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showAdminPage
     */
    function showAdminPage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}

	/* Add in our extra stuff */
	$theme =& $template->getVariableByReference('theme');
	$theme['adminTemplate'] = $templateFile;

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showErrorPage
     */
    function showErrorPage(&$template) {
	return array(null, 'error.tpl');
    }

    /**
     * @see GalleryTheme::showProgressBarPage
     */
    function showProgressBarPage(&$template, $item, $params) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::getSettings
     */
    function getSettings($itemId=null) {
	list ($ret, $settings, $params) = parent::getSettings($itemId);
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Add in our custom settings */
	$settings[] = array('key' => 'contentWidth',
			    'name' => $this->translate('Width of page content in pixels'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 2),
			    'value' => $params['contentWidth']);
	$settings[] = array('key' => 'groupByYear',
			    'name' => $this->translate(
				'Group sub-albums by year (album must be sorted by date!)'),
			    'type' => 'checkbox',
			    'value' => $params['groupByYear']);
	$settings[] = array('key' => 'enableImageMap',
			    'name' => $this->translate(
		'Enable prev/next navigation by clicking in the left/right half of the image'),
			    'type' => 'checkbox',
			    'value' => $params['enableImageMap']);

	return array(null, $settings, $params);
    }

    /**
     * @see GalleryTheme::validateSettings
     */
    function validateSettings($settings) {
	$error = parent::validateSettings($settings);
	if (!is_numeric($settings['contentWidth'])) {
	    $error['contentWidth'] = $this->translate('You must enter a number');
	}
	return $error;
    }
}
?>
