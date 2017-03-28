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
class ImageBlockPreloads {
    function preload(&$template, $preload, $paramSet) {
	global $gallery;

	switch($preload) {
	case 'ImageBlock':
	    list ($ret, $moduleParams) =
		GalleryCoreApi::fetchAllPluginParameters('module', 'imageblock');
	    if ($ret) {
		return $ret;
	    }
	    $frameIds = array();
	    foreach ($paramSet as $params) {
		foreach (array('itemFrame', 'albumFrame', 'photoFrame') as $key) {
		    if (isset($params[$key])) {
			$frameIds[$params[$key]] = 1;
		    } else if (isset($moduleParams[$key])) {
			$frameIds[$moduleParams[$key]] = 1;
		    }
		}
	    }

	    if (!empty($frameIds)) {
		$frameIds = array_keys($frameIds);
		list ($ret, $imageframe) =
		    GalleryCoreApi::newFactoryInstance('ImageFrameInterface_1_1');
		if ($ret) {
		    return $ret;
		}

		if (isset($imageframe)) {
		    $ret = $imageframe->init($template, $frameIds);
		    if ($ret) {
			if ($ret->getErrorCode() & ERROR_PLUGIN_VERSION_MISMATCH) {
			    /* It's there, but unusable so ignore it. */
			} else {
			    return $ret;
			}
		    }
		}
	    }

	    return null;
	}


	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
