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
 * ItemEditOption for uploading a custom thumbnail for an item
 * @package Thumbnail
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class CustomThumbnailOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	return array(null, GalleryUtilities::isA($item, 'GalleryItem'));
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	GalleryCoreApi::requireOnce('modules/thumbnail/classes/ThumbnailHelper.class');

	/* Is this item already using a custom thumbnail.. */
	list ($ret, $thumbnailImage) = ThumbnailHelper::fetchThumbnail($item->getId());
	if ($ret) {
	    return array($ret, null, null);
	}
	if (isset($thumbnailImage)) {
	    $thumbnailImage = (array)$thumbnailImage;
	} else {
	    /* Set the form's encoding type since we're uploading binary files */
	    if ($template->hasVariable('ItemAdmin')) {
		$ItemAdmin =& $template->getVariableByReference('ItemAdmin');
		$ItemAdmin['enctype'] = 'multipart/form-data';
	    } else {
		$ItemAdmin = array('enctype' => 'multipart/form-data');
		$template->setVariable('ItemAdmin', $ItemAdmin);
	    }
	}

	/* Check toolkit support for jpeg thumbnails */
	list ($ret, $mimeList) = GalleryCoreApi::getToolkitOperationMimeTypes('thumbnail');
	if ($ret) {
	    return array($ret, null, null);
	}

	$template->setVariable('CustomThumbnailOption',
			       array('thumbnail' => $thumbnailImage,
				     'canResize' => isset($mimeList['image/jpeg'])));

	return array(null,
		    'modules/thumbnail/templates/CustomThumbnail.tpl',
		    'modules_thumbnail');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	GalleryCoreApi::requireOnce('modules/thumbnail/classes/ThumbnailHelper.class');

	$error = $warning = array();
	if (!empty($form['tmp_name'][1])) {
	    list ($ret, $lockId) = GalleryCoreApi::acquireReadLock(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null);
	    }

	    list ($ret, $thumbnailId) = ThumbnailHelper::addItem(
		null, $item->getId(), $form['name'][1], $form['tmp_name'][1], $form['type'][1]);
	    if ($ret && ($ret->getErrorCode() & ERROR_UNSUPPORTED_FILE_TYPE)) {
		$error[] = 'form[CustomThumbnailOption][error][imageMime]';
	    } else if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null);
	    }

	    if (empty($error)) {
		$ret = ThumbnailHelper::applyThumbnail($item, $thumbnailId);
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null, null);
		}
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	} else if (isset($form['CustomThumbnailOption']['delete'])) {
	    list ($ret, $thumbnailImage) = ThumbnailHelper::fetchThumbnail($item->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }
	    if (!isset($thumbnailImage)) {
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT),
			     null, null);
	    }
	    $ret = GalleryCoreApi::deleteEntityById($thumbnailImage->getId(), 'ThumbnailImage');
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, $error, $warning);
    }
}
?>
