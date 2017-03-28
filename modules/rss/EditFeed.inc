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
 * Edit RSS feed settings
 * @package Rss
 * @subpackage UserInterface
 * @author Jonatan Heyman <http://heyman.info>,
 * @author Pierre-Luc Paour
 * @author Daniel Grund <http://www.photogrund.nl>
 * @version $Revision: 17580 $
 */
class EditFeedController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$status = $results = $error = array();

	$mode = empty($form['mode']) ? '' : $form['mode'];
	$itemId = !empty($form['itemId']) ? (int)$form['itemId']
					  : (int)GalleryUtilities::getRequestVariables('itemId');

	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $canView) = GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
	if ($ret) {
	    return array($ret, null);
	}
	if (!$canView) {
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}

	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'rss');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret) =
	    RssMapHelper::canConfigureFeed($item, $param, $gallery->getActiveUserId(), true);
	if ($ret) {
	    return array($ret, null);
	}

	/* Check which button was pressed */
	if (isset($form['action']['create']) || isset($form['action']['update'])) {
	    /*
	     * Either new feed settings where created or existing where updated.
	     * First do some error checking,
	     * if check fails display error on page
	     * first numerical values.
	     */
	    foreach (array('itemId',
			   'count',
			   'ttl') as $value) {
		if (!isset($form[$value]) || ((int)$form[$value]) < 0) {
		    $error[] = 'form[error][' . $value . ']';
		}
	    }

	    /* check strings may be '' or contain a value */
	    foreach (array('version',
			   'category',
			   'description',
			   'cloudDomain',
			   'cloudPath',
			   'cloudRegisterProcedure',
			   'cloudProtocol',
			   'cloudPort',
			   'language',
			   'feedName') as $value) {
		if (!isset($form[$value])) {
		    $error[] = 'form[error][' . $value . ']';
		}
	    }

	    /* check the name */
	    if (empty($form['feedName'])) {
		$error[] = 'form[error][feedName]';
	    }

	    /* conditionally check the recursive limit */
	    if ($form['feedType'] == 'photosRecursive' &&
		    (!isset($form['photosRecursiveLimit']) ||
			((int)$form['photosRecursiveLimit']) < 0)) {
		$error[] = 'form[error][photosRecursiveLimit]';
	    }

	    /* If there are no form errors, create array to be saved to DB */
	    if (empty($error)) {
		$parameters = array('itemId' => $form['itemId'],
		    'feedType' => $form['feedType'],
		    'feedDate' => $form['feedDate'],
		    'version' => $form['version'],
		    'count' => $form['count'],
		    'ttl' => $form['ttl'],
		    'category' => $form['category'],
		    'language' => $form['language'],
		    'copyright' => $form['copyright'],
		    'description' => $form['description'],
		    'useImage' => (isset($form['useImage']) ? '1' : '0'),
		    'useEnclosure' => (isset($form['useEnclosure']) ? '1' : '0'),
		    'useCloud' => (isset($form['useCloud']) ? '1' : '0'),
		    'useMedia' => (isset($form['useMedia']) ? '1' : '0'));

		if ($parameters['feedType'] == 'photosRecursive') {
		    $parameters['photosRecursiveLimit'] = $form['photosRecursiveLimit'];
		}

		if (isset($form['useCloud'])) {
		    $parameters = array_merge($parameters,
			array('cloudDomain' => $form['cloudDomain'],
			      'cloudPort' => $form['cloudPort'],
			      'cloudPath' => $form['cloudPath'],
			      'cloudRegisterProcedure' => $form['cloudRegisterProcedure'],
			      'cloudProtocol' => $form['cloudProtocol']));
		}

		/* Check if we need to update a current feed, or create a new one */
		if (isset($form['action']['create'])) {
		    $ret = RssMapHelper::createFeed($form['feedName'], $parameters);
		} else {
		    $ret = RssMapHelper::updateFeed($form['feedName'], $parameters);
		}

		/* Check to see if DB update was successful */
		if ($ret) {
		    /* If we got anything other than a ERROR_COLLISION, exit */
		    if (!($ret->getErrorCode() & ERROR_COLLISION)) {
			return array($ret, null);
		    }

		    /*
		     * If it is state it is a 'feedNameCollision'
		     * Set our error status and fall back to the view.
		     */
		    $error[] = 'form[error][feedNameCollision]';
		} else {
		    /* Save was successful change status to refect this */
		    $status['saved'] = 1;
		    $mode = 'edit';
		}
	    } else {
		$mode = isset($form['action']['create'])?'new':'edit';
	    }
	} else if (isset($form['action']['cancel'])) {
	    /* Cancel button was pressed, do nothing, return to feedlist */
	    $mode = 'list';
	} else if (isset($form['action']['delete'])) {
	    /*
	     * Delete button was pressed, delete feed, return to feedlist.
	     * We might want to provide an 'are you sure' here in the future.
	     */
	    $ret = RssMapHelper::deleteFeed($form['action']['delete']);
	    if ($ret) {
		return array($ret, null);
	    }

	    $status['deleted'] = 1;
	    $mode = 'list';
	}

	/* figure out the method we will use */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results['status'] = $status;
	$results['error'] = $error;
	$results[$method]['view'] = 'core.ItemAdmin';
	$results[$method]['subView'] = 'rss.EditFeed';
	$results[$method]['itemId'] = $itemId;
	$results[$method]['mode'] = $mode;

	if (empty($error)) {
	    $results[$method]['form'] = $form;
	}

	return array(null, $results);
    }
}


