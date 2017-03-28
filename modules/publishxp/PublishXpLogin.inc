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
 * Controller to initialize the process of publishing photos.
 * Allows the user to log in to Gallery to begin the process of uploading
 * photos through Windows XP's publishing mechanism.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class PublishXpLoginController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$results = $error = array();
	if (isset($form['action']['login'])) {
	    if (empty($form['userName'])) {
		$error[] = 'form[error][userName][missing]';
	    }

	    if (empty($form['password'])) {
		$error[] = 'form[error][password][missing]';
	    }

	    if (empty($error)) {
		list ($ret, $isDisabled) = GalleryCoreApi::isDisabledUsername($form['userName']);
		if ($ret) {
		    return array($ret, null);
		}
		if ($isDisabled) {
		    $error[] = 'form[error][userName][disabled]';
		}
	    }

	    if (empty($error)) {
		list ($ret, $user) = GalleryCoreApi::fetchUserByUsername($form['userName']);
		if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
		if (isset($user) && $user->isCorrectPassword($form['password'])) {
		    $gallery->setActiveUser($user);

		    $event = GalleryCoreApi::newEvent('Gallery::Login');
		    $event->setEntity($user);
		    list ($ret, $ignoreTheRedirect) = GalleryCoreApi::postEvent($event);
		    if ($ret) {
			return array($ret, null);
		    }

		    $results['redirect']['view'] = 'publishxp.SelectAlbum';
		}
	    }

	    if (!empty($error) && !empty($form['userName'])) {
		$event = GalleryCoreApi::newEvent('Gallery::FailedLogin');
		$event->setData(array('userName' => $form['userName']));
		list ($ret, $ignored) = GalleryCoreApi::postEvent($event);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	if (empty($results['redirect']) && !isset($results['delegate']['view'])) {
	    $results['delegate']['view'] = 'publishxp.PublishXpLogin';
	}
	$results['status'] = array();
	$results['error'] = $error;
	return array(null, $results);
    }
}

/**
 * View to initialize the process of publishing photos.
 * Allows the user to log in to Gallery to begin the process of uploading
 * photos through Windows XP's publishing mechanism.
 */
class PublishXpLoginView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	if ($form['formName'] != 'PublishXpLogin') {
	    $form['userName'] = '';
	    $form['formName'] = 'PublishXpLogin';
	}

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}

	if (!$isAnonymous) {
	    /* No need to log in */
	    return array(null, array('redirect' => array('view' => 'publishxp.SelectAlbum')));
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'publishxp');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Windows Publishing Wizard'));
	$template->setVariable('controller', 'publishxp.PublishXpLogin');
	$template->head('modules/publishxp/templates/Head.tpl');

	return array(null, array('body' => 'modules/publishxp/templates/PublishXpLogin.tpl',
				 'useFullScreen' => true));
    }
}
?>
