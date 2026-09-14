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
 * Timeline 2026: a photo-stream theme.
 *
 * Presents items as a date-grouped justified grid with a persistent
 * navigation rail, in the manner of a modern photo application.
 *
 * @package Themes
 * @subpackage timeline_2026
 */
class timeline_2026Theme extends GalleryTheme {

    function __construct() {
	global $gallery;

	$this->setId('timeline_2026');
	$this->setName($gallery->i18n('Timeline 2026'));
	$this->setDescription(
	    $gallery->i18n('Date-grouped photo stream with a navigation rail, light and dark'));
	$this->setVersion('1.0.0');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredThemeApi(array(2, 4));
	$this->setStandardSettings(
	    array('rows' => 50, 'columns' => 50,
		  'showImageOwner' => 0, 'showAlbumOwner' => 0,
		  'albumFrame' => '', 'itemFrame' => '', 'photoFrame' => '',
		  'colorpack' => '', 'showMicroThumbs' => 0, 'dynamicLinks' => 'browse',
		  'rowHeight' => 220, 'groupBy' => 'month',
		  'sidebarBlocks' => serialize(array(
		      array('core.ItemLinks', array('useDropdown' => false)),
		      array('core.PeerList', array()))),
		  'albumBlocks' => serialize(array(
		      array('comment.ViewComments', array()))),
		  'photoBlocks' => serialize(array(
		      array('exif.ExifInfo', array()),
		      array('comment.ViewComments', array())))));
    }


    /**
     * Build a year index across the entire item set, not just the current page,
     * so the scrubber can be drawn complete on first paint. allChildIds is in
     * the album's sort order, so the first appearance of each year marks where
     * that year begins, which also gives the page it lives on.
     */
    function _yearIndex($theme, $perPage) {
	global $gallery;
	if (empty($theme['allChildIds']) || $perPage < 1) {
	    return array();
	}
	$ids = array_slice($theme['allChildIds'], 0, 20000);
	$in = array();
	foreach ($ids as $id) {
	    $in[] = (int)$id;
	}
	list ($ret, $results) = $gallery->search(
	    'SELECT [Item::id], [Item::originationTimestamp] FROM [Item] WHERE [Item::id] IN ('
	    . implode(',', $in) . ')');
	if ($ret) {
	    return array();
	}
	$stamp = array();
	while ($row = $results->nextResult()) {
	    $stamp[(int)$row[0]] = (int)$row[1];
	}
	$marks = $seen = array();
	foreach ($ids as $i => $id) {
	    if (empty($stamp[$id])) {
		continue;
	    }
	    $year = date('Y', $stamp[$id]);
	    if (isset($seen[$year])) {
		continue;
	    }
	    $seen[$year] = 1;
	    $marks[] = array('year' => $year,
			     'page' => (int)floor($i / $perPage) + 1,
			     'index' => $i);
	}
	return $marks;
    }




    /**
     * Split the children into date groups, materialising only the first few.
     *
     * A whole-library stream is thousands of tiles; handing all of them to the
     * browser as markup costs seconds of layout before the page responds. Every
     * group still gets a section and a header, so the scrubber can jump to any
     * month immediately - but distant groups carry their tiles as compact data
     * for the client to build when they come near the viewport.
     */
    function _buildGroups(&$theme, $eagerItems=260) {
	if (empty($theme['children'])) {
	    $theme['groups'] = array();
	    $theme['lazyData'] = '{}';
	    return;
	}
	$order = $bucket = array();
	foreach ($theme['children'] as $child) {
	    $key = isset($child['groupKey']) ? $child['groupKey'] : 'undated';
	    if (!isset($bucket[$key])) {
		$bucket[$key] = array();
		$order[] = $key;
	    }
	    $bucket[$key][] = $child;
	}

	$groups = $lazy = array();
	$shown = 0;
	$filling = true;
	foreach ($order as $key) {
	    $items = $bucket[$key];
	    $count = count($items);
	    /* Materialise a contiguous run from the top and then stop: the reader
	       starts at the top, so that is the only part that must be present
	       before first paint. Skipping a large month to pick up smaller ones
	       further down would materialise tiles nobody is looking at. The
	       first group always counts, however big, so the page is never empty
	       above the fold. */
	    $eager = $filling && (empty($groups) || ($shown + $count) <= $eagerItems);
	    if ($eager) {
		$shown += $count;
	    } else {
		$filling = false;
	    }
	    $label = '';
	    foreach ($items as $one) {
		if (!empty($one['groupLabel'])) {
		    $label = $one['groupLabel'];
		    break;
		}
	    }
	    /* A rough height so the scrollbar is sensible before hydration;
	       content-visibility replaces it with the real one once measured. */
	    $rows = (int)ceil($count / 5);
	    $group = array('key' => $key, 'label' => $label, 'count' => $count,
			   'eager' => $eager ? 1 : 0, 'est' => max(1, $rows) * 236,
			   'items' => $eager ? $items : array());
	    if (!$eager) {
		$tuples = array();
		foreach ($items as $one) {
		    if (!empty($one['width']) && !empty($one['height'])) {
			$ar = round($one['width'] / $one['height'], 3);
		    } else {
			$ar = 1.333;
		    }
		    $title = (isset($one['title']) && $one['title'] !== '')
			? $one['title'] : $one['pathComponent'];
		    $tuples[] = array(
			isset($one['thumbnail']['id']) ? (int)$one['thumbnail']['id'] : 0,
			isset($one['thumbnail']['serialNumber'])
			    ? (int)$one['thumbnail']['serialNumber'] : 0,
			(int)$one['id'],
			$ar,
			$title,
			isset($one['dateLabel']) ? $one['dateLabel'] : '',
			empty($one['canContainChildren']) ? 0 : 1);
		}
		$lazy[$key] = $tuples;
	    }
	    $groups[] = $group;
	}
	$theme['groups'] = $groups;
	$theme['lazyData'] = json_encode($lazy);
    }

