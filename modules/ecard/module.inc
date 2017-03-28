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
 * eCard module
 *
 * @package Ecard
 * @author Nico Kaiser <nico@siriux.net>
 * @author Dariush Molavi <dari@nukedgallery.net>
 * @version $Revision: 18172 $
 */
class EcardModule extends GalleryModule {

    function EcardModule() {
	global $gallery;

	$this->setId('ecard');
	$this->setName($gallery->i18n('eCard'));
	$this->setDescription($gallery->i18n('Send photos as eCards'));
	$this->setVersion('1.0.4'); /* Update upgrade() too */
	$this->_templateVersion = 1;
	$this->setGroup('commerce', $gallery->i18n('Commerce'));
	$this->setCallbacks('getItemLinks|getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	if (!isset($currentVersion)) {
	    $currentVersion = '0';
	}

	/* Set default parameters */
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'ecard');
	if ($ret) {
	    return $ret;
	}
	foreach (array(
		'from' => $this->translate('Gallery &lt;gallery&gt;'),
		'subject' => $this->translate('You have been sent an eCard'),
		'bcc' => '',
		'header' => $this->translate(array(
			'text' => 'Hi %s!%s%s (%s) has sent you an eCard:', 'arg1' => '%toName%',
			'arg2' => "\n", 'arg3' => '%fromName%', 'arg4' => '%from%')),
		'footer' => $this->translate(
		    'This eCard was sent with Gallery 2 - Your photos on your website'),
		'format' => 'html',
		'validation.level' => 'HIGH')
		as $key => $value) {
	    if (!isset($params[$key])) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	switch ($currentVersion) {
	case '0':
	case '0.0.6':
	case '0.0.7':
	    list ($ret, $coreParams) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
	    if ($ret) {
		return $ret;
	    }

	    /* Register our permissions */
	    $ret = GalleryCoreApi::registerPermission($this->getId(),
		'ecard.send', $gallery->i18n('[ecard] Send eCards'), 0, array());
	    if ($ret) {
		return $ret;
	    }
	    /* Give everybody permission by default */
	    $gallery->guaranteeTimeLimit(300);
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'],
		'ecard.send', true);
	    if ($ret) {
		return $ret;
	    }
	    break;

	case '0.0.8':
	case '0.0.9':
	case '0.1.0':
	case '0.1.1':
	case '0.9.0':
	case '0.9.1':
	case '1.0.0':
	    /* .mo file migration */
	case '1.0.1':
	case '1.0.2':
	case '1.0.3':

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
		sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	$links = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    if (GalleryUtilities::isA($item, 'GalleryPhotoItem') &&
		    isset($permissions[$itemId]['ecard.send']) &&
		    isset($wantsDetailedLinks[$itemId])) {
		$params['view'] = 'ecard.SendEcard';
		$params['itemId'] = $itemId;
		$params['return'] = 1;
		$links[$itemId][] = array('text' => $this->translate('Send as eCard'),
					  'params' => $params);
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('eCard'),
				       'view' => 'ecard.AdminEcard')));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'CaptchaAdminOption', 'EcardCaptchaAdminOption', 'EcardCaptchaAdminOption',
	    'modules/ecard/classes/EcardCaptchaAdminOption.class', 'ecard', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }
}
?>
