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
 * This view will allow users to view a members uploaded items
 * @package Members
 * @subpackage UserInterface
 * @author Robert Balousek <rbalousek@hotmail.com>
 * @version $Revision: 17580 $
 */
class MembersItemsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	GalleryCoreApi::requireOnce('modules/members/classes/GalleryMembersHelper.class');
	list ($ret, $result) =
	    GalleryMembersHelper::canViewMembersModule($gallery->getActiveUserId());
	if ($ret) {
	    return array($ret, null);
	}
	if (!$result) {
	    return array(null, array('redirect' => array('view' => 'core.ShowItem')));
	}

	if (!isset($form['list']['page']) || $form['list']['page'] < 1) {
	    $form['list']['page'] = 1;
	}

	$userId = GalleryUtilities::getRequestVariables('userId');
	if (empty($userId)) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	if ($form['formName'] != 'MembersItems') {
	    /* Set some defaults */
	    $form['formName'] = 'MembersItems';
	}

	$MembersItems = array();

	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $form['list']['count']) =
	    GalleryMembersHelper::fetchUserItemCount($user->getId());
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['list']['count'] == 0) {
	    return array(null, array('redirect' => array('view' => 'core.ShowItem')));
	}

	$form['list']['pageSize'] = $form['list']['count'] > 25 ? 25 : $form['list']['count'];

	/* Figure out our max pages, make sure our current page fits in it */
	$form['list']['maxPages'] = ceil($form['list']['count'] / $form['list']['pageSize']);
	if ($form['list']['page'] > $form['list']['maxPages']) {
	    $form['list']['page'] = $form['list']['maxPages'];
	}

	/* Calculate the next/back pages */
	$form['list']['nextPage'] = min($form['list']['page']+1, $form['list']['maxPages']);
	$form['list']['backPage'] = max(1, $form['list']['page']-1);
	$form['list']['startingItem'] = ($form['list']['page'] - 1) * $form['list']['pageSize'];

	list ($ret, $lastItems) =
	    GalleryMembersHelper::fetchLastUserItems($userId,
						     $form['list']['startingItem'],
						     $form['list']['pageSize'],
						     ORDER_DESCENDING);
	if ($ret) {
	    return array($ret, null);
	}

	$MembersItems['user'] = (array)$user;

	$MembersItems['lastItems'] = array();
	foreach ($lastItems as $item) {
	    $MembersItems['lastItems'][] = (array)$item;
	}

	$template->setVariable('MembersItems', $MembersItems);

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'members');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Member Items'));
	return array(null, array('body' => 'modules/members/templates/MembersItems.tpl'));
    }
}
?>
