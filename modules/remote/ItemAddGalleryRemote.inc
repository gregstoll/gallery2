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
 * This ItemAdd plugin shows information about Gallery Remote
 * @package Remote
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemAddGalleryRemote extends ItemAddPlugin {

    /**
     * @see ItemAdd:loadTemplate
     */
    function loadTemplate(&$template, &$form, $item) {
	if ($form['formName'] != 'ItemAddGalleryRemote') {
	    $form['formName'] = 'ItemAddGalleryRemote';
	}

	$template->setVariable('ItemAddGalleryRemote', array());
	return array(null, 'modules/remote/templates/ItemAddGalleryRemote.tpl', 'modules_remote');
    }

    /**
     * @see ItemAddPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'remote');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Gallery Remote'));
    }

    /**
     * @see ItemAddPlugin::isAppropriate
     */
    function isAppropriate() {
	list ($ret, $showItemAdd) =
	    GalleryCoreApi::getPluginParameter('module', 'remote', 'showItemAdd');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $showItemAdd);
    }
}
?>
