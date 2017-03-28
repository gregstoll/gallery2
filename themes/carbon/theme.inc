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
 * Carbon theme
 * @package Themes
 * @subpackage Carbon
 * @author Mustafa Incel (based on Matrix by Bharat Mediratta <bharat@menalto.com>)
 * @version $Revision: 17580 $
 */
class CarbonTheme extends GalleryTheme {

    function CarbonTheme() {
	global $gallery;

	$this->setId('carbon');
	$this->setName($gallery->i18n('Carbon'));
	$this->setDescription($gallery->i18n('Gallery2 theme by mincel.com'));
	$this->setVersion('1.2.6');
	$this->setRequiredCoreApi(array(7, 51));
	$this->setRequiredThemeApi(array(2, 5));
	$this->setStandardSettings(
	    array('rows' => 3, 'columns' => 3,
		  'showImageOwner' => 0, 'showAlbumOwner' => 1,
		  'albumFrame' => '', 'itemFrame' => '', 'photoFrame' => '',
		  'colorpack' => '', 'showMicroThumbs' => 1, 'dynamicLinks' => 'browse',
		  'sidebarBlocks' => serialize(array(
			array('search.SearchBlock', array('showAdvancedLink' => true)),
			array('core.ItemLinks', array('useDropdown' => false)),
			array('core.PeerList', array()),
			array('imageblock.ImageBlock', array()))),
		  'albumBlocks' => serialize(array(
			array('comment.ViewComments', array()))),
		  'photoBlocks' => serialize(array(
			array('exif.ExifInfo', array()),
			array('comment.ViewComments', array()))),
		  'navigatorPhotoTop' => 1, 'navigatorPhotoBottom' => 1,
		  'photoProperties' => 1, 'fullSize' => 1,
		  'itemDetails' => 1,
		  'albumUpperBlocks' => serialize(array()),
		  'photoUpperBlocks' => serialize(array()),
		  'sidebarTop' => 106, 'maxMicroThumbs' => 12,
		  'logoImageLocation' => '',
		  'extraLink' => '', 'extraLinkUrl' => '', 'copyright' => '',
	));
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
	$settings[] = array('key' => 'navigatorPhotoTop',
			    'name' => $this->translate('Show top photo navigator'),
			    'type' => 'checkbox',
			    'value' => $params['navigatorPhotoTop']);
	$settings[] = array('key' => 'navigatorPhotoBottom',
			    'name' => $this->translate('Show bottom photo navigator'),
			    'type' => 'checkbox',
			    'value' => $params['navigatorPhotoBottom']);
	$settings[] = array('key' => 'photoProperties',
			    'name' => $this->translate('Show \'Photo Properties\' icon'),
			    'type' => 'checkbox',
			    'value' => $params['photoProperties']);
	$settings[] = array('key' => 'fullSize',
			    'name' => $this->translate('Show \'Full Size\' icon'),
			    'type' => 'checkbox',
			    'value' => $params['fullSize']);
	$settings[] = array('key' => 'itemDetails',
			    'name' => $this->translate('Show item details on album pages'),
			    'type' => 'checkbox',
			    'value' => $params['itemDetails']);
	$settings[] = array('key' => 'albumUpperBlocks',
			    'name' => $this->translate(
				'Blocks to show on album pages (below description)'),
			    'type' => 'block-list',
			    'typeParams' => array('packType' => 'block-list'),
			    'value' => $params['albumUpperBlocks']);
	$settings[] = array('key' => 'photoUpperBlocks',
			    'name' => $this->translate(
				'Blocks to show on photo pages (below description)'),
			    'type' => 'block-list',
			    'typeParams' => array('packType' => 'block-list'),
			    'value' => $params['photoUpperBlocks']);
	$settings[] = array('key' => 'sidebarTop',
			    'name' => $this->translate('Sidebar top position'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 4),
			    'value' => $params['sidebarTop']);
	$settings[] = array('key' => 'maxMicroThumbs',
			    'name' => $this->translate(
				'Maximum number of micro navigation thumbnails'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 2),
			    'value' => $params['maxMicroThumbs']);
	$settings[] = array('key' => 'logoImageLocation',
			    'name' => $this->translate('URL or path to alternate logo image'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 30),
			    'value' => $params['logoImageLocation']);
	$settings[] = array('key' => 'extraLink',
			    'name' => $this->translate('Extra link for top navigation bar'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 30),
			    'value' => $params['extraLink']);
	$settings[] = array('key' => 'extraLinkUrl',
			    'name' => $this->translate('URL for the extra link'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 30),
			    'value' => $params['extraLinkUrl']);
	$settings[] = array('key' => 'copyright',
			    'name' => $this->translate('Copyright message to display on footer'),
			    'type' => 'text-field',
			    'typeParams' => array('size' => 30),
			    'value' => $params['copyright']);

	return array(null, $settings, $params);
    }

