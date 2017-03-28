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

GalleryCoreApi::requireOnce('modules/sizelimit/classes/SizeLimitHelper.class');

/**
 * This ItemEditOption allows the user to set size limits for an album.
 * @package SizeLimit
 * @subpackage UserInterface
 * @author Felix Rabinovich <felix@rabinovich.org>
 * @version $Revision: 17580 $
 */
class SizeLimitOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	return array(null, true);
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	/*
	 * Get the parameters for this album;
	 * we are assuming that both $height and $width are either in or out...
	 */
	$SizeLimitOption = array();
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters(
	    'module', 'sizelimit', $item->getId());
	if ($ret) {
	    return array($ret, null, null);
	}
	/* if there aren't any - assume 'no limits' */
	if (!isset($params['height']) || $params['height'] == 0) {
	    $SizeLimitOption['dimensionChoice'] = 'unlimited';
	    $SizeLimitOption['height'] = '';
	    $SizeLimitOption['width'] = '';
	} else {
	    $SizeLimitOption['dimensionChoice'] = 'explicit';
	    $SizeLimitOption['height'] = $params['height'];
	    $SizeLimitOption['width'] = $params['width'];
	}

	/* Now do the same for size */
	if (!isset($params['size']) || $params['size'] == 0) {
	    $SizeLimitOption['sizeChoice'] = 'unlimited';
	    $SizeLimitOption['filesize'] = '';
	} else {
	    $SizeLimitOption['sizeChoice'] = 'explicit';
	    $SizeLimitOption['filesize'] = $params['size'];
	}

	$SizeLimitOption['keepOriginal'] =
	    (isset($params['keepOriginal']) && $params['keepOriginal'] == 1);

	$template->setVariable('SizeLimitOption', $SizeLimitOption);
	return array(null,
		     'modules/sizelimit/templates/SizeLimitOption.tpl',
		     'modules_sizelimit');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	global $gallery;

	$error = $warning = array();
	$setOriginal = false;
	if (empty($error) && isset($form['changeInDescendents']['sizelimit'])) {
	    /* TODO: We only need subalbums Items that we can administer */
	    list ($ret, $subAlbumIds) = GalleryCoreApi::fetchDescendentAlbumItemIds($item);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    array_unshift($subAlbumIds, $item->getId());
	} else {
	    $subAlbumIds = array($item->getId());
	}

	foreach ($subAlbumIds as $id) {
	    switch ($form['SizeLimitOption']['dimensionChoice']) {
	    case 'unlimited':
		/* delete parameter from plugin table */
		$ret = GalleryCoreApi::removePluginParameter(
			'module', 'sizelimit', 'height', $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		$ret = GalleryCoreApi::removePluginParameter(
			'module', 'sizelimit', 'width', $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		$ret = GalleryCoreApi::removePluginParameter(
			'module', 'sizelimit', 'keepOriginal', $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		break;

	    case 'explicit':
		/* figure out x and y and set them... */
		$width = $form['SizeLimitOption']['dimensions']['width'];
		$height = $form['SizeLimitOption']['dimensions']['height'];
		/* Validate the input data. */
		if (!is_numeric($height) && !is_numeric($width)) {
		    $error[] = 'form[error][SizeLimitOption][dimensions][missing]';
		    break;
		}
		if (!is_numeric($height)) {
		    $height = $form['SizeLimitOption']['dimensions']['height'] = $width;
		}
		if (!is_numeric($width)) {
		    $width = $form['SizeLimitOption']['dimensions']['width'] = $height;;
		}
		$ret = GalleryCoreApi::setPluginParameter(
			'module', 'sizelimit', 'height', $height, $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		$ret = GalleryCoreApi::setPluginParameter(
			'module', 'sizelimit', 'width', $width, $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		$setOriginal = true;
		break;
	    }

	    switch ($form['SizeLimitOption']['sizeChoice']) {
	    case 'unlimited':
		/* delete parameter from plugin table */
		$ret = GalleryCoreApi::removePluginParameter('module', 'sizelimit',
			'size', $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		$ret = GalleryCoreApi::removePluginParameter('module', 'sizelimit',
			'keepOriginal', $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		break;

	    case 'explicit':
		/* Validate the input data. */
		if (!is_numeric($form['SizeLimitOption']['filesize']) ||
			$form['SizeLimitOption']['filesize'] < 1) {
		    $error[] = 'form[error][SizeLimitOption][filesize][invalid]';
		    break;
		}
		/* figure out x and y and set them... */
		$filesize = $form['SizeLimitOption']['filesize'];
		$ret = GalleryCoreApi::setPluginParameter('module', 'sizelimit', 'size',
			$filesize, $id);
		if ($ret) {
		    return array($ret, null, null);
		}
		$setOriginal = true;
		break;
	    }

	    if ($setOriginal) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'sizelimit', 'keepOriginal',
			isset($form['SizeLimitOption']['keepOriginal']) ? 1 : 0, $id);
		if ($ret) {
		    return array($ret, null, null);
		}
	    }
	}
	if (empty($error) && isset($form['SizeLimitOption']['applyToDescendents'])) {
	    list ($ret, $error, $warning) = $this->_applyToDescendents($form, $item);
	    if ($ret) {
		return array($ret, null, null);
	    }

	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'sizelimit');
	    if ($ret) {
		return array($ret, null, null);
	    }

	    $templateAdapter =& $gallery->getTemplateAdapter();
	    $templateAdapter->updateProgressBar($module->translate('Resize complete'), '', 1);
	}
	return array(null, $error, $warning);
    }

    /**
     * Apply current settings to all qualified subitems
     *
     * @param array $form of form variables
     * @param GalleryAlbumItem $album to apply settings
     * @return array GalleryStatus a status code
     *               array errors, array warnings
     * @access private
     */
    function _applyToDescendents($form, $album) {
	global $gallery;
	$templateAdapter =& $gallery->getTemplateAdapter();
	$errors = $warnings = array();
	if ($setDimensions = ($form['SizeLimitOption']['dimensionChoice'] == 'explicit')) {
	    $dimensions = array($form['SizeLimitOption']['dimensions']['width'],
				$form['SizeLimitOption']['dimensions']['height']);
	}
	if ($setFilesize = ($form['SizeLimitOption']['sizeChoice'] == 'explicit')) {
	    $filesize = array($form['SizeLimitOption']['filesize']);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'sizelimit');
	if ($ret) {
	    return array($ret, null, null);
	}

	$preserveOriginal = isset($form['SizeLimitOption']['keepOriginal']) &&
	    $form['SizeLimitOption']['keepOriginal'];
	/*
	 * NOTE: we only need Descendent Data Items, so in the future we may
	 *       want to create a separate function. But for now let's just
	 *       eliminate everything else
	 */
	list ($ret, $descendentIds) =
	    GalleryCoreApi::fetchDescendentItemIds($album, null, null, 'core.edit');
	if ($ret) {
	    return array($ret, null, null);
	}

	$templateAdapter->updateProgressBar($module->translate('Resize Status'), '', 0);
	$totalItems = count($descendentIds);

	/*
	 * Process these descendents in chunks since we may have thousands of
	 * items and we don't want to give the database a heart attack.
	 */
	$chunkSize = 200;
	$numberOfItemsResized = 0;
	while (!empty($descendentIds)) {
	    $chunk = array_splice($descendentIds, 0, $chunkSize);
	    $gallery->guaranteeTimeLimit(60);

	    list ($ret, $descendents) = GalleryCoreApi::loadEntitiesById($chunk, 'GalleryItem');
	    if ($ret) {
		return array($ret, null, null);
	    }
	    foreach ($descendents as $item) {
		$numberOfItemsResized++;
		$templateAdapter->updateProgressBar(
		    $module->translate('Resizing photo'), $item->getTitle(),
		    $numberOfItemsResized / $totalItems);
		if (!GalleryUtilities::isExactlyA($item, 'GalleryPhotoItem')) {
		    continue;
		}

		if ($setDimensions && ($item->getWidth() > $dimensions[0] ||
				       $item->getHeight() > $dimensions[1])) {
		    if (!$preserveOriginal) {
			$ret = SizeLimitHelper::applyLimits($item, 'scale', $dimensions);
		    } else {
			$ret = SizeLimitHelper::buildDerivativeWithLimits(
						$item, 'scale', $dimensions);
		    }
		    if ($ret) {
			if ($ret->getErrorCode() & ERROR_UNSUPPORTED_FILE_TYPE) {
			    $warnings[] = $module->translate(
				array('text' => 'WARNING: Cannot resize mime type %s',
				      'arg1' => $item->getMimeType()));
			} else {
			    return array($ret, null, null);
			}
		    }
		}

		if ($setFilesize && $filesize[0] > 0 && ($item->getSize() >> 10) > $filesize[0]) {
		    if (!$preserveOriginal) {
			$ret = SizeLimitHelper::applyLimits($item, 'compress', $filesize);
		    } else {
			$ret = SizeLimitHelper::buildDerivativeWithLimits(
						$item, 'compress', $filesize);
		    }
		    if ($ret) {
			if ($ret->getErrorCode() & ERROR_UNSUPPORTED_FILE_TYPE) {
			    $warnings[] = $module->translate(
				array('text' => 'WARNING: Cannot compress mime type %s',
				      'arg1' => $item->getMimeType()));
			} else {
			    return array($ret, null, null);
			}
		    }
		}
	    }
	}
	return array(null, $errors, $warnings);
    }

    /**
     * @see ItemEditOption::requiresProgressBar
     */
    function requiresProgressBar($form) {
	return isset($form['SizeLimitOption']['applyToDescendents']);
    }
}
?>
