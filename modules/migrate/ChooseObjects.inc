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


GalleryCoreApi::requireOnce('modules/migrate/classes/Gallery1DataParser.class');

/**
 * Form to prepare G1 migration
 * @package Migrate
 * @subpackage UserInterface
 * @author Jesse Mullan <jmullan@visi.com>
 * @version $Revision: 17580 $
 */
class ChooseObjectsController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = $error = array();

	/* Make sure the user selected *something* to migrate */
	if (isset($form['action']['import'])) {
	    if (empty($form['migrateUser']) && empty($form['sourceAlbums'])) {
		$error[] = 'form[error][nothingSelected]';
	    }
	    foreach (array('title', 'summary', 'description') as $field) {
		if ($form['set'][$field] == 'custom' && empty($form['customfield'][$field])) {
		    $error[] = "form[error][emptyCustomField][$field]";
		}
	    }

	    if (!empty($error)) {
		$results['delegate']['view'] = 'core.SiteAdmin';
		$results['delegate']['subView'] = 'migrate.ChooseObjects';
	    } else {
		$results['delegate']['view'] = 'core.SiteAdmin';
		$results['delegate']['subView'] = 'migrate.ConfirmImport';
	    }
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'migrate.ConfirmImport';
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Form to prepare G1 migration
 */
class ChooseObjectsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$defaultLanguage = GalleryTranslator::getLanguageCodeFromRequest();

	$availableEncodings = array('af_ZA Afrikaans (ISO-8859-1)' => 'ISO-8859-1',
				    'bg_BG &#x0411;&#x044a;&#x043b;&#x0433;&#x0430;&#x0440;' .
				    '&#x0441;&#x043a;&#x0438; (CP1251)' => 'CP1251',
				    'ca_ES Catal&#xe0; (ISO-8859-1)' => 'ISO-8859-1',
				    'cs_CZ &#x010c;esky (Czech CP1250)' => 'CP1250',
				    'cs_CZ &#x010c;esky (Czech ISO-8859-2)' => 'ISO-8859-2',
				    'da_DK Dansk (ISO-8859-1)' => 'ISO-8859-1',
				    'de_DE Deutsch (ISO-8859-1)' => 'ISO-8859-1',
				    'en_GB English (UK ISO-8859-1)' => 'ISO-8859-1',
				    'en_US English (USA ISO-8859-1)' => 'ISO-8859-1',
				    'es_ES Espa&#241;ol (ISO-8859-1)' => 'ISO-8859-1',
				    'fi_FI Suomi (ISO-8859-1)' => 'ISO-8859-1',
				    'fr_FR Fran&#231;ais (French ISO-8859-1)' => 'ISO-8859-1',
				    'gl_ES Galego (ISO-8859-1)' => 'ISO-8859-1',
				    'he_IL &#1506;&#1489;&#1512;&#1497;&#1514; (UTF-8)' => 'UTF-8',
				    'hu_HU Magyar (Hungarian ISO-8859-2)' => 'ISO-8859-2',
				    'is_IS &#205;slenska (ISO-8859-1)' => 'ISO-8859-1',
				    'it_IT Italiano (ISO-8859-1)' => 'ISO-8859-1',
				    'ja_JP &#x65e5;&#x672c;&#x8a9e; (EUC-JP)' => 'EUC-JP',
				    'ko_KR &#xd55c;&#xad6d;&#xc5b4; (EUC-KR)' => 'EUC-KR',
				    'lt_LT Lietuvi&#x0173;' => 'ISO-8859-13',
				    'nl_NL Nederlands (ISO-8859-1)' => 'ISO-8859-1',
				    'no_NO Norsk bokm&#229;l (ISO-8859-1)' => 'ISO-8859-1',
				    'pl_PL Polski (ISO-8859-2)' => 'ISO-8859-2',
				    'pt_BR Portugu&#234;s Brasileiro (ISO-8859-1)' => 'ISO-8859-1',
				    'pt_PT Portugu&#234;s (ISO-8859-1)' => 'ISO-8859-1',
				    'ru_RU &#x0420;&#x0443;&#x0441;&#x0441;&#x043a;&#x0438;' .
				    '&#x0439; (CP1251/Windows)' => 'CP1251',
				    'ru_RU &#x0420;&#x0443;&#x0441;&#x0441;&#x043a;&#x0438;' .
				    '&#x0439; (KOI8-R)' => 'KOI8-R',
				    'sl_SI Sloven&#x0161;&#x010d;ina (ISO-8859-2)' => 'ISO-8859-2',
				    'sv_SE Svenska (ISO-8859-1)' => 'ISO-8859-1',
				    'tr_TR T&#252;rk&#231;e (ISO-8859-9)' => 'ISO-8859-9',
				    'uk_UA &#x0423;&#x043a;&#x0440;&#x0430;&#x0457;&#x043d;' .
				    '&#x0441;&#x044c;&#x043a;&#x0430; (KOI8-U)' => 'KOI8-U',
				    'vi_VN Ti&#7871;ng Vi&#7879;t Nam (UTF-8)' => 'UTF-8',
				    'zh_CN &#31616;&#20307;&#20013;&#25991; (Simplified Chinese ' .
				    'GB/GB2312)' => 'GB2312',
				    'zh_TW &#32321;&#39636;&#20013;&#25991; (Traditional Chinese ' .
				    'Big5)' => 'BIG5',
				    'zh_TW &#32321;&#39636;&#20013;&#25991; (Traditional Chinese ' .
				    'UTF-8)' => 'UTF-8',
				    'BIG5' => 'BIG5',
				    'CP1250' => 'CP1250',
				    'CP1251' => 'CP1251',
				    'CP1257' => 'CP1257',
				    'EUC-JP' => 'EUC-JP',
				    'EUC-KR' => 'EUC-KR',
				    'GB2312' => 'GB2312',
				    'ISO-8859-1' => 'ISO-8859-1',
				    'ISO-8859-10' => 'ISO-8859-10',
				    'ISO-8859-11' => 'ISO-8859-11',
				    'ISO-8859-13' => 'ISO-8859-13',
				    'ISO-8859-14' => 'ISO-8859-14',
				    'ISO-8859-15' => 'ISO-8859-15',
				    'ISO-8859-16' => 'ISO-8859-16',
				    'ISO-8859-2' => 'ISO-8859-2',
				    'ISO-8859-3' => 'ISO-8859-3',
				    'ISO-8859-4' => 'ISO-8859-4',
				    'ISO-8859-5' => 'ISO-8859-5',
				    'ISO-8859-6' => 'ISO-8859-6',
				    'ISO-8859-7' => 'ISO-8859-7',
				    'ISO-8859-8' => 'ISO-8859-8',
				    'ISO-8859-9' => 'ISO-8859-9',
				    'KOI8-R' => 'KOI8-R',
				    'KOI8-U' => 'KOI8-U',
				    'UTF-8' => 'UTF-8',
				    'windows-1251' => 'CP1251',
				    'windows-1257' => 'CP1257',
				    'Latin-1' => 'ISO-8859-1'
				    );
	$possibleEncodingMatch = '';
	foreach ($availableEncodings as $encodingName => $encoding) {
	    if (strpos($encodingName, $defaultLanguage)!==false) {
		$possibleEncodingMatch = $encodingName;
	    }
	}
	$platform =& $gallery->getPlatform();
	$slash = $platform->getDirectorySeparator();

	$albumsPath = GalleryUtilities::getRequestVariables('albumsPath');
	if ($albumsPath == null) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	$existingUsers = $newUsers = array();
	list ($ret, $allUsers) = Gallery1DataParser::getUserUids($albumsPath);
	if ($ret) {
	    return array($ret, null);
	}

	foreach ($allUsers as $uid => $userName) {
	    list ($ret, $testUser) = GalleryCoreApi::fetchUserByUsername($userName);
	    if ($ret && !($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		return array($ret, 'Your userdb.dat includes '
			     . $userName . ' but doesn\'t include the associated file: ' . $uid);
	    }

	    if ($testUser) {
		$existingUsers[$uid] = $allUsers[$uid];
	    } else {
		$newUsers[$uid] = $allUsers[$uid];
	    }
	}

	if ($form['formName'] == 'ChooseObjects') {
	    /* Init the checkboxes to not-checked */
	    foreach ($newUsers as $uid => $username) {
		if (!isset($form['migrateUser'][$uid])) {
		    $form['migrateUser'][$uid] = null;
		}
	    }
	} else {
	    $form['formName'] = 'ChooseObjects';

	    /* Init the checkboxes to not-checked */
	    foreach ($newUsers as $uid => $username) {
		$form['migrateUser'][$uid] = null;
	    }
	    $form['destinationAlbumID'] = '';
	    $form['urlRedirect'] = 1;
	    $form['generateThumbnails'] = 1;
	    $form['set'] = array('title' => 'caption',
				 'summary' => 'blank', 'description' => 'blank');
	    $form['customFields'] = $form['skipCustomItemFields'] = 1;
	}
	list ($ret, $g1AlbumTree) = $this->_createG1AlbumTree($albumsPath);
	if ($ret) {
	    return array($ret, null);
	}

	/* Get ids of all albums where we can add new album items */
	list ($ret, $albumIds) =
	    GalleryCoreApi::fetchAllItemIds('GalleryAlbumItem', 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load all the album entities */
	list ($ret, $albums) = GalleryCoreApi::loadEntitiesById($albumIds, 'GalleryAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}
	$g2AlbumTree = GalleryUtilities::createAlbumTree($albums);

	/* See if customfield module is available */
	list ($ret, $customFieldInterface) =
	    GalleryCoreApi::newFactoryInstance('CustomFieldInterface_1_0');
	if ($ret) {
	    return array($ret, null);
	}

	$ChooseObjects['albumsPath'] = $albumsPath;
	$ChooseObjects['newUsers'] = $newUsers;
	$ChooseObjects['existingUsers'] = $existingUsers;
	$ChooseObjects['g1AlbumTree'] = $g1AlbumTree;
	$ChooseObjects['g2AlbumTree'] = $g2AlbumTree;
	$ChooseObjects['availableEncodings'] = $availableEncodings;
	$ChooseObjects['possibleEncodingMatch'] = $possibleEncodingMatch;
	$ChooseObjects['customFieldsActive'] = isset($customFieldInterface);

	$template->setVariable('ChooseObjects', $ChooseObjects);
	$template->setVariable('controller', 'migrate.ChooseObjects');
	return array(null, array('body' => 'modules/migrate/templates/ChooseObjects.tpl'));
    }

    /**
     * Extract all G1 albums and turn it into a depth tree suitable for display
     * in a hierarchical format.
     * @access private
     */
    function _createG1AlbumTree($albumsPath) {

	list ($ret, $albumList) = Gallery1DataParser::getAlbumList($albumsPath);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $hierarchy) = Gallery1DataParser::getAlbumHierarchy($albumsPath);
	if ($ret) {
	    return array($ret, null);
	}

	$tree = array();
	foreach ($hierarchy as $albumName => $subHierarchy) {
	    $tree = array_merge(
		$tree, $this->_createG1DepthTree($albumList, $albumName, $subHierarchy));
	}

	return array(null, $tree);
    }

    /**
     * Recursively walk a parent/child map and build the depth tree.
     * @access private
     */
    function _createG1DepthTree(&$albumList, $albumName, $hierarchy, $depth=0) {
	global $gallery;

	$data = array();
	$data[] = array('depth' => $depth, 'data' => $albumList[$albumName]);
	foreach ($hierarchy as $childAlbumName => $subHierarchy) {
	    $gallery->guaranteeTimeLimit(30);
	    $data = array_merge($data, $this->_createG1DepthTree($albumList,
								 $childAlbumName, $subHierarchy,
								 $depth + 1));
	}

	return $data;
    }
}
?>
