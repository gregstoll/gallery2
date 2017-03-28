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
 * @package Members
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class MembersCallbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;

	$block =& $smarty->_tpl_vars['block'];

	switch($callback) {
	case 'LoadMembers':
	    /* The "Members List" link */
	    GalleryCoreApi::requireOnce(
		'modules/members/classes/GalleryMembersHelper.class');
	    list ($ret, $block['members']['LoadMembers']['canViewList']) =
		GalleryMembersHelper::canViewMembersModule($gallery->getActiveUserId());
	    if ($ret) {
		return $ret;
	    }

	    /* The members count */
	    list ($ret, $block['members']['LoadMembers']['count']) =
		GalleryMembersHelper::getMembersCount();
	    if ($ret) {
		return $ret;
	    }
	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
