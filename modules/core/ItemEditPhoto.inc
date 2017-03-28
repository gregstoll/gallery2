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
 * This plugin will handle the editing of a photo
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemEditPhoto extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	global $gallery;

	$status = null;
	$error = array();

	if (isset($form['action']['save'])) {

	    /* Validate the input data */
	    for ($i = 0; $i < count($form['resizes']); $i++) {
		if (empty($form['resizes'][$i]['active'])) {
		    continue;
		}

		if (empty($form['resizes'][$i]['width'])
			|| empty($form['resizes'][$i]['height'])) {
		    $error[] = sprintf('form[error][resizes][%d][size][missing]', $i);
		} else if (!($tmp = rtrim($form['resizes'][$i]['width'], '%'))
			|| !is_numeric($tmp) || $tmp <= 0
			|| !($tmp = rtrim($form['resizes'][$i]['height'], '%'))
			|| !is_numeric($tmp) || $tmp <= 0) {
		    $error[] = sprintf('form[error][resizes][%d][size][invalid]', $i);
		}
	    }

	    if (empty($error)) {
		/* Load existing resizes */
		$itemId = $item->getId();
		list ($ret, $resizes) = GalleryCoreApi::fetchResizesByItemIds(array($itemId));
		if ($ret) {
		    return array($ret, null, null, null);
		}
		$resizes = isset($resizes[$itemId]) ? $resizes[$itemId] : array();
		$resizesTable = array();
		foreach ($resizes as $resize) {
		    $postFilter = $resize->getPostFilterOperations();
		    $resizesTable[$resize->getDerivativeOperations()] = $resize;
		}

		list ($ret, $source) = GalleryCoreApi::fetchPreferredSource($item);
		if ($ret) {
		    return array($ret, null, null, null);
		}

		/* Make sure that we have a toolkit before adding back the resizes */
		list ($ret, $toolkit, $outputMimeType) =
		    GalleryCoreApi::getToolkitByOperation($source->getMimeType(), 'scale');
		if ($ret) {
		    return array($ret, null, null, null);
		}

		if (isset($toolkit)) {
		    /* Determine operations and check against existing resizes */
		    $count = count($form['resizes']);
		    for ($i = 0; $i < $count; $i++) {
			if (empty($form['resizes'][$i]['active'])) {
			    unset($form['resizes'][$i]);
			    continue;
			}

			$operations = 'scale|' . $form['resizes'][$i]['width'] . ','
			    . $form['resizes'][$i]['height'];

			list ($ret, $newOperations, $newOutputMimeType) =
			    GalleryCoreApi::makeSupportedViewableOperationSequence(
				    $outputMimeType, $operations);
			if ($ret) {
			    return array($ret, null, null, null);
			}
			if (!$newOperations || !$newOutputMimeType) {
			    $newOperations = $operations;
			    $newOutputMimeType = $outputMimeType;
			}

			if (isset($resizesTable[$newOperations])) {
			    /* Keep existing resize */
			    unset($resizesTable[$newOperations]);
			    unset($form['resizes'][$i]);
			} else {
			    /* Create resize with these settings on next pass */
			    $form['resizes'][$i]['operations'] = $newOperations;
			    $form['resizes'][$i]['mimeType'] = $newOutputMimeType;
			}
		    }
		    /* Add new resizes, using existing derivative entities until we run out */
		    foreach ($form['resizes'] as $resizeData) {
			if (!empty($resizesTable)) {
			    $derivative = array_shift($resizesTable);
			    list ($ret, $lockId) =
				GalleryCoreApi::acquireWriteLock($derivative->getId());
			    if ($ret) {
				return array($ret, null, null, null);
			    }
			} else {
			    list ($ret, $derivative) = GalleryCoreApi::newFactoryInstanceByHint(
				    'GalleryDerivative', $source->getEntityType());
			    if ($ret) {
				return array($ret, null, null, null);
			    }
			    if (!isset($derivative)) {
				return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT),
					     null, null, null);
			    }

			    $ret = $derivative->create($itemId, DERIVATIVE_TYPE_IMAGE_RESIZE);
			    if ($ret) {
				return array($ret, null, null, null);
			    }
			}

			$derivative->setMimeType($resizeData['mimeType']);
			$derivative->setDerivativeOperations($resizeData['operations']);
			$derivative->setDerivativeSourceId($source->getId());
			if (isset($postFilter)) {
			    $derivative->setPostFilterOperations($postFilter);
			}

			$ret = $derivative->save();
			if ($ret) {
			    return array($ret, null, null, null);
			}
			if (isset($lockId)) {
			    $ret = GalleryCoreApi::releaseLocks($lockId);
			    if ($ret) {
				return array($ret, null, null, null);
			    }
			    $lockId = null;
			}
		    }
		    /* Remove any leftover resizes */
		    foreach ($resizesTable as $resize) {
			$ret =
			    GalleryCoreApi::deleteEntityById($resize->getId(), 'GalleryDerivative');
			if ($ret) {
			    return array($ret, null, null, null);
			}
		    }
		}

		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
		if ($ret) {
		    return array($ret, null, null, null);
		}
		$status = $module->translate('Changes saved successfully');
	    }
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	global $gallery;

	if ($form['formName'] != 'ItemEditPhoto') {
	    /* First time around, reset the form */
	    $form['formName'] = 'ItemEditPhoto';

	    /* Load the resizes */
	    list ($ret, $resizes) = GalleryCoreApi::fetchResizesByItemIds(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null);
	    }

	    if (!empty($resizes)) {
		foreach ($resizes[$item->getId()] as $resize) {
		    if (preg_match('/(?:scale|resize)\|(\d+%?)(?:,(\d+%?))?/',
				   $resize->getDerivativeOperations(), $matches)) {
			$width = $matches[1];
			$height = empty($matches[2]) ? $width : $matches[2];
			$form['resizes'][] = array('active' => 1,
						   'width' => $width, 'height' => $height);
		    }
		}
	    }
	}

	/* Tag on a few form blanks */
	$extraBlanks = 3;
	if (isset($form['resizes'])) {
	    foreach ($form['resizes'] as $resize) {
		if (!isset($resize['active'])) {
		    $extraBlanks--;
		}
	    }
	}

	while ($extraBlanks-- > 0) {
	    $form['resizes'][] = array('active' => 0, 'width' => '', 'height' => '');
	}

	/* Make sure that 'active' is set to a value */
	for ($i = 0; $i < sizeof($form['resizes']); $i++) {
	    if (!isset($form['resizes'][$i]['active'])) {
		$form['resizes'][$i]['active'] = false;
	    }
	}

	$ItemEditPhoto = array();

	/* Check to see if we have a preferred source */
	list ($ret, $results) =
	    GalleryCoreApi::fetchPreferredsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null, null);
	}

	$sourceMimeTypes = array($item->getMimeType());
	if (empty($results)) {
	    $ItemEditPhoto['editPhoto']['hasPreferredSource'] = false;
	} else {
	    $preferred = $results[$item->getId()];
	    if (preg_match("/(rotate|resize|scale)\|/", $preferred->getDerivativeOperations())) {
		$ItemEditPhoto['editPhoto']['hasPreferredSource'] = true;
	    } else {
		$ItemEditPhoto['editPhoto']['hasPreferredSource'] = false;
	    }
	    $sourceMimeTypes[] = $preferred->getMimeType();
	}

	/* Figure out what options we can provide */
	list ($ret, $ItemEditPhoto['editSizes']['can']['createResizes']) =
	    $this->_checkForOperation('resize', $sourceMimeTypes);
	if ($ret) {
	    return array($ret, null, null);
	}

	$template->setVariable('ItemEditPhoto', $ItemEditPhoto);
	$template->setVariable('controller', 'core.ItemEditPhoto');
	return array(null, 'modules/core/templates/ItemEditPhoto.tpl', 'modules_core');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	return (GalleryUtilities::isA($item, 'GalleryPhotoItem'));
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Photo'));
    }
}
?>
