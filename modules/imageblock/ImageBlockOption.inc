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

GalleryCoreApi::requireOnce('modules/imageblock/classes/ImageBlockHelper.class');

/**
 * This option selects whether an album may be included in image blocks
 * @package ImageBlock
 * @subpackage UserInterface
 * @author Jay Rossiter <cryptographite@users.sf.net>
 * @version $Revision: 17580 $
 */
class ImageBlockOption extends ItemEditOption {

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	$error = $warning = array();
	$setDisabled = isset($form['ImageBlockOption']['setDisabled'])
	    && $form['ImageBlockOption']['setDisabled'] != 'off';
	$setRecursive = isset($form['ImageBlockOption']['setRecursive'])
	    && $form['ImageBlockOption']['setRecursive'] != 'off';

	list ($ret, $currentlyDisabled) = ImageBlockHelper::getDisabledFlag($item->getId());
	if ($ret) {
	    return array($ret, null, null);
	}

	if ($setDisabled != $currentlyDisabled) {
	    $ret = ImageBlockHelper::setDisabledFlag($item, $setRecursive,
						     $setDisabled, $setRecursive);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}
	return array(null, $error, $warning);
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	list ($ret, $form['ImageBlockOption']['setDisabled']) =
	    ImageBlockHelper::getDisabledFlag($item->getId());
	if ($ret) {
	    return array($ret, null, null);
	}

	return array(null,
		     'modules/imageblock/templates/ImageBlockOption.tpl',
		     'modules_imageblock');
    }

    /**
     * @see ItemEditOption::isSupported
     */
    function isAppropriate($item, $thumbnail) {
	return array(null, GalleryUtilities::isA($item, 'GalleryAlbumItem'));
    }

    /**
     * @see ItemEditOption::requiresProgressBar
     */
    function requiresProgressBar($form) {
	$itemId = GalleryUtilities::getRequestVariables('itemId');
	list ($ret, $currentlyDisabled) = ImageBlockHelper::getDisabledFlag($itemId);
	if ($ret) {
	    return false;
	}
	$setDisabled = isset($form['ImageBlockOption']['setDisabled'])
	    && $form['ImageBlockOption']['setDisabled'] != 'off';
	$setRecursive = isset($form['ImageBlockOption']['setRecursive'])
	    && $form['ImageBlockOption']['setRecursive'] != 'off';
	return ($setRecursive && $setDisabled != $currentlyDisabled);
    }
}
?>
