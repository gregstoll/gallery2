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
 * Try to clear the browser's authentication cache by as many tricks as possible and redirect to a
 * view which checks that logout was successful.  https://bugzilla.mozilla.org/show_bug.cgi?id=55181
 * @package HttpAuth
 * @subpackage UserInterface
 * @author Jack Bates <ms419@freezone.co.uk>
 * @version $Revision: 17580 $
 */
class TryLogoutView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	/* Ask browser to authenticate with bogus authtype */
	GalleryUtilities::setResponseHeader('HTTP/1.0 401 Unauthorized', false);
	GalleryUtilities::setResponseHeader('WWW-Authenticate: Bogus');

	/* Redirect using random username and password */
	$TryLogout = array();
	foreach (array('scriptUrl', 'hrefUrl') as $key) {
	    $url = $urlGenerator->generateUrl(array('view' => 'httpauth.FinishLogout'),
					      array('forceFullUrl' => true,
						    'htmlEntities' => $key == 'hrefUrl'));
	    $TryLogout[$key] =
		HttpAuthHelper::addHttpAuthToUrl($url, '__LOGOUT__' . rand(), rand());
	}

	/* Render HTML body */
	$template->setVariable('TryLogout', $TryLogout);

	return array(null, array('body' => 'modules/httpauth/templates/TryLogout.tpl'));
    }
}
?>
