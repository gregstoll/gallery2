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
 * @package WebDav
 * @subpackage UserInterface
 * @author Andy Staudacher <ast@gmx.ch>
 * @version $Revision: 17580 $
 */
class WebDavCallbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;

	switch ($callback) {
	case 'LoadMountLink':
	    GalleryCoreApi::requireOnce('modules/webdav/classes/WebDavHelper.class');

	    list ($ret, $hasPermission) =
		GalleryCoreApi::hasItemPermission($params['itemId'], 'core.view');
	    if ($ret) {
		return $ret;
	    }

	    $link = array();
	    if ($hasPermission) {
		$link = WebDavHelper::getMountLink($params['itemId']);
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['webdav']['LoadMountLink'] =
		array('link' => $link, 'hasPermission' => $hasPermission);

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
