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
 * ItemEditOption for hiding an item
 * @package Hidden
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @author Jess Martin <jmartin@cs.unc.edu>
 * @version $Revision: 17580 $
 */
class HiddenItemOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	list ($ret, $canHide) =
	    GalleryCoreApi::hasItemPermission($item->getId(), 'core.changePermissions');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $canHide);
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	$form['HiddenItemOption']['isHidden'] = $item->hasOnLoadHandler('Hidden');
	$form['HiddenItemOption']['isAlbum'] = $item->getCanContainChildren();

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'hidden');
	if ($ret) {
	    return array($ret, null, null);
	}

	if (false) {
	    /* Specific translations: */ _('Hide Album'); _('Hide Photo'); _('Hide Movie');
	}
	$itemTypeNames = array_merge($item->itemTypeName(), $item->itemTypeName(false));
	$form['HiddenItemOption']['heading'] = $module->_translate(
		array('text' => 'Hide %s', 'arg1' => $itemTypeNames[0]), $itemTypeNames[2]);

	return array(null, 'modules/hidden/templates/HiddenItemOption.tpl', 'modules_hidden');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	GalleryCoreApi::requireOnce('modules/hidden/classes/HiddenHelper.class');
	$error = $warning = array();
	$useProgressBar = $item->getCanContainChildren();

	$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'core.changePermissions');
	if ($ret) {
	    return array($ret, null, null);
	}
	if (!empty($form['HiddenItemOption']['setHidden'])) {
	    $ret = HiddenHelper::hideItem($item, $useProgressBar);
	    if ($ret) {
		return array($ret, null, null);
	    }
	} else {
	    $ret = HiddenHelper::unHideItem($item, $useProgressBar);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, $error, $warning);
    }

    /**
     * @see ItemEditOption::requiresProgressBar
     */
    function requiresProgressBar($form) {
	/* Progress bar if changing hidden status for an album */
	return isset($form['HiddenItemOption']['progressBar'])
	    && ($form['HiddenItemOption']['progressBar'] == 'hide')
	       == !empty($form['HiddenItemOption']['setHidden']);
    }
}
?>
