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
/**
 * @package map
 * @author Steven N. Severinghaus <sns@severinghaus.org>
 * @version $Revision: 1253 $
 *
 * This is some of the ugliest code I've ever written. Please do not
 * judge me by it. :)
 */
GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
GalleryCoreApi::requireOnce('modules/mapv3/classes/GoogleMapUtilities.class');

/**
 * @package map
 * @subpackage UserInterface
 */
class MapMarkersAdminController extends GalleryController {
	/**
	 * @see GalleryController::handleRequest
	 */
	public function handleRequest($form) {
		include dirname(__FILE__) . '/includes/MapAdminControllerInit.inc';

		global $gallery;
		$platform       = $gallery->getPlatform();
		$basegallerydir = dirname(dirname(dirname(__FILE__))) . '/';
		$module_path    = 'modules/mapv3/';
		$base           = '/';

		// If we want to "add" a marker, call the marker creation tool
		if (isset($form['action']['add'])) {
			$results['status']              = '';
			$results['error']               = '';
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapMarkersAdmin';
			$results['redirect']['mode']    = 'Tool';
		}

		// Save the Markers settings
		if (isset($form['action']['save'])) {
			// Check for errors
			if (!array_key_exists('useMarkerSet', $form)
				|| !array_key_exists('useAlbumMarkerSet', $form)
			) {
				$error[] = 'form[error][nomarkers]';
			} elseif (!is_numeric($form['regroupDist'])
				|| intval($form['regroupDist']) != $form['regroupDist']
				|| $form['regroupDist'] <= 0
			) {
				$error[] = 'form[error][regroupDist]';
			} elseif (!array_key_exists('regroupIcon', $_POST)
				and array_key_exists('regroupItems', $form) and $form['regroupItems']
			) {
				$error[] = 'form[error][marker]';
			} else {
				if (!array_key_exists('regroupDist', $form)) {
					$form['regroupDist'] = '15';
				}

				// make sure the 'Color' CustomField is inline with the marker set selected
				list($ret, $param)  = mapv3Helper::loadParameters(0, false);
				$newparam['common'] = $param['common'];

				if (isset($form['useMarkerSet'])) {
					$handle = fopen(
						$basegallerydir . $module_path . 'images/markers/'
						. $form['useMarkerSet'] . '/colorlist',
						'r'
					);
					$list   = explode(
						'*',
						fread(
							$handle,
							filesize(
								$basegallerydir . $module_path . 'images/markers/'
								. $form['useMarkerSet'] . '/colorlist'
							) - 1
						)
					);
					fclose($handle);

					$newparam['photo']   = array();
					$newparam['photo'][] = array(
						'field'   => 'Color',
						'summary' => '',
						'detail'  => 1,
						'choices' => $list,
					);
				}

				if (isset($form['useAlbumMarkerSet'])) {
					$handle = fopen(
						$basegallerydir . $module_path . 'images/markers/'
						. $form['useAlbumMarkerSet'] . '/colorlist',
						'r'
					);
					$list   = explode(
						'*',
						fread(
							$handle,
							filesize(
								$basegallerydir . $module_path . 'images/markers/'
								. $form['useAlbumMarkerSet'] . '/colorlist'
							) - 1
						)
					);
					fclose($handle);

					$newparam['album']   = array();
					$newparam['album'][] = array(
						'field'   => 'Color',
						'summary' => '',
						'detail'  => 1,
						'choices' => $list,
					);
				}

				$ret = mapv3Helper::saveParameters($newparam, 0);

				if (!isset($form['defaultphotocolor'])) {
					$form['defaultphotocolor'] = '';
				}

				if (!isset($form['defaultalbumcolor'])) {
					$form['defaultalbumcolor'] = '';
				}

				if (!isset($form['defaultgroupcolor'])) {
					$form['defaultgroupcolor'] = '';
				}

				if (!isset($_POST['regroupIcon'])) {
					$form['regroupIcon'] = '';
				} else {
					$form['regroupIcon'] = $_POST['regroupIcon'];
				}

				// Save parameters if there have been no errors
				foreach (array(
					'useMarkerSet',
					'useAlbumMarkerSet',
					'useGroupMarkerSet',
					'defaultphotocolor',
					'defaultgroupcolor',
					'defaultalbumcolor',
					'regroupDist',
					'regroupIcon',
				) as $setting) {
					GalleryCoreApi::setPluginParameter('module', 'mapv3', $setting, $form[$setting]);
				}

				foreach (array('useParentColor', 'regroupAlbums', 'regroupItems') as $setting) {
					GalleryCoreApi::setPluginParameter(
						'module',
						'mapv3',
						$setting,
						(isset($form[$setting]) && $form[$setting]) ? 1 : 0
					);
				}

				$status['saved'] = 1;
			}

			// Figure out the method we will use
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapMarkersAdmin';
		}

		// Generate the markers according to specs
		if (isset($form['action']['generate'])) {
			// Check for errors

			if (!array_key_exists('base', $form)) {
				$error[] = 'form[error][base]';
			} elseif ($form['setname'] == '') {
				$error[] = 'form[error][setname]';
			} elseif ($form['setname'] == 'multi') {
				$error[] = 'form[error][multiname]';
			} else { // No errors detected
				$basemarkername = $form['base'];
				$setname        = $form['setname'];

				// create the marker dir
				$markers_dir = $basegallerydir . $module_path . 'images/markers/' . $setname;

				if (!file_exists($markers_dir)) {
					list($ret, $dirs) = GalleryUtilities::guaranteeDirExists($markers_dir);

					if (!$ret) {
						$error[] = 'form[error][createdir]';
					}
				}

				$resize     = '';
				$basepngdir = $base . $module_path . 'images/basemarkers/';

				// Figure out if a resize is needed and create the appropriate command
				if ($form['size'][0] != 'same') {
					if ($form['size'][0] == 'custom') {
						$resize = $form['ImageHeight'] . 'x' . $form['ImageWidth'];
					} else {
						$currentsize = getimagesize(
							$basegallerydir . $module_path . 'images/basemarkers/'
							. $basemarkername . '.png'
						);
						$height50    = $currentsize[1] / 2;
						$width50     = $currentsize[0] / 2;
						$height200   = $currentsize[1] * 2;
						$width200    = $currentsize[0] * 2;

						if ($form['size'][0] == '50') {
							$resize = $width50 . 'x' . $height50;
						}

						if ($form['size'][0] == '200') {
							$resize = $width200 . 'x' . $height200;
						}
					}
				}

				// Check for the colors selected and input them to an array
				$colors = array(
					'ORANGE' => '95,127,127',
					'PURPLE' => '95,127,255',
					'YELLOW' => '105,100,140',
					'GREEN'  => '95,127,159',
					'BLUE'   => '95,95,31',
					'RED'    => '95,255,95',
					'AQUA'   => '95,120,201',
					'WHITE'  => '95,0,0',
					'GRAY'   => '47,0,0',
					'BLACK'  => '0,0,0',
				);

				if ($form['colors'] != 'all') {
					$colors = array();

					if (isset($_POST['blue'])) {
						$colors = $colors + array(
							'BLUE' => '95,95,31',
						);
					}

					if (isset($_POST['red'])) {
						$colors = $colors + array(
							'RED' => '95,255,95',
						);
					}

					if (isset($_POST['yellow'])) {
						$colors = $colors + array(
							'YELLOW' => '95,100,140',
						);
					}

					if (isset($_POST['green'])) {
						$colors = $colors + array(
							'GREEN' => '95,127,159',
						);
					}

					if (isset($_POST['orange'])) {
						$colors = $colors + array(
							'ORANGE' => '95,127,127',
						);
					}

					if (isset($_POST['aqua'])) {
						$colors = $colors + array(
							'AQUA' => '95,255,191',
						);
					}

					if (isset($_POST['purple'])) {
						$colors = $colors + array(
							'PURPLE' => '95,127,255',
						);
					}

					if (isset($_POST['white'])) {
						$colors = $colors + array(
							'WHITE' => '95,0,0',
						);
					}

					if (isset($_POST['black'])) {
						$colors = $colors + array(
							'BLACK' => '0,0,0',
						);
					}

					if (isset($_POST['grey'])) {
						$colors = $colors + array(
							'GRAY' => '47,0,0',
						);
					}
				}

				// Put the colorlist in a file for later retrieval
				$colorlist = array();
				$handle    = fopen($basegallerydir . $module_path . 'images/markers/' . $setname . '/colorlist', 'w');

				foreach ($colors as $name => $value) {
					$colorlist[] = $name;
					fwrite($handle, $name . '*');
				}
				fclose($handle);

				$statusexec = 0;

				list($ret, $magicpath) = GalleryCoreApi::getPluginParameter('module', 'imagemagick', 'path');

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}

				// For each color, creates a marker using Convert from the imagemagick toolkit
				foreach ($colors as $color => $modulate) {
					// build the array of command and arguments for $platform->exec()
					$command   = array($magicpath . 'convert');
					$command[] = '-modulate';
					$command[] = $modulate;

					if (!empty($resize)) {
						$command[] = '-resize';
						$command[] = $resize;
					}
					$command[]   = $basegallerydir . $module_path . 'images/basemarkers/' . $basemarkername
								   . '.png';
					$command[]   = $basegallerydir . $module_path . 'images/markers/' . $setname
								   . '/marker_' . $color . '.png';
					$returnstuff = $platform->exec(array($command));
					/*
					 * if the marker created does not exist, then something went wrong,
					 * break out and display an error message
					 */
					if (!$platform->file_exists(
						$basegallerydir . $module_path . 'images/markers/' . $setname
						. '/marker_' . $color . '.png'
					)
					) {
						$statusexec = 1;
						$error[]    = 'form[createicons][' . $returnstuff[2][0] . ']';

						break;
					}
				}

				if (empty($error)) {
					$status['generated'] = 1;
				}
			}

			// Redirect to the AdminPanel once done :D
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapMarkersAdmin';
			$results[$method]['mode']    = empty($error) ? '' : 'Tool';
		}

