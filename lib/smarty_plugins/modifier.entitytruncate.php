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
 *
 * NOTE: Portions of this file are copied from the Smarty modifier.truncate.php
 * file and are bound by their license, found in lib/smarty/COPYING.lib
 */

/**
 * Smarty truncate modifier plugin.  This differs from the standard Smarty plugin
 * in that it respects HTML entities and doesn't split them.
 *
 * Type:     modifier<br>
 * Name:     entitytruncate<br>
 * Purpose:  Truncate a string to a certain length if necessary,
 *           optionally splitting in the middle of a word, and
 *           appending the $etc string.  Won't split an HTML entity.
 *
 * @param string $string the input string
 * @param int $length what to truncate it to (max length upon return)
 * @param string $etc what to use to indicate that there was more (default: "...")
 * @param boolean $breakWords break words or not?
 * @return string
 */
function smarty_modifier_entitytruncate($string, $length, $etc='...', $breakWords=false) {
    if (empty($string)) {
	return '';
    }

    /*
     * Convert multibyte characters to html entities and then get an entity-safe substring.
     * Split the string exactly on the boundary.  If there's no change, then we're done.
     */
    $string = GalleryUtilities::utf8ToUnicodeEntities($string);
    list ($tmp, $piece) = GalleryUtilities::entitySubstr($string, 0, $length);
    if ($piece == $string) {
	return GalleryUtilities::unicodeEntitiesToUtf8($piece);
    }

    $etcLength = strlen($etc);
    if ($etcLength < $length) {
	/* Make room for the $etc string */
	list ($tmp, $piece) = GalleryUtilities::entitySubstr($piece, 0, $length - $etcLength);

	$pieceLength = strlen($piece);
	if (!$breakWords && $string{$pieceLength-1} != ' ' && $string{$pieceLength} != ' ') {
	    /* We split a word, and we're not allowed to.  Try to back up to the last space */
	    $splitIndex = strrpos($piece, ' ');
	    if ($splitIndex > 0) {
		/* Found a space, truncate there. */
		$piece = substr($piece, 0, $splitIndex);
	    }
	}
	$piece .= $etc;
    }

    /* Unicode entities back to UTF-8; may convert entities in original string, but that's ok */
    return GalleryUtilities::unicodeEntitiesToUtf8($piece);
}
?>
