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

GalleryCoreApi::requireOnce('modules/dynamicalbum/UpdatesAlbum.inc');

/**
 * Dynamic album view of most viewed items
 * @package DynamicAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class PopularAlbumView extends UpdatesAlbumView {

    function PopularAlbumView() {
	global $gallery;
	$this->_view = 'dynamicalbum.PopularAlbum';
	$this->_title = $gallery->i18n('Most Viewed Items');
	$this->_itemType = array($gallery->i18n('Most Viewed'), $gallery->i18n('most viewed'));
	$this->_viewDescription = $gallery->i18n('most viewed');
	$this->_param = 'views';
    }

    /**
     * Dynamic query for items
     * @param int $userId
     * @return array GalleryStatus a status code
     *               array of item ids
     * @static
     */
    function getChildIds($userId) {
	return parent::getChildIds($userId, 'views', 'viewCount', ORDER_DESCENDING,
				   'GalleryItemAttributesMap', 'itemId');
    }
}
?>
