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
 * The implementation of the Icons module
 *
 * @package Icons
 * @author Jesse Mullan <jmullan@visi.com>
 * @version $Revision: 18172 $
 */
class IconsModule extends GalleryModule {

    function IconsModule() {
	global $gallery;
	$this->setId('icons');
	$this->setName($gallery->i18n('Icons'));
	$this->setDescription($gallery->i18n('Display icons for links'));
	$this->setVersion('1.1.5'); /* Update upgrade() function below too */
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'icons');
	if ($ret) {
	    return $ret;
	}
	foreach (array('iconpack' => '') as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	if (empty($currentVersion)) {
	    $currentVersion = '0';
	}

	switch ($currentVersion) {
	case '0':
	    $ret = $this->setParameter('iconpack', 'silk');
	    if ($ret) {
		    return $ret;
	    }
	    break;

	case '0.9.0':
	case '0.9.1':
	case '0.9.2':
	    if (isset($params['iconpack']) && $params['iconpack'] == 'paularmstrongdesigns.com') {
		/* Removed ".com" from paularmstrongdesigns.com dir, IIS 5.1 didn't like it */
		$ret = $this->setParameter('iconpack', 'paularmstrongdesigns');
		if ($ret) {
		    return $ret;
		}
	    }

	case '0.9.3':
	case '0.9.4':
	case '0.9.5':
	case '1.0.0':
	case '1.0.1':
	case '1.0.2':
	case '1.1.0':
	case '1.1.1':
	case '1.1.2':
	    /* .mo file migration */
	case '1.1.3':
	case '1.1.4':

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'IconsInterface_1_0', 'IconsImpl', 'Icons',
	    'modules/icons/classes/IconsImpl.class', 'icons', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Icons'),
				 'view' => 'icons.IconsSiteAdmin')));
    }
}
?>
