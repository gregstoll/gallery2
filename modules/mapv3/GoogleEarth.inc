<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
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
// Written by Nic Roets. Placed in the public domain.
/* Changelog:
 Changed from map module 0.5.1d (Karl Newman, 2006-12-22):
 * Fixed reversed longitude/latitude bug
 * Fixed several bugs where conditional test variables were not initialized every pass
 * Removed lots of dead code related to importing GPS track
 * Validates field values to ensure they are in legal range
 * Changed to use mapv3Helper function to retrieve field values
 * Changed to recursive function to export a single item or create a hierarchy of
	all subitems of current item
 * Only include subitems (in particular ChildDataItems) that have GPS coordinates set
 * Added support for optional 'GELookAt' field to customize how GE portrays the location
 * Got rid of the View and associated form, which wasn't doing anything but confusing people.
	Note that this requires a change to the "Export to GE" link in map/module.inc
	to call this as 'controller' instead of 'view'.
	It makes the "Export to GE" a direct link to download to Google Earth.
 Further updates (Karl Newman, 2006-12-26)
 * Added "Document" enclosing structure; fixed styles to avoid using deprecated icons.
 Still more updates (Karl Newman, 2006-12-27)
 * Use "LookAt" block for albums if they have coordinates set (still won't show a point, though)
 * Include albums if they have coordinates set, even if no children do.
 * Show the album thumbnail and link to the album in the folder description.
 More refinements (Karl Newman, 2006-12-28)
 * Changed the prefix to GoogleEarth instead of GoogleEarthAlbum, since it can be used to view a
	single item as well as an album. Also, this really should be a View, not a Controller, since
	it doesn't modify any data. So, I changed it to an Immediate view, patterned after
	DownloadItem in the Core module. These changes will affect module.inc as well.
 * Added function documentation, prefix private functions with _
 */

/**
 * Module to export items to a KML file for display in Google Earth
 *
 * @package map
 * @author Nic Roets
 * @author Karl Newman
 * @version $Revision: 1253 $
 */
class GoogleEarthView extends GalleryView {
	/**
	 * @see GalleryView::isImmediate
	 */
	public function isImmediate() {
		return true;
	}

	/**
	 * @see GalleryView::isAllowedInEmbedOnly
	 */
	public function isAllowedInEmbedOnly() {
		return true;
	}

	/**
	 * @see GalleryView::shouldSaveSession
	 */
	public function shouldSaveSession() {
		return false;
	}

