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
 * Standard gallery theme
 * @package Themes
 * @subpackage Matrix
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 18172 $
 */
class MatrixTheme extends GalleryTheme {

    function MatrixTheme() {
	global $gallery;

	$this->setId('matrix');
	$this->setName($gallery->i18n('Matrix'));
	$this->setDescription($gallery->i18n('Standard Gallery2 look and feel'));
	$this->setVersion('1.1.6');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredThemeApi(array(2, 4));
	$this->setStandardSettings(
	    array('rows' => 3, 'columns' => 3,
		  'showImageOwner' => 0, 'showAlbumOwner' => 1,
		  'albumFrame' => '', 'itemFrame' => '', 'photoFrame' => '',
		  'colorpack' => '', 'showMicroThumbs' => 0, 'dynamicLinks' => 'browse',
		  'sidebarBlocks' => serialize(array(
			array('search.SearchBlock', array('showAdvancedLink' => true)),
			array('core.ItemLinks', array('useDropdown' => false)),
			array('core.PeerList', array()),
			array('imageblock.ImageBlock', array()))),
		  'albumBlocks' => serialize(array(
			array('comment.ViewComments', array()))),
		  'photoBlocks' => serialize(array(
			array('exif.ExifInfo', array()),
			array('comment.ViewComments', array())))));
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
}
?>
