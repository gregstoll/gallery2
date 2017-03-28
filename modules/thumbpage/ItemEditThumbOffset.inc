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
 * This plugin handles selecting the time offset to use for movie thumbnails
 * @package ThumbPage
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class ItemEditThumbOffset extends ItemEditPlugin {

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
	    list ($ret, $duration) = $this->_getDuration($item);
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    if ($form['offset'] > $duration || $form['offset'] < 0) {
		$error[] = 'form[error][offset][invalid]';
	    }
	}

	if (isset($form['action']['save']) && empty($error)) {
	    list ($ret, $thumbnails) =
		GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $thumb = array_shift($thumbnails);
	    $operations = $thumb->getDerivativeOperations();
	    if ($form['offset'] == 0) {
		$operations = preg_replace('/(^|;)select-offset\|[0-9.]+(;|$)/', '$1', $operations);
	    } else if (preg_match('/(((^|;)select-offset\|)([0-9.]+))(;|$)/', $operations, $regs)) {
		if ($form['offset'] != $regs[4]) {
		    $operations = str_replace($regs[1], $regs[2] . $form['offset'], $operations);
		}
	    } else {
		$operations = 'select-offset|' . $form['offset'] . ';' . $operations;
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
	if ($form['formName'] != 'ItemEditThumbOffset') {
	    $form['formName'] = 'ItemEditThumbOffset';
	    $form['offset'] = '0';
	    if (preg_match('/(^|;)select-offset\|([0-9.]+)/',
			   $thumbnail->getDerivativeOperations(), $regs)) {
		$form['offset'] = $regs[2];
	    }
	}
	list ($ret, $duration) = $this->_getDuration($item);
	if ($ret) {
	    return array($ret, null, null);
	}
	$form['duration'] = sprintf('%.3f', $duration);
	$template->setVariable('controller', 'thumbpage.ItemEditThumbOffset');
	return array(null,
		     'modules/thumbpage/templates/Offset.tpl', 'modules_thumbpage');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	if (GalleryUtilities::isA($item, 'GalleryDataItem') && isset($thumbnail)) {
	    list ($ret, $mimeTypes) =
		GalleryCoreApi::getPluginParameter('module', 'thumbpage', 'movieTypes');
	    if (!$ret) {
		$list = explode('|', $mimeTypes);
		return in_array($item->getMimeType(), $list);
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
     * Get the movie duration
     * @param GalleryDataItem $item
     * @return array GalleryStatus a status code
     *               float duration
     * @access private
     */
    function _getDuration(&$item) {
	list ($ret, $path) = $item->fetchPath();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $toolkit) =
	    GalleryCoreApi::getToolkitByProperty($item->getMimeType(), 'dimensions-and-duration');
	if ($ret) {
	    return array($ret, null);
	}
	if (!isset($toolkit)) {
	    return array(GalleryCoreApi::error(ERROR_TOOLKIT_FAILURE), null);
	}
	list ($ret, $result) = $toolkit->getProperty(
	    $item->getMimeType(), 'dimensions-and-duration', $path);
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $result[2]);
    }
}
?>
