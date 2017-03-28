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

GalleryCoreApi::requireOnce('modules/comment/classes/AkismetApi.class');
GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');

/**
 * Settings for comments
 * @package Comment
 * @subpackage UserInterface
 * @author Johannes Schmittat <Johannes@Schmittat.com>
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17990 $
 */
class CommentSiteAdminController extends GalleryController {
    var $_akismet;

    function CommentSiteAdminController() {
	$this->_akismet = new AkismetApi();
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    if (!isset($form['show']) || !is_numeric($form['show'])) {
		$error[] = 'form[error][show]';
	    }

	    $validApiKey = false;

	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $currentApiKey) = $module->getParameter('akismet.apiKey');
	    if ($ret) {
		return array($ret, null);
	    }

	    if (!empty($form['apiKey']) && $form['apiKey'] != $currentApiKey) {
		/* First validate locally (don't rely on remote server for input validation) */
		if (!preg_match('/^[A-Za-z0-9]+$/', trim($form['apiKey']))) {
		    $error[] = 'form[error][apiKey][invalid]';
		}

		if (empty($error)) {
		    list ($ret, $validApiKey) = $this->_akismet->verifyApiKey($form['apiKey']);
		    if ($ret) {
			return array($ret, null);
		    }

		    if (!$validApiKey) {
			$error[] = 'form[error][apiKey][invalid]';
		    }
		}
	    }

	    if (empty($error)) {
		$ret = $module->setParameter('comments.show', $form['show']);
		if ($ret) {
		    return array($ret, null);
		}
		$ret = $module->setParameter(
		    'comments.latest', (isset($form['latest']) && $form['latest']) ? 1 : 0);
		if ($ret) {
		    return array($ret, null);
		}

		$ret = $module->setParameter('comments.moderate', !empty($form['moderate']));
		if ($ret) {
		    return array($ret, null);
		}

		$ret = $module->setParameter('akismet.apiKey', $form['apiKey']);
		if ($ret) {
		    return array($ret, null);
		}

		$ret = $module->setParameter('akismet.active', !empty($form['apiKey']) ? 1 : 0);
		if ($ret) {
		    return array($ret, null);
		}

		$status['saved'] = 1;
	    }
	} else if (isset($form['action']['checkAllWithAkismet'])) {
	    $templateAdapter =& $gallery->getTemplateAdapter();
	    $templateAdapter->registerTrailerCallback(
		array($this, 'checkAllWithAkismet'), array(new AkismetApi()));
	    $delegate['view'] = 'core.ProgressBar';
	}

	/* Figure out where to redirect upon success */
	if (empty($delegate)) {
	    $method = empty($error) ? 'redirect' : 'delegate';
	    $results[$method]['view'] = 'core.SiteAdmin';
	    $results[$method]['subView'] = 'comment.CommentSiteAdmin';
	} else {
	    $results['delegate'] = $delegate;
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    /**
     * Check all existing comments against Akismet.
     * @param AkismetApi $akismet AkismetApi instance
     * @return GalleryStatus a status code
     */
    function checkAllWithAkismet($akismet) {
	global $gallery;
	$templateAdapter =& $gallery->getTemplateAdapter();
	$session =& $gallery->getSession();
	$urlGenerator =& $gallery->getUrlGenerator();
	$storage =& $gallery->getStorage();

	list ($ret, $rootId) = GalleryCoreApi::getDefaultAlbumId();
	if ($ret) {
	    return $ret;
	}

	$status = array();
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	if ($ret) {
	    return $ret;
	}

	/*
	 * Disable the Akismet GalleryEntity::save listener so that if Akismet reports a message
	 * as spam and we change its publish status, we don't report it back to Akismet.
	 */
	GalleryCommentHelper::setAkismetSaveListenerSwitch(false);

	$checkingCommentMessage = $module->translate('Checking all comments with Akismet');
	$checkedCount = 0;
	$spamCount = 0;
	$total = null;
	while (true) {
	    $batchSize = 100;
	    /** @todo Should we include UNPUBLISHED comments here as well? */
	    list ($ret, $comments, $total) = GalleryCommentHelper::fetchAllComments(
		$rootId, $batchSize, $checkedCount, ORDER_ASCENDING,
		COMMENT_PUBLISH_STATUS_PUBLISHED);
	    if ($ret) {
		return $ret;
	    }

	    foreach ($comments as $comment) {
		$templateAdapter->updateProgressBar(
		    $checkingCommentMessage,
		    GalleryUtilities::utf8Substring(
			$comment->getSubject() . ' ' . $comment->getComment(), 0, 100),
		    ++$checkedCount / $total);

		list ($ret, $isSpam) = $akismet->checkComment($comment);
		if ($ret) {
		    return $ret;
		}
		if ($isSpam) {
		    /** @todo lock all of these at once */
		    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($comment->getId());
		    if ($ret) {
			return $ret;
		    }

		    list ($ret, $comment) = $comment->refresh();
		    if ($ret) {
			return $ret;
		    }

		    $comment->setPublishStatus(COMMENT_PUBLISH_STATUS_SPAM);
		    $ret = $comment->save();
		    if ($ret) {
			return $ret;
		    }

		    $ret = GalleryCoreApi::releaseLocks($lockId);
		    if ($ret) {
			return $ret;
		    }
		    $spamCount++;
		}
	    }

	    if ($checkedCount >= $total) {
		break;
	    }

	    $ret = $storage->checkPoint();
	    if ($ret) {
		return $ret;
	    }
	}

	$templateAdapter->updateProgressBar($module->translate('Done checking comments'), '', 1);

	GalleryCommentHelper::setAkismetSaveListenerSwitch(true);

	$status['checked'] = array('total' => $total, 'spamCount' => $spamCount);
	$statusId = $session->putStatus($status);
	$continueUrl = $urlGenerator->generateUrl(
	    array('view' => 'core.SiteAdmin', 'subView' => 'comment.CommentSiteAdmin',
		  'statusId' => $statusId),
	    array('forceFullUrl' => true));
	$templateAdapter->completeProgressBar($continueUrl, false);

	return null;
    }
}

/**
 * Settings for comments
 */
class CommentSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'comment');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'CommentSiteAdmin') {
	    $form['formName'] = 'CommentSiteAdmin';

	    $form['latest'] = isset($params['comments.latest']) ? $params['comments.latest'] : '';
	    $form['show'] = $params['comments.show'];
	    $form['moderate'] = !empty($params['comments.moderate']);
	    $form['apiKey'] = isset($params['akismet.apiKey']) ? $params['akismet.apiKey'] : '';
	}

	$form['latest'] = !empty($form['latest']);
	$form['moderate'] = !empty($form['moderate']);

	$CommentSiteAdmin = array();
	$CommentSiteAdmin['akismetActive'] = !empty($params['akismet.active']);
	$template->setVariable('CommentSiteAdmin', $CommentSiteAdmin);
	$template->setVariable('controller', 'comment.CommentSiteAdmin');
	return array(null,
		     array('body' => 'modules/comment/templates/CommentSiteAdmin.tpl'));
    }
}
?>
