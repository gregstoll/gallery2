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
 * Dynamic album view of random items!
 * @package DynamicAlbum
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class RandomAlbumView extends UpdatesAlbumView {

    function RandomAlbumView() {
	global $gallery;
	$this->_view = 'dynamicalbum.RandomAlbum';
	$this->_title = $gallery->i18n('Random Items');
	$this->_itemType = array($gallery->i18n('Random'), $gallery->i18n('random'));
	$this->_viewDescription = $gallery->i18n('random');
	$this->_param = 'random';
    }

    /**
     * Dynamic query for items
     * @param int $userId
     * @return array GalleryStatus a status code
     *               array of item ids
     * @static
     */
    function getChildIds($userId) {
	return parent::getChildIds($userId, 'random', 'random', ORDER_ASCENDING, null, 'id');
    }
}
?>
