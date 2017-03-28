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
 * This controller will handle the creation of a user quota
 * @package Quotas
 * @subpackage UserInterface
 * @author Robert Balousek <volksport@users.sf.net>
 * @version $Revision: 17580 $
 */
class AdminCreateUserQuotaController extends GalleryController {

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
	} else if (isset($form['action']['create'])) {
	    /* If all the right fields are in place then go ahead and create the user quota */
	    if (array_key_exists('quotaSize', $form) && array_key_exists('quotaUnit', $form)) {
		if (!empty($form['quotaSize']) && is_numeric($form['quotaSize']) &&
		    $form['quotaSize'] >= 0 && !empty($form['quotaSize'])) {
		    GalleryCoreApi::requireOnce(
			'modules/quotas/classes/GalleryQuotasHelper.class');
		    $form['quotaSize'] = round((float)$form['quotaSize'], 2);
		    if ($form['quotaUnit'] == 'KB') {
			$quotaSize = round($form['quotaSize']);
		    } else if ($form['quotaUnit'] == 'MB') {
			$quotaSize = round($form['quotaSize'] * 1024.00);
		    } else if ($form['quotaUnit'] == 'GB') {
			$quotaSize = round($form['quotaSize'] * 1024.00 * 1024.00);
		    } else {
			return array(GalleryCoreApi::error(ERROR_MISSING_VALUE), null);
		    }
		    if ($quotaSize <= 2147483647) {
			if (!isset($user)) {
			    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
			}
			list ($ret, $quotaExists, $userQuota) =
			    GalleryQuotasHelper::fetchUserQuota($user->getId());
			if ($ret) {
			    return array($ret, null);
			}
			if (!$quotaExists) {
			    $ret = GalleryCoreApi::addMapEntry(
				'QuotasMap',
				array('userOrGroupId' => $user->getId(),
				      'quotaSize' => $quotaSize));
			    if ($ret) {
				return array($ret, null);
			    }

			    /* Request a redirect to the confirmation screen */
			    $redirect['view'] = 'core.SiteAdmin';
			    $redirect['subView'] = 'quotas.AdminQuotas';
			    $status['createdUser'] = $user->getUserName();
			}
		    } else {
			$error[] = 'form[error][quotaSize][tooLarge]';
			$error[] = 'form[error][quotaSize][' . $quotaSize. ']';
		    }
		} else {
		    $error[] = 'form[error][quotaSize][notNumeric]';
		}
	    } else {
		/* set our error status and fall back to the view */
		if (empty($form['quotaSize'])) {
		    $error[] = 'form[error][quotaSize][missing]';
		}
		if (empty($form['quotaUnit'])) {
		    $error[] = 'form[error][quotaUnit][missing]';
		}
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'quotas.AdminCreateUserQuota';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for data to create a new user
 */
class AdminCreateUserQuotaView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$userId = GalleryUtilities::getRequestVariables('userId');
	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminCreateUserQuota') {
	    /* Set the form defaults */
	    $form['quotaSize'] = '';
	    $form['quotaUnit'] = '';
	    $form['formName'] = 'AdminCreateUserQuota';
	}

	$form['userName'] = $user->getUserName();
	$form['userId'] = $user->getId();

	$AdminCreateUserQuota = array();
	$AdminCreateUserQuota['userId'] = $user->getId();
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'quotas');
	if ($ret) {
	    return array($ret, null);
	}
	$AdminCreateUserQuota['quotaUnitChoices'] = array('KB' => $module->translate('KB'),
							  'MB' => $module->translate('MB'),
							  'GB' => $module->translate('GB'));

	$template->setVariable('AdminCreateUserQuota', $AdminCreateUserQuota);
	$template->setVariable('controller', 'quotas.AdminCreateUserQuota');
	return array(null, array('body' => 'modules/quotas/templates/AdminCreateUserQuota.tpl'));
    }
}
?>
