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
 * Settings for Icons
 * @package Icons
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class IconsSiteAdminController extends GalleryController {

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
	    if (!isset($form['iconpack'])) {
		$error[] = 'form[error][iconpack][missing]';
	    } else {
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'icons', 'iconpack', $form['iconpack']);
		if ($ret) {
		    return array($ret, null);
		}
		$status['saved'] = 1;
	    }
	}

	/* Figure out where to redirect upon success */
	$method = empty($error) ? 'redirect' : 'delegate';
	$results[$method]['view'] = 'core.SiteAdmin';
	$results[$method]['subView'] = 'icons.IconsSiteAdmin';
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Icons
 */
class IconsSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'IconsSiteAdmin') {
	    $form['formName'] = 'IconsSiteAdmin';
	    list ($ret, $form['iconpack']) =
		GalleryCoreApi::getPluginParameter('module', 'icons', 'iconpack');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $icons) = GalleryCoreApi::newFactoryInstance('IconsInterface_1_0');
	if ($ret) {
	    return array($ret, null);
	}
	if (isset($icons)) {
	    list ($ret, $list) = $icons->getIconPacks();
	    if ($ret) {
		return array($ret, null);
	    }

	    $platform =& $gallery->getPlatform();
	    $packs = array();
	    $classes = array();
	    foreach ($list as $dir => $name) {
		if (empty($dir)) {
		    continue;
		}

		$css = $platform->file(
		    sprintf('%s/iconpacks/%s/icons.css', dirname(__FILE__), $dir));
		$map = array();
		$active = array();
		foreach ($css as $line) {
		    if (preg_match_all('/(?:gbLink|icon(?=-plugin))-(.*?)[ {,]/',
				       $line, $matches)) {
			$active = array_merge($active, $matches[1]);
		    }
		    if (preg_match('/url\((.*)\)/', $line, $matches)) {
			$image = $matches[1];
		    }
		    if (strpos($line, '}') !== false) {
			if ($image) {
			    foreach (array_flip($active) as $class => $ignored) {
				$map[$class] = $image;
				$classes[$class] = 1;
			    }
			}
			$active = array();
			$image = null;
		    }
		}
		$packs[$dir] = array('name' => $name, 'map' => $map);
	    }
	}

	ksort($classes);

	$IconsSiteAdmin = array();
	$IconsSiteAdmin['packs'] = $packs;
	$IconsSiteAdmin['classes'] = $classes;
	$IconsSiteAdmin['iconpacks'] = $list;

	$template->setVariable('IconsSiteAdmin', $IconsSiteAdmin);
	$template->setVariable('controller', 'icons.IconsSiteAdmin');
	return array(null, array('body' => 'modules/icons/templates/IconsSiteAdmin.tpl'));
    }
}
?>
