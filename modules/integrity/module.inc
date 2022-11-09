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
 *
 * Integrity Check Module
 * @package Integrity
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 1173 $
 */
class IntegrityModule extends GalleryModule {
	public function __construct() {
		global $gallery;

		$this->setId('integrity');
		$this->setName($gallery->i18n('Integrity Check'));
		$this->setDescription($gallery->i18n('Check integrity of the Gallery database and files'));
		$this->setVersion('0.1.2');
		$this->setGroup('gallery', $gallery->i18n('Gallery'));
		$this->setCallbacks('');
		$this->setRequiredCoreApi(array(7, 4));
		$this->setRequiredModuleApi(array(3, 0));
	}

	/**
	 * @see GalleryModule::performFactoryRegistrations()
	 */
	public function performFactoryRegistrations() {
		$ret = GalleryCoreApi::registerFactoryImplementation(
			'MaintenanceTask',
			'IntegrityCheckTask',
			'IntegrityCheckTask',
			'modules/integrity/classes/IntegrityCheckTask.class',
			'integrity',
			null
		);

		if ($ret) {
			return $ret;
		}

		return null;
	}
}
