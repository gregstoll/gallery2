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
 * @package Comment
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17679 $
 */
class CommentCallbacks {

    function callback($params, &$smarty, $callback, $userId) {
	global $gallery;
	$phpVm = $gallery->getPhpVm();

	switch ($callback) {
	case 'LoadComments':
	    $itemId = $params['itemId'];
	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($item->getId(), $userId);
	    if ($ret) {
		return $ret;
	    }
	    if (empty($permissions['core.view'])) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	    } else if (empty($permissions['comment.view'])) { 
		return null;
	    }

	    /* Pages shouldn't be cached if comments are not accessible for anonymous */ 
	    if (!$phpVm->headers_sent()) {
		list ($ret, $anonymousUserId) = GalleryCoreApi::getAnonymousUserId();
		if ($ret) {
		    return $ret;
		}
		/* If !comment.view || !core.view */
		list ($ret, $anonymousViewable) = GalleryCoreApi::hasItemPermission(
		    $itemId, 'comment.view', $anonymousUserId, false);
		if ($ret) {
		    return $ret;
		}
		if ($anonymousViewable) {
		    list ($ret, $anonymousViewable) = GalleryCoreApi::hasItemPermission(
			$itemId, 'core.view', $anonymousUserId, false);
		    if ($ret) {
			return $ret;
		    }
		}

		if (!$anonymousViewable) {
		    $expires = $gallery->getHttpDate($phpVm->time() - 7*24*3600);
		    $phpVm->header('Cache-Control: no-cache');
		    $phpVm->header('Pragma: no-cache');
		    $phpVm->header('Expires: ' . $expires);
		}
	    }
	    $viewMax = $params['show'];

	    GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');
	    list ($ret, $comments) =
		GalleryCommentHelper::fetchComments($item->getId(), $viewMax, ORDER_DESCENDING);
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $counts) =
		GalleryCommentHelper::fetchCommentCounts(array($item->getId()));
	    if ($ret) {
		return $ret;
	    }

	    $commenters = array();
	    if (isset($counts[$item->getId()])) {
		$totalComments = $counts[$item->getId()];

		for ($i = 0; $i < sizeof($comments); $i++) {
		    /* Get the commenter ids */
		    $commenters[$comments[$i]->getCommenterId()] = 1;

		    /* Extract the member data */
		    $comments[$i] = (array)$comments[$i];
		    $comments[$i]['randomId'] = $comments[$i]['id'] . '-' . rand() ;
		}

		/* Load all the commenters */
		if (sizeof($commenters) > 0) {
		    list ($ret, $commentersList) =
			GalleryCoreApi::loadEntitiesById(array_keys($commenters), 'GalleryUser');
		    if ($ret) {
			return $ret;
		    }
		}

		foreach ($commentersList as $commenter) {
		    $commenters[$commenter->getId()] = (array)$commenter;
		}
	    } else {
		$totalComments = 0;
	    }

	    foreach (array('delete' => 'comment.delete',
			   'edit' => 'comment.edit',
			   'view' => 'comment.view')
		     as $canFlag => $permission) {
		$can[$canFlag] = isset($permissions[$permission]);
	    }

	    $LoadComments = array();
	    $LoadComments['comments'] = $comments;
	    $LoadComments['commenters'] = $commenters;
	    $LoadComments['can'] = $can;
	    $LoadComments['item'] = (array)$item;
	    $LoadComments['totalComments'] = $totalComments;

	    $block =& $smarty->_tpl_vars['block'];
	    $block['comment']['LoadComments'] = $LoadComments;

	    return null;

	case 'AddComment':
	    /* Load our item */
	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($params['itemId'], 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }

	    GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');
	    $form =& $smarty->_tpl_vars['form'];
	    list ($ret, $AddComment) = GalleryCommentHelper::getAddComment($item, $form);
	    if ($ret) {
		if ($ret->getErrorCode() & ERROR_PERMISSION_DENIED) {
		    return null;
		}
		return $ret;
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['comment']['AddComment'] = $AddComment;

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