/**
 * This view will show a form to generate an URL to an RSS feed.
 */
class EditFeedView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	/* Load the item */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	/* Fetch module parameters and save them in an array passed to the template engine */
	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'rss');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret) =
	    RssMapHelper::canConfigureFeed($item, $param, $gallery->getActiveUserId(), true);
	if ($ret) {
	    return array($ret, null);
	}

	$EditFeed = array();
	$EditFeed['itemId'] = $item->getId();

	/* Fetch all feed definitions */
	list ($ret, $feeds) = RssMapHelper::fetchFeedsForItem($EditFeed['itemId']);
	if ($ret) {
	    return array($ret, null);
	}

	$EditFeed['mode'] = GalleryUtilities::getRequestVariables('mode');
	if (empty($EditFeed['mode'])) {
	    /* Entering this module, pick the first page */
	    if (empty($feeds)) {
		/* No feeds defined yet: show new */
		$EditFeed['mode'] = 'new';
	    } else {
		/* Otherwise show the list */
		$EditFeed['mode'] = 'list';
	    }
	}

	/* Switching on which page to display */
	if ($EditFeed['mode'] == 'new') {
	    /* Create new feed */
	    if (empty($form['error'])) {
		/* no error: load default parameters */
		$form['formName'] = 'EditFeed';

		if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		    $EditFeed['type'] = 'album';
		    $EditFeed['feedType'] = 'photos';
		} else if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
		    $EditFeed['type'] = 'photo';
		    $EditFeed['feedType'] = 'commentsPhoto';
		}

		$feedDate = 'updated';

		/*
		 * Set some default values
		 * Use the default settings from the DB.
		 * All these settings can be overwritten by user.
		 */
		foreach (array('version' => $param['defaultVersion'],
			       'count' => $param['defaultCount'],
			       'useImage' => true,
			       'useEnclosure' => false,
			       'useCloud' => false,
			       'useMedia' => true,
			       'cloudDomain' => '',
			       'cloudPort' => '',
			       'cloudPath' => '',
			       'cloudRegisterProcedure' => '',
			       'cloudProtocol' => '',
			       'photosRecursiveLimit' => '0',
			       'ttl' => $param['defaultTtl'],
			       'category' => '',
			       'description' => '',
			       'language' => 'en-us',
			       'copyright' => $param['defaultCopyright'],
			       'feedDate' => 'updated') as $key => $value) {
		    $EditFeed[$key] = $value;
		}

	    } else {
		/* There was an error: just leave the form the same it was */
		$EditFeed = array_merge($EditFeed, $form);
	    }
	} else if ($EditFeed['mode'] == 'edit') {
	    /*
	     * Editing a feed.
	     * Load the feed parameters
	     */
	    if (empty($form['error'])) {
		/* no error: load parameters from DB using the 'feedName' */
		list ($ret, $feed) = RssMapHelper::fetchFeed($form['feedName']);
		if ($ret) {
		    return array($ret, null);
		}

		$EditFeed = array_merge($EditFeed, $feed);

		if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		    $EditFeed['type'] = 'album';
		} else if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
		    $EditFeed['type'] = 'photo';
		}

		$urlgenerator = $gallery->GetUrlGenerator();
		$EditFeed['feedUrl'] = $urlgenerator->generateUrl(
		    array('view' => 'rss.Render', 'name' => $form['feedName']),
			array('forceSessionId' => false, 'forceFullUrl' => true));
	    } else {
		/* There was an error: just leave the form the same it was */
		$EditFeed = array_merge($EditFeed, $form);
	    }
	} else {
	    /*
	     * No mode stated.
	     * Retieving all the feeds for this 'itemId'
	     * Fetch all feed definitions
	     */
	    list ($ret, $feeds) = RssMapHelper::fetchFeedsForItem($EditFeed['itemId']);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $EditFeed['types']) = RssHelper::getFeedTypeTranslation();
	    if ($ret) {
		return array($ret, null);
	    }

	    $EditFeed['feeds'] = $feeds;
	}

	$EditFeed['rssVersionList'] = array('0.91', '2.0');
	$template->setVariable('EditFeed', $EditFeed);
	$template->setVariable('param', $param);
	$template->setVariable('controller', 'rss.EditFeed');
	return array(null, array('body' => 'modules/rss/templates/EditFeed.tpl'));
    }
}
?>
