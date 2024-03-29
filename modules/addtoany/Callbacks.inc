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
 * @package AddToAny
 * @author Karl Newman
 * @author Orestes Sanchez <miceno.atreides@gmail.com>
 */
class AddToAnyCallbacks
{
	function callback($params, &$smarty, $callback, $userId = null)
	{
		switch ($callback) {
			case 'AddToAnyBlockParams':/* Fetch all module parameters and store them in the $block variable */
				list ($ret, $param) = GalleryCoreApi::fetchAllPluginParameters('module', 'addtoany');
				if ($ret) {
					return array($ret, null);
				}
				$block =& $smarty->_tpl_vars['block'];
				foreach ($param as $key => $value) {
					$block['addtoany'][$key] = $value;
				}

				return null;
		}

		return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
	}
} ?>
