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
 * This plugin will handle storing multilanguage captions for GalleryItems
 * @package MultiLang
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class MultiLangItemEdit extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	global $gallery;
	$status = null;
	$error = array();

	if (isset($form['action']['save'])) {
	    GalleryCoreApi::requireOnce('modules/multilang/classes/MultiLangHelper.class');

	    list ($ret, $rootAlbumId) = GalleryCoreApi::getPluginParameter(
		'module', 'core', 'id.rootAlbum');
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    if (($item->getId() == $rootAlbumId) && empty($form['title'])) {
		$error[] = 'form[error][title][missingRootTitle]';
	    } else {
		list ($ret, $markup) =
		    GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
		if ($ret) {
		    return array($ret, null, null, null);
		}
		if ($markup == 'html') {
		    /* Strip malicious content if html markup allowed */
		    $form['title'] = GalleryUtilities::htmlSafe($form['title'], true);
		    $form['summary'] = GalleryUtilities::htmlSafe($form['summary'], true);
		    $form['description'] = GalleryUtilities::htmlSafe($form['description'], true);
		}

		$ret = MultiLangHelper::setItemData($item, $form['language'],
			$form['title'], $form['summary'], $form['description']);
		if ($ret) {
		    return array($ret, null, null, null);
		}

		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'multilang');
		if ($ret) {
		    return array($ret, null, null, null);
		}
		$status = $module->translate('Changes saved successfully');

		$session =& $gallery->getSession();
		$session->put('multilang.language', $form['language']);
	    }
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	global $gallery;
	$session =& $gallery->getSession();

	GalleryCoreApi::requireOnce('modules/multilang/classes/MultiLangHelper.class');

	if ($form['formName'] != 'MultiLangItemEdit') {
	    $form['formName'] = 'MultiLangItemEdit';
	}
	$form['mainTitle'] = $item->getTitle();
	$form['mainSummary'] = $item->getSummary();
	$form['mainDescription'] = $item->getDescription();
	list ($ret, $form['languageData']) = MultiLangHelper::getItemData($item->getId());
	if ($ret) {
	    return array($ret, null, null);
	}
	/*
	 * If there was an error and the data wasn't saved
	 * then overwrite the values from the database with the ones from the form
	 */
	if (!empty($form['error'])) {
	    $form['languageData'][$form['language']]['title'] = $form['title'];
	    $form['languageData'][$form['language']]['summary'] = $form['summary'];
	    $form['languageData'][$form['language']]['description'] = $form['description'];
	}

	$supportedLanguages = GalleryCoreApi::getSupportedLanguages();
	$languageList = array();
	foreach ($supportedLanguages as $language => $countryList) {
	    foreach ($countryList as $country => $languageData) {
		$languageList[$language . '_' . $country] = $languageData['description'];
	    }
	}
	list ($ret, $languages) =
	    GalleryCoreApi::getPluginParameter('module', 'multilang', 'languages');
	if ($ret) {
	    return array($ret, null, null);
	}
	$languages = empty($languages) ? array() : explode('|', $languages);
	sort($languages);
	foreach (array_keys($form['languageData']) as $language) {
	    /* Show all languages for item, even if no longer in languages list */
	    if (!in_array($language, $languages)) {
		$languages[] = $language;
	    }
	}

	$selectedLanguage = $session->get('multilang.language');
	if (empty($selectedLanguage)) {
	    list ($ret, $selectedLanguage) = $gallery->getActiveLanguageCode();
	    if ($ret) {
		return array($ret, null, null);
	    }
	} else {
	    $session->remove('multilang.language');
	}
	$selectedLanguageIndex = $i = 0;
	foreach ($languages as $language) {
	    $form['languageList'][$language] = isset($languageList[$language])
					     ? $languageList[$language] : $language;
	    if ($language == $selectedLanguage) {
		$selectedLanguageIndex = $i;
	    }
	    $i++;
	}
	$form['selectedLanguage'] = $selectedLanguage;
	$form['selectedIndex'] = $selectedLanguageIndex;

	$template->setVariable('controller', 'multilang.MultiLangItemEdit');
	return array(null,
		     'modules/multilang/templates/MultiLangItemEdit.tpl', 'modules_multilang');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	if (!GalleryUtilities::isA($item, 'GalleryItem')) {
	    return false;
	}
	list ($ret, $languages) =
	    GalleryCoreApi::getPluginParameter('module', 'multilang', 'languages');
	if ($ret || empty($languages)) {
	    return false;
	}
	return true;
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'multilang');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate('MultiLanguage'));
    }
}
?>