    /**
     * The navigation rail needs a direct link to the root album. Linking to a
     * bare main.php is ambiguous: a site can legitimately redirect that to a
     * different landing view, which would make "Albums" unreachable.
     */
    function _addNavContext(&$theme) {
	list ($ret, $rootId) = GalleryCoreApi::getPluginParameter(
	    'module', 'core', 'id.rootAlbum');
	if (!$ret && !empty($rootId)) {
	    $theme['rootAlbumId'] = $rootId;
	}
    }

    /**
     * Precompute the date labels each child needs.
     *
     * Doing this in the template costs three strftime calls per item, and the
     * PHP 8 strftime replacement is a regex-and-DateTime affair - about 6,600
     * calls for a 2,200 item stream. Distinct months and days are far fewer
     * than items, so memoise on those instead.
     */
    function _addDateLabels(&$theme) {
	global $gallery;
	if (empty($theme['children'])) {
	    return;
	}
	$platform =& $gallery->getPlatform();
	$months = $days = array();
	foreach (array_keys($theme['children']) as $i) {
	    $stamp = empty($theme['children'][$i]['originationTimestamp'])
		? 0 : (int)$theme['children'][$i]['originationTimestamp'];
	    if (!$stamp) {
		$theme['children'][$i]['groupKey'] = 'undated';
		$theme['children'][$i]['groupLabel'] = '';
		$theme['children'][$i]['dateLabel'] = '';
		continue;
	    }
	    $mk = date('Y-m', $stamp);
	    $dk = date('Y-m-d', $stamp);
	    if (!isset($months[$mk])) {
		$months[$mk] = $platform->strftime('%B %Y', $stamp);
	    }
	    if (!isset($days[$dk])) {
		$days[$dk] = $platform->strftime('%d %B %Y', $stamp);
	    }
	    $theme['children'][$i]['groupKey'] = $mk;
	    $theme['children'][$i]['groupLabel'] = $months[$mk];
	    $theme['children'][$i]['dateLabel'] = $days[$dk];
	}
    }

    /**
     * @see GalleryTheme::showAlbumPage
     */
    function showAlbumPage(&$template, $item, $params, $childIds) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params,
	    array('owner', 'viewCount', 'childCount', 'descendentCount', 'parents',
		  'systemLinks', 'itemLinks', 'itemSummaries', 'permissions',
		  'thumbnails', 'pageNavigator', 'jumpRange'),
	    $childIds);
	if ($ret) {
	    return array($ret, null);
	}

	$theme =& $template->getVariableByReference('theme');
	$theme['yearMarks'] = $this->_yearIndex($theme, $params['rows'] * $params['columns']);
	$this->_addDateLabels($theme);
	$this->_addNavContext($theme);
	$this->_buildGroups($theme);

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params,
	    array('owner', 'viewCount', 'parents', 'systemLinks', 'itemLinks',
		  'permissions', 'imageViews', 'navigator'));
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');
	$this->_addNavContext($theme);

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showModulePage
     */
    function showModulePage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params,
	    array('parents', 'systemLinks', 'permissions'));
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');
	$this->_addNavContext($theme);

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showAdminPage
     */
    function showAdminPage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params,
	    array('parents', 'systemLinks', 'permissions'));
	if ($ret) {
	    return array($ret, null);
	}
	$theme =& $template->getVariableByReference('theme');
	$this->_addNavContext($theme);

	return array(null, 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showErrorPage
     */
    function showErrorPage(&$template) {
	return array(null, 'error.tpl');
    }

    /**
     * @see GalleryTheme::showProgressBarPage
     */
    function showProgressBarPage(&$template, $item, $params) {
	$ret = $this->loadCommonTemplateData($template, $item, $params, array('systemLinks'));
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, 'progressbar.tpl');
    }
}
?>
