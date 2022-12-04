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
 * GetURLs module
 *
 * This module provides an easy way to get formatted HTML/BBcode links for images
 *
 * @package GetURLs
 * @version $Revision: 1045 $
 * @author Kirill Talanov <k.talanov@igloos.ca>
 * @author Andy Staudacher <ast@gmx.ch>
 */
class GetUrlsModule extends GalleryModule {

    function __construct() {
	global $gallery;

	$this->setId('geturls');
	$this->setName('Formatted URLs');
	$this->setDescription($gallery->i18n('Show BBcode/HTML formatted URLs'));
	$this->setVersion('0.9.5');
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getItemLinks|getSiteAdminViews|getItemSummaries');
	$this->setRequiredCoreApi(array(7, 0));
	$this->setRequiredModuleApi(array(3, 0));
    }

    /**
     * @see GalleryModule::upgrade()
     */
    function upgrade($currentVersion, $statusMonitor=null) {
	global $gallery;

	if (!isset($currentVersion)) {
	    /* Initial install.  Register our permissions */
	    $permissions[] = array('view', $gallery->i18n('[URLs] View formatted URLs'));

	    foreach ($permissions as $p) {
		$ret = GalleryCoreApi::registerPermission($this->getId(),
							  'geturls.' . $p[0], $p[1]);
		if ($ret) {
		    return $ret->wrap(__FILE__, __LINE__);
		}
	    }
	}

	/* We also want to set some sane defaults for our parameters */
        list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'geturls');
        if ($ret) {
            return $ret->wrap(__FILE__, __LINE__);
        }

	/* Remove the old params (changed param names and value in v0.9.0) */
	foreach (array('showcodes', 'idreporting' ) as $param) {
	    if (isset($params[$param])) {
		$ret = GalleryCoreApi::removePluginParameterByValue('module', 'geturls',
								    $param, $params[$param]);
		if ($ret) {
		    return $ret->wrap(__FILE__, __LINE__);
		}
	    }
	}

	/* Set default values */
        foreach (
	    array('warnings' => '',
		  'showCodes' => 'HtmlLink|HtmlInline|HtmlThumbnail|BbLink|BbInline|BbThumbnail',
		  'idReporting' => '',
		  'guestMode' => 1,
		  'showItemSummaries' => 0,
		  'itemSummariesWidth' => 25,
		  'showIeLinks' => 1) as $key => $value) {
            if (!isset($params[$key])) {
                $ret = $this->setParameter($key, $value);
                if ($ret) {
                    return $ret->wrap(__FILE__, __LINE__);
                }
            }
	}

	return null;
    }

    /**
     * @see GalleryModule::getItemLinks()
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId) {
	$links = array();
	foreach ($items as $item) {
	    if (isset($permissions[$item->getId()]['geturls.view'])) {
		$params['view'] = 'geturls.GetUrls';
		$params['itemId'] = $item->getId();
		$params['return'] = 1;
		$links[$item->getId()][] =
		    array('text' => $this->translate('Get item URLs'),
			  'params' => $params);
	    }
	}
	
	return array(null, $links);
    }

    /**
     * @see GalleryModule::getItemSummaries()
     */
    function getItemSummaries($items, $permissions, &$template) {
	/* Show itemSummaries or not ? */
	list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'geturls');
	if ($ret) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}
	if (empty($param['showItemSummaries'])) {
	    return array(null, array());
	}

	/* Load the G2 templating engine */
	GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');
	GalleryCoreApi::requireOnce('modules/geturls/classes/GetUrlsHelper.class');

	/** @todo Improve performance by doing the data acquiration in a batch */
	$urlsTemplate = new GalleryTemplate(dirname(__FILE__) . '/../..');
	$urlsTemplate->setVariable('l10Domain', 'modules_' . $this->getId());
	$urlsTemplate->setVariable('isBlock', true);
	$urlsTemplate->setVariable('width', $param['itemSummariesWidth']);
	$urlsTemplate->setVariable('showHeadings', 0);
	$urlsTemplate->setVariable('blockToggle', 1);
	$tpl = 'gallery:modules/geturls/templates/GetUrls.tpl';

	$summaries = array();
	foreach ($items as $item) {
	    if (isset($permissions[$item->getId()]['geturls.view'])) {
		list ($ret, $data) = GetUrlsHelper::getUrlsByItemId($item->getId());
		if ($ret) {
		    return $ret->wrap(__FILE__, __LINE__);
		}
		$urlsTemplate->setVariable('GetUrlsData', $data);

		/* Render and get the html */
		list ($ret, $html) = $urlsTemplate->fetch($tpl);
		if ($ret) {
		    return array($ret->wrap(__FILE__, __LINE__), null);
		}

		$summaries[$item->getId()] = $html;
	    }
	}

	if (!empty($summaries)) {
	    $template->javascript('lib/javascript/BlockToggle.js');
	}

	return array(null, $summaries);
    }

    /**
     * @see GalleryModule::getSiteAdminViews()
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Formatted URLs'),
				 'view' => 'geturls.GetUrlsSiteAdmin')));
    }
}
?>
