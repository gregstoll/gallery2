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
 * Settings for Item Add
 * @package ItemAdd
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ItemAddSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'itemadd');
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();
	if (isset($form['action']['addUploadLocalServerDir'])) {
	    $path = $form['uploadLocalServer']['newDir'];
	    GalleryUtilities::unsanitizeInputValues($path, false);
	    /* $path is UTF-8, $realPath is in the system charset for filesystem interactions */
	    $realPath = GalleryCoreApi::convertFromUtf8($path);
	    if (empty($path)) {
		$error[] = 'form[error][uploadLocalServer][newDir][missing]';
	    } else {
		$platform =& $gallery->getPlatform();
		$splitPath = $platform->splitPath($realPath);
		if (!preg_match('{[\\/]}', $splitPath[0]) || in_array('..', $splitPath)) {
		    /* Use realpath value if path is relative or has .. */
		    $realPath = $platform->realpath($realPath);
		    $path = GalleryCoreApi::convertToUtf8($realPath);
		}

		if ($platform->isRestrictedByOpenBaseDir($realPath)) {
		    $error[] = 'form[error][uploadLocalServer][newDir][restrictedByOpenBaseDir]';
		} else if (!$platform->is_readable($realPath)) {
		    $error[] = 'form[error][uploadLocalServer][newDir][notReadable]';
		} else if (!$platform->is_dir($realPath)) {
		    $error[] = 'form[error][uploadLocalServer][newDir][notADirectory]';
		}
	    }

	    /* Store the path in UTF-8 in the database */
	    if (empty($error)) {
		for ($i = 1; !empty($params['uploadLocalServer.dir.' . $i])
			  && ($params['uploadLocalServer.dir.' . $i] != $path); $i++) { }

		if (empty($params['uploadLocalServer.dir.' . $i])) {
		    $ret = GalleryCoreApi::setPluginParameter(
			'module', 'itemadd', 'uploadLocalServer.dir.' . $i, $path);
		    if ($ret) {
			return array($ret, null);
		    }
		    $status['addedDir'] = 1;
		}
	    }
	} else if (isset($form['action']['removeUploadLocalServerDir'])) {
	    if (!empty($form['uploadLocalServer']['selectedDir'])) {
		for ($i = $form['uploadLocalServer']['selectedDir'] + 1;
			!empty($params['uploadLocalServer.dir.' . $i]); $i++) {
		    $ret = GalleryCoreApi::setPluginParameter('module', 'itemadd',
			    'uploadLocalServer.dir.' . ($i - 1),
			    $params['uploadLocalServer.dir.' . $i]);
		    if ($ret) {
			return array($ret, null);
		    }
		}
		$ret = GalleryCoreApi::removePluginParameter('module', 'itemadd',
							     'uploadLocalServer.dir.' . ($i - 1));
		if ($ret) {
		    return array($ret, null);
		}
		$status['removedDir'] = 1;
	    } else {
		$error[] = 'form[error][uploadLocalServer][selectedDir][missing]';
	    }
	}
	if (isset($form['action']['save']) || isset($status['addedDir'])) {
	    foreach (array('fromweb', 'fromserver') as $key) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'itemadd', $key, $form[$key]);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    $status['saved'] = 1;
	} /* else $form['action']['reset'] */

	$method = empty($error) ? 'redirect' : 'delegate';
	$results = array($method => array('view' => 'core.SiteAdmin',
					  'subView' => 'itemadd.ItemAddSiteAdmin'),
			 'status' => $status,
			 'error' => $error);

	return array(null, $results);
    }
}

/**
 * Settings for Item Add
 */
class ItemAddSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'itemadd');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ItemAddSiteAdmin') {
	    $form['formName'] = 'ItemAddSiteAdmin';
	    $form['uploadLocalServer']['newDir'] = '';
	    foreach (array('fromweb', 'fromserver') as $key) {
		list ($ret, $form[$key]) = $module->getParameter($key);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'itemadd');
	if ($ret) {
	    return array($ret, null);
	}
	$localServerDirList = array();
	for ($i = 1; !empty($params['uploadLocalServer.dir.' . $i]); $i++) {
	    $localServerDirList[$i] = $params['uploadLocalServer.dir.' . $i];
	}

	$ItemAddSiteAdmin = array(
		'optionList' => array('on' =>
		    $module->translate('Available to all users with permission to add items'),
		    'admin' => $module->translate('Site Admins only'),
		    'off' => $module->translate('Disabled')
		    ),
		'localServerDirList' => $localServerDirList,
		);

	$template->setVariable('ItemAddSiteAdmin', $ItemAddSiteAdmin);
	$template->setVariable('controller', 'itemadd.ItemAddSiteAdmin');
	return array(null, array('body' => 'modules/itemadd/templates/ItemAddSiteAdmin.tpl'));
    }
}
?>
