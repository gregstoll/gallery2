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
 * This immediate view provides the backend for callbacks made by the AJAX
 * AdminEventLogViewerView.
 *
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class AdminEventLogViewerCallbackView extends GalleryView {
    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isControllerLike
     */
    function isControllerLike() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return $ret;
	}

	$session =& $gallery->getSession();
	$storage =& $gallery->getStorage();
	$platform =& $gallery->getPlatform();

	$command = GalleryUtilities::getRequestVariables('command');

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return $ret;
	}

	list ($ret, $formatDateTime) = $module->getParameter('format.datetime');
	if ($ret) {
	    return $ret;
	}

	$data = array();
	switch ($command) {
	case 'getRecords':
	    list ($pageNumber, $pageSize) =
		GalleryUtilities::getRequestVariables('pageNumber', 'pageSize');
	    $pageNumber = (int)$pageNumber;
	    if (!$pageNumber) {
		$pageNumber = 1;
	    }
	    $pageSize = (int)$pageSize;
	    if (!$pageSize) {
		$pageSize = 10;
	    }

	    $query = 'SELECT COUNT(*) FROM [EventLogMap]';
	    list ($ret, $results) = $gallery->search($query);
	    if ($ret) {
		return $ret;
	    }
	    $result = $results->nextResult();
	    $totalRecords = $result[0];
	    $totalPages = ceil($totalRecords / $pageSize);

	    list ($ret, $results) = GalleryCoreApi::getMapEntry(
		'EventLogMap',
		array('id', 'timestamp', 'type', 'location', 'client', 'summary'),
		array(),
		array('limit' => array(
			  'count' => $pageSize,
			  'offset' => ($pageNumber - 1) * $pageSize),
		      'orderBy' => array('timestamp' => ORDER_DESCENDING)));
	    if ($ret) {
		return $ret;
	    }

	    $items = array();
	    $urlGenerator =& $gallery->getUrlGenerator();
	    $linkMessage = $module->translate('Link');
	    $pageMessage = $module->translate(
		array('text' => 'Page %d of %d', 'arg1' => $pageNumber, 'arg2' => $totalPages));

	    while ($result = $results->nextResult()) {
		$items[] = array(
		    'id' => $result[0],
		    'date' => $platform->strftime($formatDateTime, $result[1]),
		    'type' => $result[2],
		    'location' => $result[3],
		    'client' => $result[4],
		    'summary' => $result[5]);
	    }
	    $data = array(
		'records' => $items,
		'pageMessage' => $pageMessage,
		'totalPages' => $totalPages);
	    break;

	case 'getRecordDetails':
	    $id = (int)GalleryUtilities::getRequestVariables('id');
	    if (empty($id)) {
		break;
	    }

	    list ($ret, $results) = GalleryCoreApi::getMapEntry(
		'EventLogMap',
		array('userId', 'type', 'summary', 'details', 'location',
		      'client', 'timestamp', 'referer'),
		array('id' => $id));
	    if ($ret) {
		return $ret;
	    }

	    if ($results->resultCount() == 0) {
		break;
	    }

	    $result = $results->nextResult();
	    $data = array(
		'id' => $id,
		'userId' => $result[0],
		'type' => $result[1],
		'summary' => $result[2],
		'details' => $result[3],
		'location' => $result[4],
		'client' => $result[5],
		'date' => $platform->strftime($formatDateTime, $result[6]),
		'referer' => $result[7]);
	    break;
	}

	GalleryCoreApi::requireOnce('lib/JSON/JSON.php');
	$json = new Services_JSON();
	print $json->encode($data);
	return null;
    }
}
?>
