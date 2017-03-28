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
 * Import for Picasa 2 XML-Exports
 * @package Picasa
 * @author Waldemar Schlackow <gallery@schlackow.de>
 * @version $Revision: 18172 $
 */
class PicasaModule extends GalleryModule {

    function PicasaModule() {
	global $gallery;
	$this->setId('picasa');
	$this->setName($gallery->i18n('Picasa'));
	$this->setDescription($gallery->i18n('Import for Picasa 2 XML-Exports'));
	$this->setVersion('1.0.10');
	$this->_templateVersion = 1;
	$this->setGroup('import', $this->translate('Import'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 45));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddFromPicasa', 'ItemAddFromPicasa',
	    'modules/picasa/ItemAddFromPicasa.inc', 'picasa', null);
	if ($ret) {
	    return $ret;
	}
	return null;
    }


    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('Picasa 2'),
				       'view' => 'picasa.SelectPicasaExportPath')));
    }
}
?>
