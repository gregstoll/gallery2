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
 * Create an appropriate Google Sitemap for this site.
 * @package Sitemap
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class SitemapView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	list ($ret, $rootId) = GalleryCoreApi::getDefaultAlbumId();
	if ($ret) {
	    return $ret;
	}

	$ret = $this->renderSitemap($rootId);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * Output a site map rooted at the given id
     *
     * @param int $rootId
     * @return GalleryStatus a status code
     */
    function renderSitemap($rootId) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();
	$phpVm = $gallery->getPhpVm();

	/*
	 * Ideas:
	 *  - Calculate priority by using a percentage of item view count to max view counts
	 */

	/*
	 * Don't use a template for this as we may wind up trying to buffer
	 * way too much data
	 */
	$phpVm->header('Content-type: application/xhtml+xml');
	print '<?xml version=\'1.0\' encoding=\'UTF-8\'?>';
	print "\n";
	print '<urlset xmlns="http://www.google.com/schemas/sitemap/0.84" ';
	print 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ';
	print 'xsi:schemaLocation="http://www.google.com/schemas/sitemap/0.84 ';
	print 'http://www.google.com/schemas/sitemap/0.84/sitemap.xsd">';
	print "\n";

	$gallery->guaranteeTimeLimit(120);
	list ($ret, $aclIds) = GalleryCoreApi::fetchAccessListIds(
	    'core.view', $gallery->getActiveUserId());
	if ($ret) {
	    return $ret;
	}
	if (!empty($aclIds)) {
	    list ($ret, $parents) = GalleryCoreApi::fetchParentSequence($rootId);
	    if ($ret) {
		return $ret;
	    }

	    $data = array();
	    $matchJoin = $matchFrag = '';
	    if (!empty($parents)) {
		$matchJoin = '
		INNER JOIN [GalleryItemAttributesMap]
			ON [GalleryItem::id] = [GalleryItemAttributesMap::itemId]
		';
		$matchFrag = '
		([GalleryItem::id] = ?
		 OR
		 [GalleryItemAttributesMap::parentSequence] LIKE ?)
		AND
		';
		$data = array($rootId, implode('/', $parents) . '/' . $rootId . '/%');
	    }

	    $query = sprintf('
	    SELECT
	      [GalleryItem::id],
	      [GalleryEntity::modificationTimestamp],
	      ((%s - [GalleryEntity::creationTimestamp]) /
	       [GalleryEntity::serialNumber]) as SecondsPerChange
	    FROM
	      [GalleryItem]
	      INNER JOIN [GalleryEntity] ON [GalleryItem::id] = [GalleryEntity::id]
	      INNER JOIN [GalleryAccessSubscriberMap]
		      ON [GalleryItem::id] = [GalleryAccessSubscriberMap::itemId]
	      %s
	    WHERE
	      %s
	      [GalleryAccessSubscriberMap::accessListId] IN (%s)
	    ORDER BY [GalleryEntity::modificationTimestamp] ASC
	    ', $phpVm->time(), $matchJoin, $matchFrag,
	       GalleryUtilities::makeMarkers(count($aclIds)));

	    $gallery->guaranteeTimeLimit(120);
	    list ($ret, $searchResults) = $gallery->search($query, array_merge($data, $aclIds));
	    if ($ret) {
		return $ret;
	    }

	    $frequencyTable = array(
		3600 => 'hourly',
		24 * 3600 => 'daily',
		7 * 24 * 3600 => 'weekly',
		30 * 24 * 3600 => 'monthly',
		365 * 24 * 3600 => 'yearly');
	    $i = 0;
	    while ($result = $searchResults->nextResult()) {
		if (($i++ % 100) == 0) {
		    $gallery->guaranteeTimeLimit(200);
		}
		$loc = $urlGenerator->generateUrl(
		    array('view' => 'core.ShowItem', 'itemId' => $result[0]),
		    array('forceSessionId' => false, 'forceFullUrl' => true));
		$lastmod = gmstrftime('%Y-%m-%dT%H:%M:%S+00:00', $result[1]);
		$changefreq = 'never';
		foreach ($frequencyTable as $comparison => $value) {
		    if ($result[2] <= $comparison) {
			$changefreq = $value;
			break;
		    }
		}
		printf('<url><loc>%s</loc><lastmod>%s</lastmod><changefreq>%s</changefreq></url>',
		       $loc, $lastmod, $changefreq);
		print "\n";
	    }
	}
	print '</urlset>';

	return null;
    }
}
?>
