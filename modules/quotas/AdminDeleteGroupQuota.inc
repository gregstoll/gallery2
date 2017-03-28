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
 * This controller will handle the deletion of a group quota
 * @package Quotas
 * @subpackage UserInterface
 * @author Robert Balousek <volksport@users.sf.net>
 * @version $Revision: 17580 $
 */
class AdminDeleteGroupQuotaController extends GalleryController {

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
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}
	if (isset($form['action']['cancel'])) {

	    /* Go back to the AdminGroups view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'quotas.AdminQuotas';

	} else if (isset($form['action']['delete'])) {

	    GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');

	    list ($ret, $quotaExists, $groupQuota) =
		GalleryQuotasHelper::fetchGroupQuota($group->getId());
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($quotaExists) {
		$ret = GalleryCoreApi::removeMapEntry(
		    'QuotasMap', array('userOrGroupId' => $group->getId()));
		if ($ret) {
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminQuotas';
		$status['deletedGroup'] = $group->getGroupName();
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'quotas.AdminDeleteGroupQuota';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for confirmation to delete a group quota
 */
class AdminDeleteGroupQuotaView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}
	GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');
	$groupId = GalleryUtilities::getRequestVariables('groupId');
	list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $quotaExists, $groupQuota) =
	    GalleryQuotasHelper::fetchGroupQuota($group->getId());
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminDeleteGroupQuota') {
	    /* First time around initialize our form */
	    $form['groupName'] = $group->getGroupName();
	    $form['formName'] = 'AdminDeleteGroupQuota';
	}

	$AdminDeleteGroupQuota = array();
	list ($ret, $AdminDeleteGroupQuota['quotaSize'], $AdminDeleteGroupQuota['quotaUnit']) =
	    GalleryQuotasHelper::humanReadableFromKilobytes($groupQuota['quota']);
	if ($ret) {
	    return array($ret, null);
	}
	$AdminDeleteGroupQuota['group'] = (array)$group;

	$template->setVariable('AdminDeleteGroupQuota', $AdminDeleteGroupQuota);
	$template->setVariable('controller', 'quotas.AdminDeleteGroupQuota');
	return array(null, array('body' => 'modules/quotas/templates/AdminDeleteGroupQuota.tpl'));
    }
}
?>
