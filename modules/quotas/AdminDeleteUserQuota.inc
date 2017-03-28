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
 * This controller will handle the deletion of a user quota
 * @package Quotas
 * @subpackage UserInterface
 * @author Robert Balousek <volksport@users.sf.net>
 * @version $Revision: 17580 $
 */
class AdminDeleteUserQuotaController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	$userId = GalleryUtilities::getRequestVariables('userId');
	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}
	if (isset($form['action']['cancel'])) {

	    /* Go back to the AdminQuotas view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'quotas.AdminQuotas';

	} else if (isset($form['action']['delete'])) {

	    GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');

	    list ($ret, $quotaExists, $userQuota) =
		GalleryQuotasHelper::fetchUserQuota($user->getId());
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($quotaExists) {
		$ret = GalleryCoreApi::removeMapEntry(
		    'QuotasMap', array('userOrGroupId' => $user->getId()));
		if ($ret) {
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminQuotas';
		$status['deletedUser'] = $user->getUserName();
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'quotas.AdminDeleteUserQuota';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for confirmation to delete a user quota
 */
class AdminDeleteUserQuotaView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}
	GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');
	$userId = GalleryUtilities::getRequestVariables('userId');
	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $quotaExists, $userQuota) =
	    GalleryQuotasHelper::fetchUserQuota($user->getId());
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminDeleteUserQuota') {
	    /* First time around initialize our form */
	    $form['userName'] = $user->getUserName();
	    $form['formName'] = 'AdminDeleteUserQuota';
	}

	$AdminDeleteUserQuota = array();
	list ($ret, $AdminDeleteUserQuota['quotaSize'], $AdminDeleteUserQuota['quotaUnit']) =
	    GalleryQuotasHelper::humanReadableFromKilobytes($userQuota['quota']);
	if ($ret) {
	    return array($ret, null);
	}
	$AdminDeleteUserQuota['user'] = (array)$user;

	$template->setVariable('AdminDeleteUserQuota', $AdminDeleteUserQuota);
	$template->setVariable('controller', 'quotas.AdminDeleteUserQuota');
	return array(null, array('body' => 'modules/quotas/templates/AdminDeleteUserQuota.tpl'));
    }
}
?>
