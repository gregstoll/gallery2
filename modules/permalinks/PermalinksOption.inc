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
 * This edit option allows you to create Permalinks for your item
 * @package Permalinks
 * @subpackage UserInterface
 * @author Pierre-Luc Paour <gallery@paour.com>
 * @version $Revision: 17580 $
 */
class PermalinksOption extends ItemEditOption {

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	$warning = $error = array();

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'permalinks');
	if ($ret) {
	    return array($ret, null, null);
	}

	if (!empty($form['PermalinksOption']['aliasName'])) {
	    $ret = PermalinksMapHelper::createAlias($form['PermalinksOption']['aliasName'],
						    $item->getId());
	    if ($ret) {
		if (!($ret->getErrorCode() & ERROR_COLLISION)) {
		    return array($ret, null, null);
		}

		/* Set our error status and fall back to the view. */
		$error[] = 'form[error][PermalinksOption][exists]';
	    } else {
		/* don't do anything if we're successful */
	    }
	}

	if (isset($form['PermalinksOption']['delete'])) {
	    foreach ($form['PermalinksOption']['delete'] as $name => $status) {
		$ret = PermalinksMapHelper::deleteAlias($name);
		if ($ret) {
		    return array($ret, null, null);
		}

		/* don't do anything if we're successful */
	    }
	    $warning[] = $module->translate('Permalink(s) deleted successfully');
	}

	return array(null, $error, $warning);
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	list ($ret, $aliases) = PermalinksMapHelper::fetchAliasesForItem($item->getId());
	if ($ret) {
	    return array($ret, null, null);
	}

	$template->setVariable('PermalinksOption', array('aliases' => $aliases));
	return array(null,
		     'modules/permalinks/templates/PermalinksOption.tpl', 'modules_permalinks');
    }

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	list ($ret, $rootId) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null,
		     GalleryUtilities::isA($item, 'GalleryItem') && $item->getId() != $rootId);
    }
}
?>
