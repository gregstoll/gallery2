<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2006 Bharat Mediratta
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
 * @version $$ $$
 * @package ItemAdd
 * @subpackage UserInterface
 * @author Orestes Sanchez <miceno.atreides@gmail.com>
 */
function create_excel_row(
	$description,
	$number,
	$terms,
	$media,
	$place,
	$author,
	$date
) {
	return array(
		'description' => $description,
		'number'      => $number,
		'terms'       => $terms,
		'media'       => $media,
		'place'       => $place,
		'author'      => $author,
		'date'        => $date,
	);
}

/**
 * This controller will handle the addition of a bulk list of files using an
 * excel file for descriptions of the images and a zip file with the images.
 *
 * @package ItemAddBulkExcelUpload
 * @subpackage UserInterface
 */
class ItemAddBulkExcelUpload extends ItemAddPlugin {
	/**
	 * @see ItemAddPlugin::handleRequest
	 *
	 * Do not perform data validation. In case form data is not valid, raise
	 * errors or exceptions.
	 * @param $form
	 * @param $item
	 * @param $addController
	 * @return array
	 */
	public function handleRequest($form, &$item, &$addController = null) {
		global $gallery;
		$platform     =& $gallery->getPlatform();
		$classes_path = 'modules/bulkexcelupload/classes/';

		$templateAdapter =& $gallery->getTemplateAdapter();
		$storage         =& $gallery->getStorage();

		$status = $error = array();

		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'bulkexcelupload');

		if ($ret) {
			return array($ret, null, null);
		}

		$heading = $module->translate('Bulk upload from XLSX and ZIP file');
		$gallery->guaranteeTimeLimit(60);
		// The "0" indicates 0 percent done, it will render the progress bar with 0% done.
		$templateAdapter->updateProgressBar($heading, $module->translate('Preparing...'), 0);

		$gallery->debug('Form contents.....');
		$gallery->debug_r($form);

