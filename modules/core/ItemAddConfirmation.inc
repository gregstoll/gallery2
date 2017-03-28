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
 * This view will show confirmation of newly added items
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemAddConfirmationView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/*
	 * We know what kind of operation happened, but for now let's compress
	 * it down so that the template doesn't have to know.
	 */
	$status = $template->getVariable('status');
	$uploaded = 0;
	if (!empty($status['addedFiles'])) {
	    $count = sizeof($status['addedFiles']);
	    for ($i = 0; $i < $count; $i++) {
		list ($ret, $entity) = GalleryCoreApi::loadEntitiesById(
		    $status['addedFiles'][$i]['id'], 'GalleryItem');
		if (!$ret) {
		    $status['addedFiles'][$i]['exists'] = true;
		    $uploaded++;
		} else if ($ret->getErrorCode() & (ERROR_MISSING_OBJECT | ERROR_BAD_PARAMETER)) {
		    $status['addedFiles'][$i]['exists'] = false;
		} else {
		    return array($ret, null);
		}
	    }
	} else {
	    $status['addedFiles'] = array();
	}

	$ItemAddConfirmation = array();
	$ItemAddConfirmation['count'] = $uploaded;
	$ItemAddConfirmation['status'] = $status;

	$template->setVariable('ItemAddConfirmation', $ItemAddConfirmation);
	$template->setVariable('controller', '');
	return array(null, array('body' => 'modules/core/templates/ItemAddConfirmation.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('add items confirmation'));
    }
}
?>
