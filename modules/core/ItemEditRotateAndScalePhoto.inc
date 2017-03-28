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
 * This plugin will handle applying operations to a photo
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 20954 $
 */
class ItemEditRotateAndScalePhoto extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null, null, null);
	}

	$status = null;
	$error = array();

	$operation = null;
	/* Figure out which command we're taking */
	if (isset($form['action']['resize'])) {
	    if (empty($form['resize']['width']) || empty($form['resize']['height'])) {
		$error[] = 'form[error][resize][size][missing]';
	    } else if (!($tmp = rtrim($form['resize']['width'], '%'))
		    || !is_numeric($tmp) || $tmp < 1
		    || !($tmp = rtrim($form['resize']['height'], '%'))
		    || !is_numeric($tmp) || $tmp < 1) {
		$error[] = 'form[error][resize][size][invalid]';
	    } else {
		$operation = 'scale';
		$args = array($form['resize']['width'], $form['resize']['height']);
	    }
	} else if (isset($form['action']['rotate']['clockwise'])) {
	    $operation = 'rotate';
	    $args = array(90);
	} else if (isset($form['action']['rotate']['counterClockwise'])) {
	    $operation = 'rotate';
	    $args = array(-90);
	} else if (isset($form['action']['rotate']['flip'])) {
	    $operation = 'rotate';
	    $args = array(180);
	} else if (isset($form['action']['revertToOriginal'])) {
	    if (!empty($preferred)) {
		/*
		 * Pull out the rotate and resize operations and let our change ripple
		 * down the derivative tree, if necessary
		 */
		$remainingOperations = array();
		foreach (explode(';', $preferred->getDerivativeOperations()) as $tmpOperation) {
		    if (preg_match("/^(rotate|resize|scale)\|/", $tmpOperation)) {
			$ret = GalleryCoreApi::adjustDependentDerivatives($preferred->getId(),
									  $tmpOperation,
									  true);
			if ($ret) {
			    return array($ret, null, null, null);
			}
		    } else {
			$remainingOperations[] = $tmpOperation;
		    }
		}

		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($preferred->getId());
		if ($ret) {
		    return array($ret, null, null, null);
		}
		list ($ret, $preferred) = $preferred->refresh();
		if ($ret) {
		    return array($ret, null, null, null);
		}

		if ($remainingOperations) {
		    $preferred->setDerivativeOperations(join(';', $remainingOperations));
		} else {
		    $preferred->setDerivativeOperations(null);
		}

		$ret = $preferred->save();
		if ($ret) {
		    return array($ret, null, null, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null, null, null);
		}

		if ($preferred->hasNoOperations()) {
		    $ret = GalleryCoreApi::remapSourceIds(
			$preferred->getId(), $preferred->getDerivativeSourceId());
		    if ($ret) {
			return array($ret, null, null, null);
		    }

		    $ret =
			GalleryCoreApi::deleteEntityById($preferred->getId(), 'GalleryDerivative');
		    if ($ret) {
			return array($ret, null, null, null);
		    }
		}

		/* Prepare our status message */
		$status = $module->translate('Reverted rotate and scale changes successfully');
	    }
	}

	$preserveOriginal = isset($form['preserveOriginal']);

	if ($form['serialNumber'] != $item->getSerialNumber()) {
	    return array(GalleryCoreApi::error(ERROR_OBSOLETE_DATA), null, null, null);
	}

	/*
	 * If we have a command, then apply it to the correct object.
	 */
	if (!empty($operation)) {
	    $ret = GalleryCoreApi::applyToolkitOperation(
		$operation, $args, $preserveOriginal, $item, $preferred);
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    /* Set our status message */
	    switch ($operation) {
	    case 'rotate':
		$status = $module->translate('Rotated photo successfully');
		break;

	    case 'scale':
		$status = $module->translate('Scaled photo successfully');
		break;
	    }
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	$ItemEditRotateAndScalePhoto = array();

	list ($ret, $ItemEditRotateAndScalePhoto['isAdmin']) =
	    GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null, null);
	}

	if ($form['formName'] != 'ItemEditRotateAndScalePhoto') {
	    /* First time around, reset the form */
	    $form['resize']['width'] = $form['resize']['height'] = '';
	    $form['preserveOriginal'] = 1;
	    $form['formName'] = 'ItemEditRotateAndScalePhoto';
	}

	if ($item->isLinked()) {
	    $ItemEditRotateAndScalePhoto['editPhoto']['isLinked'] = true;
	    $ItemEditRotateAndScalePhoto['editPhoto']['isLinkedTo'] = false;
	} else {
	    list ($ret, $linkedIds) = GalleryCoreApi::fetchEntitiesLinkedTo($item->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $ItemEditRotateAndScalePhoto['editPhoto']['isLinkedTo'] = !empty($linkedIds);
	    $ItemEditRotateAndScalePhoto['editPhoto']['isLinked'] = false;
	}

	/* Check to see if we have a preferred source */
	list ($ret, $results) =
	    GalleryCoreApi::fetchPreferredsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null, null);
	}

	/* Can we allow to delete the original? */
	$mimeType = $item->getMimeType();
	list ($ret, $toolkit) =
	    GalleryCoreApi::getToolkitByOperation($mimeType, 'rotate');
	if ($ret) {
	    return array($ret, null, null);
	}
	$ItemEditRotateAndScalePhoto['editPhoto']['noToolkitSupport'] = !isset($toolkit);
	/* Get worst of rotate and resize */
	if (isset($toolkit)) {
	    list ($ret, $toolkit) =
		GalleryCoreApi::getToolkitByOperation($mimeType, 'resize');
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $ItemEditRotateAndScalePhoto['editPhoto']['noToolkitSupport'] = !isset($toolkit);
	}

	$sourceMimeTypes = array($mimeType);

	if (empty($results)) {
	    $ItemEditRotateAndScalePhoto['editPhoto']['hasPreferredSource'] = false;
	} else {
	    $preferred = $results[$item->getId()];
	    if (preg_match("/(rotate|resize|scale)\|/", $preferred->getDerivativeOperations())) {
		$ItemEditRotateAndScalePhoto['editPhoto']['hasPreferredSource'] = true;
	    } else {
		$ItemEditRotateAndScalePhoto['editPhoto']['hasPreferredSource'] = false;
	    }
	    $sourceMimeTypes[] = $preferred->getMimeType();
	}

	/* Figure out what options we can provide */
	list ($ret, $ItemEditRotateAndScalePhoto['editPhoto']['can']['rotate']) =
	    $this->_checkForOperation('rotate', $sourceMimeTypes);
	if ($ret) {
	    return array($ret, null, null);
	}

	list ($ret, $ItemEditRotateAndScalePhoto['editPhoto']['can']['resize']) =
	    $this->_checkForOperation('scale', $sourceMimeTypes);
	if ($ret) {
	    return array($ret, null, null);
	}

	$template->setVariable('ItemEditRotateAndScalePhoto', $ItemEditRotateAndScalePhoto);
	$template->setVariable('controller', 'core.ItemEditRotateAndScalePhoto');
	return array(null,
		     'modules/core/templates/ItemEditRotateAndScalePhoto.tpl', 'modules_core');
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

	return array(null, $module->translate('Modify Photo'));
    }
}
?>
