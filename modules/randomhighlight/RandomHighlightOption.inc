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
 * This ItemEditOption allows the user to activate random highlight for an album.
 * @package RandomHighlight
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class RandomHighlightOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	return array(null, isset($thumbnail));
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	$form['RandomHighlightOption']['isRandomHighlight'] =
	    $thumbnail->hasOnLoadHandler('RandomHighlight');
	return array(null, 'modules/randomhighlight/templates/RandomHighlightOption.tpl',
			   'modules_randomhighlight');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	list ($ret, $thumbnail) = GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null, null);
	}
	$thumbnail = array_shift($thumbnail);
	$isRandomHighlight = isset($form['RandomHighlightOption']['isRandomHighlight'])
			     && $form['RandomHighlightOption']['isRandomHighlight'];

	if (isset($thumbnail)
		&& $isRandomHighlight != $thumbnail->hasOnLoadHandler('RandomHighlight')) {
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($thumbnail->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }
	    list ($ret, $thumbnail) = $thumbnail->refresh();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null);
	    }
	    if ($isRandomHighlight) {
		$thumbnail->addOnLoadHandler('RandomHighlight');
	    } else {
		$thumbnail->removeOnLoadHandler('RandomHighlight');
	    }
	    $ret = $thumbnail->save();
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return array($ret, null, null);
	    }
	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	return array(null, array(), array());
    }
}
?>
