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
 * This controller will handle the changes made to a comment
 * @package Comment
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class EditCommentController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	$commentId = (int)GalleryUtilities::getRequestVariables('commentId');

	/* Make sure we have permission to edit a comment */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'comment.edit');
	if ($ret) {
	    return array($ret, null);
	}

	$status = array();
	$error = array();
	if (isset($form['action']['save'])) {
	    /* Edit the comment */
	    list ($ret, $comment) = GalleryCoreApi::loadEntitiesById($commentId, 'GalleryComment');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Make sure that the comment<->parent relationship is right */
	    if ($comment->getParentId() != $itemId) {
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }

	    /* Verify the input parameters */
	    if (empty($form['commenterName'])) {
		$error[] = 'form[error][commenterName][missing]';
	    } else {
		list ($ret, $commenter) =
		    GalleryCoreApi::fetchUserByUsername($form['commenterName']);
		if ($ret) {
		    $error[] = 'form[error][commenterName][invalid]';
		}
	    }

	    if (empty($form['subject'])) {
		$form['subject'] = '';
	    }

	    list($ret, $isAnonymousUser) = GalleryCoreApi::isAnonymousUser(
		isset($commenter) ? $commenter->getId() : $comment->getCommenterId());
	    if ($ret) {
		return array($ret, null);
	    }

	    if (empty($form['author']) || !$isAnonymousUser) {
		$form['author'] = '';
	    }

	    if (empty($form['comment'])) {
		$error[] = 'form[error][comment][missing]';
	    }

	    if (!in_array($form['publishStatus'],
			  array(COMMENT_PUBLISH_STATUS_PUBLISHED,
				COMMENT_PUBLISH_STATUS_UNPUBLISHED,
				COMMENT_PUBLISH_STATUS_SPAM))) {
		/* UI doesn't allow this so don't bother with a clean error */
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }

	    if (empty($error)) {
		list ($ret, $markup) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock(array($comment->getId()));
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $comment) = $comment->refresh();
		if ($ret) {
		    return array($ret, null);
		}

		if ($markup == 'html') {
		    /* Strip malicious content if html markup allowed */
		    $form['subject'] = GalleryUtilities::htmlSafe($form['subject'], true);
		    $form['comment'] = GalleryUtilities::htmlSafe($form['comment'], true);
		}

		$comment->setCommenterId($commenter->getId());
		$comment->setHost($form['host']);
		$comment->setSubject($form['subject']);
		$comment->setComment($form['comment']);
		$comment->setAuthor($form['author']);
		$comment->setPublishStatus($form['publishStatus']);
		$ret = $comment->save();
		if ($ret) {
		    return array($ret, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null);
		}

		$status['changed'] = 1;
		$redirect['view'] = 'comment.CommentChangeConfirmation';
		$redirect['itemId'] = (int)$itemId;
	    }
	} else if (isset($form['action']['preview'])) {
	    /* Verify the input parameters */
	    if (empty($form['commenterName'])) {
		$error[] = 'form[error][commenterName][missing]';
	    } else {
		list ($ret, $commenter) =
		    GalleryCoreApi::fetchUserByUsername($form['commenterName']);
		if ($ret) {
		    $error[] = 'form[error][commenterName][invalid]';
		}
	    }

	    if (empty($form['subject'])) {
		$form['subject'] = '';
	    }

	    if (empty($form['author'])) {
		$form['author'] = '';
	    }

	    if (empty($form['comment'])) {
		$error[] = 'form[error][comment][missing]';
	    }

	    /* Fall through back to the current view */
	} else if (isset($form['action']['cancel'])) {
	    /* Where to go on a cancel?  View the item.*/
	    $redirect['view'] = 'core.ShowItem';
	    $redirect['itemId'] = (int)$itemId;
	}

	if (!empty($redirect)) {
	    /* Redirect back to the confirmation page by default */
	    $results['redirect'] = $redirect;
	} else {
	    /* Stay on the same page */
	    $results['delegate']['view'] = 'comment.EditComment';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view shows a form to edit a comment
 */
class EditCommentView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	/* Load our item */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	/* Make sure we have permission to edit a comment */
	$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'comment.edit');
	if ($ret) {
	    return array($ret, null);
	}

	$commentId = (int) GalleryUtilities::getRequestVariables('commentId');

	/* Load the specific comment */
	list ($ret, $comment) = GalleryCoreApi::loadEntitiesById($commentId, 'GalleryComment');
	if ($ret) {
	    return array($ret, null);
	}

	/* Make sure the comment is associated with the item */
	if ($comment->getParentId() != $item->getId()) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	/* Associate the commenter's info */
	$commenterId = $comment->getCommenterId();
	list ($ret, $commenter) = GalleryCoreApi::loadEntitiesById($commenterId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	list($ret, $isAnonymousUser) = GalleryCoreApi::isAnonymousUser($comment->getCommenterId());
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'EditComment') {
	    $form['commenterName'] = $commenter->getUserName();
	    $form['host'] = $comment->getHost();
	    $form['comment'] = $comment->getComment();
	    $form['subject'] = $comment->getSubject();
	    $form['author'] = $comment->getAuthor();
	    $form['formName'] = 'EditComment';
	    $form['publishStatus'] = $comment->getPublishStatus();
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	if ($ret) {
	    return array($ret, null);
	}
	$EditComment['publishStatusList'] = array(
	    COMMENT_PUBLISH_STATUS_PUBLISHED => $module->translate('Published'),
	    COMMENT_PUBLISH_STATUS_UNPUBLISHED => $module->translate('Unpublished'),
	    COMMENT_PUBLISH_STATUS_SPAM => $module->translate('Spam'));

	$EditComment['comment'] = (array)$comment;
	$EditComment['itemId'] = $item->getId();
	$EditComment['isGuestComment'] = $isAnonymousUser;

	$template->setVariable('EditComment', $EditComment);
	$template->setVariable('controller', 'comment.EditComment');

	return array(null,
		     array('body' => 'modules/comment/templates/EditComment.tpl'));
    }
}
?>
