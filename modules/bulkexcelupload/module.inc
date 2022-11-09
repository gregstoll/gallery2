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
 * @package BulkExcelUpload
 * @version $$ $$
 * @author Orestes Sanchez <miceno.atreides@gmail.com>
 *//**
 * BulkExcelUpload module
 *
 * This module provides toolkit support to extract items from
 * bulk files such as a zip file
 *
 * @package BulkExcelUpload
 */
class BulkExcelUploadModule extends GalleryModule {
	public function __construct() {
		global $gallery;

		$this->setId('bulkexcelupload');
		$this->setName($gallery->i18n('Bulk Excel Upload'));
		$this->setDescription($gallery->i18n('Upload files from Excel and Zip data files'));
		$this->setVersion('1.0.1');
		$this->setGroup('import', $gallery->i18n('Import'));
		$this->setCallbacks('getSiteAdminViews');
		$this->setRequiredCoreApi(array(7, 2));
		$this->setRequiredModuleApi(array(3, 0));

		$this->_templateVersion = 1;
	}

	/**
	 * @see GalleryModule::performFactoryRegistrations()
	 */
	public function performFactoryRegistrations() {
		$ret = GalleryCoreApi::registerFactoryImplementation(
			'ItemAddPlugin',
			'ItemAddBulkExcelUpload',
			'ItemAddBulkExcelUpload',
			'modules/bulkexcelupload/ItemAddBulkExcelUpload.inc',
			'bulkexcelupload',
			null
		);

		if ($ret) {
			return $ret;
		}
	}
}
