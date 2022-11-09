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
 * Settings for Share module
 * @package Share
 * @subpackage UserInterface
 * @author Karl Newman
 * @version $Revision: 1907 $
 */
class ShareSiteAdminController extends GalleryController
{

	/**
	 * @see GalleryController::handleRequest
	 */
	function handleRequest($form)
	{
		global $gallery;

		/* Make sure the user has permission to do this operation */
		$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
		if ($ret) {
			return array($ret, null);
		}

		$status = $results = $error = array();

		if (isset($form['action']['save'])) {
			/* save parameters */
			foreach (array('onlyWhenLoggedIn', 'addThisAccountId') as $setting) {
				$ret = GalleryCoreApi::setPluginParameter('module', 'share', $setting,
					(isset($form[$setting]) && $form[$setting]) ? $form[$setting] : 0);
				if ($ret) {
					return array($ret, null);
				}
			}
			$status['saved'] = 1;

		} elseif (isset($form['action']['reset'])) {
			/* do nothing */
		} else {
			$error[] = 'form[error][unknownAction]';
		}

		/* figure out the method we will use */
		$method                      = empty($error) ? 'redirect' : 'delegate';
		$results['status']           = $status;
		$results['error']            = $error;
		$results[$method]['view']    = 'core.SiteAdmin';
		$results[$method]['subView'] = 'share.ShareSiteAdmin';

		return array(null, $results);
	}
}

/**
 * Settings for Share link blocks
 */
class ShareSiteAdminView extends GalleryView
{

	/**
	 * @see GalleryView::loadTemplate
	 */
	function loadTemplate(&$template = NULL, &$form = NULL)
	{
		$form['formName'] = 'ShareSiteAdmin';

		$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
		if ($ret) {
			return array($ret, null);
		}

		/* fetch module parameters and save them in form variable */
		list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'share');
		if ($ret) {
			return array($ret, null);
		}
		foreach ($param as $key => $value) {
			$form[$key] = $value;
		}

		$template->setVariable('controller', 'share.ShareSiteAdmin');

		return array(null, array('body' => 'modules/share/templates/ShareSiteAdmin.tpl'));
	}
}

?>