    /**
     * @see GalleryTheme::validateSettings
     */
    function validateSettings($settings) {
	$error = parent::validateSettings($settings);
	if (empty($settings['sidebarTop']) || !is_numeric($settings['sidebarTop'])) {
	    $error['sidebarTop'] = $this->translate('You must enter a number greater than 0');
	}
	if (empty($settings['maxMicroThumbs']) || !is_numeric($settings['maxMicroThumbs'])) {
	    $error['maxMicroThumbs'] = $this->translate('You must enter a number greater than 0');
	}
	foreach (array('albumUpperBlocks', 'photoUpperBlocks') as $blockKey) {
	    if (isset($standard[$blockKey])) {
		if (!empty($settings[$blockKey])) {
		    list ($success, $newValue) =
			$this->packSetting('block-list', $settings[$blockKey]);
		    if (!$success) {
			$error[$blockKey] =
			    $this->translate('Format: [module.BlockName param=value] ...');
		    }
		}
	    }
	}
	return $error;
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

	/* Add in our extra stuff */
	$theme =& $template->getVariableByReference('theme');
	$theme['columnWidthPct'] = floor(100 / $params['columns']);

	/* Check for JavaScript warning flag */
	if (GalleryUtilities::getRequestVariables('jsWarning') != null) {
	    $template->setVariable('jsWarning', true);
	}

	/* Add our header and styles */
	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	$dataTypes = array('owner', 'parents', 'systemLinks', 'itemLinks', 'permissions',
			   'itemLinksDetailed', 'itemNavigator', 'imageViews');
	if (!empty($params['showMicroThumbs'])) {
	    $dataTypes[] = 'navThumbnails';
	}
	$ret = $this->loadCommonTemplateData($template, $item, $params, $dataTypes);
	if ($ret) {
	    return array($ret, null);
	}

	/* Check for JavaScript warning flag */
	if (GalleryUtilities::getRequestVariables('jsWarning') != null) {
	    $template->setVariable('jsWarning', true);
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

	/* Check for JavaScript warning flag */
	if (GalleryUtilities::getRequestVariables('jsWarning') != null) {
	    $template->setVariable('jsWarning', true);
	}

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
     * @see GalleryPlugin::upgrade
     */
    function upgrade($currentVersion) {
	if (isset($currentVersion) && version_compare($currentVersion, '1.1.2', '<')) {
	    /* Rename albumBlocks2 and photoBlocks2 */
	    foreach (array('albumBlocks2' => 'albumUpperBlocks',
			   'photoBlocks2' => 'photoUpperBlocks') as $oldKey => $newKey) {
		list ($ret, $value) = $this->getParameter($oldKey);
		if ($ret) {
		    return $ret;
		}
		$ret = $this->setParameter($newKey, $value);
		if ($ret) {
		    return $ret;
		}
		$ret = $this->removeParameter($oldKey);
		if ($ret) {
		    return $ret;
		}
	    }
	}
	return null;
    }
}
?>
