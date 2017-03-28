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
 * @package GalleryCore
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class CoreCallbacks {
    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;
	$block =& $smarty->_tpl_vars['block'];
	$theme =& $smarty->_tpl_vars['theme'];

	switch($callback) {
	case 'LoadLanguageSelector':
	    $languageList = array();
	    $supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	    foreach ($supportedLanguages as $language => $countryList) {
		foreach ($countryList as $country => $languageData) {
		    $languageList[$language . '_' . $country] = $languageData['description'];
		}
	    }

	    list ($ret, $language) = $gallery->getActiveLanguageCode();
	    if ($ret) {
		return $ret;
	    }

	    $block['core']['LanguageSelector'] =
		array('list' => $languageList, 'language' => $language);
	    return null;

	case 'ShouldShowEmergencyEditItemLink':
	    $check = isset($params['checkBlocks']) ? $params['checkBlocks'] : '';
	    /**
	     * @todo Remove support for check[Sidebar|Album|Photo]Blocks on next major api bump
	     */
	    foreach (array('checkSidebarBlocks' => 'sidebar',
			   'checkAlbumBlocks' => 'album',
			   'checkPhotoBlocks' => 'photo') as $oldParam => $key) {
		if (!empty($params[$oldParam])) {
		    $check .= empty($check) ? $key : ",$key";
		}
	    }

	    $block['core']['ShouldShowEmergencyEditItemLink'] = false;
	    if (isset($params['permissions']['core_edit'])) {
		$block['core']['ShouldShowEmergencyEditItemLink'] = true;
		foreach (explode(',', $check) as $key) {
		    foreach ($theme['params'][$key . 'Blocks'] as $entry) {
			if ($entry[0] == 'core.ItemLinks') {
			    $block['core']['ShouldShowEmergencyEditItemLink'] = false;
			    break 2;
			}
		    }
		}
	    }
	    return null;

	case 'LoadPeers':
	    $item = $params['item'];
	    if (isset($item['parent']->getChildrenFunction)) {
		$parent = $item['parent'];
		list ($ret, $peerIds) = call_user_func($parent->getChildrenFunction, $userId);
		if ($ret) {
		    return $ret;
		}
	    } else if ($item['parentId'] > 0) {
		list ($ret, $canViewParent) =
		    GalleryCoreApi::hasItemPermission($item['parentId'], 'core.view', $userId);
		if ($ret) {
		    return $ret;
		}
		if ($canViewParent) {
		    list ($ret, $parent) =
			GalleryCoreApi::loadEntitiesById($item['parentId'], 'GalleryItem');
		    if ($ret) {
			return $ret;
		    }
		    list ($ret, $peerIds) =
			GalleryCoreApi::fetchChildItemIds($parent, null, null, $userId);
		    if ($ret) {
			return $ret;
		    }
		}
	    }
	    if (!empty($peerIds)) {
		foreach ($peerIds as $i => $id) {
		    if ($id == $item['id']) {
			$peerItemIndex = $i;
			break;
		    }
		}
	    }
	    if (isset($peerItemIndex)) {
		$windowSize = isset($params['windowSize']) ? ($params['windowSize'] - 1) : 6;
		$addEnds = isset($params['addEnds']) ? $params['addEnds'] : true;
		$peerLast = count($peerIds) - 1;
		$peerNeighborStart = max($peerItemIndex - (int)($windowSize/2), 0);
		$peerNeighborEnd = min($peerItemIndex + (int)(($windowSize+1)/2), $peerLast);
		/* If the window is pinned to one end, expand it to the entire windowSize */
		if ($peerNeighborStart == 0) {
		    $peerNeighborEnd = min($peerLast, $windowSize);
		} else if ($peerNeighborEnd == $peerLast) {
		    $peerNeighborStart = max($peerLast - $windowSize, 0);
		}
		if ($peerNeighborStart > 0 && $addEnds) {
		    $peerMap[0] = $peerIds[0];
		}
		for ($i = $peerNeighborStart; $i <= $peerNeighborEnd; $i++) {
		    $peerMap[$i] = $peerIds[$i];
		}
		if ($peerNeighborEnd < $peerLast && $addEnds) {
		    $peerMap[$peerLast] = $peerIds[$peerLast];
		}

		list ($ret, $peerItems) = GalleryCoreApi::loadEntitiesById($peerMap, 'GalleryItem');
		if ($ret) {
		    return $ret;
		}
		if (!empty($params['loadThumbnails'])) {
		    list ($ret, $thumbTable) = GalleryCoreApi::fetchThumbnailsByItemIds($peerMap);
		    if ($ret) {
			return $ret;
		    }
		}
		$j = 0;
		foreach ($peerMap as $i => $id) {
		    $peer = (array)$peerItems[$j++];
		    $peer['peerIndex'] = $i + 1;
		    if (isset($thumbTable[$id])) {
			$peer['thumbnail'] = (array)$thumbTable[$id];
		    }
		    $peers[] = $peer;
		}
		$block['core']['LoadPeers'] = array('peers' => $peers,
		    'peerCount' => count($peerIds), 'thisPeerIndex' => $peerItemIndex + 1,
		    'parent' => (array)$parent);
	    } else {
		$block['core']['LoadPeers'] =
		    array('peers' => array(), 'peerCount' => 0);
	    }
	    return null;

	case 'LoadValidationPlugins':
	    if (!empty($params['level'])) {
		$options['level'] = $params['level'];
	    } else {
		list ($ret, $options['level']) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'validation.level');
		if ($ret) {
		    return $ret;
		}
	    }
	    if ($options['level'] == 'MEDIUM' && !empty($params['key'])) {
		$options['key'] = $params['key'];
	    }
	    if ($options['level'] == 'OFF') {
		$allPluginIds = array();
	    } else {
		list ($ret, $allPluginIds) =
		    GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
		if ($ret) {
		    return $ret;
		}
	    }

	    /* Let each plugin load its template data */
	    $block['core']['ValidationPlugins'] = array();
	    foreach (array_keys($allPluginIds) as $pluginId) {
		list ($ret, $plugin) =
		    GalleryCoreApi::newFactoryInstanceById('GalleryValidationPlugin', $pluginId);
		if ($ret) {
		    return $ret;
		}

		list ($ret, $data['file'], $data['l10Domain']) =
		    $plugin->loadTemplate($smarty->_tpl_vars['form'], $options);
		if ($ret) {
		    return $ret;
		}
		if (isset($data['file'])) {
		    $block['core']['ValidationPlugins'][] = $data;
		}
	    }
	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
