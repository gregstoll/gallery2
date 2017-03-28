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
 * The implementation of the Slideshow module
 *
 * @package Slideshow
 * @author Alan Harder <alan.harder@sun.com>
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 18100 $
 */
class SlideshowModule extends GalleryModule {

    function SlideshowModule() {
	global $gallery;
	$this->setId('slideshow');
	$this->setName($gallery->i18n('Slideshow'));
	$this->setDescription($gallery->i18n('Slideshow'));
	$this->setVersion('2.0.0');
	$this->_templateVersion = 1;
	$this->setGroup('display', $gallery->i18n('Display'));
	$this->setCallbacks('getItemLinks|getSiteAdminViews');
	$this->setRequiredCoreApi(array(7, 49));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'SlideshowInterface_1_0', 'SlideshowImpl', 'Slideshow',
	    'modules/slideshow/classes/SlideshowImpl.class', 'slideshow', null);
	if ($ret) {
	    return $ret;
	}

	$ret = GalleryCoreApi::registerFactoryImplementation('GalleryEventListener',
	    'SlideshowModule', 'SlideshowModule',
	    'modules/slideshow/module.inc', 'slideshow',
	    array('Gallery::BeforeDisplay'));
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;
	$platform =& $gallery->getPlatform();

	$dir = $gallery->getConfig('data.gallery.plugins_data') . 'modules/slideshow';
	list ($success) = GalleryUtilities::guaranteeDirExists($dir);
	if (!$success) {
	    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					"Unable to create directory: $dir");
	}

	return null;
    }

    /**
     * @see GalleryModule::activate
     */
    function activate($postActivationEvent=true) {
	global $gallery;

	list ($ret, $uninstalled) = $this->getParameter('piclens.uninstalled');
	if ($ret) {
	    return array($ret, null);
	}

	if (!isset($uninstalled)) {
	    GalleryCoreApi::requireOnce('modules/slideshow/classes/PicLensHelper.class');
	    list ($ret, $version) = PicLensHelper::install();
	    if ($ret && $ret->getErrorCode() & ERROR_STORAGE_FAILURE) {
		return array($ret, null);
	    } else if ($ret) {
		/* Should be safe to swallow this error */
		GalleryCoreApi::addEventLogEntry(
		    'slideshow module', 'Unable to install PicLens',
		    $ret->getAsText());
	    } else {
		$ret = GalleryCoreApi::setPluginParameter(
		    'module', 'slideshow', 'piclens.version', $version);
		if ($ret) {
		    return array($ret, null);
		}
	    }
	}

	return parent::activate($postActivationEvent);
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	list ($ret, $picLensVersion) = $this->getParameter('piclens.version');
	if ($ret) {
	    return array($ret, null);
	}

	$links = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();
	    $jsUrlParams = array('forJavascript' => true, 'forceFullUrl' => true);
	    $jsUrlParamsDirect = array_merge($jsUrlParams, array('forceDirect' => true));
	    if (isset($wantsDetailedLinks[$itemId])) {
		/*
		 * Photo: do slideshow of parent album, start at this photo.
		 * Show View Slideshow link, only when album has images in it.
		 */

		$showSlideshowLink = false;

		if ($item->getCanContainChildren()) {
		    if (!isset($userId)) {
			$userId = $gallery->getActiveUserId();
		    }

		    list ($ret, $query, $data) = GalleryCoreApi::buildItemQuery(
			'GalleryChildEntity', 'id', '[GalleryChildEntity::parentId] = ?', null,
			null, 'GalleryPhotoItem', 'core.view', null, $userId);

		    if (!$ret || !empty($query)) {
			array_unshift($data, $item->getId());
			list ($ret, $searchResults) = $gallery->search($query, $data, null);

			if (!$ret) {
			    $showSlideshowLink = $searchResults->resultCount() > 0;
			}
		    }
		} else {
		    /** @todo Check with isA instead of entityType (thus matching subclasses). */
		    $showSlideshowLink = $item->getEntityType() === 'GalleryPhotoItem';
		}

		if ($showSlideshowLink) {
		    if ($picLensVersion) {
			$script = sprintf(
			    'var p=PicLensLite;'
			    . 'p.setCallbacks({onNoPlugins:function(){location.href=\'%s\'}});'
			    . 'p.setLiteURLs({swf:\'%s\'});'
			    . 'p.start({feedUrl:\'%s\', guid:%d, pid:\'2PWfB4lurT4g\', delay:10})',
			    $urlGenerator->generateUrl(
				array('view' => 'slideshow.Slideshow',
				      'itemId' => $itemId),
				$jsUrlParams),
			    $urlGenerator->generateUrl(
				array('view' => 'slideshow.DownloadPicLens',
				      'file' => 'swf',
				      'v' => $picLensVersion),
				$jsUrlParamsDirect),
			    $urlGenerator->generateUrl(
				array('view' => 'slideshow.SlideshowMediaRss',
				      'itemId' => $item->getId()),
				$jsUrlParams),
				$itemId);
			$links[$itemId][] =
			    array('text' => $this->translate('View Slideshow'),
				  'params' => array('view' => 'slideshow.Slideshow',
						    'itemId' => $itemId),
				  'script' => $script);
		    } else {
			$links[$itemId][] =
			    array('text' => $this->translate('View Slideshow'),
				  'params' => array('view' => 'slideshow.Slideshow',
				  'itemId' => $itemId));
		    }
		}
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null,
		     array(array('name' => $this->translate('Slideshow'),
				 'view' => 'slideshow.AdminSlideshow')));
    }

    /**
     * @see GalleryModule::getRewriteRules
     */
    function getRewriteRules() {
	$rules = array();

	$rule = array();
	$rule['comment'] = $this->translate('View Slideshow');
	$rule['match'] = array('view' => 'slideshow.Slideshow');
	$rule['onLoad'] = array('rewrite', 'RewriteSimpleHelper', 'loadItemIdFromPath');
	$rule['pattern'] = 'v/%path%/slideshow.html';
	$rule['keywords'] = array(
	    'path' => array(
		'pattern' => '([^?]+)',
		'function' => array('rewrite', 'RewriteSimpleHelper', 'parsePath'),
		'help' => $this->translate('Path to an item (eg, /album/image.jpg.html)')));
	$rules[] = $rule;

	return $rules;
    }

    /**
     * @see GalleryModule::handleEvent
     */
    function handleEvent($event) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$data = $event->getData();
	if ($event->getEventName() == 'Gallery::BeforeDisplay'
	        && $data['view']->getViewType() == VIEW_TYPE_SHOW_ITEM) {
	    /**
	     * We store the template in the entity field because we need a reference to it, and
	     * that's the only reference to be had.  Theoretically in PHP5 this problem goes away
	     * and we can store lots of stuff in the data member.  But for now, we store the
	     * template there, and we get the item by leaning on GalleryView.
	     */
	    $template =& $event->getEntity();

	    list ($ret, $item, $ignored) = GalleryView::getItem();
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $picLensVersion) = $this->getParameter('piclens.version');
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($picLensVersion) {
		$template->link(
		    array('rel' => 'alternate',
			  'type' => 'application/rss+xml',
			  'title' => 'Photo RSS',
			  'href' => $urlGenerator->generateUrl(
			      array('view' => 'slideshow.SlideshowMediaRss',
				    'itemId' => $item->getId()))));

		$template->javascript($urlGenerator->generateUrl(
					  array('view' => 'slideshow.DownloadPicLens',
						'file' => 'js',
						'v' => $picLensVersion),
					  array('forceDirect' => true, 'forceFullUrl' => true)));
	    }
	}

	return array(null, null);
    }
}
?>
