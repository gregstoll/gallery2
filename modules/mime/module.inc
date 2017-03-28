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
 * This module allows gallery administrator
 * to maintain MIME types and corresponding file extensions
 *
 * @package Mime
 * @author Felix Rabinovich <Felix@Rabinovich.org>
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 18172 $
 */
class MimeModule extends GalleryModule {

    function MimeModule() {
	global $gallery;
	$this->setId('mime');
	$this->setName($gallery->i18n('MIME Maintenance'));
	$this->setDescription($gallery->i18n('Maintain MIME types and file extensions'));
	$this->setVersion('1.1.5');
	$this->_templateVersion = 1;
	$this->setGroup('data', $gallery->i18n('Extra Data'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('MIME Types'),
				 'view' => 'mime.MimeAdmin')));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddOption', 'RestrictMimeOption', 'RestrictMimeOption',
	    'modules/mime/RestrictMimeOption.inc', 'mime', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'mime');
	if ($ret) {
	    return $ret;
	}
	foreach (array('allowMime' => 'all', 'mimeList' => '') as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }
}
?>