	/**
	 * @see GalleryView::renderImmediate
	 */
	public function renderImmediate($status, $error) {
		global $gallery;
		$phpVm = $gallery->getPhpVm();

		$itemId            = GalleryUtilities::getRequestVariables('itemId');
		list($ret, $item)  = GalleryCoreApi::loadEntitiesById($itemId);

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null);
		}

		list($ret, $placemarks) = GoogleEarthView::_createGETree($item, array(), array());

		if ($ret) {
			return array($ret->wrap(__FILE__, __LINE__), null);
		}

		if (!empty($placemarks)) {
			$outstring = '<?xml version="1.0" encoding="UTF-8"?>'
			. '<kml xmlns="http://earth.google.com/kml/2.0">'
			. "\n<Document><name>Gallery2</name><open>1</open>\n"
			. '<Style id="PhotoIconNormal"><IconStyle>' . "\n"
			. '<Icon><href>https://maps.google.com/mapfiles/kml/pal4/icon46.png</href></Icon>' . "\n"
			. '</IconStyle></Style>' . "\n"
			. '<Style id="PhotoIconHighlight"><IconStyle><scale>1.1</scale>' . "\n"
			. '<Icon><href>https://maps.google.com/mapfiles/kml/pal4/icon38.png</href></Icon>' . "\n"
			. '</IconStyle><LabelStyle><scale>1.1</scale></LabelStyle></Style>' . "\n"
			. '<StyleMap id="PhotoIconPair">' . "\n"
			. '<Pair><key>normal</key><styleUrl>#PhotoIconNormal</styleUrl></Pair>' . "\n"
			. '<Pair><key>highlight</key><styleUrl>#PhotoIconHighlight</styleUrl></Pair></StyleMap>'
			. "\n" . $placemarks . "</Document>\n</kml>\n";
			$phpVm->header('Content-Type: application/vnd.google-earth.kml+xml');
			$phpVm->header('Content-Description: View Gallery items in Google Earth');
			$phpVm->header("Content-Disposition: inline; filename=GalleryGE-{$itemId}.kml");
			$phpVm->header('Content-Length: ' . strlen($outstring));
			echo $outstring;
		}
		// Should probably return some error that no child elements had valid GPS coordinates

		return null;
	}

	/*
	 * Recursive function to generate the Folder and Placemark elements to represent the albums
	 * and items that have GPS coordinates set.
	 *
	 * param GalleryItem $item the current item/album to process
	 * param array $data array of arrays (keyed by itemID) of field data (keyed by field name)
	 *     associated with the $item (e.g., GPS coordinates)
	 * param array $ancestors list of parent item IDs already processed (to prevent infinite loops)
	 *
	 * returns string KML subelements: folder(s) and/or placemark(s)
	 */
	public function _createGETree($item, $data, $ancestors) {
		$base_path = dirname(__FILE__) . '/';

		GalleryCoreApi::requireOnce($base_path . 'classes/mapv3Helper.class');

		$returnstring = ''; // start with a blank string
		if (empty($data)) { // workaround for first call if no field data passed
			list($ret, $data) = mapv3Helper::fetchFieldValues(array($item));
		}

		// If it's an album or similar
		if ($item->getCanContainChildren()) {
			// Get all the direct children of this item, in album sort order
			list($ret, $childIds) = GalleryCoreApi::fetchChildItemIds($item);

			if ($ret) {
				return array($ret->wrap(__FILE__, __LINE__), null);
			}
			// If the list of direct children wasn't empty
			if (!empty($childIds)) {
				// If we haven't already included this item in a parent folder
				if (!array_search($item->getId(), $ancestors)) {
					// Add ourselves to the ancestors array
					$ancestors[] = $item->getId();
					// Load the children
					list($ret, $childItems) = GalleryCoreApi::loadEntitiesById($childIds);

					if ($ret) {
						return array($ret->wrap(__FILE__, __LINE__), null);
					}

					// Grab the field data for the child items
					list($ret, $fielddata) = mapv3Helper::fetchFieldValues($childItems);

					if ($ret) {
						return array($ret->wrap(__FILE__, __LINE__), null);
					}

					foreach ($childItems as $childItem) {
						// Recurse into this function with the next level down
						list($ret, $childstring) = GoogleEarthView::_createGETree($childItem, $fielddata, $ancestors);

						if ($ret) {
							return array($ret->wrap(__FILE__, __LINE__), null);
						}
						$returnstring .= $childstring;
					}

					/*
					 * Only add containing folder info if there are child elements or if the folder
					 * has coordinates of its own.
					 */
					if (!empty($returnstring) or !empty($data[$item->getId()]['GPS'])) {
						$coordstring  = (!empty($data[$item->getId()]['GPS'])) ? $data[$item->getId()]['GPS'] : '';
						$LookAtstring = (!empty($data[$item->getId()]['GELookAt'])) ? $data[$item->getId()]['GELookAt'] : '';
						$returnstring = '<Folder id="g2id_' . $item->getId() . '">'
						. GoogleEarthView::_createTextElements($item)
						. GoogleEarthView::_createLookAtBlock($coordstring, $LookAtstring)
						. $returnstring . "</Folder>\n";
					}
				}
			}
		} else { // It's not a container item
			if (GalleryUtilities::isA($item, 'GalleryDataItem')) {
				list($ret, $childitemstring) = GoogleEarthView::_createGEChildItem($item, $data[$item->getId()]);

				if ($ret) {
					return array($ret->wrap(__FILE__, __LINE__), null);
				}
				$returnstring .= $childitemstring;
			}
		}

		return array(null, $returnstring);
	}

	/*
	 * Create name, Snippet and description elements single Placemark element
	 *
	 * param GalleryItem $item the current GalleryDataItem for which to generate text elements
	 *
	 * return string name, Snippet and description elements for the item, properly CDATA-escaped
	 *
	 */
	public function _createTextElements($item) {
		$returnstring = '';

		GalleryCoreApi::requireOnce('lib/smarty_plugins/modifier.entitytruncate.php');

		// Get the item title if not blank, else use the file/directory name (path component)
		$title = $item->getTitle() ? $item->getTitle() : $item->getPathComponent();
		// Use the item summary for the Snippet, otherwise use the item description
		$summstring = $item->getSummary() ? GalleryUtilities::markup($item->getSummary(), 'strip') : smarty_modifier_entitytruncate(
			GalleryUtilities::markup($item->getDescription(), 'strip'),
			80
		);
		/** @todo If/when KML officially supports HTML in Snippet blocks, remove the 'strip' */
		$returnstring = '<name><![CDATA[' . GalleryUtilities::markup($title)
			. "]]></name>\n<description><![CDATA["
			. ($item->getSummary() ? GalleryUtilities::markup($item->getSummary()) . "<br>\n" : '')
			. GoogleEarthView::_createThumbnailLink($item)
			. GalleryUtilities::markup($item->getDescription())
			. ']]></description><Snippet>' . $summstring . "</Snippet>\n";

		return $returnstring;
	}

	/*
	 * Create a single Placemark element
	 *
	 * param GalleryItem $item the current GalleryDataItem to represent in a Placemark
	 * param array $fielddata field data associated with the $item
	 *
	 * return string KML placemark element
	 *
	 */
	public function _createGEChildItem($item, $fielddata) {
		$goodcoords   = false;
		$returnstring = '';

		// Select items that have GPS coordinates filled out
		if (!empty($fielddata['GPS'])) {
			$posxy = explode(',', $fielddata['GPS']);

			if (count($posxy) >= 2) { // changed to >= 2 to accommodate possible elevation field
				$lat        = (float)$posxy[0];
				$lon        = (float)$posxy[1];
				$goodcoords = ((abs($lat) < 90) and (abs($lat) < 180));
			}
		}

		if ($goodcoords) {
			$coord = sprintf('%s,%s', $posxy[1], $posxy[0]);
			// This spot is one of the weird ones that wants longitude listed first
			$point = "<Point><altitudeMode>clampToGround</altitudeMode>\n<coordinates>"
			 . $coord . ",0</coordinates>\n</Point>\n";

			// Output Placemark to Google Earth
			$coordstring   = (!empty($fielddata['GPS'])) ? $fielddata['GPS'] : '';
			$LookAtstring  = (!empty($fielddata['GELookAt'])) ? $fielddata['GELookAt'] : '';
			$returnstring .= '<Placemark id="g2id_' . $item->getId() . '">'
				 . GoogleEarthView::_createTextElements($item)
				 . "<styleUrl>#PhotoIconPair</styleUrl>\n" . $point
				 . GoogleEarthView::_createLookAtBlock($coordstring, $LookAtstring)
				 . "</Placemark>\n";
		}

		return array(null, $returnstring);
	}

	/*
	 * Create a LookAt block to apply to a folder or placemark
	 *
	 * param string $GPSstring the GPS coordinates in latitude,longitude format
	 * param string $LookAtstring the Google Earth LookAt parameters in heading,tilt,range format
	 *
	 * returns string KML LookAt block
	 *
	 */
	public function _createLookAtBlock($GPSstring, $LookAtstring) {
		$headingstr   = '0';
		$tiltstr      = '45';
		$rangestr     = '3000';
		$returnstring = '';
		$goodcoords   = false;
		$posxy        = array();

		if (!empty($GPSstring)) {
			$posxy = explode(',', $GPSstring);

			if (count($posxy) >= 2) { // changed to >= 2 to accommodate possible elevation field
				$lat        = (float)$posxy[0];
				$lon        = (float)$posxy[1];
				$goodcoords = ((abs($lat) < 90) and (abs($lon) < 180));
			}
		}
		/*
		 * Use sprintf and %F to force non-locale-aware float printing to avoid inadvertant
		 * commas in the float values. Limit decimal places for range, tilt, heading. If %F is
		 * not supported (PHP 4.3.9 or 5.0.2 or earlier), fall back to the raw string.
		 */

		if ($goodcoords) {
			// Parse the 'GELookAt' field
			if (!empty($LookAtstring)) {
				$lookat = explode(',', $LookAtstring);
				/*
				 * If at least one parameter, then #1 is the heading in degrees.
				 * Must be between 0 and 360 degrees
				 */
				if (count($lookat) >= 1 and !empty($lookat[0])
					and $lookat[0] >= -360 and $lookat[0] <= 360
				) {
					$heading    = (float)$lookat[0];
					$headingstr = sprintf('%.2F', $heading) == '' ? $lookat[0] : sprintf('%.2F', $heading);
				}
				/*
				 * If at least two parameters, then #2 is the tilt in degrees.
				 * Must be between 0 and 90 degrees
				 */
				if (count($lookat) >= 2 and !empty($lookat[1])
					and $lookat[1] >= 0 and $lookat[1] <= 90
				) {
					$tilt    = (float)$lookat[1];
					$tiltstr = sprintf('%.2F', $tilt) == '' ? $lookat[1] : sprintf('%.2F', $tilt);
				}
				// If at least three parameters, then #3 is the range in meters. Must be > 0
				if (count($lookat) >= 3 and !empty($lookat[2]) and $lookat[2] > 0) {
					$range    = (float)$lookat[2];
					$rangestr = sprintf('%.2F', $range) == '' ? $lookat[2] : sprintf('%.2F', $range);
				}
			}
			$lonstr       = $posxy[1];
			$latstr       = $posxy[0];
			$returnstring = sprintf(
				'<LookAt><longitude>%s</longitude><latitude>%s</latitude>'
					. '<range>%s</range><tilt>%s</tilt><heading>%s</heading>',
				$lonstr,
				$latstr,
				$rangestr,
				$tiltstr,
				$headingstr
			)
						. "</LookAt>\n";
		}

		return $returnstring;
	}

	/*
	 * Create an HTML snippet to display the thumbnail for an item and create a link to the gallery
	 * item
	 *
	 * param GalleryItem $item the current GalleryDataItem to represent in a Placemark
	 *
	 * returns string HTML snippet suitable for inclusion in a CDATA-escaped description element
	 *
	 */
	public function _createThumbnailLink($item) {
		$hsitemLink = '';
		$dthumbLink = '';
		global $gallery;

		$urlGenerator =& $gallery->getUrlGenerator();
		$returnstring = '';
		$theid        = $item->getId();
		// Get the thumbnail item ID for the item
		list($ret, $thumbs) = GalleryCoreApi::fetchThumbnailsByItemIds(array($theid));

		$thumbId = $thumbs[$theid]->getId();

		$dthumbLink = $urlGenerator->generateUrl(
			array(
				'view'   => 'core.DownloadItem',
				'itemId' => $thumbId,
			),
			array(
				'forceFullUrl' => true,
				'htmlEntities' => false,
			)
		);
		$hsitemLink = $urlGenerator->generateUrl(
			array(
				'view'   => 'core.ShowItem',
				'itemId' => $theid,
			),
			array(
				'forceFullUrl' => true,
				'htmlEntities' => false,
			)
		);

		if ($hsitemLink != '' and $dthumbLink != '') {
			$returnstring = '<a href="' . $hsitemLink . "\">\n<img src=\"" . $dthumbLink
				. '"></a><br>';
		}

		return $returnstring;
	}
}
