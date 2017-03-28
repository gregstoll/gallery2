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
 * Settings for Registration
 * @package Registration
 * @subpackage UserInterface
 * @author Sebastian Eichner <mailsp@sebastian-eichner.de>
 * @version $Revision: 17580 $
 */
class AdminSelfRegistrationController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$error = $status = array();
	if (isset($form['action']['save'])) {

	    $form['emailadmins'] = isset($form['emailadmins']) ? 1 : 0;
	    $form['emailusers'] = isset($form['emailusers']) ? 1 : 0;
	    foreach (array('confirmation', 'emailadmins', 'adminsubject', 'subject', 'from',
			   'emailusers', 'usersubject') as $key) {
		if (isset($form[$key])) {
		    $ret = GalleryCoreApi::setPluginParameter(
					   'module', 'register', $key, trim($form[$key]));
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    $status['saved'] = 1;
	} else if (isset($form['action']['activate'])) {
	    GalleryCoreApi::requireOnce(
		'modules/register/classes/GalleryPendingUserHelper.class');
	    list ($ret, $pendingUser) =
		GalleryCoreApi::loadEntitiesById($form['userId'], 'GalleryPendingUser');
	    if ($ret) {
		if (!($ret->getErrorCode() &  ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
	    } else {
		$userName = $pendingUser->getUserName();
		$ret = GalleryPendingUserHelper::createGalleryUser($pendingUser);
		if ($ret) {
		    return array($ret, null);
		}
		$status['activated'] = $userName;
	    }
	} else if (isset($form['action']['delete'])) {
	    list ($ret, $pendingUser) =
		GalleryCoreApi::loadEntitiesById($form['userId'], 'GalleryPendingUser');
	    if ($ret) {
		if (!($ret->getErrorCode() &  ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
	    } else {
		$userName = $pendingUser->getUserName();
		list ($ret, $lock) = GalleryCoreApi::acquireWriteLock(array($pendingUser->getId()));
		if ($ret) {
		    return array($ret, null);
		}
		$ret = $pendingUser->delete();
		if ($ret) {
		    return array($ret, null);
		}
		$ret = GalleryCoreApi::releaseLocks($lock);
		if ($ret) {
		    return array($ret, null);
		}
		$status['deleted'] = $userName;
	    }
	} /* else $form['action']['cancel'] */

	if (empty($error)) {
	    $results['redirect'] = array('view' => 'core.SiteAdmin',
					 'subView' => 'register.AdminSelfRegistration');
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'register.AdminSelfRegistration';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Registration
 */
class AdminSelfRegistrationView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/register/classes/GalleryPendingUserHelper.class');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'register');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load our default values if we didn't just come from this form. */
	if ($form['formName'] != 'AdminSelfRegistration') {
	    foreach (array('confirmation', 'emailadmins', 'adminsubject', 'subject', 'from',
			   'emailusers', 'usersubject') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'register', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $form['formName'] = 'AdminSelfRegistration';
	}

	if (!isset($form['list']['page']) || $form['list']['page'] < 1) {
	    $form['list']['page'] = 1;
	}

	list ($ret, $form['list']['count']) = GalleryPendingUserHelper::fetchUserCount();
	if ($ret) {
	    return array($ret, null);
	}
	if ($form['list']['count'] > 0) {

	    $form['list']['pageSize'] = min($form['list']['count'], 10);
	    $form['list']['maxPages'] = ceil($form['list']['count'] / $form['list']['pageSize']);
	    if ($form['list']['page'] > $form['list']['maxPages']) {
		$form['list']['page'] = $form['list']['maxPages'];
	    }
	    $form['list']['nextPage'] = min($form['list']['page']+1, $form['list']['maxPages']);
	    $form['list']['backPage'] = max(1, $form['list']['page']-1);

	    list ($ret, $form['list']['userNames']) = GalleryPendingUserHelper::fetchUserData(
		$form['list']['pageSize'], ($form['list']['page'] - 1) * $form['list']['pageSize']);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$selfregistration = array();
	$selfregistration['emailConfirmationList'] = array(
	    'admin' => $module->translate('Activation by administrator'),
	    'email' => $module->translate('Use confirmation emails'),
	    'auto' => $module->translate('Accept without confirmation') );
	$selfregistration['reallySendList'] = array(
	    'true' => $module->translate('Yes'), 'false' => $module->translate('No') );

	$template->setVariable('SelfRegistration', $selfregistration);
	$template->setVariable('controller', 'register.AdminSelfRegistration');
	return array(null, array('body' => 'modules/register/templates/AdminSelfRegistration.tpl'));
    }
}
?>
