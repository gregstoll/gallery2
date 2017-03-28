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
 * Migrate Module
 *
 * This module provides support for Migrating the Gallery
 *
 * @package Migrate
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 20954 $
 */
class MigrateModule extends GalleryModule /* and GalleryEventListener */ {
    function MigrateModule() {
	global $gallery;

	$this->setId('migrate');
	$this->setName($gallery->i18n('Migration'));
	$this->setDescription($gallery->i18n('Migrate your Gallery 1 albums to Gallery 2'));
	$this->setVersion('1.0.14'); /* Update upgrade() also */
	$this->_templateVersion = 1;
	$this->setGroup('import', $gallery->i18n('Import'));
	$this->setCallbacks('getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 53));
	$this->setRequiredModuleApi(array(3, 6));
    }

    /**
     * @see GalleryModule::install
     *
     * We pushed all this code into its own file since we need it very rarely so it doesn't
     * make sense to load it every single time.
     */
    function upgrade($currentVersion, $statusMonitor) {
	global $gallery;
	$storage =& $gallery->getStorage();

	/* We don't care about anything older than 1.0.4 */
	if (version_compare($currentVersion, '1.0.4', '<=')) {
	    $currentVersion = '1.0.4';
	}

	switch($currentVersion) {
	case '1.0.4':
	    /* Added index */
	    $ret = $storage->configureStore($this->getId(), array('G1MigrateMap:1.0'));
	    if ($ret) {
		return $ret;
	    }

	case '1.0.5':
	case '1.0.6':
	case '1.0.7':
	    /* .mo file migration */
	case '1.0.8':
	case '1.0.9':
	    /* Refactored to new event registration from core API 7.34 */
	case '1.0.10':
	case '1.0.11':
	case '1.0.12':
	case '1.0.13':

	case 'end of upgrade path':
	    /*
	     * Leave this bogus case at the end of the legitimate case statements so that we
	     * always properly terminate our upgrade path with a break.
	     */
	    break;
	}

	return null;
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'GalleryEventListener', 'MigrateModule', 'MigrateModule', 
	    'modules/migrate/module.inc', 'migrate', array('GalleryEntity::delete'));
	if ($ret) {
	    return $ret;
	}
    }

    /**
     * @see GalleryEventListener::handleEvent
     */
    function handleEvent($event) {
	$item = $event->getEntity();
	if (GalleryUtilities::isA($item, 'GalleryItem')) {
	    $ret = GalleryCoreApi::removeMapEntry('G1MigrateMap',
						  array('itemId' => $item->getId()));
	    if ($ret) {
		return array($ret, null);
	    }
	}
	return array(null, null);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	global $gallery;
	return array(null,
		     array(array('name' => $this->translate('Gallery 1'),
				 'view' => 'migrate.SelectGallery')));
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();
	$url = $urlGenerator->generateUrl(array('view' => 'core.SiteAdmin',
						'subView' => 'migrate.SelectGallery'));
	return array(
	    array('comment' => $this->translate('G1-&gt;G2 Mapping'),
		  'pattern' => '%path%',
		  'parser' => 'preGallery',
		  'keywords' => array('path' => array('pattern' => '([^?]+)',
						      'help' => $this->translate('G1 path'))),
		  'queryString' => array('controller' => 'migrate.Redirect'),
		  'locked' => 1,
		  'help' =>$this->translate(
		      array('text' => 'G1 URL redirect. Use only if G2 is installed where G1 used '
			  . 'to be and you have run a G1 import with the URL redirect option '
			  . 'selected. Otherwise follow instructions in %sMigrate Site Admin%s.',
			    'arg1' => '<a href="' . $url . '">', 'arg2' => '</a>'))
	    )
	);
    }
}
?>
