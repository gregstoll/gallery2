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
 * Callbacks for the quotas block
 * @package Quotas
 * @author  Robert Balousek <volksport@users.sf.net>
 * @version $Revision: 17580 $
 */
class QuotasCallbacks {

    /**
     * Load the data for the user's quota and used space into the template.
     * LoadQuotas: This will return various data about the user's usage and quota.
     *             Data includes current size of usage, any user/group quotas and
     *            their associated size, and a percent value of used/unused space.
     *
     * @return GalleryStatus a status code
     */
    function callback($params, &$smarty, $callback, $userId=null) {
	global $gallery;

	$blocks =& $smarty->_tpl_vars['block'];
	$block =& $blocks['quotas']['LoadQuotas'];

	switch($callback) {
	case 'LoadQuotas':

	    GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');
	    /* The user's quota, if any */
	    list ($ret, $block['quotaExists'], $block['quotaValue']) =
		GalleryQuotasHelper::getUserDiskQuota($userId);
	    if ($ret) {
		return $ret;
	    }

	    $userQuotaValueKB = $block['quotaValue'];

	    list ($ret, $block['quotaValue'], $block['quotaValueUnit']) =
		GalleryQuotasHelper::humanReadableFromKilobytes($block['quotaValue']);
	    if ($ret) {
		return $ret;
	    }

	    /* The user's current usage */
	    list ($ret, $block['currentUsage']) = GalleryQuotasHelper::getUserDiskUsage($userId);
	    if ($ret) {
		return $ret;
	    }

	    if ($block['quotaExists']) {
		$block['currentUsagePercent'] =
		    (int) round(($block['currentUsage'] / $userQuotaValueKB) * 100);
		if ($block['currentUsagePercent'] > 100) {
		    $block['currentUsagePercent'] = 100;
		}
		$block['currentUnusedPercent'] = 100 - $block['currentUsagePercent'];
	    }
	    list ($ret, $block['currentUsage'], $block['currentUsageUnit']) =
		GalleryQuotasHelper::humanReadableFromKilobytes($block['currentUsage']);
	    if ($ret) {
		return $ret;
	    }
	    return null;
	}
	return GalleryCoreApi::error(ERROR_BAD_PARAMETER);
    }
}
?>
