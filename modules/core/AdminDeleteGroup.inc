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
 * This controller will handle the deletion of a group
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminDeleteGroupController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	$groupId = GalleryUtilities::getRequestVariables('groupId');

	if (isset($form['action']['cancel'])) {

	    /* Go back to the AdminGroups view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminGroups';

	} else if (isset($form['action']['delete'])) {

	    /* Only allow users to delete GROUP_NORMAL groups. */
	    list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	    if ($ret) {
		return array($ret, null);
	    }

	    $ret = GalleryCoreApi::deleteEntityById($group->getId(), 'GalleryGroup');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Request a redirect to the confirmation screen */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminGroups';
	    $status['deletedGroup'] = $group->getGroupName();
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminDeleteGroup';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for confirmation to delete a group
 */
class AdminDeleteGroupView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$groupId = GalleryUtilities::getRequestVariables('groupId');
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminDeleteGroup') {
	    /* First time around initialize our form */
	    $form['groupName'] = $group->getGroupName();
	    $form['formName'] = 'AdminDeleteGroup';
	}

	$AdminDeleteGroup = array();
	$AdminDeleteGroup['group'] = (array)$group;

	/* Render the HTML body */
	$template->setVariable('AdminDeleteGroup', $AdminDeleteGroup);
	$template->setVariable('controller', 'core.AdminDeleteGroup');

	return array(null,
		     array('body' => 'modules/core/templates/AdminDeleteGroup.tpl'));
    }
}
?>
