<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
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
 * This view will show formatted URLs for an item
 *
 * @package GetURLs
 * @subpackage UserInterface
 * @version $Revision: 904 $
 * @author Kirill Talanov <k.talanov@igloos.ca>
 * @author Andy Staudacher <ast@gmx.ch>
 */
class GetUrlsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$itemId = (int)GalleryUtilities::getRequestVariables('itemId');

	GalleryCoreApi::requireOnce('modules/geturls/classes/GetUrlsHelper.class');
	list ($ret, $data) = GetUrlsHelper::getUrlsByItemId($itemId);
	if ($ret) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}

	$template->setVariable('GetUrlsData', $data);

	return array(null,
		     array('body' => 'modules/geturls/templates/GetUrls.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription()
     */
    function getViewDescription() {
	list ($ret, $geturls) = GalleryCoreApi::loadPlugin('module', 'geturls');
	if ($ret) {
	    return array($ret->wrap(__FILE__, __LINE__), null, null);
	}

	return array(null, $geturls->translate('Get item URLs'));
    }
}
?>
