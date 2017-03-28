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

/*
 * Smarty plugin
 * -------------------------------------------------------------
 * Type:     modifier
 * Name:     ireplace
 * Purpose:  Just like Smarty's replace modifier, except case
 *           insensitive and gives you the option to use \\1
 *           to get the original string (useful to preserve case
 *           in the replacement)
 * -------------------------------------------------------------
 */
function smarty_modifier_ireplace($string, $search, $replace) {
    $search = preg_quote($search, '/');
    return preg_replace("/($search)/i", $replace, $string);
}
?>