		// Deleting a markerSet
		if (isset($form['action']['delete'])) {
			$status['deleted'] = 0;

			$dir = $basegallerydir . $module_path . 'images/markers/' . $form['deleteMarkerSet'];

			// Delete all file in the folder and then delete the folder
			$status['deleted'] = $platform->recursiveRmdir($dir);

			// return status and display the Legend Admin Panel again
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapMarkersAdmin';
		}

		// Upload a new basemarker and then return to the tool page
		if (isset($form['action']['upload'])) {
			// checking for error
			if ($form['name'][1] == '') {
				$error[] = 'form[error][nofileselected]';
			} elseif ($form['type'][1] != 'image/png' and $form['type'][1] != 'image/x-png') {
				$error[] = 'form[error][badfileselected]';
			} elseif ($form['tmp_name'][1] == '') {
				$error[] = 'form[error][notuploaded]';
			} elseif (!move_uploaded_file(
				$form['tmp_name'][1],
				$basegallerydir
				. $module_path . 'images/basemarkers/' . $form['name'][1]
			)
			) {
				$error[] = 'form[error][rightserror]';
			} else {
				$status['uploaded'] = 1;
			}
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapMarkersAdmin';
			$results[$method]['mode']    = 'Tool';
		}

		// Cancel hit, return to the default adminpage
		if (isset($form['action']['cancel'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapMarkersAdmin';
		}

		if (isset($form['action']['viewall'])) {
			$results['status']              = $status;
			$results['error']               = array();
			$results['redirect']['view']    = 'core.SiteAdmin';
			$results['redirect']['subView'] = 'mapv3.MapMarkersAdmin';
			$results['redirect']['mode']    = 'ViewMarkers';
		}

		// Upload a new icon and then return to the regroup page
		if (isset($form['action']['uploadicon'])) {
			// checking for error
			if ($form['name'][2] == '') {
				$error[] = 'form[error][nofileselected]';
			} elseif ($form['type'][2] != 'image/png' and $form['type'][2] != 'image/x-png') {
				$error[] = 'form[error][badfileselected]';
			} elseif ($form['tmp_name'][2] == '') {
				$error[] = 'form[error][notuploaded]';
			} elseif (!move_uploaded_file(
				$form['tmp_name'][2],
				$basegallerydir . $module_path . 'images/multi/' . $form['name'][2]
			)
			) {
				$error[] = 'form[error][rightserror]';
			} else {
				$status['uploaded'] = 1;
			}
			$method                      = empty($error) ? 'redirect' : 'delegate';
			$results['status']           = $status;
			$results['error']            = $error;
			$results[$method]['view']    = 'core.SiteAdmin';
			$results[$method]['subView'] = 'mapv3.MapMarkersAdmin';
		}

		return array(null, $results);
	}
}

/**
 * @package map
 * @subpackage UserInterface
 */
class MapMarkersAdminView extends GalleryView {
	/**
	 * @see GalleryView::loadTemplate
	 */
	public function loadTemplate(&$template = null, &$form = null) {
		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';

		include dirname(__FILE__) . '/includes/MapAdminViewInit.inc';

		$base_path = 'modules/mapv3/';

		$form['formName'] = 'MapMarkersAdmin';
		$template->head($base_path . 'includes/GoogleMap.css');
		$template->javascript($base_path . 'GoogleMap.js');

		$basegallerydir = dirname(dirname(dirname(__FILE__))) . '/';
		$base           = '/';

		// Absolute path to the folder holding the PNG files.
		$basepngdir = $basegallerydir . $base_path . 'images/basemarkers/';

		// Relative path to the folder holding the PNG files.
		$pngdir = $base . $base_path . 'images/basemarkers/';

		// verify that the module install was done right, maybe some missing files ?
		if (!is_dir($basepngdir)) {
			$form['nobase'] = 'brokendir';
		} else {
			$form['basemarkers'] = GoogleMapUtilities::TableFromFiles(
				$basegallerydir,
				$base,
				$base_path . 'images/basemarkers/'
			);
		}

		$form['multimarkers'] = GoogleMapUtilities::TableFromFiles(
			$basegallerydir,
			$base,
			$base_path . 'images/multi/'
		);

		$mydir = dir($basegallerydir . $base_path . 'images/markers/');

		while (($file = $mydir->read()) !== false) {
			if ((is_dir($basegallerydir . $base_path . 'images/markers/' . $file))
				and (substr($file, -1) != '.') and ($file != 'CVS')
			) {
				$form['markers'][] = $file;
			}
		}

		if (array_key_exists('markers', $form)) {
			$form['markerlist'] = '"' . implode('","', $form['markers']) . '"';
		}


		// Set default values for undefined plugin parameters
		if (!array_key_exists('regroupAlbums', $form)) {
			$form['regroupAlbums'] = false;
		}

		if (!array_key_exists('regroupItems', $form)) {
			$form['regroupItems'] = false;
		}

		if (!array_key_exists('regroupDist', $form)) {
			$form['regroupDist'] = 15;
		}

		if (!array_key_exists('regroupIcon', $form)) {
			$form['regroupIcon'] = 'zoom-in-trans';
		}

		if (!array_key_exists('multirows', $form)) {
			$form['multirows'] = array();
		}

		if (!array_key_exists('useMarkerSet', $form)) {
			$form['useMarkerSet'] = 'smallpushpins';
		}

		if (!array_key_exists('deleteMarkerSet', $form)) {
			$form['deleteMarkerSet'] = '';
		}

		if (!array_key_exists('useAlbumMarkerSet', $form)) {
			$form['useAlbumMarkerSet'] = 'smallpushpins';
		}

		if (!array_key_exists('useGroupMarkerSet', $form)) {
			$form['useGroupMarkerSet'] = 'smallpushpins';
		}

		if (!array_key_exists('defaultalbumcolor', $form)) {
			$form['defaultalbumcolor'] = 'YELLOW';
		}

		if (!array_key_exists('defaultgroupcolor', $form)) {
			$form['defaultgroupcolor'] = 'RED';
		}

		if (!array_key_exists('defaultphotocolor', $form)) {
			$form['defaultphotocolor'] = 'GREEN';
		}

		if (!array_key_exists('useParentColor', $form)) {
			$form['useParentColor'] = false;
		}

		if (!array_key_exists('Colorlists', $form)) {
			$form['Colorlists'] = array();
		}

		if (!array_key_exists('colorAoption', $form)) {
			$form['colorAoption'] = '';
		}

		if (!array_key_exists('colorPoption', $form)) {
			$form['colorPoption'] = '';
		}

		if (!array_key_exists('colorGoption', $form)) {
			$form['colorGoption'] = '';
		}

		// Create the list of marker available and select the right one for Photos & albums
		$form['markerset']      = GoogleMapUtilities::OptionFromFolder(
			$basegallerydir . $base_path . 'images/markers/',
			$form['useMarkerSet']
		);
		$form['albummarkerset'] = GoogleMapUtilities::OptionFromFolder(
			$basegallerydir . $base_path . 'images/markers/',
			$form['useAlbumMarkerSet']
		);
		$form['groupmarkerset'] = GoogleMapUtilities::OptionFromFolder(
			$basegallerydir . $base_path . 'images/markers/',
			$form['useGroupMarkerSet']
		);

		/*
		 * Create the list of available color for the selected markerset for Photos, albums
		 * and Groups
		 */
		$form['colorPoption'] = GoogleMapUtilities::OptionFromColorList(
			$basegallerydir . $base_path . 'images/markers/' . $form['useMarkerSet'],
			$form['defaultphotocolor']
		);
		$form['colorGoption'] = GoogleMapUtilities::OptionFromColorList(
			$basegallerydir . $base_path . 'images/markers/' . $form['useGroupMarkerSet'],
			$form['defaultgroupcolor']
		);
		$form['colorAoption'] = GoogleMapUtilities::OptionFromColorList(
			$basegallerydir . $base_path . 'images/markers/' . $form['useAlbumMarkerSet'],
			$form['defaultalbumcolor']
		);

		include dirname(__FILE__) . '/includes/MapAdminEdits.inc';

		$form['allmarkers'] = GoogleMapUtilities::AllMarkers();

		if ($template->hasVariable('SiteAdmin')) {
			$SiteAdmin            =& $template->getVariableByReference('SiteAdmin');
			$SiteAdmin['enctype'] = 'multipart/form-data';
		} else {
			$SiteAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('SiteAdmin', $SiteAdmin);
		}

		$template->setVariable('controller', 'mapv3.MapMarkersAdmin');

		return array(
			null,
			array(
				'body' => $base_path . '/templates/MapMarkersAdmin.tpl',
			),
		);
	}
}
