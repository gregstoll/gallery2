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
 * Provide the user the option of downloading the Windows registry file.
 * This plugin is only responsible for building the config file to allow the user to register
 * Gallery as a publisher in XP, it doesn't actually do any of the processing of images.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class ItemAddPublishXp extends ItemAddPlugin {

    /**
     * @see ItemAddPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	$httpUserAgent = GalleryUtilities::getServerVar('HTTP_USER_AGENT');

	$isWindowsVista = strpos($httpUserAgent, 'Windows NT 6') !== false;
	$template->setVariable('ItemAddPublishXp',
			       array('isUsingWindowsVista' => (int)$isWindowsVista,
				     'isUsingOtherWindows' => (int)!$isWindowsVista));

	return array(null, 'modules/publishxp/templates/ItemAddPublishXp.tpl', 'modules_publishxp');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'publishxp');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate('Windows Publishing Wizard'));
    }
}
?>
