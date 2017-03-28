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

/** Possible GalleryComment::publishStatus constants */
define('COMMENT_PUBLISH_STATUS_PUBLISHED', 0);
define('COMMENT_PUBLISH_STATUS_UNPUBLISHED', 1);
define('COMMENT_PUBLISH_STATUS_SPAM', 2);

/**
 * Comment Module
 *
 * This module provides support for adding comments to items
 *
 * @package Comment
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 18172 $
 */
class CommentModule extends GalleryModule {

    function CommentModule() {
	global $gallery;

	$this->setId('comment');
	$this->setName($gallery->i18n('Comments'));
	$this->setDescription($gallery->i18n('User commenting system'));
	$this->setVersion('1.1.14'); /* Update upgrade() function below too */
	$this->_templateVersion = 1;
	$this->setGroup('data', $gallery->i18n('Extra Data'));
	$this->setCallbacks('getItemLinks|getItemSummaries|' .
			    'getSiteAdminViews|getItemAdminViews');
	$this->setRequiredCoreApi(array(7, 41));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	$storage =& $gallery->getStorage();

	if (!isset($currentVersion)) {
	    $currentVersion = '0';
	} else if (version_compare($currentVersion, '0.9.6', '<=')
		|| substr($currentVersion, 0, 4) == '0.81') {
	    /* Instead of enumerating all previous versions... */
	    $currentVersion = '0.9.6';
	}

	switch ($currentVersion) {
	case '0':
	    /* Initial install.  Register our permissions */
	    $permissions[] = array('add', $gallery->i18n('[comment] Add comments'), 0, array());
	    $permissions[] = array('edit', $gallery->i18n('[comment] Edit comments'), 0, array());
	    $permissions[] = array('delete', $gallery->i18n('[comment] Delete comments'),
				   0, array());
	     $permissions[] = array('view', $gallery->i18n('[comment] View comments'), 0, array());
	     $permissions[] = array('all', $gallery->i18n('[comment] All access'),
		 GALLERY_PERMISSION_COMPOSITE,
		 array('comment.add', 'comment.edit', 'comment.delete', 'comment.view'));
	     foreach ($permissions as $p) {
		$ret = GalleryCoreApi::registerPermission($this->getId(),
							  'comment.' . $p[0],
							  $p[1], $p[2], $p[3]);
		if ($ret) {
		    return $ret;
		}
	     }

	     foreach (array('comments.show' => 10, 'comments.latest' => 1,
			    'comments.moderate' => 0, 'validation.level' => 'HIGH')
		     as $key => $value) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	    break;

	case '0.9.6':
	    /*
	     * Remove comment.search permission included in older module versions.
	     * As this is a composite permission we can simply remove its entry in the
	     * PermissionSet table and not touch any item permissions.
	     */
	    $ret = GalleryCoreApi::removeMapEntry(
		'GalleryPermissionSetMap',
		array('module' => 'comment', 'permission' => 'comment.search'));
	    if ($ret) {
		return $ret;
	    }

	case '0.9.7':
	case '1.0.0':
	    $ret = $storage->configureStore($this->getId(), array('GalleryComment:1.0'));
	    if ($ret) {
		return $ret;
	    }

	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	case '1.0.4':
	case '1.0.5':
	    foreach (array('comments.show' => 10, 'comments.captcha' => 0, 'comments.latest' => 1)
		     as $key => $value) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }

	case '1.0.6':
	case '1.0.7':
	    list ($ret, $level) = $this->getParameter('comments.captcha');
	    if ($ret) {
		return $ret;
	    }
	    $ret = $this->setParameter('validation.level', $level ? 'HIGH' : 'OFF');
	    if ($ret) {
		return $ret;
	    }
	    $ret = $this->removeParameter('comments.captcha');
	    if ($ret) {
		return $ret;
	    }

	case '1.0.8':
	case '1.0.9':
	case '1.1.0':
	case '1.1.1':
	    $ret = $storage->configureStore($this->getId(), array('GalleryComment:1.1'));
	    if ($ret) {
		return $ret;
	    }

	case '1.1.2':
	case '1.1.3':
	case '1.1.3.1':
	    /* .mo file migration */
	case '1.1.3.2':
	    /* Gallery 2.2.4 security release */
	case '1.1.4':
	case '1.1.5':
	    /* Add Akismet support */
	    $ret = $storage->configureStore($this->getId(), array('GalleryComment:1.2'));
	    if ($ret) {
		return $ret;
	    }
	case '1.1.6':
	    /* Add CommentAddNotification class */
	case '1.1.7':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.1.8':
	case '1.1.9':
	    /* Combined YUI libraries into a single utilities.js file */ 
	case '1.1.10':
	case '1.1.11':
	    /* Add Comment Moderation Queue Support */
	    $ret = $this->setParameter('comments.moderate', 0);
	    if ($ret) {
		return $ret;
	    }

	case '1.1.12':
	case '1.1.13':

	case 'end of upgrade path':
	    /*
	     * Leave this bogus case at the end of the legitimate case statements so that we
	     * always properly terminate our upgrade path with a break.
	     */
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					 sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'GalleryCommentHelper', 'GalleryCommentHelper', 
	    'modules/comment/classes/GalleryCommentHelper.class', 'comment', 
	    array('GalleryEntity::delete', 'GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEntity', 'GalleryComment', 'GalleryComment',
	    'modules/comment/classes/GalleryComment.class', 'comment', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GallerySearchInterface_1_0', 'GalleryCommentSearch', 'comment',
	    'modules/comment/classes/GalleryCommentSearch.class', 'comment', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'CaptchaAdminOption', 'CommentCaptchaAdminOption', 'CommentCaptchaAdminOption',
	    'modules/comment/classes/CommentCaptchaAdminOption.class', 'comment', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'NotificationEvent_1_0', 'CommentAddNotification', 'CommentAddNotification',
	    'modules/comment/classes/CommentAddNotification.class', 'comment', 
	    array('GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getModuleEntityTypes
     */
    function getModuleEntityTypes() {
	return array('GalleryComment');
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	list ($ret, $showAll) = $this->getParameter('comments.latest');
	if ($ret) {
	    return array($ret, null);
	}
	$links = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if (isset($wantsDetailedLinks[$itemId])) {
		if (isset($permissions[$itemId]['comment.add'])) {
		    $links[$itemId][] = array(
			'text' => $this->translate('Add Comment'),
			'params' => array('view' => 'comment.AddComment',
					  'itemId' => $itemId, 'return' => 1));
		}

		if ($showAll && isset($permissions[$itemId]['comment.view'])) {
		    $links[$itemId][] = array(
			'text' => $this->translate('View Latest Comments'),
			'params' => array('view' => 'comment.ShowAllComments',
					  'itemId' => $itemId, 'return' => 1));
		}
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getItemSummaries
     */
    function getItemSummaries($items, $permissions, &$template) {
	$ids = array();
	foreach ($items as $item) {
	    $ids[] = $item->getId();
	}

	GalleryCoreApi::requireOnce('modules/comment/classes/GalleryCommentHelper.class');
	list ($ret, $commentCounts) = GalleryCommentHelper::fetchCommentCounts($ids);
	if ($ret) {
	    return array($ret, null);
	}

	$summaries = array();
	foreach ($items as $item) {
	    $message = array();

	    if (isset($permissions[$item->getId()]['comment.view'])) {
		if (!empty($commentCounts[$item->getId()])) {
		    $summaries[$item->getId()] =
			$this->translate(array('text' => 'Comments: %d',
					       'arg1' => $commentCounts[$item->getId()]));
		}
	    }
	}

	return array(null, $summaries);
    }

    /**
     * @see GalleryModule::getItemAdminViews
     */
    function getItemAdminViews($item) {
	$views = array();
	list ($ret, $permissions) = GalleryCoreApi::getPermissions($item->getId());
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($permissions['comment.edit']) ||
	    isset($permissions['comment.delete']) ||
	    isset($permissions['comment.view'])) {
		$views[] = array('name' => $this->translate('View Comments'),
				 'view' => 'comment.ShowComments');
	}

	return array(null, $views);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Comments'),
				 'view' => 'comment.CommentSiteAdmin'),
			   array('name' => $this->translate('Moderate Comments'),
				 'view' => 'comment.AdminModerateSpam')));
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	$rules = array();

	$rule = array();
	$rule['match'] = array('view' => 'comment.AddComment');
	$rule['pattern'] = 'c/add/%itemId%.html';
	$rule['comment'] = $this->translate('Add Comment');
	$rules[] = $rule;

	$rule = array();
	$rule['match'] = array('view' => 'comment.ShowAllComments');
	$rule['pattern'] = 'c/view/%itemId%.html';
	$rule['comment'] = $this->translate('View Comments');
	$rules[] = $rule;

	return $rules;
    }
}
?>
