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
 * @package AlbumSelect
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17739 $
 */
class AlbumSelectCallbacks {

    function callback($callbackParams, &$smarty, $callback, $userId) {
	global $gallery;
	static $idCount = '';
	switch($callback) {
	case 'LoadAlbumData':
	    $urlGenerator =& $gallery->getUrlGenerator();

	    if (isset($callbackParams['albumTree']) && $callbackParams['albumTree']) {
		/* Set unique name for javascript object */
		$albumTreeName = 'albumTree' . $idCount++;
	    }
	    list ($ret, $params) =
		GalleryCoreApi::fetchAllPluginParameters('module', 'albumselect');
	    if ($ret) {
		return $ret;
	    }

	    /* Try to load it from the cache */
	    list ($ret, $languageCode) = $gallery->getActiveLanguageCode();
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $rootAlbumId) = GalleryCoreApi::getDefaultAlbumId();
	    if ($ret) {
		return $ret;
	    }
	    /*
	     * Respect session permissions in cache-lookup. Might as well switch from
	     * userId + sessionAclIds to a aclIds as a cache-key.
	     */
	    list ($ret, $aclIdsWithSessionPermissions) =
		GalleryCoreApi::fetchAccessListIds('core.view', $userId, true);
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $aclIds) =
		GalleryCoreApi::fetchAccessListIds('core.view', $userId, false);
	    if ($ret) {
		return $ret;
	    }
	    $sessionPermissions = implode('-', array_diff($aclIdsWithSessionPermissions, $aclIds));
	    $cachePathInfo = array('type' => 'module-data',
				   'module' => 'albumselect',
				   'itemId' => $userId . '_' . $sessionPermissions
				   . '_' . $languageCode
				   . '_' . substr(md5(serialize(array($callbackParams,
								      $rootAlbumId))), 0, 5));
	    list ($treeList, $titles, $titlesForJs, $links) =
		GalleryDataCache::getFromDisk($cachePathInfo);
	    if (!isset($titles) || !isset($treeList) || !isset($titlesForJs) || !isset($links)) {
		if (isset($callbackParams['stripTitles'])) {
		    GalleryCoreApi::requireOnce('lib/smarty_plugins/modifier.markup.php');
		}
		if (isset($callbackParams['truncateTitles'])) {
		    GalleryCoreApi::requireOnce('lib/smarty_plugins/modifier.entitytruncate.php');
		}
		list ($ret, $rootAlbum) =
		    GalleryCoreApi::loadEntitiesById($rootAlbumId, 'GalleryItem');
		if ($ret) {
		    return $ret;
		}
		$titles = array('root' => preg_replace('/\r\n/', ' ', $rootAlbum->getTitle()));
		if (isset($callbackParams['stripTitles'])) {
		    $titles['root'] = smarty_modifier_markup($titles['root'], 'strip');
		}
		if (isset($callbackParams['truncateTitles'])) {
		    $titles['root'] = smarty_modifier_entitytruncate(
			$titles['root'], $callbackParams['truncateTitles']);
		}
		$treeList = array();

		if ($params['sort'] != 'album') {
		    list ($ret, $tree) =
			GalleryCoreApi::fetchAlbumTree($rootAlbumId, null, $userId);
		    if ($ret) {
			if ($ret->getErrorCode() & ERROR_PERMISSION_DENIED) {
			    $tree = null;
			} else {
			    return $ret;
			}
		    }
		    if (empty($tree)) {
			return null;
		    }

		    list ($ret, $items) = GalleryCoreApi::loadEntitiesById(
			GalleryUtilities::arrayKeysRecursive($tree), 'GalleryAlbumItem');
		    if ($ret) {
			return $ret;
		    }
		    foreach ($items as $item) {
			$title = $item->getTitle() ? $item->getTitle() : $item->getPathComponent();
			$title = preg_replace('/\r\n/', ' ', $title);
			if (isset($callbackParams['stripTitles'])) {
			    $title = smarty_modifier_markup($title, 'strip');
			}
			if (isset($callbackParams['truncateTitles'])) {
			    $title = smarty_modifier_entitytruncate(
				$title, $callbackParams['truncateTitles']);
			}
			$titles[$item->getId()] = $title;
		    }

		    $sorter = ($params['sort'] == 'title') ?
			new AlbumSelectTreeSorter($titles) : null;
		    $nodeId = 0;
		    $this->_parseTree($tree, $treeList, $sorter, $nodeId,
				      0, 0, $callbackParams, $titles);
		} else {
		    list ($ret, $canView) =
			GalleryCoreApi::hasItemPermission($rootAlbumId, 'core.view', $userId);
		    if ($ret) {
			return $ret;
		    }
		    if ($canView) {
			$ret = $this->_buildTree($rootAlbum, $treeList, $titles, $nodeId,
						 $userId, 0, 0, $callbackParams);
			if ($ret) {
			    return $ret;
			}
		    }
		    if (empty($treeList)) {
			return null;
		    }
		}

		/* Need to double escape for javascript (double-quote delimiter, backslash, ..) */
		if (!function_exists('smarty_modifier_escape')) {
		    GalleryCoreApi::requireOnce('lib/smarty/plugins/modifier.escape.php');
		}

		$titlesForJs = $links = array();
		foreach ($titles as $id => $title) {
		    $titlesForJs[$id] = smarty_modifier_escape($title, 'javascript');
		    if ($id == 'root') {
			continue;
		    }
		    $links[$id] = $urlGenerator->generateUrl(
			    array('view' => 'core.ShowItem', 'itemId' => $id),
			    array('htmlEntities' => false, 'forceServerRelativeUrl' => true,
			          'forceSessionId' => false));
		    /* Find common prefix across all URLs */
		    if (!isset($links['prefix'])) {
			$links['prefix'] = $links[$id];
			$prefixLength = strlen($links['prefix']);
		    } else {
			while ($prefixLength
				&& strncmp($links[$id], $links['prefix'], $prefixLength)) {
			    $links['prefix'] = substr($links['prefix'], 0, -1);
			    $prefixLength--;
			}
		    }
		}
		if (!empty($prefixLength)) {
		    foreach ($links as $id => $link) {
			if ($id != 'prefix') {
			    $links[$id] = substr($link, $prefixLength);
			}
		    }
		}

		GalleryDataCache::putToDisk($cachePathInfo,
					    $dataToCache =
					       array($treeList, $titles, $titlesForJs, $links));
	    }

