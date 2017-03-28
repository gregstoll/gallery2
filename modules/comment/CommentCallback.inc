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
 * This view handles Ajax calls.
 * @package Comment
 * @author  Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class CommentCallbackView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isControllerLike
     */
    function isControllerLike() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;

	$result = array();
	list ($command, $commentId) = GalleryUtilities::getRequestVariables('command', 'commentId');
	$commentId = (int)$commentId;
	switch ($command) {
	case 'delete':
	    $ret = $this->deleteComment($commentId);
	    break;

	case 'spam':
	    $ret = $this->changePublishStatus($commentId, COMMENT_PUBLISH_STATUS_SPAM);
	    break;

	case 'despam':
	    $ret = $this->changePublishStatus($commentId, COMMENT_PUBLISH_STATUS_PUBLISHED);
	    break;

	default:
	    $ret = GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}

	if ($ret) {
	    $result['status'] = 'error';
	    $storage =& $gallery->getStorage();
	    $ret->putInSession();
	} else {
	    $result['status'] = 'success';
	}

	GalleryCoreApi::requireOnce('lib/JSON/JSON.php');
	$json = new Services_JSON();
	print $json->encode($result);
	return null;
    }

    /**
     * Delete the comment with the given id.
     * @param $id int the id
     */
    function deleteComment($commentId) {
	if (empty($commentId)) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}

	list ($ret, $comment) = GalleryCoreApi::loadEntitiesById($commentId, 'GalleryComment');
	if ($ret) {
	    return $ret;
	}

	list ($ret, $permissions) = GalleryCoreApi::getPermissions($comment->getParentId());
	if ($ret) {
	    return $ret;
	}
	if (empty($permissions['core.view'])) {
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	} else if (empty($permissions['comment.delete'])) {
	    return GalleryCoreApi::error(ERROR_PERMISSION_DENIED);
	}

	$ret = GalleryCoreApi::deleteEntityById($commentId, 'GalleryComment');
	if ($ret) {
	    return $ret;
	}
    }

    /**
     * Set the publish status for the comment with the given id to given value
     * @param $commentId int the id
     * @param $newPublishStatus the new publish status
     */
    function changePublishStatus($commentId, $newPublishStatus) {
	if (empty($commentId)) {
	    return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}

	list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock(array($commentId));
	if ($ret) {
	    return $ret;
	}

	list ($ret, $comment) = GalleryCoreApi::loadEntitiesById($commentId, 'GalleryComment');
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockId);
	    return $ret;
	}

	list ($ret, $permissions) = GalleryCoreApi::getPermissions($comment->getParentId());
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockId);
	    return $ret;
	}
	if (empty($permissions['core.view'])) {
	    GalleryCoreApi::releaseLocks($lockId);
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	} else if (empty($permissions['comment.edit'])) {
	    GalleryCoreApi::releaseLocks($lockId);
	    return GalleryCoreApi::error(ERROR_PERMISSION_DENIED);
	}

	$comment->setPublishStatus($newPublishStatus);
	$ret = $comment->save();
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockId);
	    return $ret;
	}

	$ret = GalleryCoreApi::releaseLocks($lockId);
	if ($ret) {
	    return $ret;
	}

	return null;
    }
}
?>