		if (isset($form['action']['add'])) {
			// Load helper
			GalleryCoreApi::requireOnce($classes_path . 'ItemAddBulkExcelUpload_helper.class');

			list($isValidExcel, $warningExcel, $error_excel) = ItemAddBulkExcelUpload_helper::_validate_file($form, 'excelPath');

			if (!$isValidExcel) {
				$templateAdapter->updateProgressBar('Error', null, 1);
				$status['addedFiles'][] = array(
					'fileName' => $form['name']['excelPath'] ?: 'Excel file',
					'id'       => null,
					'warnings' => array("<span class='giError'>" . $module->translate($warningExcel) . '</span>'),
				);
			} else {
				$excelFile = array(
					'name'     => $form['name']['excelPath'],
					'type'     => $form['type']['excelPath'],
					'tmp_name' => $form['tmp_name']['excelPath'],
					'size'     => $form['size']['excelPath'],
				);
			}

			list($isValidZip, $warningZip, $error_zip) = ItemAddBulkExcelUpload_helper::_validate_file($form, 'zipPath');

			if (!$isValidZip) {
				$templateAdapter->updateProgressBar('Error', null, 1);
				$status['addedFiles'][] = array(
					'fileName' => $form['name']['zipPath'] ?: 'ZIP file',
					'id'       => null,
					'warnings' => array("<span class='giError'>" . $module->translate($warningZip) . '</span>'),
				);
			} else {
				$zipFile = array(
					'name'     => $form['name']['zipPath'],
					'type'     => $form['type']['zipPath'],
					'tmp_name' => $form['tmp_name']['zipPath'],
					'size'     => $form['size']['zipPath'],
				);
			}

			// TODO: Move files to Gallery temp file (see ItemAddFromBrowser)
			$hasHeader = array_key_exists('hasHeader', $form) ? $form['hasHeader'] == 'on' : false;

			if ($isValidExcel && $isValidZip) {
				// Set default preview to true.
				$debug                = true;
				$LOOP_PROGRESS        = 1;
				$status['addedFiles'] = array();

				$templateAdapter->updateProgressBar($heading, $module->translate('Initializing data...'), 0.05);

				/**
				 * Create the temporary folder that will hold the images.
				 *
				 */
				$base = $platform->tempnam($gallery->getConfig('data.gallery.tmp'), 'tmp_');
				// TODO: Remove when we are done testing.
				$base        = $gallery->getConfig('data.gallery.tmp');
				$baseTmpPath = $base . 'excelupload.dir';
				$gallery->debug("Basepath = $baseTmpPath");

				if (!$platform->is_dir($baseTmpPath)) {
					if (!$platform->mkdir($baseTmpPath)) {
						return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
					}
				}

				/**
				 * Read Excel File
				 */
				for ($j = 0; $j < $LOOP_PROGRESS; $j++) {
					$templateAdapter->updateProgressBar($heading, $module->translate('Reading XLSX file...'), 0.10);
				}
				list($ret, $excel_data) = ItemAddBulkExcelUpload_helper::_readExcel($excelFile['tmp_name'], $hasHeader);

				if ($ret) {
					return array(GalleryCoreApi::error(ERROR_BAD_DATA_TYPE, null, null, $ret), null, null);
				}
				$gallery->debug('Excel data');
				$gallery->debug_r($excel_data, true);
				/**
				 * Read list of files in zip
				 */
				for ($j = 0; $j < $LOOP_PROGRESS; $j++) {
					$templateAdapter->updateProgressBar($heading, $module->translate('Reading ZIP listing...'), 0.15);
				}

				$fileNames = ItemAddBulkExcelUpload_helper::_readZip($zipFile['tmp_name']);
				$gallery->debug('Zip files');
				$gallery->debug_r($fileNames);

				// Check length of data matches.
				$count_rows  = count($excel_data);
				$count_files = count($fileNames);

				if ($count_files != $count_rows) {
					$message = "El numero de filas del fichero XLSX ($count_rows) no coincide con el numero de lineas del fichero ZIP ($count_files)";

					return array(GalleryCoreApi::error(ERROR_BAD_DATA_TYPE, null, null, $message), null, null);
				}
				/**
				 * Uncompress ZIP file.
				 */
				for ($j = 0; $j < $LOOP_PROGRESS; $j++) {
					$templateAdapter->updateProgressBar($heading, $module->translate('Uncompressing ZIP file...'), 0.20);
				}
				// Prevent PHP timeouts
				$gallery->guaranteeTimeLimit(60);

				list($ret) = ItemAddBulkExcelUpload_helper::_expandZip($zipFile['tmp_name'], $baseTmpPath, $fileNames);

				if ($ret) {
					return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null, null);
				}

				$SETUP_PROGRESS = 0.25;
				/**
				 * Create CSV from excel and zip.
				 */
				for ($j = 0; $j < $LOOP_PROGRESS; $j++) {
					$templateAdapter->updateProgressBar(
						$heading,
						$module->translate('Preparing photo metadata...'),
						$SETUP_PROGRESS
					);
				}
				$batch_size       = 50;
				$total_batches    = ceil(count($excel_data) / $batch_size);
				$batch_progress   = 2 * (1.0 - $SETUP_PROGRESS) / $total_batches;
				$current_progress = $SETUP_PROGRESS;
				$gallery->debug("Preparing to process batches of $batch_size elements, for a total of $total_batches.");

				for ($current_batch = 0; $current_batch < $total_batches; $current_batch++) {
					$gallery->guaranteeTimeLimit(60);
					$current_progress += $batch_progress;
					$templateAdapter->updateProgressBar($heading, 'Preparing photo metadata...', $current_progress);

					// Get excel_data and filenames
					$current_excel_data = array_slice($excel_data, $batch_size * $current_batch, $batch_size);
					$current_filenames  = array_slice($fileNames, $batch_size * $current_batch, $batch_size);

					// create delimited data
					list($ret, $current_delimited_data) = ItemAddBulkExcelUpload_helper::_createDelimitedData(
						$current_excel_data,
						$current_filenames,
						$baseTmpPath,
						false,
						null
					);

					if ($ret) {
						return array(
							GalleryCoreApi::error(
								ERROR_BAD_DATA_TYPE,
								null,
								null,
								$ret
							),
							null,
							null,
						);
					}
					$gallery->debug('Output data for createDelimitedFile');
					$gallery->debug_r($current_delimited_data, true);

					/**
					 * Bulk upload.
					 */
					list($ret, $lockId) = GalleryCoreApi::acquireReadLock($item->getId());

					if ($ret) {
						return array($ret, null, null);
					}

					$current_progress += $batch_progress;
					$templateAdapter->updateProgressBar($heading, 'Adding photos...', $current_progress);

					$gallery->guaranteeTimeLimit(60);
					list($ret, $bulk_error, $bulk_status) = ItemAddBulkExcelUpload_helper::_bulkUpload(
						$current_delimited_data,
						$baseTmpPath,
						$item
					);

					if ($ret) {
						GalleryCoreApi::releaseLocks($lockId);

						return array($ret, null, null);
					}

					// Merge results.
					$error                = array_merge($error, $bulk_error);
					$status['addedFiles'] = array_merge($status['addedFiles'], $bulk_status['addedFiles']);

					// Make checkpoint
					$gallery->guaranteeTimeLimit(60);
					$ret = $storage->checkPoint();

					if ($ret) {
						GalleryCoreApi::releaseLocks($lockId);

						return array($ret, null, null);
					}
					$gallery->guaranteeTimeLimit(60);
					list($ret, $optionErrors) = $addController->postprocessItems($form, $status);

					if ($ret) {
						return array($ret, null, null);
					}
					$error = array_merge($error, $optionErrors);

					if ($optionErrors) {
						break;
					}

					$ret = GalleryCoreApi::releaseLocks($lockId);

					if ($ret) {
						return array($ret, null, null);
					}
				} // for
			} // if isValidxxx

