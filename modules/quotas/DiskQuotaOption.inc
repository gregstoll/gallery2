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
 * This ItemAddOption enforces disk quotas
 * @package Quotas
 * @subpackage UserInterface
 * @author Robert Balousek <volksport@users.sf.net>
 * @version $Revision: 17580 $
 */
class DiskQuotaOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	return array(null, true);
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	global $gallery;
	GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');

	$warnings = $errors = array();

	/* user's disk quota in KB */
	list ($ret, $quotaExists, $userDiskQuota) =
	    GalleryQuotasHelper::getUserDiskQuota($gallery->getActiveUserId());
	if ($ret) {
	    return array($ret, null, null);
	}

	/* if user has a quota */
	if ($quotaExists) {
	    $excludedIds = array();
	    foreach ($items as $item) {
		$excludedIds[] = $item->getId();
	    }
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'quotas');
	    if ($ret) {
		return array($ret, null, null);
	    }
	    foreach ($items as $i => $item) {
		$warnings[$i] = array();

		/* we should come here only if we are adding a data item, not sub-album */
		$albumId = $item->getParentId();

		/* calculate the item ids of the newly added items */
		array_shift($excludedIds);

		/* user's disk usage in KiloBytes */
		list ($ret, $userDiskUsage) = GalleryQuotasHelper::getUserDiskUsage(
		    $gallery->getActiveUserId(), $excludedIds);
		if ($ret) {
		    return array($ret, null, null);
		}

		if ($userDiskUsage > $userDiskQuota) {
		    list ($ret, $userDiskQuotaHumanReadable, $userDiskQuotaHumanReadableUnit) =
			GalleryQuotasHelper::humanReadableFromKilobytes($userDiskQuota);
		    if ($ret) {
			return array($ret, null, null);
		    }
		    $warnings[$i][] = $module->translate(
			array('text' => 'Warning: You have reached your disk quota (%s %s), '
					. 'this item will not be added.',
			      'arg1' => $userDiskQuotaHumanReadable,
			      'arg2' => $userDiskQuotaHumanReadableUnit));

		    $gallery->addShutdownAction(
			array('GalleryCoreApi', 'deleteEntityById'), array($item->getId()));
		} else {
		    list ($ret, $userDiskQuotaHumanReadable, $userDiskQuotaHumanReadableUnit) =
			GalleryQuotasHelper::humanReadableFromKilobytes($userDiskQuota);
		    if ($ret) {
			return array($ret, null, null);
		    }
		    list ($ret, $userDiskUsageHumanReadable, $userDiskUsageHumanReadableUnit) =
			GalleryQuotasHelper::humanReadableFromKilobytes($userDiskUsage);
		    if ($ret) {
			return array($ret, null, null);
		    }

		    $warnings[$i][] = $module->translate(
			array('text' => 'You are using %s %s of your allotted %s %s.',
			      'arg1' => $userDiskUsageHumanReadable,
			      'arg2' => $userDiskUsageHumanReadableUnit,
			      'arg3' => $userDiskQuotaHumanReadable,
			      'arg4' => $userDiskQuotaHumanReadableUnit));
		}
	    }
	}
	return array(null, $errors, $warnings);
    }
}
?>
