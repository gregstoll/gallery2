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
 * RandomHighlight module - periodically change the album highlight
 * @package RandomHighlight
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class RandomHighlightModule extends GalleryModule {

    function RandomHighlightModule() {
	global $gallery;
	$this->setId('randomhighlight');
	$this->setName($gallery->i18n('Random Highlight'));
	$this->setDescription($gallery->i18n('Periodically change the album highlight'));
	$this->setVersion('1.0.9'); /* Update upgrade() function below too */
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	list ($ret, $params) =
	    GalleryCoreApi::fetchAllPluginParameters('module', 'randomhighlight');
	if ($ret) {
	    return $ret;
	}
	foreach (array('duration' => 7200 /* 2 hours */) as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	if (!empty($currentVersion)) {
	    switch ($currentVersion) {
	    case '0.5':
	    case '0.8.1':
	    case '0.8.2':
	    case '0.8.3':
	    case '0.8.4':
	    case '0.8.5':
	    case '0.8.6':
		/* Changed from RandomHighlightDerivativeImage entity type to onLoadHandler */
		$storage =& $gallery->getStorage();
		$query = 'UPDATE [GalleryEntity]
		    SET [::entityType] = \'GalleryDerivativeImage\',
			[::onLoadHandlers] = \'|RandomHighlight|\'
		  WHERE [GalleryEntity::entityType] = \'RandomHighlightDerivativeImage\'
		    AND [GalleryEntity::onLoadHandlers] IS NULL';
		$ret = $storage->execute($query);
		if ($ret) {
		    return $ret;
		}
		list ($ret, $handlers) = $storage->getFunctionSql('CONCAT',
		    array('[GalleryEntity::onLoadHandlers]', "'RandomHighlight|'"));
		if ($ret) {
		    return $ret;
		}
		$query = 'UPDATE [GalleryEntity]
		    SET [::entityType] = \'GalleryDerivativeImage\',
			[::onLoadHandlers] = ' . $handlers . '
		  WHERE [GalleryEntity::entityType] = \'RandomHighlightDerivativeImage\'
		    AND [GalleryEntity::onLoadHandlers] IS NOT NULL';
		$ret = $storage->execute($query);
		if ($ret) {
		    return $ret;
		}

	    case '0.9.0':
	    case '0.9.1':
	    case '0.9.2':
	    case '0.9.3':
	    case '0.9.4':
	    case '0.9.5':
	    case '0.9.6':
	    case '0.9.7':
	    case '0.9.8':
	    case '0.9.9':
	    case '1.0.0':
	    case '1.0.1':
	    case '1.0.2':
	    case '1.0.3':
	    case '1.0.4':
	    case '1.0.5':
	    case '1.0.6':
	    case '1.0.6.1':
		/* .mo file migration */
	    case '1.0.7':
	    case '1.0.8':

	    case 'end of upgrade path':
		break;

	    default:
		return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					    sprintf('Unknown module version %s', $currentVersion));
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryOnLoadHandler', 'RandomHighlightModule', 'RandomHighlight',
	    'modules/randomhighlight/module.inc', 'randomhighlight', null);
	if ($ret) {
	    return $ret;
	}
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryOnLoadHandler', 'RandomHighlightModule', 'RandomHighlightPrivate',
	    'modules/randomhighlight/module.inc', 'randomhighlight', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemEditOption', 'RandomHighlightOption', 'RandomHighlightOption',
	    'modules/randomhighlight/RandomHighlightOption.inc', 'randomhighlight',
	    array('ItemEditAlbum'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getOnLoadHandlerIds
     */
    function getOnLoadHandlerIds() {
	return array('RandomHighlight', 'RandomHighlightPrivate');
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Random Highlight'),
				 'view' => 'randomhighlight.RandomHighlightSiteAdmin')));
    }

    /**
     * Check age and pick new highlight if needed..
     */
    function onLoad(&$thumbnail, $duringUpgrade) {
	$picknew = false;
	$id = $thumbnail->getId();
	$loop = GalleryDataCache::containsKey("randomhighlight:load-$id");
	$count = GalleryDataCache::containsKey('randomhighlight:count')
	    ? GalleryDataCache::get('randomhighlight:count') : 0;
	/* Don't refresh the highlight during an upgrade; also limit # of reselects in 1 request */
	if ($duringUpgrade || $loop || $count >= 3) {
	    return null;
	}
	/* Acquire information whether we need to choose a new highlight */
	list ($ret, $duration) = $this->getParameter('duration');
	if ($ret) {
	    return $ret;
	}
	$isOld = (time() - $thumbnail->getModificationTimestamp() > $duration);
	$canSee = true;
	if (!$isOld && $thumbnail->hasOnLoadHandler('RandomHighlightPrivate')) {
	    /* Highlight is currently a non-public item.. can current user view that item? */
	    list ($ret, $highlight) = GalleryCoreApi::loadEntitiesById(
		$thumbnail->getDerivativeSourceId(),
		array('GalleryFileSystemEntity', 'GalleryDerivative'));
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $canSee) =
		GalleryCoreApi::hasItemPermission($highlight->getParentId(), 'core.view');
	    if ($ret) {
		return $ret;
	    }
	}
	$viewName = GalleryUtilities::getRequestVariables('view');
	/*
	 * Check if we need to pick a new highlight. Don't pick a new one on DownloadItem
	 * requests, unless we don't have the permission to see the current highlight
	 */
	if (($isOld && $viewName != 'core.DownloadItem') || !$canSee) {
	    /* Lock and refresh to ensure another request didn't just update highlight */
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($id);
	    if ($ret) {
		return $ret;
	    }
	    /* ->refresh() will call the onLoad handler again, thus use the GDC $loop variable */
	    GalleryDataCache::put("randomhighlight:load-$id", true);
	    list ($ret, $refresh) = $thumbnail->refresh();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    if ($refresh->getSerialNumber() == $thumbnail->getSerialNumber()) {
		$picknew = true;
	    }
	}

	if ($picknew) {
	    global $gallery;
	    GalleryDataCache::put('randomhighlight:count', $count + 1);
	    /* Query for child items */
	    list ($ret, $query, $data) = GalleryCoreApi::buildItemQuery(
		    'GalleryChildEntity', 'id', '[GalleryChildEntity::parentId] = ?', 'id', null,
		    null, 'core.view', false, $gallery->getActiveUserId());
	    if ($ret || empty($query)) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    array_unshift($data, $thumbnail->getParentId());
	    list ($ret, $searchResults) = $gallery->search($query, $data);
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    $itemIds = array();
	    while ($result = $searchResults->nextResult()) {
		$itemIds[] = (int)$result[0];
	    }
	    /* Pick a new random highlight */
	    $query = '
	    SELECT [GalleryDerivative::id] FROM [GalleryDerivative]
	    INNER JOIN [GalleryChildEntity]
		  ON [GalleryDerivative::id] = [GalleryChildEntity::id]
	    WHERE
		[GalleryDerivative::derivativeType] = ?
	      AND
		[GalleryDerivative::id] <> ?
	      AND
		([GalleryDerivative::isBroken] = 0 OR [GalleryDerivative::isBroken] IS NULL)
	      AND
		[GalleryChildEntity::parentId] IN (';
	    for ($n = 5; !empty($itemIds); $n += 10) {
		/* Hopefully most items have a thumbnail; pick n ids at a time until we find one */
		$data = array(DERIVATIVE_TYPE_IMAGE_THUMBNAIL, $thumbnail->getDerivativeSourceId());
		for ($i = 0; !empty($itemIds) && $i < $n; $i++) {
		    $oneItemIdAsArray = array_splice($itemIds, rand(0, count($itemIds) - 1), 1);
		    $data[] = array_shift($oneItemIdAsArray);
		}
		list ($ret, $searchResults) = $gallery->search(
		    $query . GalleryUtilities::makeMarkers(count($data) - 2) . ')', $data);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return $ret;
		}
		if ($result = $searchResults->nextResult()) {
		    $highlightId = (int)$result[0];
		    break;
		}
	    }
	    if (empty($highlightId)) {
		GalleryCoreApi::releaseLocks($lockId);
		return null;
	    }
	    list ($ret, $highlight) =
		GalleryCoreApi::loadEntitiesById($highlightId, 'GalleryDerivative');
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }

	    /* Check if we have selected a public item for the highlight */
	    list ($ret, $guestId) = GalleryCoreApi::getAnonymousUserId();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    list ($ret, $isPublic) =
		GalleryCoreApi::hasItemPermission($highlight->getParentId(), 'core.view', $guestId);
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    $onLoadHandlers = $thumbnail->getOnLoadHandlers();
	    $thumbnail->setOnLoadHandlers(
		preg_replace('{\|RandomHighlight(Private)?\|}',
			     '|RandomHighlight' . ($isPublic ? '|' : 'Private|'), $onLoadHandlers));

	    $thumbnail->setDerivativeSourceId($highlight->getId());
	    GalleryCoreApi::estimateDerivativeDimensions($thumbnail, $highlight);
	    $thumbnail->save();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	    $ret = GalleryCoreApi::expireDerivativeTreeBySourceIds(array($thumbnail->getId()));
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	}
	if (isset($lockId)) {
	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return $ret;
	    }
	}

	return null;
    }
}
?>
