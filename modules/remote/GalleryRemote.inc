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

GalleryCoreApi::requireOnce('modules/remote/classes/GalleryRemoteProperties.class');
GalleryCoreApi::requireOnce('modules/remote/classes/GalleryRemoteConstants.class');

/**
 * This controller fields requests from Gallery Remote and performs any required changes
 * to the data model.
 * @package Remote
 * @author Pierre-Luc Paour <paour@users.sourceforge.net>
 * @version $Revision: 20954 $
 */
class GalleryRemoteController extends GalleryController {
    /**
     * ItemAddOption instances to use when handling this request.  Only used by test code.
     * @var array (optionId => ItemAddOption) $_optionInstances
     * @access private
     */
    var $_optionInstances;

    /**
     * Tests can use this method to hardwire a specific set of option instances to use.
     * This avoids situations where some of the option instances will do unpredictable
     * things and derail the tests.
     *
     * @param array $optionInstances (optionId => ItemAddOption, ...)
     */
    function setOptionInstances($optionInstances) {
	$this->_optionInstances = $optionInstances;
    }

    /**
     * @see GalleryController::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * @see GalleryController::omitAuthTokenCheck
     */
    function omitAuthTokenCheck() {
	return true;
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$session =& $gallery->getSession();

	$start = array_sum(explode(' ', microtime()));

	$status = $error = array();
	$response = new GalleryRemoteProperties();
	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	if (!empty($form['cmd'])) {
	    /* Request forgery check: Only allow whitelisted, view-like operations */
	    $gallery->debug('GR Command: ' . $form['cmd']);
	    switch ($form['cmd']) {
	    case 'login':
	    case 'fetch-albums':
	    case 'fetch-albums-prune':
	    case 'fetch-album-images':
	    case 'album-properties':
	    case 'image-properties':
	    case 'no-op':
		$gallery->debug('No CSRF check');
		break;
	    default:
		/* Backwards-compatible request-forgery protection based on the user-agent string */
		$httpUserAgent = GalleryUtilities::getServerVar('HTTP_USER_AGENT');
		if (strstr($httpUserAgent, 'Gallery Remote') === false
			&& strstr($httpUserAgent, 'RPT-HTTPClient') === false
			&& strstr($httpUserAgent, 'iPhotoToGallery') === false) {
		    /* Newer GalleryRemote versions handle auth tokens */
		    $ret = GalleryController::assertIsGenuineRequest();
		    if ($ret) {
			$gallery->debug('Unrecognized User Agent: '  . $httpUserAgent);
			return array(GalleryCoreApi::error(ERROR_REQUEST_FORGED), null);
		    }
		}
	    }

	    switch ($form['cmd']) {
	    case 'login':
		$ret = $this->login($form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'fetch-albums':
		$ret = $this->fetchAlbums(false, $form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'fetch-albums-prune':
		$ret = $this->fetchAlbums(true, $form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'add-item':
		$ret = $this->addDataItem($form, $response);
		if ($ret) {
		    $response->setProperty('status', $grStatusCodes['UPLOAD_PHOTO_FAIL']);
		    $response->setProperty('status_text',
			sprintf("Upload failed: '%s'.", $ret->getErrorMessage()));
		    $response->setProperty('status_debug', print_r($ret, false));
		    print_r($ret);
		}
		break;

	    case 'new-album':
		$ret = $this->newAlbum($form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'fetch-album-images':
		$ret = $this->fetchAlbumImages($form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'album-properties':
		$ret = $this->albumProperties($form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'increment-view-count':
		$ret = $this->incrementViewCount($form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'image-properties':
		$ret = $this->imageProperties($form, $response);
		if ($ret) {
		    $status['controllerError'] = $ret;
		}
		break;

	    case 'no-op':
		$response->setProperty('status', $grStatusCodes['SUCCESS']);
		$response->setProperty('status_text', 'No-op successful');
		break;

	    default:
		$response->setProperty('status', $grStatusCodes['UNKNOWN_COMMAND']);
		$response->setProperty('status_text', "Command '${form['cmd']}' unknown.");
		break;
	    }
	} else {
	    $response->setProperty('status', $grStatusCodes['UNKNOWN_COMMAND']);
	    $response->setProperty('status_text', 'No cmd passed');
	}

	$user = $gallery->getActiveUser();
	if (isset($user)) {
	    $response->setProperty('debug_user', $user->getuserName());
	} else {
	    $response->setProperty('debug_user', 'error getting user');
	}

	$end = array_sum(explode(' ', microtime()));
	$response->setProperty('debug_time', round($end - $start, 3).'s');
	if (!empty($status['controllerError'])) {
	    $response->setProperty('debug_exception',
				   $status['controllerError']->getErrorMessage());
	}

	if ($session->isPersistent()) {
	    $response->setProperty('auth_token', $session->getAuthToken());
	}

	$status['controllerResponse'] = $response;

	$results['delegate']['view'] = 'remote.GalleryRemote';
	$results['status'] = $status;
	$results['error'] = $error;
	return array(null, $results);
    }

    /**
     * Log into Gallery
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function login($form, &$response) {
	global $gallery;

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();
	$grVersionCodes = GalleryRemoteConstants::getVersionCodes();

	/* If they don't provide a username, try the anonymous user */
	if (!empty($form['uname'])) {
	    list ($ret, $user) = GalleryCoreApi::fetchUserByUsername($form['uname']);
	    if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		return $ret;
	    }

	    list ($ret, $isDisabled) = GalleryCoreApi::isDisabledUsername($form['uname']);
	    if ($ret) {
		return $ret;
	    }
	    $password = isset($form['password'])
		? GalleryUtilities::htmlEntityDecode($form['password']) : '';
	    if (!$isDisabled && $user != null && $user->isCorrectPassword($password)) {
		/* login successful */
		$gallery->setActiveUser($user);
		$response->setProperty('server_version',
		    sprintf('%d.%d', $grVersionCodes['MAJ'], $grVersionCodes['MIN']));

		$response->setProperty('debug_core_version',
		    implode(',', GalleryCoreApi::getApiVersion()));

		list ($ret, $version) =
		    GalleryCoreApi::getPluginParameter('module', 'remote', '_version');
		/* don't fail on error */
		$response->setProperty('debug_module_version', $version);

		$response->setProperty('status', $grStatusCodes['SUCCESS']);
		$response->setProperty('status_text', 'Login successful.');

		$event = GalleryCoreApi::newEvent('Gallery::Login');
		$event->setEntity($user);
		list ($ret, $ignored) = GalleryCoreApi::postEvent($event);
		if ($ret) {
		    return $ret;
		}

		return null;
	    } else {
		/* login unsuccessful */
		$response->setProperty('status', $grStatusCodes['PASSWORD_WRONG']);
		$response->setProperty('status_text', 'Password incorrect.');

		$event = GalleryCoreApi::newEvent('Gallery::FailedLogin');
		$event->setData(array('userName' => $form['uname']));
		list ($ret, $ignored) = GalleryCoreApi::postEvent($event);
		if ($ret) {
		    return $ret;
		}

		return null;
	    }
	} else {
	    if ($gallery->getActiveUser()) {
		/* already logged in... this sounds like the applet logging in with a cookie */
		$response->setProperty('server_version',
		    sprintf('%d.%d', $grVersionCodes['MAJ'], $grVersionCodes['MIN']));
		$response->setProperty('status', $grStatusCodes['LOGIN_MISSING']);
		$response->setProperty('status_text', 'Login parameters not found.');
		return null;
	    } else {
		/* They're logged in as the guest account */
		list ($ret, $anonymousUserId) = GalleryCoreApi::getAnonymousUserId();
		if ($ret) {
		    return $ret;
		}

		list ($ret, $anonymousUser) =
		    GalleryCoreApi::loadEntitiesById($anonymousUserId, 'GalleryUser');
		if ($ret) {
		    return $ret;
		}

		$gallery->setActiveUser($anonymousUser);

		$response->setProperty('debug_anonymous_login', true);
		$response->setProperty('server_version',
		    sprintf('%d.%d', $grVersionCodes['MAJ'], $grVersionCodes['MIN']));
		$response->setProperty('status', $grStatusCodes['SUCCESS']);
		$response->setProperty('status_text', 'Login successful.');
		return null;
	    }
	}
    }

    /**
     * Load the album list into our response object
     *
     * @param boolean $prune prune albums where user can't upload to or create sub-albums
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function fetchAlbums($prune, $form, &$response) {
	global $gallery;

	$start = array_sum(explode(' ', microtime()));
	$fetchPermissions = empty($form['no_perms']);

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	list ($ret, $rootId) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return $ret;
	}

	if ($prune) {
	    /* find and load the list of albums we can add items to */
	    list ($ret, $albumIds1) =
		GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addDataItem');
	    if ($ret) {
		return $ret;
	    }

	    /* find and load the list of albums we can add albums to */
	    list ($ret, $albumIds2) =
		GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addAlbumItem');
	    if ($ret) {
		return $ret;
	    }

	    $albumIds = array_unique(array_merge($albumIds1, $albumIds2));

	    if (!empty($albumIds)) {
		/** @todo Use fetchAllItemIds with multiple permissions once the API is available */
		list ($ret, $permissions) = GalleryCoreApi::fetchPermissionsForItems($albumIds);
		if ($ret) {
		    return $ret;
		}
		$filteredIds = array();
		foreach ($albumIds as $id) {
		    if (!empty($permissions[$id]['core.view'])) {
			$filteredIds[] = $id;
		    }
		}
		$albumIds = $filteredIds;
	    }
	} else {
	    /* find and load all albums that can be viewed */
	    list ($ret, $albumIds) = GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem');
	    if ($ret) {
		return $ret;
	    }
	}

	$response->setProperty('debug_time_albumids',
			       round(array_sum(explode(' ', microtime())) - $start, 3) . 's');

	if (empty($albumIds)) {
	    $response->setProperty('status', $grStatusCodes['SUCCESS']);
	    $response->setProperty('album_count', 0);
	    $response->setProperty('status_text', 'No viewable albums.');
	    return null;
	}

	/* Load all the entities */
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return $ret;
	}

	$response->setProperty('debug_time_entities',
			       round(array_sum(explode(' ', microtime())) - $start, 3) . 's');

	if ($prune) {
	    /* Recursively load parents */
	    list ($ret, $albums, $albumIds) =
		$this->loadParentsToRoot($rootId, $albumIds, $albums, $albums);
	    if ($ret) {
		return $ret;
	    }
	}

	if ($fetchPermissions) {
	    /* Load the permissions for all those albums */
	    list ($ret, $permissionsTable) = GalleryCoreApi::fetchPermissionsForItems($albumIds);
	    if ($ret) {
		return $ret;
	    }
	}

	$response->setProperty('debug_time_permissions',
			       round(array_sum(explode(' ', microtime())) - $start, 3) . 's');

	/* Now add all the albums to the tree */
	$i = 1;
	foreach ($albums as $album) {
	    /* Use id because path component is not unique */
	    $response->setProperty('album.name.' . $i, $album->getId());
	    $response->setProperty('album.title.' . $i, $album->getTitle());
	    $response->setProperty('album.summary.' . $i, $album->getSummary());
	    $response->setProperty('album.parent.' . $i, $album->getParentId());

	    if ($fetchPermissions) {
		$perms = $permissionsTable[$album->getId()];

		$response->setProperty('album.perms.add.' . $i,
				       isset($perms['core.addDataItem']) ? 'true' : 'false');
		$response->setProperty('album.perms.write.' . $i,
				       isset($perms['core.edit']) ? 'true' : 'false');
		$response->setProperty('album.perms.del_alb.' . $i,
				       isset($perms['core.delete']) ? 'true' : 'false');
		$response->setProperty('album.perms.create_sub.' . $i,
				       isset($perms['core.addAlbumItem']) ? 'true' : 'false');
	    }

	    $response->setProperty('album.info.extrafields.' . $i, "Summary,Description");

	    $i++;
	}

	$response->setProperty('debug_time_generate',
			       round(array_sum(explode(' ', microtime())) - $start, 3) . 's');

	list ($ret, $perm) = GalleryCoreApi::hasItemPermission($rootId, 'core.addAlbumItem');
	if ($ret) {
	    return $ret;
	}

	$response->setProperty('can_create_root', $perm ? 'true' : 'false');
	$response->setProperty('album_count', sizeof($albums));
	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'Fetch-albums successful.');

	return null;
    }

    function loadParentsToRoot($rootId, $albumIds, $albumsLoaded, $albumsToLoad) {
	$parentIds = array();
	foreach ($albumsToLoad as $album) {
	    $parentId = $album->getParentId();
	    if ($parentId && !in_array($parentId, $albumIds)) {
		$parentIds[] = $parentId;
		$albumIds[] = $parentId;
	    }
	}

	if (!empty($parentIds)) {
	    /* load all the new parents at once */
	    list ($ret, $parents) = GalleryCoreApi::loadEntitiesById($parentIds, 'GalleryItem');
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $albumsLoaded = array_merge($albumsLoaded, $parents);

	    /* and recurse */
	    list ($ret, $albumsLoaded, $albumIds) =
		$this->loadParentsToRoot($rootId, $albumIds, $albumsLoaded, $parents);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}
	return array(null, $albumsLoaded, $albumIds);
    }

    /**
     * Add a data item to Gallery
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function addDataItem($form, &$response) {
	global $gallery;

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	$file = GalleryUtilities::getFile('userfile');

	if (!empty($form['set_albumId'])) {
	    list ($ret, $parentItem) =
		GalleryCoreApi::loadEntitiesById($form['set_albumId'], 'GalleryAlbumItem');
	    if ($ret) {
		return $ret;
	    }
	    $parentId = $parentItem->getId();
	    unset($parentItem);
	} else if (!empty($form['set_albumPath'])) {
	    list ($ret, $parentId) =
		GalleryCoreApi::fetchItemIdByPath(urldecode($form['set_albumName']));
	    if ($ret) {
		return $ret;
	    }
	} else if (!empty($form['set_albumName'])) {
	    /* todo: delete this G1/early G2 throwback */
	    $parentId = $form['set_albumName'];
	} else {
	    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	}

