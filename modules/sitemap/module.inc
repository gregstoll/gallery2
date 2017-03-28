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
 * Module that creates a Google Sitemap for your Gallery2 install.
 * @package Sitemap
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 20954 $
 */
class SitemapModule extends GalleryModule {

    function SitemapModule() {
	global $gallery;
	$this->setId('sitemap');
	$this->setName($gallery->i18n('Sitemap'));
	$this->setDescription($gallery->i18n('Generate a Google Sitemap for your Gallery'));
	$this->setVersion('1.0.6');
	$this->_templateVersion = 1;
	$this->setGroup('export', $gallery->i18n('Export'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 20));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	$data = array(array('name' => $this->translate('Google Sitemap'),
			    'view' => 'sitemap.AdminSitemap'));
	return array(null, $data);
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	$rules = array();

	$rule = array();
	$rule['match'] = array('view' => 'sitemap.Sitemap');
	$rule['pattern'] = 'sitemap';
	$rule['comment'] = $this->translate('Download Sitemap');
	$rules[] = $rule;

	return $rules;
    }
}
?>
