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
 * This controller will handle the deletion of an user
 *
 * User Story:
 * "Joe clicks on 'Users' in the Site Admin view. He wants to delete a user and clicks on 'delete'
 * next to user x. The next view shows a form. If user x was the owner of at least 1 item, Joe is
 * presented with a choice between two radio buttons, between 'Delete user x and...'
 * a) 'assign a new owner for all his items' and
 * b) 'delete all his items and assign a new owner for all non empty albums'
 * Next there's a textfield 'New owner' in the form. Leaving blank this form means to assign the
 * items to the default new owner, 'admin'.  If user x didn't own any items, Joe can just decide
 * between 'cancel' and 'delete'.  When clicking 'cancel', Joe is redirected to the AdminUsers
 * view.  When hitting 'delete' and in case
 * a) the controller checks for the new owner field and assigns the new owner to all items of
 *    user x. Then it deletes user x and redirects to the AdminUser view.
 * b) the controller deletes all GalleryItems which are not AlbumItems, then it checks all albums
 *    of user x. It deletes all empty albums and assigns a new owner to all non empty albums.
 *
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminDeleteUserController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	$userId = GalleryUtilities::getRequestVariables('userId');

	if (isset($form['action']['cancel'])) {

	    /* Go back to the AdminUsers view */
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'core.AdminUsers';

	} else if (isset($form['action']['delete'])) {

	    /* Get the anonymous user for checks */
	    list ($ret, $anonymousUserId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.anonymousUser');
	    if ($ret) {
		return array($ret, null);
	    }

	    /*
	     * Check if the new owner exists (if the name was spelled correctly)
	     */
	    if (isset($form['text']['newOwner']) && $form['text']['newOwner'] != null) {
		list ($ret, $newOwner) =
		    GalleryCoreApi::fetchUserByUserName($form['text']['newOwner']);
		if ($ret) {
		    if (!($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
			return array($ret, null);
		    } else { /* the user was spelled incorrectly, return an error page */
			$error[] = 'form[error][text][noSuchUser]';
		    }
		} else if ($newOwner->getId() == $userId) {
		    /* new Owner = deleted user, doesn't make sense */
		    $error[] = 'form[error][text][newOwnerIsDeletedUser]';
		} else if ($newOwner->getId() == $anonymousUserId) {
		    $error[] = 'form[error][text][newOwnerIsGuest]';
		}
	    } else { /* new owner field is empty, set the default new owner: 'admin' */
		$activeUserId = $gallery->getActiveUserId(); /* activeUser = site admin */
		list ($ret, $newOwner) =
		    GalleryCoreApi::loadEntitiesById($activeUserId, 'GalleryUser');
		if ($ret) {
		    return array($ret, null);
		}
	    }

	    /* Verify the user exists */
	    list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Get all items by the User */
	    list ($ret, $itemIds) = GalleryCoreApi::fetchAllItemIdsByOwnerId($user->getId());
	    if ($ret) {
		return array($ret, null);
	    }

	    /*
	     * Only continue to delete the user if we have no errors and if we don't try
	     * to delete the anonymous user or the active user.  In theory we should never
	     * get to this point unless we're operating on a valid user, so don't bother
	     * sending errors back in case we can't delete.
	     */
	    if (empty($error) && $userId != $anonymousUserId
			&& $userId != $gallery->getActiveUserId()
			&& (empty($itemIds) || (!empty($itemIds) && isset($form['deletionVariant'])
			&& ($form['deletionVariant'] == 'assignNewOwner'
			|| $form['deletionVariant'] == 'deleteItems')))) {

		/* Items for this user exist, first delete the items, then the user */
		if (!empty($itemIds)) {

		/* Only delete items if we choose this deletion variant */
		if ($form['deletionVariant'] == 'deleteItems') {
		    /*
		     * Delete all items the user has permission to delete,
		     * don't delete albums that still contain items
		     */
		    $ret = GalleryCoreApi::deleteUserItems($user->getId());
		    if ($ret) {
			return array($ret, null);
		    }
		}

		/* Assign a new owner for the (remaining) items */
		$ret = GalleryCoreApi::remapOwnerId($user->getId(), $newOwner->getId());
		if ($ret) {
		    return array($ret, null);
		}

		} /* /if !empty($itemIds) */

		/* Delete the user */
		$ret = GalleryCoreApi::deleteEntityById($user->getId(), 'GalleryUser');
		if ($ret) {
		    return array($ret, null);
		}

		/* Request a redirect to the confirmation screen */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'core.AdminUsers';
		$status['deletedUser'] = $user->getUsername();
	    } /* /if empty($error) && $userId != $anonymousUserId ... */
	} /* /if isset($form['action']['delete']) */

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'core.AdminDeleteUser';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for confirmation to delete a user
 */
class AdminDeleteUserView extends GalleryView {

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

	if ($form['formName'] != 'AdminDeleteUser') {
	    /* First time around initialize our form */
	    $form['text']['newOwner'] = '';
	    $form['formName'] = 'AdminDeleteUser';
	}

	$AdminDeleteUser = array();
	$AdminDeleteUser['user'] = (array)$user;
	/* Get all items / the item count of the oldUser */
	list ($ret, $itemIds) = GalleryCoreApi::fetchAllItemIdsByOwnerId($user->getId());
	if ($ret) {
	    return array($ret, null);
	}
	$AdminDeleteUser['numberOfItems'] = count($itemIds);

	$template->setVariable('AdminDeleteUser', $AdminDeleteUser);
	$template->setVariable('controller', 'core.AdminDeleteUser');
	return array(null,
		     array('body' => 'modules/core/templates/AdminDeleteUser.tpl'));
    }
}
?>
