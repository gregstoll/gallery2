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
 * @package CustomField
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class CustomFieldCallbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/customfield/classes/CustomFieldHelper.class');

	switch ($callback) {
	case 'LoadCustomFields':
	    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($params['itemId'], 'GalleryItem');
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $hasPermission) =
		GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
	    if ($ret) {
		return $ret;
	    }
	    if (!$hasPermission) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return GalleryCoreApi::error(ERROR_MISSING_OBJECT);
	    }

	    list ($ret, $fields) = CustomFieldHelper::fetchFieldValues(array($item), 'detail');
	    if ($ret) {
		return $ret;
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['customfield']['LoadCustomFields']['fields'] = 
		isset($fields[$item->getId()]) ? $fields[$item->getId()] : array();

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
