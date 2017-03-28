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
 * Show WebDAV mount instructions.
 * @package WebDav
 * @subpackage UserInterface
 * @author Andy Staudacher <ast@gmx.ch>
 * @version $Revision: 17580 $
 */
class WebDavMountView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $canView) = GalleryCoreApi::hasItemPermission($item->getId(), 'core.view');
	if ($ret) {
	    return array($ret, null);
	}
	if (!$canView) {
	    /* Avoid information disclosure, act as if the item didn't exist. */
	    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	}

    	$template->setVariable('item', (array)$item);
	$template->setVariable('isView', true);

	return array(null, array('body' => 'modules/webdav/templates/WebDavMount.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'webdav');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Mount with WebDAV'));
    }
}
?>
