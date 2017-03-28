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
 * Plugin for editing data common to all items
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemEditItem extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	global $gallery;

	$status = null;
	$error = array();
	if (isset($form['action']['undo'])) {
	    /*
	     * Take no action and we'll be redirected back to the same page which will
	     * reset the form.
	     */
	} else if (isset($form['action']['save'])) {
	    list ($ret, $rootAlbumId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    $isRootAlbum = $item->getId() == $rootAlbumId;

	    /* Validate the input data. */
	    $platform =& $gallery->getPlatform();

	    foreach (array('title', 'summary', 'keywords', 'description') as $key) {
		if (!empty($form[$key]) && is_string($form[$key])) {
		    $form[$key] = trim($form[$key]);
		} else {
		    $form[$key] = null;
		}
	    }

	    if (!$isRootAlbum) {
		if (empty($form['pathComponent']) || !is_string($form['pathComponent'])) {
		    $error[] = 'form[error][pathComponent][missing]';
		    $form['pathComponent'] = '';
		} else if (!$platform->isLegalPathComponent($form['pathComponent'])) {
		    $error[] = 'form[error][pathComponent][invalid]';
		}
	    }

	    if ($isRootAlbum) {
		if (empty($form['title'])) {
		    $error[] = 'form[error][title][missingRootTitle]';
		}
	    }

	    if (!empty($form['originationTimestampSplit'])) {
		$newOriginationTimestamp = mktime($form['originationTimestampSplit']['Time_Hour'],
						  $form['originationTimestampSplit']['Time_Minute'],
						  $form['originationTimestampSplit']['Time_Second'],
						  $form['originationTimestampSplit']['Date_Month'],
						  $form['originationTimestampSplit']['Date_Day'],
						  $form['originationTimestampSplit']['Date_Year']);
		GalleryUtilities::putRequestVariable(
		    'form[originationTimestamp]', $newOriginationTimestamp);
	    }

	    if (!empty($form['thumbnail']['size']) &&
		    (!is_numeric($form['thumbnail']['size']) || $form['thumbnail']['size'] < 0)) {
		$error[] = 'form[error][thumbnail][size][invalid]';
	    }

	    if (empty($error) && isset($form['thumbnail']['size'])) {
		/* Load the thumbnail */
		list ($ret, $thumbnail) =
		    GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
		if ($ret) {
		    return array($ret, null, null, null);
		}
		$thumbnail = empty($thumbnail) ? null : $thumbnail[$item->getId()];

		if (!isset($thumbnail) && $form['thumbnail']['size'] > 0) {
		    if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
			/* Create album highlight (with default thumbnail size) */
			list ($ret) = GalleryCoreApi::guaranteeAlbumHasThumbnail($item->getId());
			if ($ret) {
			    return array($ret, null, null, null);
			}
			list ($ret, $thumbnail) =
			    GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
			if ($ret) {
			    return array($ret, null, null, null);
			}
			if (empty($thumbnail)) {
			    $error[] = 'form[error][thumbnail][create][forAlbum]';
			    $thumbnail = null;
			} else {
			    $thumbnail = $thumbnail[$item->getId()];
			}
		    } else {
			/* Prepare new thumbnail source and toolkit operations */
			list ($ret, $source) = GalleryCoreApi::fetchPreferredSource($item);
			if ($ret) {
			    return array($ret, null, null, null);
			}
			if (isset($source)) {
			    list ($ret, $operations, $outputMimeType) =
				GalleryCoreApi::makeSupportedViewableOperationSequence(
					$source->getMimeType(),
					'thumbnail|' . $form['thumbnail']['size']);
			    if ($ret) {
				return array($ret, null, null, null);
			    }
			}
			if (!isset($operations)) {
			    $error[] = 'form[error][thumbnail][create][forItem]';
			}
		    }
		}

		if (isset($thumbnail) && $form['thumbnail']['size'] > 0) {
		    /* Change the thumbnail */
		    $thumbnail->setDerivativeOperations(
			preg_replace('/((^|;)thumbnail)\|\d+/', '$1|' .
				     $form['thumbnail']['size'],
				     $thumbnail->getDerivativeOperations()));

		    if ($thumbnail->isModified()) {
			list ($ret, $lockId) =
			    GalleryCoreApi::acquireWriteLock($thumbnail->getId());
			if ($ret) {
			    return array($ret, null, null, null);
			}
			$ret = $thumbnail->save();
			if ($ret) {
			    return array($ret, null, null, null);
			}
			$ret = GalleryCoreApi::releaseLocks(array($lockId));
			if ($ret) {
			    return array($ret, null, null, null);
			}
		    }
		} else if (isset($thumbnail) && empty($form['thumbnail']['size'])) {
		    /* Delete the thumbnail */
		    $ret = GalleryCoreApi::deleteEntityById($thumbnail->getId(),
							    'GalleryDerivative');
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		} else if ($form['thumbnail']['size'] > 0 && empty($error)) {
		    /* Create the thumbnail */
		    list ($ret, $thumbnail) =
			GalleryCoreApi::newFactoryInstanceByHint('GalleryDerivative',
								 $source->getEntityType());
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		    if (!isset($thumbnail)) {
			return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT),
				     null, null, null);
		    }

		    $ret = $thumbnail->create($item->getId(), DERIVATIVE_TYPE_IMAGE_THUMBNAIL);
		    if ($ret) {
			return array($ret, null, null, null);
		    }

		    $thumbnail->setMimeType($outputMimeType);
		    $thumbnail->setDerivativeSourceId($source->getId());
		    $thumbnail->setDerivativeOperations($operations);

		    $ret = $thumbnail->save();
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}
	    }

	    if (empty($error)) {
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($item->getId());
		if ($ret) {
		    return array($ret, null, null, null);
		}

		/*
		 * It's possible to get a collision when we rename, so try that
		 * first.  If there's no collision then make the rest of the changes.
		 */
		$originalPathComponent = $item->getPathComponent();
		if (isset($form['pathComponent']) &&
			$item->getPathComponent() != $form['pathComponent']) {
		    if ($isRootAlbum) {
			GalleryCoreApi::releaseLocks($lockId);
			return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER),
				     null, null, null);
		    } else {
			$ret = $item->rename($form['pathComponent']);
			if ($ret && !($ret->getErrorCode() & ERROR_COLLISION)) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return array($ret, null, null, null);
			}
		    }
		}

		if ($ret && $ret->getErrorCode() & ERROR_COLLISION) {
		    $error[] = 'form[error][pathComponent][collision]';
		} else {
		    list ($ret, $markup) =
			GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		    if ($markup == 'html') {
			/* Strip malicious content if html markup allowed */
			$form['title'] = GalleryUtilities::htmlSafe($form['title'], true);
			$form['summary'] = GalleryUtilities::htmlSafe($form['summary'], true);
			$form['description'] =
			    GalleryUtilities::htmlSafe($form['description'], true);
		    }

		    $item->setTitle($form['title']);
		    $item->setSummary($form['summary']);
		    $item->setKeywords($form['keywords']);
		    $item->setDescription($form['description']);
		    $item->setSerialNumber($form['serialNumber']);

		    /* Change the originationTimestamp */
		    if (!empty($newOriginationTimestamp)) {
			$item->setOriginationTimestamp($newOriginationTimestamp);
		    }

		    $ret = $item->save();
		    if ($ret) {
			if (isset($form['pathComponent']) &&
				$originalPathComponent != $form['pathComponent']) {
			    /*
			     * Move the file back and hope that this succeeds
			     * since we're already in an error state
			     */
			    $item->rename($originalPathComponent);
			}

			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null, null);
		    }

		    /* Prepare our status message */
		    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
		    if ($ret) {
			return array($ret, null, null, null);
		    }

		    $status = $module->translate('Changes saved successfully');
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null, null, null);
		}
	    }
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	if ($form['formName'] != 'ItemEditItem') {
	    /* First time around, load the form with item data */
	    $form['formName'] = 'ItemEditItem';

	    $form['title'] = $item->getTitle();
	    $form['summary'] = $item->getSummary();
	    $form['keywords'] = $item->getKeywords();
	    $form['description'] = $item->getDescription();
	    $form['pathComponent'] = $item->getPathComponent();
	    $form['originationTimestamp'] = $item->getOriginationTimestamp();

	    $form['thumbnail']['size'] = null;
	    if (!empty($thumbnail) && preg_match('/thumbnail\|(\d+)/',
						 $thumbnail->getDerivativeOperations(),
						 $matches)) {
		$form['thumbnail']['size'] = $matches[1];
	    }
	}

	foreach (array('title', 'description', 'summary', 'pathComponent') as $key) {
	    if (!empty($form[$key]) && !is_string($form[$key])) {
		$form[$key] = '';
	    }
	}

	list ($ret, $rootAlbumId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return array($ret, null, null);
	}

	$can = array('createThumbnail' => false);
	$can['changePathComponent'] = $can['editThumbnail'] = ($item->getId() != $rootAlbumId);

	if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    $can['createThumbnail'] = true;
	    if (!isset($thumbnail)) {
		list ($ret, $childIds) = GalleryCoreApi::fetchChildItemIds($item, 0, 1);
		if ($ret) {
		    return array($ret, null, null);
		}
		if (empty($childIds)) {
		    $can['editThumbnail'] = false;
		}
	    }
	} else if (isset($thumbnail) && $thumbnail->getDerivativeSourceId() != $item->getId()) {
	    list ($ret, $source) = GalleryCoreApi::loadEntitiesById(
		$thumbnail->getDerivativeSourceId(),
		array('GalleryFileSystemEntity', 'GalleryDerivative'));
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $can['createThumbnail']) = GalleryCoreApi::isSupportedOperationSequence(
		    $source->getMimeType(), $thumbnail->getDerivativeOperations());
	    if ($ret) {
		return array($ret, null, null);
	    }
	} else if (method_exists($item, 'getMimeType')) {
	    list ($ret, $operations) = GalleryCoreApi::makeSupportedViewableOperationSequence(
						       $item->getMimeType(), 'thumbnail|150');
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $can['createThumbnail'] = isset($operations);
	}

	$ItemEditItem = array('can' => $can,
		'typeName' => array_merge($item->itemTypeName(), $item->itemTypeName(false)));

	/* The originateTimestamp that we can get from our toolkits */
	list ($ret, $originationTimestamp) = GalleryCoreApi::fetchOriginationTimestamp($item);
	if ($ret) {
	    return array($ret, null, null);
	}
	if ($originationTimestamp > 0) {
	    $ItemEditItem['originationTimestamp'] = array(
		'timestamp' => $originationTimestamp,
		'Time_Hour' => $platform->strftime('%H', $originationTimestamp),
		'Time_Minute' => $platform->strftime('%M', $originationTimestamp),
		'Time_Second' => $platform->strftime('%S', $originationTimestamp),
		'Date_Day' => ltrim($platform->strftime('%d', $originationTimestamp), '0'),
		'Date_Month' => $platform->strftime('%m', $originationTimestamp),
		'Date_Year' => $platform->strftime('%Y', $originationTimestamp),
	    );
	}

	/* The template needs to differentiate some important itemTypes */
	if (GalleryUtilities::isA($item, 'GalleryPhotoItem')) {
	    $ItemEditItem['isItemPhoto'] = true;
	} else if (GalleryUtilities::isA($item, 'GalleryUnknownItem')) {
	    $ItemEditItem['isItemUnknown'] = true;
	}

	/* Render the HTML body */
	$template->setVariable('ItemEditItem', $ItemEditItem);
	$template->setVariable('controller', 'core.ItemEditItem');

	return array(null,
		     'modules/core/templates/ItemEditItem.tpl', 'modules_core');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	return (GalleryUtilities::isA($item, 'GalleryItem'));
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('General'));
    }
}
?>
