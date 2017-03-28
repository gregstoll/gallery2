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

GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');

/**
 * Edit default placement and settings for a watermark image
 * @package Watermark
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class WatermarkSiteAdminEditController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $results) = WatermarkHelper::handleEditRequest($form,
	    'core.SiteAdmin', 'watermark.WatermarkSiteAdmin', 'watermark.WatermarkSiteAdminEdit');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $results);
    }
}

/**
 * Edit default placement and settings for a watermark image
 */
class WatermarkSiteAdminEditView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$ret = WatermarkHelper::loadEditTemplate($template, $form,
						 'WatermarkSiteAdminEdit', 'SiteAdmin');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null,
		     array('body' => 'modules/watermark/templates/WatermarkSiteAdminEdit.tpl'));
    }
}
?>
