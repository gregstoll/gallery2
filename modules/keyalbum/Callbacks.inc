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
 * @package KeyAlbum
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class KeyAlbumCallbacks {

    function callback($params, &$smarty, $callback, $userId) {

	switch ($callback) {
	case 'LoadKeywords':
	    $onlyPublic = (bool)$params['onlyPublic'];
	    $sizeLimit = (int)$params['sizeLimit'];
	    $param = 'allKeywords' . ($onlyPublic ? '.public' : '')
		. ($sizeLimit ? '.' . $sizeLimit : '');
	    $includeFrequency = (bool)$params['includeFrequency'];
	    list ($ret, $lastUpdated) =
		GalleryCoreApi::getPluginParameter('module', 'keyalbum', $param . '.time');
	    if ($ret) {
		return $ret;
	    }
	    /* Cache keyword list for one day */
	    if (time() - $lastUpdated > 3600 * 24) {
		$ret = KeyAlbumCallbacks::_findAllKeywords($param, $onlyPublic, $sizeLimit,
							   $params['maxCloudFontEnlargement'],
							   $includeFrequency);
		if ($ret) {
		    return $ret;
		}
	    }
	    list ($ret, $keywords) =
		GalleryCoreApi::getPluginParameter('module', 'keyalbum', $param);
	    if ($ret) {
		return $ret;
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['keyalbum']['keywords'] = !empty($keywords) ? unserialize($keywords) : array();

	    return null;

	case 'SplitKeywords':
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'keyalbum');
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $split) = $module->getParameter('split');
	    if ($ret) {
		return $ret;
	    }

	    $block =& $smarty->_tpl_vars['block'];
	    $block['keyalbum']['keywords'] = $module->splitKeywords($params['keywords'], $split);

	    return null;
	}

	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }

    /**
     * Get all unique keywords and store in a module parameter.
     * @param string $param save result to this plugin paramter
     * @param boolean $onlyPublic true to only get keywords from public items
     * @param int $sizeLimit max number of keywords (least common are dropped; 0 = no limit)
     * @param int $maxWeight max value for font enlargement in keyword cloud
     * @param boolean $includeFrequency false to only get distinct keywords, but not consider their
     *        frequency (cloud requires frequency)
     * @return GalleryStatus a status code
     * @static
     */
    function _findAllKeywords($param, $onlyPublic, $sizeLimit, $maxWeight, $includeFrequency) {
	global $gallery;

	$query = '
	SELECT ' . ($includeFrequency ? '' : 'DISTINCT') . '
	  [GalleryItem::keywords]
	FROM
	  [GalleryItem]
	';
	$keywords = $aclIds = array();
	if ($onlyPublic) {
	    list ($ret, $guestId) = GalleryCoreApi::getAnonymousUserId();
	    if ($ret) {
		return $ret;
	    }
	    list ($ret, $aclIds) = GalleryCoreApi::fetchAccessListIds('core.view', $guestId);
	    if ($ret) {
		return $ret;
	    }
	    if (empty($aclIds)) {
		/* Force empty results */
		$query .= ' WHERE [GalleryItem::id] IS NULL';
	    } else {
		$query .= ' INNER JOIN [GalleryAccessSubscriberMap]
		  ON [GalleryItem::id] = [GalleryAccessSubscriberMap::itemId]
		WHERE
		  [GalleryAccessSubscriberMap::accessListId] IN ('
		  . GalleryUtilities::makeMarkers(count($aclIds)) . ')
		';
	    }
	}
	list ($ret, $searchResults) = $gallery->search($query, $aclIds);
	if ($ret) {
	    return $ret;
	}
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'keyalbum');
	if ($ret) {
	    return $ret;
	}
	list ($ret, $split) = GalleryCoreApi::getPluginParameter('module', 'keyalbum', 'split');
	if ($ret) {
	    return $ret;
	}
	$maxCount = 1;
	while ($result = $searchResults->nextResult()) {
	    foreach ($module->splitKeywords($result[0], $split) as $rawKeyword => $keyword) {
		if (!isset($keywords[$keyword])) {
		    $keywords[$keyword] =
			array('name' => $keyword, 'raw' => $rawKeyword, 'count' => 1);
		} else if (++$keywords[$keyword]['count'] > $maxCount) {
		    $maxCount = $keywords[$keyword]['count'];
		}
	    }
	}
	$minCount = $maxCount;
	foreach ($keywords as $keyword) {
	    if ($keyword['count'] < $minCount) {
		$minCount = $keyword['count'];
	    }
	}

	if ($sizeLimit > 0 && count($keywords) > $sizeLimit) {
	    /* Sort by frequency and drop least common */
	    uasort($keywords, create_function('$a,$b', '$c = $a[\'count\']; $d = $b[\'count\']; '
						    . 'return $c > $d ? -1 : ($c == $d ? 0 : 1);'));

	    /* Avoid renumbering of numeric keys from array_splice */
	    for ($newKeywords = array(), reset($keywords), $i = 0; $i < $sizeLimit; $i++) {
		$entry = each($keywords);
		$newKeywords[$entry[0]] = $entry[1];
	    }
	    $keywords = $newKeywords;
	}
	ksort($keywords);
	$keywords = array_values($keywords);

	/* 
	 * Compute weights, used for font enlargement in keyword cloud.  The adjustment shifts the
	 * whole range from [x>=1, maxWeight] to [1, maxWeight - minWeight].  In case all
	 * frequencies are very close together, this makes the resulting font-size rather normal
	 * than large.
	 */
	$adjustment = pow($maxWeight, (float)$minCount / $maxCount) - 1.0;
	foreach ($keywords as $i => $keyword) {
	    /* Scale the resulting weights to be floats between 1 and $maxWeight. */
	    $keywords[$i]['weight'] = GalleryUtilities::roundToString(
		    pow($maxWeight, (float)$keyword['count'] / $maxCount) - $adjustment, 2);
	    unset($keywords[$i]['count']);
	}

	$ret = GalleryCoreApi::setPluginParameter('module', 'keyalbum',
						  $param, serialize($keywords));
	if ($ret) {
	    return $ret;
	}
	$ret = GalleryCoreApi::setPluginParameter('module', 'keyalbum', $param . '.time', time());
	if ($ret) {
	    return $ret;
	}
	return null;
    }
}
?>
