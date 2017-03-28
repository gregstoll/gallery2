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

GalleryCoreApi::requireOnce('modules/webdav/classes/WebDavHelper.class');

/**
 * Show configuration warnings and settings to site admins.
 * @package WebDav
 * @subpackage UserInterface
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 17580 $
 */
class WebDavSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$WebDavSiteAdmin = array();

	list ($ret, $WebDavSiteAdmin['code']) = WebDavHelper::checkConfiguration();
	if ($ret) {
	    return array($ret, null);
	}

	/* Render HTML body */
	$template->setVariable('WebDavSiteAdmin', $WebDavSiteAdmin);

	return array(null, array('body' => 'modules/webdav/templates/WebDavSiteAdmin.tpl'));
    }
}
?>
