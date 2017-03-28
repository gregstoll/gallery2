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
 * This controller will handle the creation of a group
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminCreateGroupController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	if (isset($form['action']['cancel'])) {

	    /* Go back to the AdminGroups view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminGroups';

	} else if (isset($form['action']['create'])) {
	    /*
	     * If all the right fields are in place then go ahead and
	     * create the group.
	     */
	    if (!empty($form['groupName'])) {
		list ($ret, $group) =
		    GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryGroup');
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($group)) {
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT),
				 null);
		}

		$ret = $group->create($form['groupName']);
		if ($ret) {
		    if (!($ret->getErrorCode() & ERROR_COLLISION)) {
			return array($ret, null);
		    }

		    // Set our error status and fall back to the view.
		    $error[] = 'form[error][groupName][exists]';
		} else {
		    $ret = $group->save();
		    if ($ret) {
			return array($ret, null);
		    }

		    /* Request a redirect to the confirmation screen */
		    $redirect['view'] = 'core.SiteAdmin';
		    $redirect['subView'] = 'core.AdminGroups';
		    $status['createdGroup'] = $group->getGroupName();
		}
	    } else {
		// Set our error status and fall back to the view.
		$error[] = 'form[error][groupName][missing]';
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminCreateGroup';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for data to create a new group
 */
class AdminCreateGroupView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminCreateGroup') {
	    $form['groupName'] = '';
	    $form['formName'] = 'AdminCreateGroup';
	}

	$template->setVariable('AdminCreateGroup', array());
	$template->setVariable('controller', 'core.AdminCreateGroup');
	return array(null,
		     array('body' => 'modules/core/templates/AdminCreateGroup.tpl'));
    }
}
?>