			if (!empty($status)) {
				$gallery->debug('Non empty status');
				$gallery->debug_r($status);
			}

			if (!empty($error)) {
				$gallery->debug('Non empty error');
				$gallery->debug_r($error);
			}
		} else {
			$gallery->error('No valid action');
		}
		$gallery->debug('Returning from Excel import... ');

		return array(null, $error, $status);
	}

	/**
	 * @see ItemAdd:loadTemplate
	 *
	 * Validate field values.
	 *
	 */
	public function loadTemplate(&$template, &$form, $item) {
		global $gallery;

		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'bulkexcelupload');

		if ($ret) {
			return array($ret, null, null);
		}

		if ($form['formName'] != 'ItemAddBulkExcelUpload') {
			$form['formName'] = 'ItemAddBulkExcelUpload';
		}

		if (empty($form['hasHeader'])) {
			$form['hasHeader'] = 'on';
		}

		$fileUploadsBool = GalleryUtilities::getPhpIniBool('file_uploads');
		$totalUploadSize = ini_get('post_max_size');

		if (preg_match('/(\d+)M/', $totalUploadSize, $matches)) {
			$totalUploadSize = $matches[1] * 1024 * 1024;
		}

		$maxFileSize = ini_get('upload_max_filesize');

		if (preg_match('/(\d+)M/', $maxFileSize, $matches)) {
			$maxFileSize = $matches[1] * 1024 * 1024;
		}

		foreach (array('totalUploadSize', 'maxFileSize') as $key) {
			if ($$key >= 1024 * 1024) {
				$$key = $module->translate(
					array(
						'one'   => '%d megabyte',
						'many'  => '%d megabytes',
						'count' => (int)($$key / (1024 * 1024)),
						'arg1'  => (int)($$key / (1024 * 1024)),
					)
				);
			} elseif ($$key >= 1024) {
				$$key = $module->translate(
					array(
						'one'   => '%d kilobytes',
						'many'  => '%d kilobytes',
						'count' => (int)($$key / (1024)),
						'arg1'  => (int)($$key / (1024)),
					)
				);
			}
		}

		$status = GalleryUtilities::getRequestVariables('ItemAddBulkExcelUploadStatus');
		$template->setVariable(
			'ItemAddBulkExcelUpload',
			array(
				'totalUploadSize'  => $totalUploadSize,
				'maxFileSize'      => $maxFileSize,
				'uploadsPermitted' => $fileUploadsBool,
				'status'           => $status,
			)
		);

		// Set the ItemAdmin form's encoding type specially since we're uploading binary files
		if ($template->hasVariable('ItemAdmin')) {
			$ItemAdmin            =& $template->getVariableByReference('ItemAdmin');
			$ItemAdmin['enctype'] = 'multipart/form-data';
		} else {
			$ItemAdmin['enctype'] = 'multipart/form-data';
			$template->setVariable('ItemAdmin', $ItemAdmin);
		}

		return array(
			null,
			'modules/bulkexcelupload/templates/ItemAddBulkExcelUpload.tpl',
			'modules_bulkexcelupload',
		);
	}

	/**
	 * @see ItemAddPlugin::getTitle
	 */
	public function getTitle() {
		list($ret, $module) = GalleryCoreApi::loadPlugin('module', 'bulkexcelupload');

		if ($ret) {
			return array($ret, null);
		}

		return array(null, $module->translate('Bulk Excel Upload'));
	}
}
