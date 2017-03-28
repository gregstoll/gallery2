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
 * Render image block for use in external pages.
 * @package ImageBlock
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17657 $
 */
class ExternalView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isAllowedInEmbedOnly
     */
    function isAllowedInEmbedOnly() {
	return true;
    }

    /**
     * Specify the template instance to use for our immediate view.  Used by our test code.
     * @param GalleryTemplate $template
     */
    function setTemplate(&$template) {
	$this->_template =& $template;
    }

    /**
     * @see GalleryView::loadTemplate
     */
    function renderImmediate($status, $error) {
	GalleryCoreApi::requireOnce('modules/imageblock/classes/ImageBlockHelper.class');
	global $gallery;

	/* Force UrlGenerator to not add session id into urls.. */
	$session =& $gallery->getSession();
	$session->_isUsingCookies = true;

	$frameIds = array();
	$params = array('maxSize' => null, 'exactSize' => null);
	foreach (array('blocks', 'show', 'itemId', 'maxSize', 'exactSize',
		       'link', 'linkTarget', 'itemFrame', 'albumFrame') as $key) {
	    $tmp = GalleryUtilities::getRequestVariables($key);
	    if (!empty($tmp)) {
		$params[$key] = $tmp;
		if ($key == 'itemFrame' || $key == 'albumFrame') {
		    $frameIds[$tmp] = 1;
		}
	    }
	}

	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	if (isset($this->_template)) {
	    $template =& $this->_template;
	} else {
	    $template = new GalleryTemplate(dirname(dirname(dirname(__FILE__))));
	}

	$ret = ImageBlockHelper::loadImageBlocks($template, $params);
	if ($ret) {
	    return $ret;
	}

	if (!empty($frameIds)) {
	    $frameIds = array_keys($frameIds);

	    list ($ret, $imageframe) =
		GalleryCoreApi::newFactoryInstance('ImageFrameInterface_1_1');
	    if ($ret) {
		return $ret;
	    }

	    if (isset($imageframe)) {
		$ret = $imageframe->init($template, $frameIds);
		if ($ret) {
		    return $ret;
		}
	    }
	}

	$phpVm = $gallery->getPhpVm();
	if ($template->hasVariable('ImageBlockData')) {
	    $ImageBlockData =& $template->getVariableByReference('ImageBlockData');
	    $ImageBlockData['forceFullUrl'] = 1;

	    /* Check for rawImage - if requested, send binary image (only one image) */
	    if (isset($params['show']) && $params['show'] === 'rawImage') {
		if (isset($ImageBlockData['blocks'][0]['deriv']['id'])) {
		    GalleryUtilities::putRequestVariable('itemId',
			    $ImageBlockData['blocks'][0]['deriv']['id']);

		    list ($ret, $view) = GalleryView::loadView('core.DownloadItem');
		    if ($ret) {
			return $ret;
		    }
		    return $view->renderImmediate(null, null);
		} else if (!$phpVm->headers_sent()) {
		    $phpVm->header('HTTP/1.0 404 Not Found');
		}
	    } else {
		if (!$phpVm->headers_sent()) {
		    $phpVm->header("Content-type: text/html; charset=UTF-8");
		    $expires = $gallery->getHttpDate($phpVm->time() - 7*24*3600);
		    $phpVm->header('Cache-Control: no-cache');
		    $phpVm->header('Pragma: no-cache');
		    $phpVm->header('Expires: ' . $expires);	    
		}

		$template->setVariable('l10Domain', 'modules_imageblock');
		$template->display('gallery:modules/imageblock/templates/ImageBlock.tpl');
	    }
	}

	return null;
    }
}
?>
