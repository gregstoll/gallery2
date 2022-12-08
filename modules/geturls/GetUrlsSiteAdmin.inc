<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
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
 * Administration controller
 * @package GetURLs
 * @subpackage UserInterface
 * @author Andy Staudacher <ast@gmx.ch>
 */
class GetUrlsSiteAdminController extends GalleryController {
    
    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	/* Make sure we have adequate permissions */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}
	
	$status = array();
	if (isset($form['action']['save'])) {
	    $param = array();
	    foreach (array('suppressSourceWarning', 'suppressResizedWarning')
		     as $key) {
		if (isset($form[$key]) && $form[$key]) {
		    $param[] = $key;
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter('module', 'geturls',
						      'warnings', implode('|', $param));
	    if ($ret) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }
		
	    $param = array();
	    foreach (array('HtmlLink', 'HtmlInline', 'HtmlThumbnail', 'HtmlResize2Full',
			   'BbLink', 'BbInline', 'BbThumbnail', 'BbResize2Full')
		     as $key) {
		if (isset($form[$key]) && $form[$key]) {
		    $param[] = $key;
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter('module', 'geturls',
						      'showCodes', implode('|', $param));
	    if ($ret) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }
		
	    $param = array();
	    foreach (array('MiscItemId', 'MiscResizeId', 'MiscThumbnailId')
		     as $key) {
		if (isset($form[$key]) && $form[$key]) {
		    $param[] = $key;
		}
	    }
	    $ret = GalleryCoreApi::setPluginParameter('module', 'geturls',
						      'idReporting', implode('|', $param));
	    if ($ret) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }

	    foreach (array('guestMode', 'showItemSummaries', 'showIeLinks') as $param) {
		if (!isset($form[$param]) || empty($form[$param])) {
		    $value = 0;
		} else {
		    $value = 1;
		}
		$ret = GalleryCoreApi::setPluginParameter('module', 'geturls',
							  $param, $value);
		if ($ret) {
		    return array($ret->wrap(__FILE__, __LINE__), null);
		}
	    }
	    if (isset($form['itemSummariesWidth']) &&
		    is_numeric($form['itemSummariesWidth']) ) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'geturls', 'itemSummariesWidth',
							  $form['itemSummariesWidth']);
		if ($ret) {
		    return array($ret->wrap(__FILE__, __LINE__), null);
		}
	    }
	    $status['saved'] = 1;
	} /* else $form['action']['reset'] */

	/* Figure out where to redirect upon success */
	$results['redirect']['view'] = 'core.SiteAdmin';
	$results['redirect']['subView'] = 'geturls.GetUrlsSiteAdmin';
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * Administration view
 * @package GetURLs
 * @subpackage UserInterface
 */
class GetUrlsSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	foreach (array('suppressSourceWarning', 'suppressResizedWarning', 'HtmlLink', 'HtmlInline',
		       'HtmlThumbnail', 'HtmlResize2Full', 'BbLink', 'BbInline', 'BbThumbnail',
		       'BbResize2Full', 'MiscItemId', 'MiscResizeId', 'MiscThumbnailId',
		       'guestMode', 'showItemSummaries', 'showIeLinks', 'itemSummariesWidth')
		 as $param) {
	    /* Checkbox values are not defined if false */
	    $form[$param] = empty($form[$param]) ? 0 : $form[$param];
	}

	$getUrlsAdmin['itemSummariesWidthList'] = array(10, 15, 20, 25, 30, 40, 50, 60, 70, 85, 100,
							120, 150);

	/* Load defaults */
	if ($form['formName'] != 'GetUrlsSiteAdmin') {
	    $form['formName'] = 'GetUrlsSiteAdmin';

	    list ($ret, $param) =
		GalleryCoreApi::fetchAllPluginParameters('module', 'geturls');
	    if ($ret) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }
	    foreach (explode('|', $param['warnings']) as $tmp) {
		$form[$tmp] = 1;
	    }
	    foreach (explode('|', $param['showCodes']) as $tmp) {
		$form[$tmp] = 1;
	    }
	    foreach (explode('|', $param['idReporting']) as $tmp) {
		$form[$tmp] = 1;
	    }
	    $form['guestMode'] = $param['guestMode'];
	    $form['showItemSummaries'] = $param['showItemSummaries'];
	    $form['itemSummariesWidth'] = $param['itemSummariesWidth'];
	    $form['showIeLinks'] = $param['showIeLinks'];
	}

	$template->setVariable('controller', 'geturls.GetUrlsSiteAdmin');
	$template->setVariable('GetUrlsAdmin', $getUrlsAdmin);
	return array(null,
		     array('body' => 'modules/geturls/templates/GetUrlsSiteAdmin.tpl'));
    }
}
?>