	$ret = $this->_assertHasViewPermission($parentId);
	if ($ret) {
	    return $ret;
	}

	/* Make sure we have permission do edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($parentId, 'core.addDataItem');
	if ($ret) {
	    return $ret;
	}

	list ($ret, $lockIds[]) = GalleryCoreApi::acquireReadLock($parentId);
	if ($ret) {
	    return $ret;
	}

	if (empty($file['name'])) {
	    $response->setProperty('status', $grStatusCodes['NO_FILENAME']);
	    $response->setProperty('status_text', 'Filename not specified.');
	    return null;
	}

	/* Get the mime type. */
	list ($ret, $mimeType) = GalleryCoreApi::getMimeType($file['name'], $file['type']);
	if ($ret) {
	    return $ret;
	}

	if (isset($form['force_filename'])) {
	    $itemName = $form['force_filename'];
	} else {
	    $itemName = $file['name'];
	}

	list ($ret, $markup) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	if ($ret) {
	    return $ret;
	}
	if ($markup == 'html') {
	    /* Strip malicious content if html markup allowed */
	    if (isset($form['caption'])) {
		$form['caption'] = GalleryUtilities::htmlSafe($form['caption'], true);
	    }
	    if (isset($form['extrafield.Summary'])) {
		$form['extrafield.Summary'] =
		    GalleryUtilities::htmlSafe($form['extrafield.Summary'], true);
	    }
	    if (isset($form['extrafield.Description'])) {
		$form['extrafield.Description'] =
		    GalleryUtilities::htmlSafe($form['extrafield.Description'], true);
	    }
	}

