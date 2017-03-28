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
 * This view will show an error message when an item cannot be displayed
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ShowItemErrorView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $item, $wasSpecified) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	$problem = GalleryUtilities::getRequestVariables('problem');

	switch ($problem) {
	case 'missingTheme':
	    if ($wasSpecified) {
		list ($ret, $themeId) = GalleryCoreApi::fetchThemeId($item);
		if ($ret) {
		    return array($ret, null);
		}

		if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
		    $targetId = $item->getParentId();
		} else {
		    $targetId = $item->getId();
		}

		list ($ret, $canEdit) = GalleryCoreApi::hasItemPermission($targetId, 'core.edit');
		if ($ret) {
		    return array($ret, null);
		}
	    } else {
		list ($ret, $themeId) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'default.theme');
		if ($ret) {
		    return array($ret, null);
		}

		$targetId = null;
		$canEdit = false;
	    }

	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret) {
		return array($ret, null);
	    }

	    $template->setVariable('ShowItemError',
				   array('itemId' => $targetId,
					 'themeId' => $themeId,
					 'canEdit' => $canEdit,
					 'isAdmin' => $isAdmin));
	    return array(null,
			 array('body' => 'modules/core/templates/ShowItemError.tpl'));

	default:
	    /*
	     * We know of no other problems!   Returning an empty result will
	     * throw up a security violation page.
	     */
	    return array(null, array());
	}
    }

    /**
     * @see GalleryView::getViewType
     */
    function getViewType() {
	return VIEW_TYPE_ADMIN;
    }
}
?>
