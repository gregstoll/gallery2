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
 * This view will allow users to view the member list
 * @package Members
 * @subpackage UserInterface
 * @author Robert Balousek <rbalousek@hotmail.com>
 * @version $Revision: 17580 $
 */
class MembersListView extends GalleryView {

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

	if ($form['formName'] != 'MembersList') {
	    /* Set some defaults */
	    $form['text']['userName'] = '';
	    $form['text']['userId'] = '';
	    $form['formName'] = 'MembersList';
	    $MembersList = array();
	}

	if (!isset($form['list']['page']) || $form['list']['page'] < 1) {
	    $form['list']['page'] = 1;
	}

	list ($ret, $groupId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.allUserGroup');
	if ($ret) {
	    return array($ret, null);
	}

	/* Fetch the user count every time we reload */
	list ($ret, $totalUserCount) = GalleryCoreApi::fetchUserCount(null, $groupId);
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['count'] = $totalUserCount;
	$form['list']['pageSize'] = $totalUserCount > 25 ? 25 : $totalUserCount;

	/* Figure out our max pages, make sure our current page fits in it */
	$form['list']['maxPages'] = ceil($form['list']['count'] / $form['list']['pageSize']);
	if ($form['list']['page'] > $form['list']['maxPages']) {
	    $form['list']['page'] = $form['list']['maxPages'];
	}

	/* Calculate the next/back pages */
	$form['list']['nextPage'] = min($form['list']['page']+1, $form['list']['maxPages']);
	$form['list']['backPage'] = max(1, $form['list']['page']-1);
	$form['list']['startingUser'] = ($form['list']['page'] - 1) * $form['list']['pageSize'];

	list ($ret, $userNames) = GalleryCoreApi::fetchUsersForGroup($groupId,
								     $form['list']['pageSize'],
								     $form['list']['startingUser']);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $users) =
	    GalleryCoreApi::loadEntitiesById(array_keys($userNames), 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	$MembersList['users'] = array();
	foreach ($users as $user) {
	    $MembersList['users'][$user->getId()] = (array)$user;
	}

	$form['list']['totalUserCount'] = $totalUserCount;

	$template->setVariable('MembersList', $MembersList);

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'members');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Members List'));
	return array(null, array('body' => 'modules/members/templates/MembersList.tpl'));
    }
}
?>
