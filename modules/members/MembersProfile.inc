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
 * This view will allow users to view a members profile
 * @package Members
 * @subpackage UserInterface
 * @author Robert Balousek <rbalousek@hotmail.com>
 * @version $Revision: 17580 $
 */
class MembersProfileView extends GalleryView {

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

	$userId = GalleryUtilities::getRequestVariables('userId');
	if (empty($userId)) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	if ($form['formName'] != 'MembersProfile') {
	    /* Set some defaults */
	    $form['formName'] = 'MembersProfile';
	}

	$MembersProfile = array();

	list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}
	$MembersProfile['user'] = (array)$user;

	list ($ret, $lastItems) =
	   GalleryMembersHelper::fetchLastUserItems($user->getId(), 0, 5, ORDER_DESCENDING);
	if ($ret) {
	    return array($ret, null);
	}

	$MembersProfile['lastItems'] = array();
	foreach ($lastItems as $item) {
	    $MembersProfile['lastItems'][] = (array)$item;
	}

	list ($ret, $daysSinceCreation) = GalleryMembersHelper::daysSinceCreation($user->getId());
	if ($ret) {
	    return array($ret, null);
	}
	$MembersProfile['daysSinceCreation'] = $daysSinceCreation;

	list ($ret, $canViewProfileEmail) =
	    GalleryMembersHelper::canViewProfileEmail($gallery->getActiveUserId());
	if ($ret) {
	    return array($ret, null);
	}
	$MembersProfile['canViewProfileEmail'] = $canViewProfileEmail;

	$template->setVariable('MembersProfile', $MembersProfile);

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'members');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Members Profile'));
	return array(null, array('body' => 'modules/members/templates/MembersProfile.tpl'));
    }
}
?>
