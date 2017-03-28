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
 * Settings for Link Items
 * @package LinkItem
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class LinkItemSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['save'])) {
	    foreach (array('arrowWatermark', 'urlSnapshot') as $key) {
		$ret = GalleryCoreApi::setPluginParameter(
			'module', 'linkitem', $key, empty($form[$key]) ? 0 : 1);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter(
		    'module', 'linkitem', 'snapshotExec', $form['snapshotExec']);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['saved'] = 1;
	}

	/* Figure out where to redirect upon success */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'linkitem.LinkItemSiteAdmin';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Link Items
 */
class LinkItemSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'LinkItemSiteAdmin') {
	    $form['formName'] = 'LinkItemSiteAdmin';
	    foreach (array('arrowWatermark', 'urlSnapshot', 'snapshotExec') as $key) {
		list ($ret, $form[$key]) =
		    GalleryCoreApi::getPluginParameter('module', 'linkitem', $key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	GalleryCoreApi::requireOnce('modules/linkitem/classes/LinkItemToolkit.class');
	$template->setVariable('LinkItemSiteAdmin',
		array('snapshotParams' => LinkItemToolkit::getSnapshotParams(),
		      'paramsPath' => dirname(__FILE__)
				      . $platform->getDirectorySeparator() . 'params.txt'));

	$template->setVariable('controller', 'linkitem.LinkItemSiteAdmin');
	return array(null, array('body' => 'modules/linkitem/templates/LinkItemSiteAdmin.tpl'));
    }
}
?>
