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

GalleryCoreApi::requireOnce('modules/permalinks/classes/PermalinksMapHelper.class');

/**
 * Settings for Permalinks
 * @package Permalinks
 * @subpackage UserInterface
 * @author Pierre-Luc Paour <gallery@paour.com>
 * @version $Revision: 17580 $
 */
class PermalinksSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'permalinks');
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($form['PermalinksSiteAdmin']['delete'])) {
	    foreach ($form['PermalinksSiteAdmin']['delete'] as $id => $id1) {
		$ret = PermalinksMapHelper::deleteAlias($id);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['PermalinksSiteAdmin']['success'] =
		$module->translate('Permalink(s) deleted successfully');
	}

	list ($ret, $autoPermalink) =
	    GalleryCoreApi::getPluginParameter('module', 'permalinks', 'autoPermalink');
	if ($ret) {
	    return array($ret, null);
	}

	$autoPermalink = isset($autoPermalink) ? $autoPermalink : 0;
	$newAutoPermalink = isset($form['PermalinksSiteAdmin']['autoPermalink']) ? 1 : 0;

	if ($autoPermalink != $newAutoPermalink) {
	    $ret = GalleryCoreApi::setPluginParameter('module', 'permalinks',
						      'autoPermalink', $newAutoPermalink);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Figure out where to redirect upon success */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'permalinks.PermalinksSiteAdmin';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Permalinks
 */
class PermalinksSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$PermalinksSiteAdmin = array();
	list ($ret, $PermalinksSiteAdmin['aliases']) = PermalinksMapHelper::fetchAliasesForItem();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $PermalinksSiteAdmin['autoPermalink']) =
	    GalleryCoreApi::getPluginParameter('module', 'permalinks', 'autoPermalink');
	if ($ret) {
	    return array($ret, null);
	}

	$template->setVariable('PermalinksSiteAdmin', $PermalinksSiteAdmin);
	$template->setVariable('controller', 'permalinks.PermalinksSiteAdmin');
	return array(null, array('body' => 'modules/permalinks/templates/PermalinksSiteAdmin.tpl'));
     }
}
?>
