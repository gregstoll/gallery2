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
 * This controller will handle the creation of an user
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminCreateUserController extends GalleryController {

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

	    /* Go back to the AdminUsers view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminUsers';

	} else if (isset($form['action']['create'])) {
	    /*
	     * If all the right fields are in place then go ahead and
	     * create the user.
	     */
	    if (!empty($form['userName']) && !empty($form['email'])
		    && !empty($form['password1']) && $form['password1'] == $form['password2']) {

		list ($ret, $user) =
		    GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryUser');
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($user)) {
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT),
				 null);
		}

		$ret = $user->create($form['userName']);
		if ($ret) {
		    if (!($ret->getErrorCode() & ERROR_COLLISION)) {
			return array($ret, null);
		    }

		    /* Set our error status and fall back to the view */
		    $error[] = 'form[error][userName][exists]';
		} else {
		    $user->setEmail($form['email']);
		    $user->setFullName($form['fullName']);
		    $user->setLanguage($form['language']);
		    GalleryUtilities::unsanitizeInputValues($form['password1'], false);
		    $user->changePassword($form['password1']);

		    $ret = $user->save();
		    if ($ret) {
			return array($ret, null);
		    }

		    /* Request a redirect to the confirmation screen */
		    $redirect['view'] = 'core.SiteAdmin';
		    $redirect['subView'] = 'core.AdminUsers';
		    $status['createdUser'] = $form['userName'];
		}
	    } else {
		foreach (array('userName', 'email', 'password1', 'password2') as $key) {
		    if (empty($form[$key])) {
			$error[] = 'form[error][' . $key . '][missing]';
		    }
		}

		if (!empty($form['password1']) && !empty($form['password2'])
			&& $form['password1'] != $form['password2']) {
		    $error[] = 'form[error][password2][mismatch]';
		}
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminCreateUser';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for data to create a new user
 */
class AdminCreateUserView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminCreateUser') {
	    /* First time around, set our defaults here. */
	    $form['userName'] = '';
	    $form['email'] = '';
	    $form['fullName'] = '';
	    $form['language'] = '';
	    $form['formName'] = 'AdminCreateUser';
	}

	/* Set up our language selection list */
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}
	$languageList = array();
	$languageList[''] = $module->translate('&lt;none&gt;');

	$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	foreach ($supportedLanguages as $language => $countryList) {
	    foreach ($countryList as $country => $languageData) {
		$languageList[$language . '_' . $country] =
		    $languageData['description'];
	    }
	}

	$AdminCreateUser = array();
	$AdminCreateUser['languageList'] = $languageList;

	$template->setVariable('AdminCreateUser', $AdminCreateUser);
	$template->setVariable('controller', 'core.AdminCreateUser');

	return array(null,
		     array('body' => 'modules/core/templates/AdminCreateUser.tpl'));
    }
}
?>
