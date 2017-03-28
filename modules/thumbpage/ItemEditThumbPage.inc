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
 * This plugin handles selecting the page to use for thumbnails
 * @package ThumbPage
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ItemEditThumbPage extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	$status = null;
	$error = array();
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'thumbpage');
	if ($ret) {
	    return array($ret, null, null, null);
	}

	if (isset($form['action']['save'])) {
	    list ($ret, $thumbnails) =
		GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $thumb = array_shift($thumbnails);
	    $operations = $thumb->getDerivativeOperations();
	    if ($form['page'] == 1) {
		$operations = preg_replace('/(^|;)select-page\|\d+(;|$)/', '$1', $operations);
	    } else if (preg_match('/(((^|;)select-page\|)(\d+))(;|$)/', $operations, $regs)) {
		if ($form['page'] != $regs[4]) {
		    $operations = str_replace($regs[1], $regs[2] . $form['page'], $operations);
		}
	    } else {
		$operations = 'select-page|' . $form['page'] . ';' . $operations;
	    }
	    list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($thumb->getId());
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $thumb->setDerivativeOperations($operations);
	    $ret = $thumb->save();
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $ret = GalleryCoreApi::releaseLocks($lockId);
	    if ($ret) {
		return array($ret, null, null, null);
	    }

	    $status = $module->translate('Changes saved successfully');
	} /* else $form['action']['reset']) */

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	if ($form['formName'] != 'ItemEditThumbPage') {
	    $form['formName'] = 'ItemEditThumbPage';
	    $form['page'] = 1;
	    if (preg_match('/(^|;)select-page\|(\d+)/',
			   $thumbnail->getDerivativeOperations(), $regs)) {
		$form['page'] = $regs[2];
	    }
	}
	list ($ret, $count) = $this->_getPageCount($item);
	if ($ret) {
	    return array($ret, null, null);
	}
	$ItemEditThumbPage = array('pageList' => array());
	for ($i = 1; $i <= $count; $i++) {
	    $ItemEditThumbPage['pageList'][$i] = $i;
	}
	$template->setVariable('ItemEditThumbPage', $ItemEditThumbPage);
	$template->setVariable('controller', 'thumbpage.ItemEditThumbPage');
	return array(null,
		     'modules/thumbpage/templates/Page.tpl', 'modules_thumbpage');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	if (GalleryUtilities::isA($item, 'GalleryDataItem') && isset($thumbnail)) {
	    list ($ret, $mimeTypes) =
		GalleryCoreApi::getPluginParameter('module', 'thumbpage', 'pageTypes');
	    if (!$ret) {
		$list = explode('|', $mimeTypes);
		if (in_array($item->getMimeType(), $list)) {
		    list ($ret, $count) = $this->_getPageCount($item);
		    return (!$ret && $count > 1);
		}
	    }
	}
	return false;
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'thumbpage');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate('Thumbnail'));
    }

    /**
     * Get the page-count property
     * @param GalleryDataItem $item
     * @return array GalleryStatus a status code
     *               int page count
     * @access private
     */
    function _getPageCount(&$item) {
	list ($ret, $path) = $item->fetchPath();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $toolkit) =
	    GalleryCoreApi::getToolkitByProperty($item->getMimeType(), 'page-count');
	if ($ret) {
	    return array($ret, null);
	}
	if (!isset($toolkit)) {
	    return array(GalleryCoreApi::error(ERROR_TOOLKIT_FAILURE), null);
	}
	list ($ret, $result) = $toolkit->getProperty($item->getMimeType(), 'page-count', $path);
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $result[0]);
    }
}
?>
