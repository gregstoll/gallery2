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

GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');

/**
 * Controller for moderating spam
 *
 * @package Comment
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17784 $
 */
class AdminModerateSpamController extends GalleryController {
    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $rootId) = GalleryCoreApi::getDefaultAlbumId();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['deleteAllSpam'])) {
	    $batchSize = 100;
	    while (true) {
		/** @todo use a progress bar here */
		list ($ret, $comments, $count) = GalleryCommentHelper::fetchAllComments(
		    $rootId, $batchSize, null, ORDER_ASCENDING, COMMENT_PUBLISH_STATUS_SPAM);
		if ($ret) {
		    return array($ret, null);
		}

		foreach ($comments as $comment) {
		    $ret = GalleryCoreApi::deleteEntityById($comment->getId(), 'GalleryComment');
		    if ($ret) {
			return array($ret, null);
		    }
		}

		if ($count <= $batchSize) {
		    break;
		}
	    }

	    $status['allSpamDeleted'] = 1;
	}

	/* Figure out where to redirect upon success */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'comment.AdminModerateSpam';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show all comments stored in the Gallery
 * @package Comment
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17784 $
 */
class AdminModerateSpamView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $rootId) = GalleryCoreApi::getDefaultAlbumId();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $showCount) =
	    GalleryCoreApi::getPluginParameter('module', 'comment', 'comments.show');
	if ($ret) {
	    return array($ret, null);
	}

	$page = GalleryUtilities::getRequestVariables('page');
	if (empty($page)) {
	    $page = 1;
	}

	list ($ret, $comments, $count) = GalleryCommentHelper::fetchAllComments(
	    $rootId, $showCount, ($page - 1) * $showCount, ORDER_DESCENDING,
	    array(COMMENT_PUBLISH_STATUS_SPAM, COMMENT_PUBLISH_STATUS_UNPUBLISHED));
	if ($ret) {
	    return array($ret, null);
	}

	$commenters = $itemIds = $can = $itemData = array();
	foreach ($comments as $i => $comment) {
	    /* Get the commenter ids */
	    $commenters[$comments[$i]->getCommenterId()] = 1;
	    $itemIds[$comments[$i]->getParentId()] = 1;

	    /* Extract the member data */
	    $comments[$i] = (array)$comments[$i];
	    $comments[$i]['randomId'] = $comments[$i]['id'] . '-' . rand();
	}

	if (!empty($commenters)) {
	    list ($ret, $commentersList) =
		GalleryCoreApi::loadEntitiesById(array_keys($commenters), 'GalleryUser');
	    if ($ret) {
		return array($ret, null);
	    }

	    $commenters = array();
	    foreach ($commentersList as $commenter) {
		$commenters[$commenter->getId()] = (array)$commenter;
	    }
	}

	if (!empty($itemIds)) {
	    $itemIds = array_keys($itemIds);
	    $ret = GalleryCoreApi::studyPermissions($itemIds);
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $itemList) = GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($itemList as $it) {
		$items[$it->getId()] = (array)$it;
	    }

	    /* get peer thumbnails and resizes */
	    list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds($itemIds);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	foreach ($comments as $comment) {
	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($comment['parentId']);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach (array('delete' => 'comment.delete',
			   'edit' => 'comment.edit',
			   'markNotSpam' => 'comment.edit') as $canFlag => $permission) {
		$can[$comment['id']][$canFlag] = isset($permissions[$permission]);
	    }

	    if (!isset($itemData[$comment['parentId']])) {
		$it = $items[$comment['parentId']];
		if (!empty($derivatives[$comment['parentId']])) {
		    foreach ($derivatives[$comment['parentId']] as $derivative) {
			$type = $derivative->getDerivativeType();
			if (empty($it['resize']) && $type == DERIVATIVE_TYPE_IMAGE_RESIZE) {
			    $it['resize'] = (array)$derivative;
			} else if ($type == DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
			    $it['thumb'] = (array)$derivative;
			}
		    }
		}
		$itemData[$comment['parentId']] = $it;
	    }
	}

	$navigator = array('page' => $page, 'pageCount' => ceil($count/$showCount));
	$urlParams = array('view' => 'core.SiteAdmin', 'subView' => 'comment.AdminModerateSpam');
	if ($page > 1) {
	    $navigator['first'] = array('urlParams' =>
		array_merge($urlParams, array('itemId' => $rootId, 'page' => 1)));
	    $navigator['back'] = array('urlParams' =>
		array_merge($urlParams, array('itemId' => $rootId, 'page' => $page - 1)));
	}
	if ($page < $navigator['pageCount']) {
	    $navigator['next'] = array('urlParams' =>
		array_merge($urlParams, array('itemId' => $rootId, 'page' => $page + 1)));
	    $navigator['last'] = array('urlParams' => array_merge($urlParams,
		array('itemId' => $rootId, 'page' => $navigator['pageCount'])));
	}

	$template->setVariable('controller', 'comment.AdminModerateSpam');
	$template->setVariable('AdminModerateSpam', array(
	    'comments' => $comments,
	    'commenters' => $commenters,
	    'can' => $can,
	    'itemData' => $itemData,
	    'navigator' => $navigator,
	    'total' => $count));
	$template->style('modules/comment/comment.css');
	$template->javascript('lib/yui/utilities.js');
	$template->javascript('lib/yui/container-min.js');
	return array(null, array('body' => 'modules/comment/templates/AdminModerateSpam.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('moderate spam'));
    }
}
?>
