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
 * This controller will handle the addition of an album
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17589 $
 */
class ItemAddAlbumController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* Make sure we have permission do edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLock($itemId);
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['create'])) {
	    /* Validate the input data */
	    $platform =& $gallery->getPlatform();

	    if (empty($form['pathComponent'])) {
		$error[] = 'form[error][pathComponent][missing]';
	    } else if (!$platform->isLegalPathComponent($form['pathComponent'], true)) {
		$error[] = 'form[error][pathComponent][invalid]';
	    }

	    if (empty($error)) {
		list ($ret, $markup) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
		if ($ret) {
		    return array($ret, null);
		}
		if ($markup == 'html') {
		    /* Strip malicious content if html markup allowed */
		    $form['title'] = GalleryUtilities::htmlSafe($form['title'], true);
		    $form['summary'] = GalleryUtilities::htmlSafe($form['summary'], true);
		    $form['description'] = GalleryUtilities::htmlSafe($form['description'], true);
		}

		list ($ret, $instance) =
		    GalleryCoreApi::createAlbum($itemId, $form['pathComponent'], $form['title'],
						$form['summary'], $form['description'],
						$form['keywords']);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		if (!isset($instance)) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
		}

		/* Figure out where to redirect upon success */
		$redirect['itemId'] = $instance->getId();
		$ret = GalleryCoreApi::assertHasItemPermission($instance->getId(), 'core.edit');
		if (empty($ret)) {
		    $redirect['view'] = 'core.ItemAdmin';
		    $redirect['subView'] = 'core.ItemEdit';
		    $redirect['editPlugin'] = 'ItemEditAlbum';
		}

		/* Prepare our status message */
		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockIds);
		    return array($ret, null);
		}

		$status['editMessage'] = $module->translate('Album created successfully.');
	    }
	}

	$ret = GalleryCoreApi::releaseLocks($lockIds);
	if ($ret) {
	    return array($ret, null);
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'core.ItemAddAlbum';
	}
	$results['status'] = $status;
	$results['error'] = $error;


	return array(null, $results);
    }
}

/**
 * This view will prompt for the data for a new album to be created
 */
class ItemAddAlbumView extends GalleryView {

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

	/* Make sure we have permission do edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] == 'ItemAddAlbum') {

	    /* Complain if we have any invalid data */

	} else {
	    /* First time around, load the form with item data */
	    $form['pathComponent'] = '';
	    $form['title'] = '';
	    $form['summary'] = '';
	    $form['keywords'] = '';
	    $form['description'] = '';
	    $form['formName'] = 'ItemAddAlbum';
	}

	$template->setVariable('ItemAddAlbum', array());
	$template->setVariable('controller', 'core.ItemAddAlbum');
	return array(null, array('body' => 'modules/core/templates/ItemAddAlbum.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $core->translate('add sub-album'));
    }
}
?>
