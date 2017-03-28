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
 * Settings for MultiLang
 * @package MultiLang
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class MultiLangSiteAdminController extends GalleryController {

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
	    $languages = isset($form['languages']) ? $form['languages'] : array();
	    $ret = GalleryCoreApi::setPluginParameter(
			  'module', 'multilang', 'languages', implode('|', $languages));
	    if ($ret) {
		return array($ret, null);
	    }
	    /* TODO: if removed some language then optionally delete all data for that language? */
	    $status['saved'] = 1;
	} /* else $form['action']['reset'] */

	/* Figure out where to redirect upon success */
	$results['redirect']['view'] = 'core.SiteAdmin';
	$results['redirect']['subView'] = 'multilang.MultiLangSiteAdmin';
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * Settings for MultiLang
 */
class MultiLangSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'MultiLangSiteAdmin') {
	    $form['formName'] = 'MultiLangSiteAdmin';
	    list ($ret, $languages) =
		GalleryCoreApi::getPluginParameter('module', 'multilang', 'languages');
	    if ($ret) {
		return array($ret, null);
	    }
	    $form['languages'] = empty($languages) ? array() : explode('|', $languages);
	}

	$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	$languageList = array();
	foreach ($supportedLanguages as $language => $countryList) {
	    foreach ($countryList as $country => $languageData) {
		$languageList[$language . '_' . $country] = $languageData['description'];
	    }
	}
	$selectedList = array();
	foreach ($form['languages'] as $language) {
	    if (isset($languageList[$language])) {
		$selectedList[$language] = $languageList[$language];
		unset($languageList[$language]);
	    } else {
		$selectedList[$language] = $language;
	    }
	}

	$template->setVariable('MultiLangSiteAdmin', array('availableList' => $languageList,
							   'selectedList' => $selectedList));
	$template->setVariable('controller', 'multilang.MultiLangSiteAdmin');
	return array(null, array('body' => 'modules/multilang/templates/MultiLangSiteAdmin.tpl'));
    }
}
?>
