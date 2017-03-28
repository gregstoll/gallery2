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
 * @package ImageBlock
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ImageBlockCallbacks {

    function callback($params, &$smarty, $callback, $userId=null) {
	GalleryCoreApi::requireOnce('modules/imageblock/classes/ImageBlockHelper.class');

	switch($callback) {
	case 'LoadImageBlock':
	    /* Pick up our overrides */
	    if (!$params['useDefaults']) {
		/*
		 * If we're overriding the defaults, then overlay our changes on top of the
		 * defaults and pass in a complete 'show' parameter.
		 */
		list ($ret, $moduleParams) =
		    GalleryCoreApi::fetchAllPluginParameters('module', 'imageblock');
		if ($ret) {
		    return $ret;
		}

		/* GalleryEmbed::getBlocks() sets 'show' as a pipe delimited list */
		if (!isset($params['show'])) {
		    /* As a theme block the 'show' elements are set as separate properties */
		    $showMap = array_flip(explode('|', $moduleParams['show']));
		    foreach (array('showHeading' => 'heading',
				   'showTitle' => 'title', 'showViews' => 'views',
				   'showOwner' => 'owner', 'showDate' => 'date')
			    as $blockVarName => $showMapName) {
			if (empty($params[$blockVarName])) {
			    unset($showMap[$showMapName]);
			} else {
			    $showMap[$showMapName] = 1;
			}
		    }

		    $params['show'] = implode('|', array_keys($showMap));
		}
	    }

	    if (isset($userId)) {
		/* Support guest-preview mode */
		$params['userId'] = $userId;
	    }

	    /*
	     * ImageBlockHelper expects a GalleryTemplate, but Smarty callbacks don't provide
	     * one, so create a temporary wrapper.
	     */
	    $template = new GalleryTemplate(dirname(__FILE__), false);
	    $template->_smarty =& $smarty;

	    /* Remove null parameters */
	    foreach (array_keys($params) as $key) {
		if (!isset($params[$key])) {
		    unset($params[$key]);
		}
	    }

	    $ret = ImageBlockHelper::loadImageBlocks($template, $params);
	    if ($ret) {
		return $ret;
	    }

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
