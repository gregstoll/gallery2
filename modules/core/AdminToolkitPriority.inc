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
 * This controller adjusts relative toolkit priorities
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class AdminToolkitPriorityController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$status = array();
	if (isset($form['action']['save'])) {
	    list ($ret, $list) = GalleryCoreApi::getRedundantToolkitPriorities();
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($form['priority'] as $priority => $toolkit) {
		if ($priority != $list[$toolkit]) {
		    $ret = GalleryCoreApi::updateMapEntry(
			'GalleryToolkitOperationMimeTypeMap',
			array('toolkitId' => $toolkit), array('priority' => $priority));
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	    GalleryDataCache::removeByPattern('GalleryToolkitHelper::');
	    $status['saved'] = 1;
	} /* else $form['action']['reset'] */

	$results['return'] = 0;
	$results['redirect']['view'] = 'core.SiteAdmin';
	$results['redirect']['subView'] = 'core.AdminToolkitPriority';
	$results['status'] = $status;
	$results['error'] = array();

	return array(null, $results);
    }
}

/**
 * This view will show toolkits and their relative priorities
 */
class AdminToolkitPriorityView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'AdminToolkitPriority') {
	    $form['formName'] = 'AdminToolkitPriority';
	}

	list ($ret, $list) = GalleryCoreApi::getRedundantToolkitPriorities();
	if ($ret) {
	    return array($ret, null);
	}
	asort($list);
	/* Reset all priorities to start of managed-priority window; ensure we break any ties */
	$priority = 20;
	foreach ($list as $toolkit => $tmp) {
	    $list[$toolkit] = $priority++;
	}

	$AdminToolkitPriority = array('list' => $list, 'count' => count($list));
	$template->setVariable('AdminToolkitPriority', $AdminToolkitPriority);
	$template->setVariable('controller', 'core.AdminToolkitPriority');

	return array(null, array('body' => 'modules/core/templates/AdminToolkitPriority.tpl'));
    }
}
?>
