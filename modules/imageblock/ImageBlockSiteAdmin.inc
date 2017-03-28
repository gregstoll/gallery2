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
 * Settings for Image Block
 * @package ImageBlock
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ImageBlockSiteAdminController extends GalleryController {

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
	    $param = array();
	    foreach (array('heading', 'title', 'date', 'views', 'owner') as $key) {
		if (isset($form[$key]) && $form[$key]) {
		    $param[] = $key;
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter('module', 'imageblock',
						      'show', implode('|', $param));
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach (array('albumFrame', 'itemFrame') as $key) {
		if (isset($form[$key])) {
		    $ret = GalleryCoreApi::setPluginParameter('module', 'imageblock',
							      $key, $form[$key]);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	    $status['saved'] = 1;
	} /* else $form['action']['reset'] */

	/* Figure out where to redirect upon success */
	$results['redirect']['view'] = 'core.SiteAdmin';
	$results['redirect']['subView'] = 'imageblock.ImageBlockSiteAdmin';
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * Settings for Image Block
 */
class ImageBlockSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ImageBlockSiteAdmin') {
	    $form['formName'] = 'ImageBlockSiteAdmin';
	    foreach (array('heading', 'title', 'date', 'views', 'owner') as $tmp) {
		$form[$tmp] = 0;
	    }
	    list ($ret, $param) =
		GalleryCoreApi::fetchAllPluginParameters('module', 'imageblock');
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach (explode('|', $param['show']) as $tmp) {
		$form[$tmp] = 1;
	    }
	    $form['albumFrame'] = $param['albumFrame'];
	    $form['itemFrame'] = $param['itemFrame'];
	}
	$ImageBlockSiteAdmin = array('prefix' => GALLERY_FORM_VARIABLE_PREFIX);

	/* ImageFrame settings, if available */
	list ($ret, $imageframe) = GalleryCoreApi::newFactoryInstance('ImageFrameInterface_1_1');
	if ($ret) {
	    return array($ret, null);
	}
	if (isset($imageframe)) {
	    list ($ret, $ImageBlockSiteAdmin['list']) = $imageframe->getImageFrameList();
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $ImageBlockSiteAdmin['sampleUrl']) = $imageframe->getSampleUrl();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$template->setVariable('ImageBlockSiteAdmin', $ImageBlockSiteAdmin);
	$template->setVariable('controller', 'imageblock.ImageBlockSiteAdmin');
	return array(null, array('body' => 'modules/imageblock/templates/ImageBlockSiteAdmin.tpl'));
    }
}
?>
