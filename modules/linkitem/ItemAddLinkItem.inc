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
 * This plugin handles adding new link items
 * @package LinkItem
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ItemAddLinkItem extends ItemAddPlugin {

    /**
     * @see ItemAddPlugin::handleRequest
     */
    function handleRequest($form, &$item) {
	global $gallery;

	$status = $error = array();
	if (isset($form['action']['addLinkItem'])) {
	    if ($form['linkType'] == 'album' && empty($form['linkedAlbumId'])) {
		$error[] = 'form[error][linkedAlbumId][missing]';
	    } else if ($form['linkType'] == 'url' && empty($form['linkUrl'])) {
		$error[] = 'form[error][linkUrl][missing]';
	    }

	    if (empty($error)) {
		list ($ret, $lockId) = GalleryCoreApi::acquireReadLock($item->getId());
		if ($ret) {
		    return array($ret, null, null);
		}

		list ($ret, $newItem) =
		    GalleryCoreApi::newFactoryInstance('GalleryEntity', 'GalleryLinkItem');
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}
		if (empty($newItem)) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null, null);
		}

		$link = ($form['linkType'] == 'album') ? (int)$form['linkedAlbumId']
						       : $form['linkUrl'];
		$ret = $newItem->create($item->getId(), $link);
		if ($ret) {
		    if ($ret->getErrorCode() & (ERROR_MISSING_OBJECT | ERROR_BAD_PARAMETER)) {
			$error[] = 'form[error][linkedAlbumId][invalid]';
		    } else {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }
		}

		if (empty($error)) {
		    $ret = $newItem->save();
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }

		    /* set order weight -- add to end of parent album */
		    list ($ret, $maxWeight) =
			GalleryCoreApi::fetchExtremeChildWeight($item->getId(), HIGHER_WEIGHT);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }
		    $ret = GalleryCoreApi::setItemOrderWeight($newItem->getId(), $maxWeight + 1000);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return array($ret, null, null);
		    }

		    /* add thumbnail */
		    $source = $newItem;
		    if ($form['linkType'] == 'album') {
			list ($ret, $thumbnailTable) =
			    GalleryCoreApi::fetchThumbnailsByItemIds(array($link));
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockId);
			    return array($ret, null, null);
			}
			if (!empty($thumbnailTable)) {
			    $source = array_shift($thumbnailTable);
			}
		    }
		    list ($ret, $derivative) = GalleryCoreApi::newFactoryInstanceByHint(
			'GalleryDerivative', $source->getEntityType());
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null, null);
		    }
		    if (!isset($derivative)) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null, null);
		    }
		    $ret = $derivative->create($newItem->getId(), DERIVATIVE_TYPE_IMAGE_THUMBNAIL);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null, null);
		    }
		    list ($ret, $preferences) =
			GalleryCoreApi::fetchDerivativePreferencesForItem($item->getId());
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null, null);
		    }
		    foreach ($preferences as $preference) {
			if ($preference['derivativeType'] == DERIVATIVE_TYPE_IMAGE_THUMBNAIL) {
			    $derivativeOperations = $preference['derivativeOperations'];
			    break;
			}
		    }
		    if (empty($derivativeOperations)) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array(GalleryCoreApi::error(ERROR_MISSING_VALUE), null, null);
		    }
		    list ($ret, $derivativeOperations, $outputMimeType) =
			GalleryCoreApi::makeSupportedViewableOperationSequence(
			    $source->getMimeType(), $derivativeOperations);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockIds);
			return array($ret, null, null);
		    }
		    if (!empty($derivativeOperations)) {
			$derivative->setDerivativeOperations($derivativeOperations);
			$derivative->setMimeType($outputMimeType);

			list ($ret, $isSupported) = GalleryCoreApi::getPluginParameter(
				'module', 'linkitem', 'arrowWatermark');
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null, null);
			}
			if ($isSupported) {
			    $watermarkOp = 'composite|plugins_data/modules/linkitem/arrow.png,'
					 . 'image/png,14,14,bottom-right,0,0';
			    list ($ret, $isSupported, $outputMimeType) =
				GalleryCoreApi::isSupportedOperationSequence(
					$outputMimeType, $watermarkOp);
			    if ($ret) {
				GalleryCoreApi::releaseLocks($lockIds);
				return array($ret, null, null);
			    }
			    if ($isSupported) {
				$derivative->setPostFilterOperations($watermarkOp);
				$derivative->setMimeType($outputMimeType);
			    }
			}

			$derivative->setDerivativeSourceId($source->getId());
			$derivative->setWidth(0);
			$derivative->setHeight(0);
			$ret = GalleryCoreApi::estimateDerivativeDimensions($derivative, $source);
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null, null);
			}
			$ret = $derivative->save();
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null, null);
			}

			/* Set as album highlight if no highlight yet */
			list ($ret, $thumbnailTable) =
			    GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
			if ($ret) {
			    GalleryCoreApi::releaseLocks($lockIds);
			    return array($ret, null, null);
			}
			if (empty($thumbnailTable)) {
			    list ($ret) = GalleryCoreApi::setThumbnailFromItem(
							  $item->getId(), $newItem->getId());
			    if ($ret) {
				GalleryCoreApi::releaseLocks($lockIds);
				return array($ret, null, null);
			    }
			}
		    }

		    $fileName = $newItem->getTitle();
		    $status['addedFiles'][] = array('fileName' => $fileName,
						    'id' => $newItem->getId(),
						    'warnings' => array());
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null, null);
		}
	    }
	}

	return array(null, $error, $status);
    }

    /**
     * @see ItemAdd::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	if ($form['formName'] != 'ItemAddLinkItem') {
	    $form['formName'] = 'ItemAddLinkItem';
	    $form['linkType'] = 'album';
	    $form['linkedAlbumId'] = $form['linkUrl'] = '';
	}

	list ($ret, $albumIds) = GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.view');
	if ($ret) {
	    return array($ret, null, null);
	}
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null, null);
	}
	$template->setVariable('ItemAddLinkItem',
	    array('albumTree' => GalleryUtilities::createAlbumTree($albums)));
	$template->javascript('lib/yui/yahoo-dom-event.js');
	$template->javascript('lib/yui/container-min.js');
	$template->javascript('lib/yui/treeview-min.js');
	$template->style('modules/core/data/tree.css');
	return array(null, 'modules/linkitem/templates/ItemAddLinkItem.tpl', 'modules_linkitem');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'linkitem');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Link'));
    }
}
?>
