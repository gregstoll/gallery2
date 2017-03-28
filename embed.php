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
 * Access point for external application in which Gallery is embedded.
 * See modules/core/classes/GalleryEmbed.class  and
 * http://codex.gallery2.org/index.php/Gallery2:Embedding  for more details.
 *
 * @package Gallery
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17657 $
 */

/* Define G2_EMBED = 1 to remember to generate correct URLs and return the HTML, etc. */
require_once(dirname(__FILE__) . '/modules/core/classes/GalleryDataCache.class');
GalleryDataCache::put('G2_EMBED', 1, true);
require(dirname(__FILE__) . '/main.php');
require(dirname(__FILE__) . '/modules/core/classes/GalleryEmbed.class');

GalleryEmbed::getEmbedPathByHttpRequest();
?>
