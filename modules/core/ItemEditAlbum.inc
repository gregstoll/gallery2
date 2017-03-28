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
 * This plugin will handle the changes users make to an album.
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemEditAlbum extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	global $gallery;

	$status = null;
	$error = array();
	$requiresProgressBar = false;
	if (isset($form['action']['undo'])) {
	    /*
	     * Take no action and we'll be redirected back to the same page
	     * which will reset the form
	     */
	} else if (isset($form['action']['save'])) {
	    /* Validate the input data */
	    if (!is_numeric($form['thumbnail']['size']) || $form['thumbnail']['size'] < 1) {
		$error[] = 'form[error][thumbnail][size][invalid]';
	    }

	    $count = count($form['resizes']);
	    for ($i = 0; $i < $count; $i++) {
		if (empty($form['resizes'][$i]['active'])) {
		    unset($form['resizes'][$i]);
		} else if (empty($form['resizes'][$i]['width'])
			|| empty($form['resizes'][$i]['height'])) {
		    $error[] = 'form[error][resizes][' . $i . '][size][missing]';
		} else if (!($tmp = rtrim($form['resizes'][$i]['width'], '%'))
			|| !is_numeric($tmp) || $tmp < 1
			|| !($tmp = rtrim($form['resizes'][$i]['height'], '%'))
			|| !is_numeric($tmp) || $tmp < 1) {
		    $error[] = 'form[error][resizes][' . $i . '][size][invalid]';
		}
	    }

	    if (empty($error)) {
		/* Delete existing derivative preferences */
		$ret = GalleryCoreApi::removeDerivativePreferencesForItem($item->getId());
		if ($ret) {
		    return array($ret, null, null, null);
		}

		$changeTypes = array();
		if (isset($form['changeInDescendents'])) {
		    list ($ret, $subAlbumIds) =
			GalleryCoreApi::fetchDescendentAlbumItemIds($item);
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}
		/* Add the thumbnail size back in */
		$ret = GalleryCoreApi::addDerivativePreference(0, $item->getId(),
		    DERIVATIVE_TYPE_IMAGE_THUMBNAIL,
		    'thumbnail|' . $form['thumbnail']['size']);
		if ($ret) {
		    return array($ret, null, null, null);
		}
		if (isset($form['changeInDescendents']['thumbnail'])) {
		    $changeTypes[] = DERIVATIVE_TYPE_IMAGE_THUMBNAIL;
		}

		/* Add the resize-sizes back in */
		$i = 0;
		foreach ($form['resizes'] as $resize) {
		    $ret = GalleryCoreApi::addDerivativePreference(
			    $i++, $item->getId(), DERIVATIVE_TYPE_IMAGE_RESIZE,
			    'scale|' . $resize['width'] . ',' . $resize['height']);
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}
		if (isset($form['changeInDescendents']['resizes'])) {
		    $changeTypes[] = DERIVATIVE_TYPE_IMAGE_RESIZE;
		}

		/* Use appropriate settings in descendent albums */
		if (!empty($changeTypes) && !empty($subAlbumIds)) {
		    $ret = GalleryCoreApi::removeDerivativePreferenceForItemType(
			    $subAlbumIds, $changeTypes);
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		    if (isset($form['changeInDescendents']['thumbnail'])) {
			$ret = GalleryCoreApi::addDerivativePreference(0, $subAlbumIds,
				DERIVATIVE_TYPE_IMAGE_THUMBNAIL,
				'thumbnail|' . $form['thumbnail']['size']);
			if ($ret) {
			    return array($ret, null, null, null);
			}
		    }
		    if (isset($form['changeInDescendents']['resizes'])) {
			$i = 0;
			foreach ($form['resizes'] as $resize) {
			    $ret = GalleryCoreApi::addDerivativePreference(
				    $i++, $subAlbumIds, DERIVATIVE_TYPE_IMAGE_RESIZE,
				    'scale|' . $resize['width'] . ',' . $resize['height']);
			    if ($ret) {
				return array($ret, null, null, null);
			    }
			}
		    }
		}

		/* Recreate the thumbnails, if requested to do so */
		if (isset($form['recreateThumbnails'])) {
		    $templateAdapter =& $gallery->getTemplateAdapter();
		    $templateAdapter->registerTrailerCallback(
			array($this, 'runRecreateThumbnails'), array($form, $item, $preferred));
		    $requiresProgressBar = true;
		}

		/* Recreate the resizes, if requested to do so */
		if (isset($form['recreateResizes'])) {
		    $templateAdapter =& $gallery->getTemplateAdapter();
		    $templateAdapter->registerTrailerCallback(
			array($this, 'runRecreateResizes'), array($form, $item, $preferred));
		    $requiresProgressBar = true;
		}

		/* Save basic settings */
		if (!empty($form['presort'])) {
		    $item->setOrderBy($form['presort'] . '|' . $form['orderBy']);
		} else {
		    $item->setOrderBy($form['orderBy']);
		}
		if (isset($form['orderDirection'])) {
		    $item->setOrderDirection($form['orderDirection']);
		}
		$item->setSerialNumber($form['serialNumber']);

		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($item->getId());
		if ($ret) {
		    return array($ret, null, null, null);
		}

		$ret = $item->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null, null, null);
		}

		/* for sort order we need to load subalbums */
		if (!empty($subAlbumIds) && isset($form['changeInDescendents']['sort'])) {
		    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($subAlbumIds);
		    if ($ret) {
			return array($ret, null, null, null);
		    }

		    list ($ret, $subAlbums) =
			GalleryCoreApi::loadEntitiesById($subAlbumIds, 'GalleryAlbumItem');
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		    foreach ($subAlbums as $album) {
			if (isset($form['changeInDescendents']['sort'])) {
			    if (!empty($form['presort'])) {
				$album->setOrderBy($form['presort'] . '|' . $form['orderBy']);
			    } else {
				$album->setOrderBy($form['orderBy']);
			    }
			    if (isset($form['orderDirection'])) {
				$album->setOrderDirection($form['orderDirection']);
			    }
			}
			$ret = $album->save();
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return array($ret, null, null, null);
			}
		    }
		    $ret = GalleryCoreApi::releaseLocks($lockId);
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}

		/* Prepare our status message */
		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
		if ($ret) {
		    return array($ret, null, null, null);
		}

		$status = $module->translate('Settings saved successfully.');
	    }
	}

	return array(null, $error, $status, $requiresProgressBar);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	if ($form['formName'] != 'ItemEditAlbum') {
	    $tmp = explode('|', $item->getOrderBy(), 2);
	    if (count($tmp) < 2) {
		$form['orderBy'] = $item->getOrderBy();
		$form['presort'] = '';
	    } else {
		$form['orderBy'] = $tmp[1];
		$form['presort'] = $tmp[0];
	    }
	    $form['orderDirection'] = $item->getOrderDirection();
	    $form['formName'] = 'ItemEditAlbum';

	    /* Load up the data for the resizes table */
	    list ($ret, $preferences) =
		GalleryCoreApi::fetchDerivativePreferencesForItem($item->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }

	    foreach ($preferences as $preference) {
		if (preg_match('/(?:resize|scale|thumbnail)\|(\d+%?)(?:,(\d+%?))?/',
			       $preference['derivativeOperations'], $matches)) {
		    $size = $matches[1];
		    $height = empty($matches[2]) ? $size : $matches[2];
		}

		switch ($preference['derivativeType']) {
		case DERIVATIVE_TYPE_IMAGE_THUMBNAIL:
		    $form['thumbnail']['size'] = $size;
		    break;

		case DERIVATIVE_TYPE_IMAGE_RESIZE:
		    if (empty($size)) {
			$form['resizes'][] = array('active' => 0, 'width' => '', 'height' => '');
		    } else {
			$form['resizes'][] = array('active' => 1, 'width' => $size,
						   'height' => $height);
		    }
		    break;
		}
	    }

	    /* Tag on a few form blanks */
	    if (empty($form['resizes'])) {
		$extraBlanks = 3;
	    } else {
		$extraBlanks = max(2 - count($form['resizes']), 0) + 1;
	    }

	    while ($extraBlanks-- > 0) {
		$form['resizes'][] = array('active' => 0, 'width' => '', 'height' => '');
	    }

	    /* Always force these to be false */
	    $form['recreateThumbnails'] = false;
	    $form['recreateResizes'] = false;
	    $form['buildThumbnails'] = false;
	    $form['buildResizes'] = false;
	}

	/* Checkboxes are annoying in that they are empty if false */
	$form['recreateThumbnails'] = !empty($form['recreateThumbnails']);
	$form['recreateResizes'] = !empty($form['recreateThumbnails']);
	$form['buildThumbnails'] = !empty($form['buildThumbnails']);
	$form['buildResizes'] = !empty($form['buildResizes']);
	for ($i = 0; $i < count($form['resizes']); $i++) {
	    $form['resizes'][$i]['active'] = !empty($form['resizes'][$i]['active']);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Set up our sort order selection list */
	GalleryCoreApi::requireOnce('modules/core/classes/GallerySortInterface_1_2.class');
	list ($ret, $orderByList, $presortList, $orderDirectionList) =
	    GallerySortInterface_1_2::getAllSortOrders();
	if ($ret) {
	    return array($ret, null, null);
	}

	$ItemEditAlbum['orderByList'] = $orderByList;
	$ItemEditAlbum['presortList'] = $presortList;
	$ItemEditAlbum['orderDirectionList'] = $orderDirectionList;

	$template->setVariable('ItemEditAlbum', $ItemEditAlbum);
	$template->setVariable('controller', 'core.ItemEditAlbum');
	return array(null,
		     'modules/core/templates/ItemEditAlbum.tpl', 'modules_core');
    }

    function runRecreateResizes($form, $item, $preferred) {
	global $gallery;
	$storage =& $gallery->getStorage();
	$templateAdapter =& $gallery->getTemplateAdapter();

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return $ret;
	}
	$heading = $module->translate('Apply resized image settings');
	$templateAdapter->updateProgressBar($heading, $module->translate('Preparing...'), 0);

	/* Get the items to process */
	if (empty($form['changeInDescendents']['recreateResizes'])) {
	    list ($ret, $childIds) =
		GalleryCoreApi::fetchChildItemIdsWithPermission($item->getId(), 'core.edit');
	    if ($ret) {
		return $ret;
	    }
	} else {
	    list ($ret, $childIds) =
		GalleryCoreApi::fetchDescendentItemIds($item, null, null, 'core.edit');
	    if ($ret) {
		return $ret;
	    }
	}

	$batchSize = 100;
	$total = count($childIds);
	$ind = 0;
	$step = min(200, intval($total / 20) + 1);
	$resizePrefs = array();
	while (!empty($childIds)) {
	    $currentChildIds = array_splice($childIds, 0, $batchSize);
	    /* Load the children */
	    list ($ret, $childItems) =
		GalleryCoreApi::loadEntitiesById($currentChildIds, 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }

	    /* Load existing resizes */
	    list ($ret, $resizesSet) = GalleryCoreApi::fetchResizesByItemIds($currentChildIds);
	    if ($ret) {
		return $ret;
	    }
	    $resizesTable = array();
	    foreach ($resizesSet as $childId => $resizes) {
		foreach ($resizes as $resize) {
		    $resizesTable[$childId][$resize->getDerivativeOperations()] = $resize;
		}
	    }

	    /* Update the resizes */
	    foreach ($childItems as $child) {
		if (!(++$ind % $step)) {
		    $message = $module->translate(array('text' => 'Processing image %d of %d',
							'arg1' => $ind, 'arg2' => $total));
		    $templateAdapter->updateProgressBar($heading, $message, $ind / $total);
		}
		if (!GalleryUtilities::isA($child, 'GalleryDataItem')) {
		    continue;
		}

		$childId = $child->getId();
		$albumId = $child->getParentId();
		if (!isset($resizePrefs[$albumId])) {
		    /* Keep resizes for albums in memory */
		    list ($ret, $preferences) =
			GalleryCoreApi::fetchDerivativePreferencesForItem($albumId);
		    if ($ret) {
			return $ret;
		    }

		    $resizePrefs[$albumId] = array();
		    foreach ($preferences as $preference) {
			if ($preference['derivativeType'] == DERIVATIVE_TYPE_IMAGE_RESIZE
				&& preg_match('/(?:resize|scale)\|(\d+)(?:,(\d+))?/',
					      $preference['derivativeOperations'], $matches)) {
			    $width = $matches[1];
			    $height = empty($matches[2]) ? $width : $matches[2];
			    $resizePrefs[$albumId][] =
				array('operations' => $preference['derivativeOperations'],
				      'width' => $width, 'height' => $height);
			}
		    }
		}

		list ($ret, $source) = GalleryCoreApi::fetchPreferredSource($child);
		if ($ret) {
		    return $ret;
		}
		$mimeType = $source->getMimeType();

		/* Determine operations and check against existing resizes */
		$newResizes = array();
		for ($i = 0; $i < count($resizePrefs[$albumId]); $i++) {
		    if (!isset($resizePrefs[$albumId][$i][$mimeType])) {
			list ($ret, $resizePrefs[$albumId][$i][$mimeType]['operations'],
				    $resizePrefs[$albumId][$i][$mimeType]['outputMimeType']) =
			    GalleryCoreApi::makeSupportedViewableOperationSequence(
				    $mimeType, $resizePrefs[$albumId][$i]['operations'], false);
			if ($ret) {
			    return $ret;
			}
		    }
		    $resize = $resizePrefs[$albumId][$i];

		    /* Validate toolkit support before adding back the resizes */
		    if (empty($resize[$mimeType]['operations'])) {
			continue;
		    }
		    $operations = $resize[$mimeType]['operations'];

		    /* Special case to make sure that we don't upsample photos */
		    if (GalleryUtilities::isA($child, 'GalleryPhotoItem')
			    && $child->getWidth() <= $resize['width']
			    && $child->getHeight() <= $resize['height']) {
			continue;
		    }

		    if (isset($resizesTable[$childId][$operations])) {
			/* Keep existing resize, build it if requested */
			if (!empty($form['buildResizes'])) {
			    list ($ret) = GalleryCoreApi::rebuildDerivativeCacheIfNotCurrent(
				    $resizesTable[$childId][$operations]->getId());
			    if ($ret) {
				return $ret;
			    }
			}
			unset($resizesTable[$childId][$operations]);
		    } else {
			/* Create resize with these settings on next pass */
			$newResizes[] = $resize;
		    }
		}
		/* Add new resizes, using existing derivative entities until we run out */
		foreach ($newResizes as $resize) {
		    if (!empty($resizesTable[$childId])) {
			$derivative = array_shift($resizesTable[$childId]);
			list ($ret, $lockId) =
			    GalleryCoreApi::acquireWriteLock($derivative->getId());
			if ($ret) {
			    return $ret;
			}
		    } else {
			list ($ret, $derivative) = GalleryCoreApi::newFactoryInstanceByHint(
				'GalleryDerivative', $source->getEntityType());
			if ($ret) {
			    return $ret;
			}
			if (!isset($derivative)) {
			    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
			}

			$ret = $derivative->create($childId, DERIVATIVE_TYPE_IMAGE_RESIZE);
			if ($ret) {
			    return $ret;
			}
		    }

		    $derivative->setDerivativeSourceId($source->getId());
		    $derivative->setDerivativeOperations($resize[$mimeType]['operations']);
		    $derivative->setMimeType($resize[$mimeType]['outputMimeType']);

		    $ret = GalleryCoreApi::estimateDerivativeDimensions($derivative, $source);
		    if ($ret) {
			return $ret;
		    }

		    $ret = $derivative->save();
		    if ($ret) {
			return $ret;
		    }
		    if (isset($lockId)) {
			$ret = GalleryCoreApi::releaseLocks($lockId);
			if ($ret) {
			    return $ret;
			}
			$lockId = null;
		    }
		    /* Build if requested */
		    if (!empty($form['buildResizes'])) {
			list ($ret) = GalleryCoreApi::rebuildDerivativeCacheIfNotCurrent(
				$derivative->getId());
			if ($ret) {
			    return $ret;
			}
		    }
		}
		/* Remove any leftover resizes */
		if  (isset($resizesTable[$childId])) {
		    foreach ($resizesTable[$childId] as $resize) {
			$ret = GalleryCoreApi::deleteEntityById($resize->getId(),
								'GalleryDerivative');
			if ($ret) {
			    return $ret;
			}
		    }
		}
	    }
	    $ret = $storage->checkPoint();
	    if ($ret) {
		return $ret;
	    }
	}
	$templateAdapter->updateProgressBar($heading, '', 1);
	$redirect = array('view' => 'core.ItemAdmin', 'subView' => 'core.ItemEdit',
			  'itemId' => $item->getId());

	$urlGenerator =& $gallery->getUrlGenerator();
	$templateAdapter->completeProgressBar($urlGenerator->generateUrl($redirect));

	return null;
    }

    function runRecreateThumbnails($form, $item, $preferred) {
	global $gallery;
	$storage =& $gallery->getStorage();
	$templateAdapter =& $gallery->getTemplateAdapter();

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return $ret;
	}
	$heading = $module->translate('Apply thumbnail settings');
	$templateAdapter->updateProgressBar($heading, $module->translate('Preparing...'), 0);

	/* Get the items to process */
	if (empty($form['changeInDescendents']['recreateThumbnails'])) {
	    list ($ret, $childIds) =
		GalleryCoreApi::fetchChildItemIdsWithPermission($item->getId(), 'core.edit');
	    if ($ret) {
		return $ret;
	    }
	} else {
	    list ($ret, $childIds) =
		GalleryCoreApi::fetchDescendentItemIds($item, null, null, 'core.edit');
	    if ($ret) {
		return $ret;
	    }
	}

	$batchSize = 100;
	$total = count($childIds);
	$ind = 0;
	$step = min(500, intval($total / 20) + 1);
	$thumbnailSizes = $thumbnailBuild = array();
	while (!empty($childIds)) {
	    $currentChildIds = array_splice($childIds, 0, $batchSize);
	    /* Load the children */
	    list ($ret, $childItems) =
		GalleryCoreApi::loadEntitiesById($currentChildIds, 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }
	    /* Load the thumbnail of the children */
	    list ($ret, $thumbTable) = GalleryCoreApi::fetchThumbnailsByItemIds($currentChildIds);
	    if ($ret) {
		return $ret;
	    }

	    $lockIds = array();
	    foreach ($childItems as $child) {
		if (!(++$ind % $step) || $ind == $total) {
		    $message = $module->translate(array('text' => 'Processing image %d of %d',
							'arg1' => $ind, 'arg2' => $total));
		    $templateAdapter->updateProgressBar($heading, $message, $ind / $total);
		}

		$childId = $child->getId();
		$albumId = $child->getParentId();
		if (empty($thumbnailSizes[$albumId])) {
		    /* Keep thumbnail sizes for albums in memory */
		    list ($ret, $preferences) =
			GalleryCoreApi::fetchDerivativePreferencesForItem($albumId);
		    if ($ret) {
			return $ret;
		    }

		    foreach ($preferences as $preference) {
			if (preg_match('/thumbnail\|(\d+)/',
			    $preference['derivativeOperations'], $matches)) {
			    $thumbnailSizes[$albumId] = $matches[1];
			    break;
			}
		    }
		    if (empty($thumbnailSizes[$albumId])) {
			return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
		    }
		}
		$thumbnailSize = $thumbnailSizes[$albumId];
		$thumbnail = null;
		if (isset($thumbTable[$childId])) {
		    /* We already have a thumbnail */
		    $thumbnail = $thumbTable[$childId];
		    $sourceId = $thumbnail->getDerivativeSourceId();

		    /* Load the source of the thumbnail */
		    list ($ret, $source) = GalleryCoreApi::loadEntitiesById(
			$sourceId, array('GalleryFileSystemEntity', 'GalleryDerivative'));
		    if ($ret && $ret->getErrorCode() & ERROR_MISSING_OBJECT) {
			/* Someone deleted the source image, we can only delete the thumbnail */
			list ($ret, $lockIds[]) =
			    GalleryCoreApi::acquireWriteLock($thumbnail->getId());
			if ($ret) {
			    return $ret;
			}
			$ret = $thumbnail->delete();
			if ($ret) {
			    return $ret;
			}
			continue;
		    } else if ($ret) {
			return $ret;
		    }

		    $operation = preg_replace('/((^|;)thumbnail)\|\d+/',
					      '$1|' . $thumbnailSize,
					      $thumbnail->getDerivativeOperations());
		} else {
		    /*
		     * There is no thumbnail yet (maybe the file was uploaded when there was no
		     * graphic toolkit). Build new thumbnail from source if it's a GalleryDataItem.
		     */
		    if (!GalleryUtilities::isA($child, 'GalleryDataItem')) {
			/* It's an album or something else, we can't make a thumbnail */
			continue;
		    }

		    list ($ret, $source) = GalleryCoreApi::fetchPreferredSource($child);
		    if ($ret) {
			return $ret;
		    }

		    list ($ret, $thumbnail) =
			GalleryCoreApi::newFactoryInstanceByHint('GalleryDerivative',
								 $source->getEntityType());
		    if ($ret) {
			return $ret;
		    }

		    $ret = $thumbnail->create($child->getId(), DERIVATIVE_TYPE_IMAGE_THUMBNAIL);
		    if ($ret) {
			return $ret;
		    }

		    $operation = 'thumbnail|' . $thumbnailSize;
		}

		if ($thumbnail == null) {
		    return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
		}

		/* Change the thumbnail */
		list ($ret, $operation, $outputMimeType) =
		    GalleryCoreApi::makeSupportedViewableOperationSequence(
			$source->getMimeType(), $operation);
		if ($ret) {
		    return $ret;
		}

		if (!empty($operation)) {
		    $thumbnail->setMimeType($outputMimeType);
		    $thumbnail->setDerivativeSourceId($source->getId());
		    $thumbnail->setDerivativeOperations($operation);
		    $thumbnail->expireCache();

		    if ($thumbnail->isModified()) {
			list ($ret, $lockIds[]) =
			    GalleryCoreApi::acquireWriteLock($thumbnail->getId());
			if ($ret) {
			    return $ret;
			}
			$ret = GalleryCoreApi::estimateDerivativeDimensions($thumbnail, $source);
			if ($ret) {
			    return $ret;
			}

			$ret = $thumbnail->save();
			if ($ret) {
			    return $ret;
			}
		    }

		    if (!empty($form['buildThumbnails'])) {
			$thumbnailBuild[] = $thumbnail->getId();
		    }
		}
	    }
	    $ret = GalleryCoreApi::releaseLocks($lockIds);
	    if ($ret) {
		return $ret;
	    }
	    $ret = $storage->checkPoint();
	    if ($ret) {
		return $ret;
	    }
	}

	/* Build thumbnails if requested */
	if (!empty($thumbnailBuild)) {
	    $message = $module->translate('Rebuilding thumbnails...');
	    $templateAdapter->updateProgressBar($heading, $message, 0);
	    $total = count($thumbnailBuild);
	    $ind = 0;
	    $step = min(500, intval($total / 20) + 1);
	}
	foreach ($thumbnailBuild as $thumbnailId) {
	    list ($ret) = GalleryCoreApi::rebuildDerivativeCacheIfNotCurrent($thumbnailId);
	    if ($ret) {
		return $ret;
	    }
	    if (!(++$ind % $step) || $ind == $total) {
		$templateAdapter->updateProgressBar($heading, $message, $ind / $total);
	    }
	    if (!($ind % $batchSize)) {
		$ret = $storage->checkPoint();
		if ($ret) {
		    return $ret;
		}
	    }
	}

	if (!isset($form['recreateResizes'])) {
	    /* Don't show "Complete" yet if there is another task */
	    $redirect = array('view' => 'core.ItemAdmin', 'subView' => 'core.ItemEdit',
			      'itemId' => $item->getId());

	    $urlGenerator =& $gallery->getUrlGenerator();
	    $templateAdapter->completeProgressBar($urlGenerator->generateUrl($redirect));
	}
	return null;
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	return (GalleryUtilities::isA($item, 'GalleryAlbumItem'));
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Album'));
    }
}
?>
