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

GalleryCoreApi::requireOnce('modules/httpauth/classes/HttpAuthHelper.class');

/**
 * Check that logout was successful and warn the user to close the browser window if not.
 * @package HttpAuth
 * @subpackage UserInterface
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 17580 $
 */
class FinishLogoutView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser();
	if ($ret) {
	    return array($ret, null);
	}

	if ($isAnonymous) {
	    /* Use an absolute return URI to remove user:pass again after the logout */
	    $url = $urlGenerator->generateUrl(array('view' => GALLERY_DEFAULT_VIEW),
						    array('forceFullUrl' => true));

	    return array(null, array('redirectUrl' => HttpAuthHelper::stripHttpAuthFromUrl($url)));
	}

	return array(null, array('body' => 'modules/httpauth/templates/FinishLogout.tpl'));
    }
}
?>
