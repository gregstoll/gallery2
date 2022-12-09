<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
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
 * @package map
 * @version $Revision: 1253 $
 *
 */
// Written by Nic Roets. Placed in the public domain.
class mapv3Preloads {
	public function preload(&$template, $preload, $paramSet) {
		static $loaded = false;
		$loaded        = true;
		$itemId        = GalleryUtilities::getRequestVariables('itemId');

		if (!empty($itemId)) {
			list($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId);

			if (!$ret  // extend to all data items ?
				&& GalleryUtilities::isA($item, 'GalleryPhotoItem')
			) {
				GalleryCoreApi::requireOnce('modules/mapv3/classes/mapv3Helper.class');
				list($ret, $tmp, $param, $isContainerSettings) = mapv3Helper::fetchFieldValues(array($item), null, 'photo');

				if ($ret) {
					return $ret->wrap(__FILE__, __LINE__);
				}

				if (isset($tmp[$item->getId()]['GPS'])
					&& preg_match('/^([^,]+),(.*)$/', $tmp[$item->getId()]['GPS'], $regs)
				) {
					$template->head('modules/mapv3/templates/Header.tpl');
					$template->setVariable('latitude', $regs[1]);
					$template->setVariable('longitude', $regs[2]);
					$template->setVariable('itemTitle', $item->getTitle());
				}
			}
		}

		return null;
	}
}
