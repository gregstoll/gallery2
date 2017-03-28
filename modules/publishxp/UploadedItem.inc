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
 * Empty view used when the photo is successfully uploaded.
 * Each photo is uploaded via a seperate HTTP request.  The initial response from
 * this page is to return JavaScript code that instructs Windows how to upload
 * the photos.  Subsequent requests from Windows include the photo data and text.
 * This page processes the data and applies the options to be used while uploading.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */
class UploadedItemView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	/* Views need to either return a body template or a redirect. Just return a dummy here. */
	return array(null, array('redirect' => array('view' => 'core.ShowItem'),
				 'status' => array(), 'error' => array()));
    }
}
?>
