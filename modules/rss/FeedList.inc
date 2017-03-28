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

GalleryCoreApi::requireOnce('modules/rss/classes/RssMapHelper.class');

/**
 * This view will show a list of feeds.
 * @package Rss
 * @subpackage UserInterface
 * @author Jonatan Heyman <http://heyman.info>
 * @author Pierre-Luc Paour
 * @author Daniel Grund <http://www.photogrund.nl>
 * @version $Revision: 17580 $
 */
class FeedListView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	list ($ret, $feeds) = RssMapHelper::fetchFeedsForItem();
	if ($ret) {
	    return array($ret, null);
	}

	$template->setVariable('FeedList', array('feeds' => $feeds));
	$template->setVariable('view', 'rss.FeedList');
	return array(null, array('body' => 'modules/rss/templates/FeedList.tpl'));
    }
}
?>
