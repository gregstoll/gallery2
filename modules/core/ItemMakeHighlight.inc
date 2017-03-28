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
 * This controller will make an item the highlight for its parent
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Ernesto Baschny <ernst@baschny.de>
 * @version $Revision: 17580 $
 */
class ItemMakeHighlightController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$status = array();
	$error = array();

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	if (isset($form['action']['makeHighlight'])) {
	    if (empty($form['parentId'])) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }
	    $parentId = (int)$form['parentId'];

	    list ($ret, $canView) = GalleryCoreApi::hasItemPermission($parentId, 'core.view');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!$canView) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }

	    /* Make sure we have permission to edit this item */
	    $ret = GalleryCoreApi::assertHasItemPermission($parentId, 'core.edit');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* XXX: What should we do if this fails? */
	    list ($ret, $success) =
		GalleryCoreApi::setThumbnailFromItem($parentId, $itemId);
	    if ($ret) {
		return array($ret, null);
	    }
	} /* else $form['action']['cancel'] */

	$results['return'] = true;
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for which ancestor album to set the highlight
 */
class ItemMakeHighlightView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();
	$parentId = $item->getParentId();

	/* Make sure we have permission do edit the parent of this item */
	$ret = GalleryCoreApi::assertHasItemPermission($parentId, 'core.edit');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ItemMakeHighlight') {
	    $form['formName'] = 'ItemMakeHighlight';
	}

	list ($ret, $parents) =
	    GalleryCoreApi::fetchParents($item, array('core.edit', 'core.view'), true);
	if ($ret) {
	    return array($ret, null);
	}

	$parentList = array();
	foreach (array_reverse($parents) as $parent) {
	    $parentList[] = (array)$parent;
	}

	$template->setVariable('ItemMakeHighlight', array('parentList' => $parentList));
	$template->setVariable('controller', 'core.ItemMakeHighlight');
	return array(null, array('body' => 'modules/core/templates/ItemMakeHighlight.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('make highlight'));
    }
}
?>
