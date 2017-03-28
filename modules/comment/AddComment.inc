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
 * Add a comment to an item
 * @package Comment
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AddCommentController extends GalleryController {
    /**
     * ValidationPlugin instances to use when handling this request.  Only used by test code.
     *
     * @var array $_plugins (array of GalleryValidationPlugin)
     * @access private
     */
    var $_pluginInstances;

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

	/* Make sure we have permission to add a comment */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'comment.add');
	if ($ret) {
	    return array($ret, null);
	}

	list($ret, $isAnonymousUser) = GalleryCoreApi::isAnonymousUser($gallery->getActiveUserId());
	if ($ret) {
	    return array($ret, null);
	}

	$redirect = $status = $error = array();
	if (isset($form['action']['add'])) {
	    GalleryCoreApi::requireOnce(
		'modules/comment/classes/GalleryCommentHelper.class');

	    if (empty($form['subject'])) {
		$form['subject'] = '';
	    }

	    if (empty($form['author']) || !$isAnonymousUser) {
		$form['author'] = '';
	    }

	    if (empty($form['comment'])) {
		$error[] = 'form[error][comment][missing]';
	    }

	    /* Check for flooding - don't allow more than 1 comment every 30 seconds */
	    list ($ret, $commentTime) = GalleryCommentHelper::getLatestCommentTime(
		GalleryUtilities::getRemoteHostAddress());
	    if ($ret) {
		return array($ret, null);
	    }
	    $phpVm = $gallery->getPhpVm();
	    $currentTime = $phpVm->time();
	    if ($currentTime - $commentTime < 30) {
		$error[] = 'form[error][comment][flood]';
	    }
		
	    list ($ret, $useValidationPlugins) = GalleryCommentHelper::useValidationPlugins();
	    if ($ret) {
		return array($ret, null);
	    }
	    if (empty($error) && $useValidationPlugins) {
		if (isset($this->_pluginInstances)) {
		    $pluginInstances = $this->_pluginInstances;
		} else {
		    list ($ret, $pluginInstances) =
			GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
		    if ($ret) {
			return array($ret, null);
		    }

		    foreach (array_keys($pluginInstances) as $pluginId) {
			list ($ret, $pluginInstances[$pluginId]) =
			    GalleryCoreApi::newFactoryInstanceById('GalleryValidationPlugin',
								   $pluginId);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}

		/* Let each plugin do its verification */
		foreach ($pluginInstances as $plugin) {
		    list ($ret, $pluginErrors, $continue) = $plugin->performValidation($form);
		    if ($ret) {
			return array($ret, null);
		    }

		    $error = array_merge($error, $pluginErrors);
		    if (!$continue) {
			break;
		    }
		}
	    }

	    if (empty($error)) {
		/* Add the comment */
		list ($ret, $comment) =
		    GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryComment');
		if ($ret) {
		    return array($ret, null);
		}

		if (!isset($comment)) {
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT),
				 null);
		}

		$ret = $comment->create($itemId);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $markup) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
		if ($ret) {
		    return array($ret, null);
		}
		if ($markup == 'html') {
		    /* Strip malicious content if html markup allowed */
		    $form['author'] = GalleryUtilities::htmlSafe($form['author'], true);
		    $form['subject'] = GalleryUtilities::htmlSafe($form['subject'], true);
		    $form['comment'] = GalleryUtilities::htmlSafe($form['comment'], true);
		}

		$comment->setCommenterId($gallery->getActiveUserId());
		$comment->setHost(GalleryUtilities::getRemoteHostAddress());
		$comment->setSubject($form['subject']);
		$comment->setComment($form['comment']);
		$comment->setAuthor($form['author']);
		$comment->setDate($currentTime);

		$ret = $comment->save();
		if ($ret) {
		    return array($ret, null);
		}

		/* Send the user to a confirmation page, for now */
		$redirect['view'] = 'comment.CommentChangeConfirmation';
		$redirect['itemId'] = (int)$itemId;
		$status['added'] = 1;
	    }
	} else if (isset($form['action']['preview'])) {
	    if (empty($form['subject'])) {
		$form['subject'] = '';
	    }

	    if (empty($form['author'])|| !$isAnonymousUser) {
		$form['author'] = '';
	    }

	    if (empty($form['comment'])) {
		$error[] = 'form[error][comment][missing]';
	    }

	    /* Fall through back to the current view */
	} else if (isset($form['action']['cancel'])) {
	    /* Where to go on a cancel?  Back to viewing the item. */
	    $redirect['view'] = 'core.ShowItem';
	    $redirect['itemId'] = (int)$itemId;
	}

	/* Prepare our results */
	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'comment.AddComment';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show a form to add a new comment to an item
 */
class AddCommentView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');

	/* Load our item */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $AddComment) = GalleryCommentHelper::getAddComment($item, $form);
	if ($ret) {
	    return array($ret, null);
	}

	$template->setVariable('AddComment', $AddComment);
	$template->setVariable('controller', 'comment.AddComment');

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Add Comment'));

	return array(null,
		     array('body' => 'modules/comment/templates/AddComment.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('add comment'));
    }
}
?>
