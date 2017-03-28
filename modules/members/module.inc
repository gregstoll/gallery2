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
 * The implementation of the members module
 * @package Members
 * @author Robert Balousek <rbalousek@hotmail.com>
 * @version $Revision: 18172 $
 */
class MembersModule extends GalleryModule {

    function MembersModule() {
	global $gallery;

	$this->setId('members');
	$this->setName($gallery->i18n('Members'));
	$this->setDescription($gallery->i18n('Members List and Profiles'));
	$this->setVersion('1.0.8');
	$this->_templateVersion = 1;
	$this->setGroup('gallery', $gallery->i18n('Gallery'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	if (!isset($currentVersion)) {
	    list ($ret, $viewSelectedGroupId) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'id.everybodyGroup');
	    if ($ret) {
		return $ret;
	    }

	    foreach (array('canViewMembersModule' => $viewSelectedGroupId,
			   'canViewEmailAddress' => 0) as $key => $value) {
		$ret = $this->setParameter($key, $value);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate('Members'),
				       'view' => 'members.AdminMembers')));
    }
}
?>