	$title = isset($form['caption'])?$form['caption']:NULL;
	$summary = isset($form['extrafield.Summary'])?$form['extrafield.Summary']:NULL;
	$description = isset($form['extrafield.Description'])?$form['extrafield.Description']:NULL;

	/*
	 * Don't use uploaded files, because the framework cannot safely copy them.
	 * Move it to our temporary directory first.
	 */
	$platform =& $gallery->getPlatform();
	if ($platform->is_uploaded_file($file['tmp_name'])) {
	    $tmpFile = $platform->move_uploaded_file($file['tmp_name']);
	    if (!$tmpFile) {
		return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE);
	    }
	    $needToDeleteTmpFile = true;
	} else {
	    $tmpFile = $file['tmp_name'];
	    $needToDeleteTmpFile = false;
	}

	list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
	    $tmpFile, basename($itemName), $title,
	    $summary, $description, $mimeType, $parentId);

	/* Get rid of the tmp file if necessary */
	if ($needToDeleteTmpFile) {
	    @$platform->unlink($tmpFile);
	}

	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::releaseLocks($lockIds);
	if ($ret) {
	    return $ret;
	}

	if (isset($this->_optionInstances)) {
	    $optionInstances = $this->_optionInstances;
	} else {
	    GalleryCoreApi::requireOnce('modules/core/ItemAdd.inc');
	    list ($ret, $optionInstances) = ItemAddOption::getAllAddOptions();
	    if ($ret) {
		return $ret;
	    }
	}

	/* Allow ItemAddOptions to process added item(s) */
	foreach ($optionInstances as $option) {
	    list ($ret, $optionErrors, $optionWarnings) =
		$option->handleRequestAfterAdd($form, array($newItem));
	    if ($ret) {
		return $ret;
	    }

	    /*
	     * Swallow option warnings and errors for now.
	     *
	     * TODO: Report warnings and errors back to Gallery Remote.  If the upload is denied
	     * because of quota limitations, then we'll get an error that we should report back.
	     */
	}

	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'Add photo successful.');
	$response->setProperty('item_name', $newItem->getId());

	return null;
    }

    /**
     * Create a new album
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function newAlbum($form, &$response) {
	global $gallery;

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	$itemId = $form['set_albumName'];	  /* TODO: Eliminate this throwback to G1 */

	$ret = $this->_assertHasViewPermission($itemId);
	if ($ret) {
	    return $ret;
	}

	/* Make sure we have permission do edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.addAlbumItem');
	if ($ret) {
	    return $ret;
	}

	/* Use a suitable default for the album name if none is given */
	if (!isset($form['newAlbumName']) || '' == $form['newAlbumName']) {
	    $form['newAlbumName'] = 'album';
	}

	/* Create the album */
	list ($ret, $album) = GalleryCoreApi::createAlbum($itemId, $form['newAlbumName'],
							  $form['newAlbumTitle'], null,
							  $form['newAlbumDesc'], null);
	if ($ret) {
	    return $ret;
	}

	$response->setProperty('album_name', $album->getid());
	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'New-album successful.');

	return null;
    }

    /**
     * Fetch album images
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function fetchAlbumImages($form, &$response) {
	global $gallery;

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	$albumsToo = isset($form['albums_too']) && $form['albums_too'] == 'yes';
	$allSizes = isset($form['all_sizes']) && $form['all_sizes'] == 'yes';

	list ($ret, $rootId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return $ret;
	}

	if (isset($form['set_albumName'])) {
	    /* list an actual album */
	    $albumId = $form['set_albumName'];
	} else {
	    /* list the root album */
	    $albumId = $rootId;
	}

	$ret = $this->_assertHasViewPermission($albumId);
	if ($ret) {
	    return $ret;
	}

	/* check that the item is an album */
	list ($ret, $album) = GalleryCoreApi::loadEntitiesById($albumId, 'GalleryAlbumItem');
	if ($ret) {
	    if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
		$response->setProperty('status', $grStatusCodes['ERROR']);
		$response->setProperty('status_text', 'albumName is not a valid G2 object ID');
		return null;
	    } else {
		return $ret;
	    }
	}

	$response->setProperty('album.caption', $album->getTitle());
	if (isset($form['extrafields']) && $form['extrafields'] == 'yes') {
	    $response->setProperty('album.extrafields', "Summary,Description");
	}

	if (isset($form['random']) && $form['random'] == 'yes' && isset($form['limit'])) {
	    /* get random descendants (only GalleryPhotoItems) of the album */
	    list ($ret, $aclIds) = GalleryCoreApi::fetchAccessListIds('core.view',
		    $gallery->getActiveUserId());
	    if (empty($aclIds)) {
		$response->setProperty('status', $grStatusCodes['ERROR']);
		$response->setProperty('status_text', 'Can\'t get ACLs for the current user.');
		return null;
	    }
	    $aclMarkers = GalleryUtilities::makeMarkers(count($aclIds));

	    /* set parent sequence */
	    if ($albumId == $rootId) {
		$parentSequence = '';
	    } else {
		list ($ret, $parentSequence) = GalleryCoreApi::fetchParentSequence($albumId);
		if ($ret) {
		    return $ret;
		}

		$parentSequence[] = $albumId;
		$parentSequence = implode('/', $parentSequence);
	    }

	    /* get the random function for the DB */
	    $storage =& $gallery->getStorage();
	    list ($ret, $orderBy) = $storage->getFunctionSql('RAND', array());
	    if ($ret) {
		return $ret;
	    }

	    $query = '
		SELECT
		  [GalleryDataItem::id]
		FROM
		  [GalleryDataItem], [GalleryItemAttributesMap],
		  [GalleryAccessSubscriberMap]
		WHERE
		  [GalleryDataItem::id] = [GalleryItemAttributesMap::itemId]';

	    if (!empty($parentSequence)) {
		$query .= "
		  AND
		  [GalleryItemAttributesMap::parentSequence] LIKE '$parentSequence/%'";
	    }

	    $query .= "
		  AND
		  [GalleryDataItem::id] = [GalleryAccessSubscriberMap::itemId]
		  AND
		  [GalleryAccessSubscriberMap::accessListId] IN ($aclMarkers)
		ORDER BY
		  $orderBy
		";

	    $data = $aclIds;

	    $params = array();
	    $params['limit'] = array('count' => $form['limit']);

	    list ($ret, $searchResults) = $gallery->search($query, $data, $params);
	    if ($ret) {
		return $ret;
	    }

	    $ids = array();
	    while ($result = $searchResults->nextResult()) {
		$ids[] = $result[0];
	    }
	} else {
	    /* get children of the album */
	    list ($ret, $ids) =
		    GalleryCoreApi::fetchChildItemIdsWithPermission($albumId, 'core.view');
	    if ($ret) {
		return $ret;
	    }
	}

	if (empty($ids)) {
	    /* nothing in this album */
	    $response->setProperty('image_count', 0);
	    $response->setProperty('status', $grStatusCodes['SUCCESS']);
	    $response->setProperty(
		'status_text', 'Fetch-album-images successful, but no items found.');
	    return null;
	}

	list ($ret, $items) = GalleryCoreApi::loadEntitiesById($ids, 'GalleryItem');
	if ($ret) {
	    return $ret;
	}

	/* Load the derivatives for all those items */
	list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds($ids);
	if ($ret) {
	    return $ret;
	}

	/* Precache item permissions */
	$ret = GalleryCoreApi::studyPermissions($ids);
	if ($ret) {
	    return $ret;
	}

	$i = 1;
	foreach ($items as $item) {
	    if (GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
		list ($ret, $permissions) = GalleryCoreApi::getPermissions($item->getId());
		if ($ret) {
		    return $ret;
		}
		if (isset($permissions['core.viewSource'])) {
		    $response->setProperty('image.name.' . $i, $item->getId());
		    $response->setProperty('image.raw_width.' . $i, $item->getWidth());
		    $response->setProperty('image.raw_height.' . $i, $item->getHeight());
		    $response->setProperty('image.raw_filesize.' . $i, $item->getSize());
		} else if (!isset($permissions['core.viewResizes'])) {
		    continue;
		}

		$thumb = null;
		$resizes = array();
		if (isset($derivatives[$item->getId()])) {
		    foreach ($derivatives[$item->getId()] as $index => $der) {
			if ($der->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
			    $thumb = $der;
			} else if ($der->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_PREFERRED &&
				   isset($permissions['core.viewSource'])) {
			    $response->setProperty('image.name.' . $i, $der->getId());
			    $response->setProperty('image.raw_width.' . $i, $der->getWidth());
			    $response->setProperty('image.raw_height.' . $i, $der->getHeight());
			    $response->setProperty('image.raw_filesize.' . $i,
						   $der->getDerivativeSize());
			} else if ($der->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_RESIZE &&
				   isset($permissions['core.viewResizes'])) {
			    $resizes[$index] = $der->getWidth() * $der->getHeight();
			}
		    }
		    arsort($resizes);
		    $resizes = array_keys($resizes);
		}
		if (!$response->hasProperty('image.name.' . $i)) {
		    if (empty($resizes)) {
			/* No permission on full size and no resizes found */
			continue;
		    }
		    $der = $derivatives[$item->getId()][array_shift($resizes)];
		    $response->setProperty('image.name.' . $i, $der->getId());
		    $response->setProperty('image.raw_width.' . $i, $der->getWidth());
		    $response->setProperty('image.raw_height.' . $i, $der->getHeight());
		    $response->setProperty('image.raw_filesize.' . $i, $der->getDerivativeSize());
		}
		if (!empty($resizes)) {
		    $der = $derivatives[$item->getId()][array_shift($resizes)];
		    $response->setProperty('image.resizedName.' . $i, $der->getId());
		    $response->setProperty('image.resized_width.' . $i, $der->getWidth());
		    $response->setProperty('image.resized_height.' . $i, $der->getHeight());
		    
		    if ($allSizes) {
			$resizedNum = 0;
			while (!empty($resizes)) {
			    $der = $derivatives[$item->getId()][array_shift($resizes)];
			    $resizedNum++;
			    $response->setProperty(
			      'image.resized.' . $resizedNum . '.name.' . $i, $der->getId());
			    $response->setProperty(
			      'image.resized.' . $resizedNum . '.width.' . $i, $der->getWidth());
			    $response->setProperty(
			      'image.resized.' . $resizedNum . '.height.' . $i, $der->getHeight());
			}
			$response->setProperty('image.resizedNum.' . $i, $resizedNum);
		    }
		}
		if (isset($thumb)) {
		    $response->setProperty('image.thumbName.' . $i, $thumb->getId());
		    $response->setProperty('image.thumb_width.' . $i, $thumb->getWidth());
		    $response->setProperty('image.thumb_height.' . $i, $thumb->getHeight());
		}

		list ($ret, $extensions) =
		    GalleryCoreApi::convertMimeToExtensions($item->getMimeType());
		if ($ret) {
		    return $ret;
		}
		if (isset($extensions) && isset($extensions[0])) {
		    $response->setProperty('image.forceExtension.' . $i, $extensions[0]);
		}

		$response->setProperty('image.caption.' . $i, $item->getTitle());
		$response->setProperty('image.title.' . $i, $item->getPathComponent());
		$response->setProperty('image.hidden.' . $i, 'no');
		
		if (isset($form['extrafields']) && $form['extrafields'] == 'yes') {
		    if ($item->getDescription()) {
			$response->setProperty('image.extrafield.Description.' . $i,
					       $item->getDescription());
		    }
		    if ($item->getSummary()) {
			$response->setProperty('image.extrafield.Summary.' . $i,
					       $item->getSummary());
		    }
		}

		$i++;
	    } else if (GalleryUtilities::isA($item, 'GalleryAlbumItem') && $albumsToo) {
		$response->setProperty('album.name.' . $i, $item->getId());

		$i++;
	    }
	}

	$urlGenerator =& $gallery->getUrlGenerator();
	$response->setProperty('baseurl', str_replace('&amp;', '&',
	    GalleryUrlGenerator::appendParamsToUrl(
		$urlGenerator->getCurrentUrlDir(true) . GALLERY_MAIN_PHP,
		array('view' => 'core.DownloadItem', 'itemId' => ''))));
	$response->setProperty('image_count', $i - 1);
	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'Fetch-album-images successful.');

	return null;
    }

    /**
     * Album properties
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function albumProperties($form, &$response) {
	global $gallery;

	/*
	 * GR now only supports 1-dimension size constraints. It used to support 2-d
	 * but since G1 supported 1-d, I simplified GR a while ago, so now we have
	 * to cheat a bit and select the largest of width and height to return to GR
	 */

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	if (isset($form['set_albumName'])) {
	    /* list an actual album */
	    $albumId = $form['set_albumName'];
	} else {
	    /* list the root album */
	    list ($ret, $albumId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return $ret;
	    }
	}

	$ret = $this->_assertHasViewPermission($albumId);
	if ($ret) {
	    return $ret;
	}

	$biggestDerivative = $maxSize = 0;

	/* Find the derivative preferences for the album */
	list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativePreferencesForItem($albumId);
	if ($ret) {
	    return $ret;
	}

	foreach ($derivatives as $derivative) {
	    preg_match('/scale\|(\d*),(\d*)/', $derivative['derivativeOperations'], $matches);

	    if ($derivative['derivativeType'] != DERIVATIVE_TYPE_IMAGE_THUMBNAIL &&
		    isset($matches[1])) {
		if ($matches[1] > $biggestDerivative) {
		    $biggestDerivative = $matches[1];
		}
		if ($matches[2] > $biggestDerivative) {
		    $biggestDerivative = $matches[2];
		}
	    }
	}

	/* Is the sizelimit module active? */
	list ($ret, $modules) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
	    return $ret;
	}

	if (isset($modules['sizelimit']['active']) && $modules['sizelimit']['active']) {
	    /* If the sizelimit module is enabled, find the max size for the album */
	    list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters(
		'module', 'sizelimit', $albumId);
	    if ($ret) {
		return $ret;
	    }

	    if (isset($param['width'])) {
		$maxSize = $param['width'];
	    }

	    if (isset($param['height']) && $param['height'] > $maxSize) {
		$maxSize = $param['height'];
	    }
	}

	$response->setProperty('auto_resize', $biggestDerivative);
	$response->setProperty('max_size', $maxSize);
	$response->setProperty('extrafields', "Summary,Description");

	list ($ret, $album) = GalleryCoreApi::loadEntitiesById($albumId, 'GalleryAlbumItem');
        if ($ret) {
            return $ret;
        }

	$response->setProperty('title', $album->getTitle());

	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'Album-properties successful.');

	return null;
    }

    /**
     * Increment view count
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function incrementViewCount($form, &$response) {
	global $gallery;
	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	if (empty($form['itemId'])) {
	    $response->setProperty('status', $grStatusCodes['MISSING_ARGUMENTS']);
	    $response->setProperty('status_text', 'increment-view-count failed.');
	    return null;
	}

	$itemId = $form['itemId'];

	$ret = $this->_assertHasViewPermission($itemId);
	if ($ret) {
	    return $ret;
	}

	/* Increment the view count */
	$ret = GalleryCoreApi::incrementItemViewCount($itemId);
	if ($ret) {
	    return $ret;
	}

	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'increment-view-count successful.');

	return null;
    }

    /**
     * Image properties
     *
     * @param array $form key/value pairs from Gallery Remote
     * @param GalleryRemoteProperties $response a reference to our response object
     * @return GalleryStatus a status code
     */
    function imageProperties($form, &$response) {
	global $gallery;

	/*
	 * GR now only supports 1-dimension size constraints. It used to support 2-d
	 * but since G1 supported 1-d, I simplified GR a while ago, so now we have
	 * to cheat a bit and select the largest of width and height to return to GR
	 */

	$grStatusCodes = GalleryRemoteConstants::getStatusCodes();

	if (isset($form['id'])) {
	    $id = $form['id'];
	} else {
	    /* todo: handle error */
	}

	list ($ret, $item) = GalleryCoreApi::loadEntitiesById($id, 'GalleryItem');
	if ($ret) {
	    return $ret;
	}

	$ret = $this->_assertHasViewPermission($id);
	if ($ret) {
	    return $ret;
	}

	if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    /* if we have an album, get his thumbnail instead */
	    list ($ret, $thumbs) = GalleryCoreApi::fetchThumbnailsByItemIds(array($id));
	    if ($ret) {
		return $ret;
	    }

	    $response->setProperty('image.thumbName', $thumbs[$id]->getId());
	    $response->setProperty('image.thumb_width', $thumbs[$id]->getWidth());
	    $response->setProperty('image.thumb_height', $thumbs[$id]->getHeight());
	} else if (GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
	    /* Load the derivatives for all those items */
	    list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds(array($id));
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($item->getId());
	    if ($ret) {
		return $ret;
	    }
	    if (!isset($permissions['core.viewSource'])
		    && !isset($permissions['core.viewResizes'])) {
		$response->setProperty('status', $grStatusCodes['NO_VIEW_PERMISSION']);
		$response->setProperty('status_text', 'You are not allowed to see this image.');
	    }

	    $response->setProperty('image.name', $item->getId());
	    $response->setProperty('image.raw_width', $item->getWidth());
	    $response->setProperty('image.raw_height', $item->getHeight());
	    $response->setProperty('image.raw_filesize', $item->getSize());

	    list ($ret, $extensions) =
		GalleryCoreApi::convertMimeToExtensions($item->getMimeType());
	    if ($ret) {
		return $ret;
	    }
	    if (isset($extensions) && isset($extensions[0])) {
		$response->setProperty('image.forceExtension', $extensions[0]);
	    }

	    $response->setProperty('image.caption', $item->getTitle());
	    $response->setProperty('image.title', $item->getPathComponent());
	    $response->setProperty('image.hidden', 'no');

	    $thumb = null;
	    $resizes = array();
	    if (isset($derivatives[$item->getId()])) {
		foreach ($derivatives[$item->getId()] as $index => $der) {
		    if ($der->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
			$thumb = $der;
		    } else if ($der->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_PREFERRED &&
			       isset($permissions['core.viewSource'])) {
			/* override the source image with the preferred version */
			$response->setProperty('image.name', $der->getId());
			$response->setProperty('image.raw_width', $der->getWidth());
			$response->setProperty('image.raw_height', $der->getHeight());
			$response->setProperty('image.raw_filesize', $der->getDerivativeSize());
		    } else if ($der->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_RESIZE &&
			       isset($permissions['core.viewResizes'])) {
			$resizes[$index] = $der->getWidth() * $der->getHeight();
		    }
		}
		arsort($resizes);
		$resizes = array_keys($resizes);
	    }

	    if (!empty($resizes)) {
		$der = $derivatives[$item->getId()][array_shift($resizes)];
		$response->setProperty('image.resizedName', $der->getId());
		$response->setProperty('image.resized_width', $der->getWidth());
		$response->setProperty('image.resized_height', $der->getHeight());
	    }
	    if (isset($thumb)) {
		$response->setProperty('image.thumbName', $thumb->getId());
		$response->setProperty('image.thumb_width', $thumb->getWidth());
		$response->setProperty('image.thumb_height', $thumb->getHeight());
	    }
	}

	$response->setProperty('status', $grStatusCodes['SUCCESS']);
	$response->setProperty('status_text', 'image-properties successful.');

	return null;
    }

    /**
     * Assert that the active user can view the specified item.
     * Otherwize, avoid information disclosure by acting as if the item didn't exist.
     * @param int $itemId
     * @return GalleryStatus
     */
    function _assertHasViewPermission($itemId) {
	list ($ret, $canView) = GalleryCoreApi::hasItemPermission($itemId, 'core.view');
	if ($ret) {
	    return $ret;
	}
	/* Avoid information disclosure, act as if the item didn't exist. */
	return $canView ? null : GalleryCoreApi::error(ERROR_MISSING_OBJECT);
    }
}

/**
 * This is an immediate view that emits well formed Gallery Remote protocol 2 output
 */
class GalleryRemoteView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	if (!headers_sent()) {
	    header("Content-type: text/plain; charset=UTF-8");
	}

	if (isset($status['controllerError'])) {
	    print 'Error: ' . $status['controllerError']->getAsText();
	}

	if (isset($status['controllerResponse'])) {
	    print $status['controllerResponse']->listProperties();
	}

	if (isset($controllerError)) {
	    return $ret;
	} else {
	    return null;
	}
    }
}
?>
