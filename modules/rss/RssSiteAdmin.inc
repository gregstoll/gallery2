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

GalleryCoreApi::requireOnce('modules/rss/classes/RssMapHelper.class');
GalleryCoreApi::requireOnce('modules/rss/classes/RssHelper.class');

/**
 * Settings for RSS
 * @package Rss
 * @subpackage UserInterface
 * @author Jonatan Heyman <http://heyman.info>
 * @author Pierre-Luc Paour
 * @author Daniel Grund <http://www.photogrund.nl>
 * @version $Revision: 17580 $
 */
class RssSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	/* Make sure the user has permission to do this operation */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $results = $error = array();
	$mode = empty($form['mode']) ? '' : $form['mode'];

	if (isset($form['action']['save'])) {
	    if ($mode == 'settings') {
		/* error checking */
	       if (!isset($form['defaultVersion']) ||
			!($form['defaultVersion'] == '0.91' ||
			$form['defaultVersion'] == '2.0')) {
		    $error[] = 'form[error][defaultVersion]';
	       } else if (!isset($form['defaultCount']) || ((int)$form['defaultCount']) <= 0) {
		    $error[] = 'form[error][defaultCount]';
	       } else if (!isset($form['maxCount']) || ((int)$form['maxCount']) <= 0) {
		    $error[] = 'form[error][maxCount]';
	       } else if (!isset($form['maxAge']) || ((int)$form['maxAge']) <= 0) {
		    $error[] = 'form[error][maxAge]';
	       } else if (!isset($form['defaultTtl']) || ((int)$form['defaultTtl']) < 0) {
		    $error[] = 'form[error][defaultTtl]';
	       } else if (!isset($form['defaultCopyright'])) {
		    $error[] = 'form[error][defaultCopyright]';
	       } else {
		    /* save parameters */
		    foreach (array('defaultVersion',
				   'defaultCount',
				   'maxCount',
				   'maxAge',
				   'defaultCopyright',
				   'defaultTtl') as $setting) {
			$ret = GalleryCoreApi::setPluginParameter('module', 'rss',
								  $setting, $form[$setting]);
			if ($ret) {
			    return array($ret, null);
			}
		    }

		    foreach (array('allowSimpleFeed',
				   'allowConfigurableFeed',
				   'allowAlbums',
				   'allowPhotos',
				   'allowPhotosRecursive',
				   'allowCommentsPhoto',
				   'allowCommentsAlbum',
				   'allowCommentsRecursive',
				   'allowPhotosRandom',
				   'allowPhotosRandomRecursive') as $setting) {
			$ret = GalleryCoreApi::setPluginParameter('module', 'rss', $setting,
			    (isset($form[$setting]) && $form[$setting]) ? 1 : 0);
			if ($ret) {
			    return array($ret, null);
			}
		    }

		    $status['saved'] = 1;
		}
	    } else if ($mode == 'simplefeedsettings') {
		/* error checking */
		if (!isset($form['sfPhotosRecursiveLimit'])
			|| ((int)$form['sfPhotosRecursiveLimit']) < 0) {
		    $error[] = 'form[error][sfPhotosRecursiveLimit]';
		} else {
		    /* save parameters */
		    foreach (array('sfAlbumType',
				   'sfPhotosRecursiveLimit',
				   'sfDate') as $setting) {
			$ret = GalleryCoreApi::setPluginParameter('module', 'rss',
								  $setting, $form[$setting]);
			if ($ret) {
			    return array($ret, null);
			}
		    }

		    $status['saved'] = 1;
		}
	    }
	} else if (isset($form['action']['reset'])) {
	    /* do nothing */
	} else if (isset($form['action']['delete'])) {
	    $ret = RssMapHelper::deleteFeed($form['action']['delete']);
	    if ($ret) {
		return array($ret, null);
	    }

	    $mode = 'list';
	} else {
	    $error[] = 'form[error][unknownAction]';
	}

	/* figure out the method we will use */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results['status'] = $status;
	$results['error'] = $error;
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'rss.RssSiteAdmin';
	$results[$method]['mode'] = $mode;

	return array(null, $results);
    }
}

/**
 * Settings for RSS
 */
class RssSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$form['formName'] = 'RssSiteAdmin';
	$RssSiteAdmin = array();
	$RssSiteAdmin['rssVersionList'] = array('0.91', '2.0');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$RssSiteAdmin['mode'] = GalleryUtilities::getRequestVariables('mode');
	if (empty($RssSiteAdmin['mode'])) {
	    $RssSiteAdmin['mode'] = 'settings';
	}

	/* fetch module parameters and save them in form variable */
	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'rss');
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($param as $key => $value) {
	    $form[$key] = $value;
	}

	if ($RssSiteAdmin['mode'] == 'list') {
	    /* fetch all feed definitions */
	    list ($ret, $feeds) = RssMapHelper::fetchFeedsForItem();
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $RssSiteAdmin['types']) = RssHelper::getFeedTypeTranslation();
	    if ($ret) {
		return array($ret, null);
	    }

	    if (!empty($feeds)) {
		/* replace item Ids with actual items */
		$itemIds = array();
		foreach ($feeds as $feed) {
		    $itemIds[] = $feed['itemId'];
		}

		list ($ret, $items) = GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryItem');
		if ($ret) {
		    return array($ret, null);
		}

		for ($i = 0; $i < sizeof($feeds); $i++) {
		    $feeds[$i]['item'] = $items[$i];
		}
	    }

	    $RssSiteAdmin['feeds'] = $feeds;
	}

	$template->setVariable('RssSiteAdmin', $RssSiteAdmin);
	$template->setVariable('controller', 'rss.RssSiteAdmin');
	return array(null, array('body' => 'modules/rss/templates/RssSiteAdmin.tpl'));
    }
}
?>
