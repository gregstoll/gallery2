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
 * Settings for Mime types
 * @package Mime
 * @subpackage UserInterface
 * @author Felix Rabinovich <Felix@Rabinovich.org>
 * @version $Revision: 17580 $
 */
class MimeAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = array();
	if (isset($form['action']['save'])) {
	    $ret = GalleryCoreApi::setPluginParameter('module', 'mime',
						      'allowMime', $form['allowMime']);
	    if ($ret) {
		return array($ret, null);
	    }
	    $mimeList = ($form['allowMime'] == 'all')
		? '' : implode(' | ', array_keys($form['upload']));
	    $ret = GalleryCoreApi::setPluginParameter('module', 'mime', 'mimeList', $mimeList);
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['saved'] = true;
	} else if (isset($form['action']['delete'])) {
	    $mimeType = $form['action']['delete'];
	    $ret = GalleryCoreApi::removeMimeType(array('mimeType' => $mimeType));
	    if ($ret) {
		return array($ret, null);
	    }
	    $status['deleted'] = $mimeType;
	} /* else $form['action']['reset'] */

	$results = array(
		'redirect' => array('view' => 'core.SiteAdmin', 'subView' => 'mime.MimeAdmin'),
		'status' => $status,
		'error' => array());

	return array(null, $results);
    }
}

/**
 * Settings for Mime types
 */
class MimeAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'MimeAdmin') {
	    $form['formName'] = 'MimeAdmin';

	    list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'mime');
	    if ($ret) {
		return array($ret, null);
	    }
	    $form['allowMime'] = $params['allowMime'];
	    $form['upload'] = array();
	    foreach (explode(' | ', $params['mimeList']) as $mime) {
		$form['upload'][$mime] = true;
	    }
	}
	GalleryCoreApi::requireOnce('modules/mime/classes/MimeHelper.class');
	list ($ret, $typeMap) = MimeHelper::getMimeTypeMap();
	if ($ret) {
	    return array($ret, null);
	}

	$MimeAdmin = array();
	foreach (array_keys($typeMap['ext']) as $mimeType) {
	    $ext = implode(', ', $typeMap['ext'][$mimeType]);
	    $MimeAdmin[$mimeType]['ext'] = $ext;
	    $MimeAdmin[$mimeType]['viewable'] = $typeMap['viewable'][$mimeType];
	}

	$template->setVariable('MimeAdmin', $MimeAdmin);
	$template->setVariable('controller', 'mime.MimeAdmin');
	return array(null, array('body' => 'modules/mime/templates/MimeAdmin.tpl'));
    }
}
?>
