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
 * This view will show all comments stored in the Gallery
 * @package Comment
 * @subpackage UserInterface
 * @author Johannes Schmittat <Johannes@Schmittat.com>
 * @version $Revision: 17657 $
 */
class ShowAllCommentsView extends GalleryView {

    /**
     * @see GalleryView::autoCacheControl
     */
    function autoCacheControl() {
        return false;
    }
    
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'comment.view');
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
	    $item->getId(), $showCount, ($page - 1) * $showCount, ORDER_DESCENDING);
	if ($ret) {
	    return array($ret, null);
	}

	$commenters = $itemIds = $can = $itemData = array();
	for ($i = 0; $i < count($comments); $i++) {
	    /* Get the commenter ids */
	    $commenters[$comments[$i]->getCommenterId()] = 1;
	    $itemIds[$comments[$i]->getParentId()] = 1;

	    /* Extract the member data */
	    $comments[$i] = (array)$comments[$i];
	    $comments[$i]['randomId'] = $comments[$i]['id'] . '-' . rand() ;
	}

	if (!empty($commenters)) {
	    list ($ret, $commentersList) =
		GalleryCoreApi::loadEntitiesById(array_keys($commenters), 'GalleryUser');
	    if ($ret) {
		return array($ret, null);
	    }

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

	$cacheControl = false;
	/* Pages shouldn't be cached if any of the comments is not accessible for anonymous */ 
	list ($ret, $anonymousUserId) = GalleryCoreApi::getAnonymousUserId();
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($comments as $comment) {
	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($comment['parentId']);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach (array('delete' => 'comment.delete',
			   'edit' => 'comment.edit',
			   'markSpam' => 'comment.edit') as $canFlag => $permission) {
		$can[$comment['id']][$canFlag] = isset($permissions[$permission]);
	    }
	    /* If we already set cache control, don't check any more */
	    if (!$cacheControl) {
		/* If !comment.view || !core.view */
		list ($ret, $anonymousViewable) = GalleryCoreApi::hasItemPermission(
		    $comment['parentId'], 'comment.view', $anonymousUserId, false);
		if ($ret) {
		    return array($ret, null);
		}
		if ($anonymousViewable) {
		    list ($ret, $anonymousViewable) = GalleryCoreApi::hasItemPermission(
			$comment['parentId'], 'core.view', $anonymousUserId, false);
		    if ($ret) {
			return array($ret, null);
		    }
		}
		
		if (!$anonymousViewable) {
		    $this->setCacheControl($template, 'no-cache');
		    $cacheControl = true;
		}
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
	if ($page > 1) {
	    $navigator['first'] = array('urlParams' => array('view' => 'comment.ShowAllComments',
				  'itemId' => $item->getId(), 'page' => 1));
	    $navigator['back'] = array('urlParams' => array('view' => 'comment.ShowAllComments',
				 'itemId' => $item->getId(), 'page' => $page - 1));
	}
	if ($page < $navigator['pageCount']) {
	    $navigator['next'] = array('urlParams' => array('view' => 'comment.ShowAllComments',
				 'itemId' => $item->getId(), 'page' => $page + 1));
	    $navigator['last'] = array('urlParams' => array('view' => 'comment.ShowAllComments',
				 'itemId' => $item->getId(), 'page' => $navigator['pageCount']));
	}

	$template->setVariable('ShowAllComments', array(
	    'comments' => $comments,
	    'commenters' => $commenters,
	    'can' => $can,
	    'item' => (array)$item,
	    'itemData' => $itemData,
	    'navigator' => $navigator));
	$template->style('modules/comment/comment.css');
	$template->javascript('lib/yui/utilities.js');
	$template->javascript('lib/yui/container-min.js');
	return array(null,
		     array('body' => 'modules/comment/templates/ShowAllComments.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'comment');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('show comments'));
    }
}
?>