	    $translator =& $gallery->getTranslator();
	    $params['rtl'] = $translator->isRightToLeft() ? '_rtl' : '';

	    /* Add a sessionId to the URLs if necessary */
	    $session =& $gallery->getSession();
	    if (!$session->isUsingCookies()) {
		foreach ($links as $id => $url) {
		    if ($id != 'prefix') {
			$links[$id] = GalleryUrlGenerator::appendParamsToUrl(
			    $url, array($session->getKey() => $session->getId()), true, false);
		    }
		}
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $type = isset($callbackParams['albumTree']) ? 'albumTree' : 'albumSelect';
	    $block['albumselect']['LoadAlbumData'][$type] = array(
		'tree' => $treeList,
		'titles' => $titles,
		'titlesForJs' => $titlesForJs,
		'links' => $links,
		'params' => $params);
	    if (isset($albumTreeName)) {
		$block['albumselect']['LoadAlbumData'][$type]['albumTreeName'] = $albumTreeName;
	    }
	    list ($ret, $block['albumselect']['cookiePath']) = $urlGenerator->getCookiePath();
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $block['albumselect']['cookieDomain']) = $session->getCookieDomain();
	    if ($ret) {
		return $ret;
	    }

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }

    /**
     * Build template data for subalbum tree (manual and title sorting)
     * @access private
     */
    function _parseTree($tree, &$treeList, &$sorter, &$nodeId, $parentNode, $depth,
			&$callbackParams, &$titles) {
	if (isset($sorter)) {
	    uksort($tree, array($sorter, 'sort'));
	}
	foreach ($tree as $id => $list) {
	    $treeList[] = array('id' => $id, 'nodeId' => ++$nodeId,
				'parentNode' => $parentNode, 'depth' => $depth);
	    if (isset($callbackParams['createTextTree'])) {
		$titles[$id] = str_repeat('-- ', $depth) . $titles[$id];
	    }
	    if (!empty($list)) {
		$this->_parseTree($list, $treeList, $sorter, $nodeId, $nodeId, $depth + 1,
				  $callbackParams, $titles);
	    }
	}
    }

    /**
     * Build template data for subalbum tree (apply sort preference of each album)
     * @return GalleryStatus a status code
     * @access private
     */
    function _buildTree($album, &$treeList, &$titles, &$nodeId, $userId,
			$parentNode, $depth, &$callbackParams) {
	list ($ret, $subAlbumIds) =
	    GalleryCoreApi::fetchChildAlbumItemIds($album, null, null, $userId);
	if ($ret) {
	    return $ret;
	}
	if (!empty($subAlbumIds)) {
	    list ($ret, $subAlbums) =
		GalleryCoreApi::loadEntitiesById($subAlbumIds, 'GalleryAlbumItem');
	    if ($ret) {
		return $ret;
	    }
	    foreach ($subAlbums as $subAlbum) {
		$treeList[] = array('id' => $subAlbum->getId(), 'nodeId' => ++$nodeId,
				    'parentNode' => $parentNode, 'depth' => $depth);
		$title = $subAlbum->getTitle() ? $subAlbum->getTitle()
					       : $subAlbum->getPathComponent();
		$title = preg_replace('/\r\n/', ' ', $title);
		if (isset($callbackParams['stripTitles'])) {
		    $title = smarty_modifier_markup($title, 'strip');
		}
		if (isset($callbackParams['truncateTitles'])) {
		    $title = smarty_modifier_entitytruncate($title,
							    $callbackParams['truncateTitles']);
		}
		if (isset($callbackParams['createTextTree'])) {
		    $title = str_repeat('--' , $depth) . $title;
		}
		$titles[$subAlbum->getId()] = $title;
		$ret = $this->_buildTree($subAlbum, $treeList, $titles,
					 $nodeId, $userId, $nodeId, $depth + 1,
					 $callbackParams);
		if ($ret) {
		    return $ret;
		}
	    }
	}
	return null;
    }
}

/**
 * Sort albums on a specific field
 */
class AlbumSelectTreeSorter {
    var $_titles;
    function AlbumSelectTreeSorter(&$titles) {
	$this->_titles =& $titles;
    }
    function sort($a, $b) {
	return strcasecmp($this->_titles[$a], $this->_titles[$b]);
    }
}
?>
