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
 * This edit plugin allows you to customize the watermark for an image or album
 * @package Watermark
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemEditWatermark extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	global $gallery;
	$status = null;
	$error = array();

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'watermark');
	if ($ret) {
	    return array($ret, null, null, null);
	}
	if ($params['forceDefaultWatermark'] && !empty($params['defaultWatermarkId'])) {
	    $ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	}
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'watermark');
	if ($ret) {
	    return array($ret, null, null, null);
	}

	if (isset($form['action']['save'])) {
	    if (!(isset($form['whichDerivative']['thumbnail']) ||
		  isset($form['whichDerivative']['resize']) ||
		  isset($form['whichDerivative']['preferred']))) {
		$error[] = 'form[error][versions][missing]';
	    }

	    if (empty($error)) {
		list ($ret, $watermark) =
		    GalleryCoreApi::loadEntitiesById($form['watermarkId'], 'WatermarkImage');
		if ($ret) {
		    return array($ret, null, null, null);
		}

		$which = array(DERIVATIVE_TYPE_IMAGE_PREFERRED =>
				!empty($form['whichDerivative']['preferred']),
			       DERIVATIVE_TYPE_IMAGE_RESIZE =>
				!empty($form['whichDerivative']['resize']),
			       DERIVATIVE_TYPE_IMAGE_THUMBNAIL =>
				!empty($form['whichDerivative']['thumbnail']));

		GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');
		if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		    $templateAdapter =& $gallery->getTemplateAdapter();
		    $templateAdapter->registerTrailerCallback(array($this, 'bulkOperation'),
			array($item, 'apply',
			      array($watermark, null, $form['xPercent'], $form['yPercent'], $which),
			      !empty($form['recursive']), $gallery->i18n('Apply watermark')));
		    $requiresProgressBar = true;
		} else {
		    $ret = WatermarkHelper::watermarkItem(
			    $watermark, $item, $form['xPercent'], $form['yPercent'], $which);
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}
		$status = $module->translate('Watermark applied successfully');
	    }
	} else if (isset($form['action']['remove'])) {
	    if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
		$templateAdapter =& $gallery->getTemplateAdapter();
		$templateAdapter->registerTrailerCallback(array($this, 'bulkOperation'),
		    array($item, 'remove', array(), !empty($form['recursive']),
			  $gallery->i18n('Remove watermarks')));
		$requiresProgressBar = true;
	    } else {
		$ret = $this->removeWatermark($item->getId());
		if ($ret) {
		    return array($ret, null, null, null);
		}
	    }
	    $status = $module->translate('Watermark removed successfully');
	}

	return array(null, $error, $status, isset($requiresProgressBar));
    }

    function bulkOperation($item, $operation, $params, $recursive, $message) {
	global $gallery;
	$storage =& $gallery->getStorage();
	$urlGenerator =& $gallery->getUrlGenerator();
	$redirect = $urlGenerator->generateUrl(array('view' => 'core.ItemAdmin',
		    'subView' => 'core.ItemEdit', 'itemId' => $item->getId()));

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'watermark');
	if ($ret) {
	    return $ret;
	}

	$message = $module->translate($message);
	$templateAdapter =& $gallery->getTemplateAdapter();
	$templateAdapter->updateProgressBar($message, $module->translate('Preparing...'), 0);

	if ($recursive) {
	    list ($ret, $itemIds) =
		GalleryCoreApi::fetchDescendentItemIds($item, null, null, 'core.edit');
	    if ($ret) {
		return $ret;
	    }
	} else {
	    list ($ret, $itemIds) =
		GalleryCoreApi::fetchChildItemIdsWithPermission($item->getId(), 'core.edit');
	    if ($ret) {
		return $ret;
	    }
	}

	$i = 0;
	$total = count($itemIds);
	while (!empty($itemIds)) {
	    $batchIds = array_splice($itemIds, 0, 100);
	    list ($ret, $items) = GalleryCoreApi::loadEntitiesById($batchIds, 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }
	    foreach ($items as $item) {
		if (!($i++ % 5) || $i == $total) {
		    $templateAdapter->updateProgressBar($message, '', $i / $total);
		}
		if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
		    if ($operation == 'remove') {
			$ret = $this->removeWatermark($item->getId());
			if ($ret) {
			    return $ret;
			}
		    } else if ($this->isSupported($item, null)) {
			$params[1] = $item;
			$ret = call_user_func_array(
				array('WatermarkHelper', 'watermarkItem'), $params);
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

	$templateAdapter->completeProgressBar($redirect);
	return null;
    }

    function removeWatermark($itemId) {
	list ($ret, $derivativeTable) = GalleryCoreApi::fetchDerivativesByItemIds(array($itemId));
	if ($ret) {
	    return $ret;
	}
	if (empty($derivativeTable[$itemId])) {
	    return null;
	}
	$derivatives = $derivativeTable[$itemId];

	$ids = array();
	foreach ($derivatives as $derivative) {
	    $ids[] = $derivative->getId();
	}

	/* Lock them all */
	list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($ids);
	if ($ret) {
	    return $ret;
	}

	foreach ($derivatives as $derivative) {
	    list ($ret, $derivative) = $derivative->refresh();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }

	    $newPostFilters = GalleryCoreApi::removeDerivativeOperation(
		    'composite', $derivative->getPostFilterOperations());
	    $derivative->setPostFilterOperations($newPostFilters);

	    if ($derivative->getDerivativeType() == DERIVATIVE_TYPE_IMAGE_PREFERRED) {
		$preferred = $derivative;
	    }

	    $ret = $derivative->save();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	}

	$ret = GalleryCoreApi::releaseLocks($lockId);
	if ($ret) {
	    return $ret;
	}

	if (isset($preferred)) {
	    list ($ret, $preferred) = $preferred->refresh();
	    if ($ret) {
		return $ret;
	    }

	    if ($preferred->hasNoOperations()) {
		$ret = GalleryCoreApi::remapSourceIds(
			$preferred->getId(), $preferred->getDerivativeSourceId());
		if ($ret) {
		    return $ret;
		}

		$ret = GalleryCoreApi::deleteEntityById($preferred->getId(), 'GalleryDerivative');
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	/* Load up all watermarks */
	GalleryCoreApi::requireOnce('modules/watermark/classes/WatermarkHelper.class');
	list ($ret, $watermarks) = WatermarkHelper::fetchWatermarks();
	if ($ret) {
	    return array($ret, null, null);
	}
	foreach (array_keys($watermarks) as $watermarkId) {
	    $watermarks[$watermarkId] = (array)$watermarks[$watermarkId];
	}

	/* Grab the smallest derivative that's greater than 400px on a side */
	$itemId = $item->getId();
	if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
	    $displayImage = $item;
	    list ($ret, $derivatives) = GalleryCoreApi::fetchDerivativesByItemIds(array($itemId));
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $derivatives = isset($derivatives[$itemId]) ? $derivatives[$itemId] : array();
	    foreach ($derivatives as $derivative) {
		if ($derivative->getWidth() >= 400 || $derivative->getHeight() >= 400) {
		    if (!isset($smallestSize) || $derivative->getDerivativeSize() < $smallestSize) {
			$displayImage = $derivative;
			$smallestSize = $derivative->getDerivativeSize();
		    }
		}
	    }
	}

	if ($form['formName'] != 'ItemEditWatermark') {
	    /* First time around, reset the form */
	    $form['formName'] = 'ItemEditWatermark';

	    if (!empty($watermarks) && GalleryUtilities::isA($item, 'GalleryDataItem')) {
		/* Get the current watermark from any of the derivatives */
		foreach ($derivatives as $derivative) {
		    $ops = $derivative->getPostFilterOperations();
		    if (preg_match('{composite\|plugins_data/modules/watermark/([^;]*)}',
				   $ops, $matches)) {
			switch ($derivative->getDerivativeType()) {
			case DERIVATIVE_TYPE_IMAGE_PREFERRED:
			    $form['whichDerivative']['preferred'] = 1;
			    break;

			case DERIVATIVE_TYPE_IMAGE_RESIZE:
			    $form['whichDerivative']['resize'] = 1;
			    break;

			case DERIVATIVE_TYPE_IMAGE_THUMBNAIL:
			    $form['whichDerivative']['thumbnail'] = 1;
			    break;
			}

			$args = preg_split('/,/', $matches[1]);
			$fileName = $args[0];
			$form['xPercent'] = $args[5];
			$form['yPercent'] = $args[6];
			foreach ($watermarks as $watermark) {
			    if ($watermark['fileName'] == $fileName) {
				$form['watermarkId'] = $watermark['id'];
				break;
			    }
			}
		    }
		}
	    }

	    if (!empty($watermarks) && !isset($form['watermarkId'])) {
		$watermarkIds = array_keys($watermarks);
		$form['watermarkId'] = $watermarkIds[0];
		$form['xPercent'] = $watermarks[$watermarkIds[0]]['xPercentage'];
		$form['yPercent'] = $watermarks[$watermarkIds[0]]['yPercentage'];
	    }
	}

	$ItemEditWatermark = array('watermarks' => $watermarks,
				   'item' => (array)$item,
				   'image' => isset($displayImage) ? (array)$displayImage : null,
				   'isAlbum' => GalleryUtilities::isA($item, 'GalleryAlbumItem'));

	$template->javascript('lib/yui/utilities.js');
	$template->javascript('modules/watermark/watermark.js');

	$template->setVariable('ItemEditWatermark', $ItemEditWatermark);
	return array(null,
		     'modules/watermark/templates/ItemEditWatermark.tpl', 'modules_watermark');
    }

    /**
     * @see ItemEditPlugin::isSupported
     * @todo Broaden this interface to allow a GalleryStatus return code
     */
    function isSupported($item, $thumbnail) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'watermark');
	if ($ret) {
	    return false;
	}
	if ($params['forceDefaultWatermark'] && !empty($params['defaultWatermarkId'])) {
	    /* Only Site Admins can edit when a single watermark is enforced */
	    list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	    if ($ret || !$isAdmin) {
		return false;
	    }
	}

	if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    $mimeType = 'image/jpeg'; /* Just check common type to show tab for albums */
	} else if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
	    $mimeType = $item->getMimeType();
	} else {
	    return false;
	}

	list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation($mimeType, 'composite');
	if ($ret) {
	    return false; /* Swallow the error for now */
	}
	if (isset($toolkit)) {
	    return true;
	}

	/* Ok we have no composite for the item .. but maybe we do for its preferred? */
	list ($ret, $preferredTable) =
	    GalleryCoreApi::fetchPreferredsByItemIds(array($item->getId()));
	if ($ret) {
	    return false; /* Swallow the error for now */
	}

	if (!empty($preferredTable[$item->getId()])) {
	    $preferred = $preferredTable[$item->getId()];
	    list ($ret, $toolkit) =
		GalleryCoreApi::getToolkitByOperation($preferred->getMimeType(), 'composite');
	    if ($ret) {
		return false; /* Swallow the error for now */
	    }
	    if (isset($toolkit)) {
		return true;
	    }
	}

	return false;
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'watermark');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Watermark'));
    }
}
?>
