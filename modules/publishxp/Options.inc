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
 * Controller to set options on how photos are uploaded.
 * Allows the user to specify options that control how the photos are created within Gallery.
 * The two options supported are for whether the file extension should be used in the title
 * and if the captions should be set automatically.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class OptionsController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$results = array();
	if (isset($form['action']['setOptions'])) {
	    /* No form processing/validation required so just continue the next page */
	    $results['redirect']['view'] = 'publishxp.UploadItems';
	    $results['redirect']['stripExtensions'] = isset($form['stripExtensions']);
	    $results['redirect']['setCaptions'] = isset($form['setCaptions']);
	    $results['redirect']['albumId'] = $form['albumId'];

	    /*
	     * Any options set by ItemAddOptions also need to get passed forward but
	     * we don't know what or how complex they are, so let's take the easy route of storing
	     * the form in the session for later.
	     */
	    $session =& $gallery->getSession();
	    $session->put('publishxp.extraOptionsForm', serialize($form));
	} else {
	    $results['delegate']['view'] = 'publishxp.Options';
	}
	$results['status'] = array();
	$results['error'] = array();
	return array(null, $results);
    }
}

/**
 * View to set options on how photos are uploaded.
 * Allows the user to specify options that control how the photos are created within Gallery.
 * The two options supported are for whether the file extension should be used in the title
 * and if the captions should be set automatically.
 */
class OptionsView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	if ($form['formName'] != 'Options') {
	    $form['formName'] = 'Options';

	    list ($form['albumId'], $form['stripExtensions'], $form['setCaptions']) =
		GalleryUtilities::getRequestVariables('albumId', 'stripExtensions', 'setCaptions');

	    /* Strip extensions defaults to true, if there was no query parameter for it */
	    if ($form['stripExtensions'] == null) {
		$form['stripExtensions'] = true;
	    }

	    if (empty($form['albumId'])) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }
	}
	$form['albumId'] = (int)$form['albumId'];

	/* Get all the add options */
	GalleryCoreApi::requireOnce('modules/core/ItemAdd.inc');
	if (isset($this->_optionInstances)) {
	    $optionInstances = $this->_optionInstances;
	} else {
	    list ($ret, $optionInstances) = ItemAddOption::getAllAddOptions();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $permissions) = GalleryCoreApi::getPermissions($form['albumId']);
	if ($ret) {
	    return array($ret, null);
	}
	if (empty($permissions['core.view'])) {
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	} else if (empty($permissions['core.addDataItem'])
		   && empty($permissions['core.addAlbumItem'])) {
	    return array(GalleryCoreApi::error(ERROR_PERMISSION_DENIED), null);
	}

	/* Now let all options load their template data */
	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($form['albumId'], 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	$Options['options'] = array();
	foreach ($optionInstances as $option) {
	    list ($ret, $entry['file'], $entry['l10Domain']) =
		$option->loadTemplate($template, $form, $item);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!empty($entry['file'])) {
		$Options['options'][] = $entry;
	    }
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'publishxp');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Windows Publishing Wizard'));
	$template->setVariable('Options', $Options);
	$template->head('modules/publishxp/templates/Head.tpl');

	return array(null, array('body' => 'modules/publishxp/templates/Options.tpl',
				 'useFullScreen' => true));
    }
}
?>
