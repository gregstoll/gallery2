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
 * @package Rss
 * @author Pierre-Luc Paour
 * @version $Revision: 17580 $
 */
class RssCallbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	static $feeds;

	GalleryCoreApi::requireOnce('modules/rss/classes/RssMapHelper.class');
	switch($callback) {
	case 'FeedList':
	    if (empty($params['number'])) {
		/* we're called during preload, so we have to find the params another way */
	    	if (!empty($smarty->_tpl_vars['theme']['params']['sidebarBlocks'])) {
		    $blocks = $smarty->_tpl_vars['theme']['params']['sidebarBlocks'];

		    foreach ($blocks as $block) {
			if ($block[0] == 'rss.RssBlock') {
			    $params = $block[1];
			    break;
			}
		    }
		}

		if (empty($params['number'])) {
		    /* still empty, probably because the user never changed the default */
		    $params['number'] = '5';
		}
	    }

	    /* since we get called twice (once for the block, once for the head), cache the feeds */
	    if (!isset($feeds)) {
		list ($ret, $feeds) = RssMapHelper::fetchFeedNames((int) $params['number']);
		if ($ret) {
		    return $ret;
		}
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['rss']['feeds'] = $feeds;
	    $block['rss']['showMore'] = !empty($feeds);

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
