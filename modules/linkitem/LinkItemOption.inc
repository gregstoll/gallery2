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
 * ItemEditOption for editing the target of a LinkItem
 * @package LinkItem
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class LinkItemOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	return array(null,
	    GalleryUtilities::isA($item, 'GalleryLinkItem') && !is_numeric($item->getLink()));
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	if (!isset($form['LinkItemOption'])) {
	    $form['LinkItemOption']['link'] = $item->getLink();
	}

	return array(null, 'modules/linkitem/templates/LinkItemOption.tpl', 'modules_linkitem');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	$error = $warning = array();
	if (empty($form['LinkItemOption']['link'])) {
	    $error[] = 'form[LinkItemOption][error][link][missing]';
	} else {
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null);
	    }

	    list ($ret, $item) = $item->refresh();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null);
	    }

	    $item->setLink($form['LinkItemOption']['link']);
	    $ret = $item->save();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null);
	    }

	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, $error, $warning);
    }
}
?>
