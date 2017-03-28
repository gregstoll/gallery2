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
 * This view will present the available options to administer a user
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class UserAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$user = $gallery->getActiveUser();

	/* Load the module list */
	list ($ret, $moduleStatus) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
	    return array($ret, null);
	}

	/* Get a list of all the admin views for each module */
	$subViewChoices = array();
	foreach ($moduleStatus as $moduleId => $status) {
	    if (empty($status['active'])) {
		continue;
	    }

	    /* Get the selected module's admin view */
	    if (in_array('getUserAdminViews', explode('|', $status['callbacks']))) {
		list ($ret, $module) = GalleryCoreApi::loadPlugin('module', $moduleId);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_PLUGIN_VERSION_MISMATCH) {
			continue;
		    }
		    return array($ret, null);
		}

		list ($ret, $moduleViews) = $module->getUserAdminViews($user);
		if ($ret) {
		    return array($ret, null);
		}

		$subViewChoices = array_merge($subViewChoices, $moduleViews);
	    }
	}

	/* If we have a specific sub view, load it now */
	$subViewName = GalleryUtilities::getRequestVariables('subView');
	if ($subViewName == 'core.UserAdmin') {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}
	if (empty($subViewName) && !empty($subViewChoices)) {
	    $subViewName = $subViewChoices[0]['view'];
	}

	list ($ret, $isSiteAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	/* Our sub view may have set some hints, like the encoding type */
	if ($template->hasVariable('UserAdmin')) {
	    $UserAdmin =& $template->getVariableByReference('UserAdmin');
	} else {
	    $UserAdmin = array();
	    $template->setVariableByReference('UserAdmin', $UserAdmin);
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	/* Set up my view parameters */
	$UserAdmin['subViewChoices'] = $subViewChoices;
	$UserAdmin['subViewName'] = $subViewName;
	$UserAdmin['isSiteAdmin'] = $isSiteAdmin;

	if (!empty($subViewName)) {
	    list ($ret, $subView) = GalleryView::loadView($subViewName);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $results) = $subView->loadTemplate($template, $form);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($results['redirect'])) {
		return array(null, $results);
	    }

	    $UserAdmin['viewBodyFile'] = $results['body'];
	    $UserAdmin['viewL10Domain'] = $subView->getL10Domain();
	}

	$template->setVariable('UserAdmin', $UserAdmin);
	$template->title($module->translate('Gallery User Administration'));
	return array(null, array('body' => 'modules/core/templates/UserAdmin.tpl'));
    }

    /**
     * @see GalleryView::isAllowedInMaintenance
     */
    function isAllowedInMaintenance() {
	return (GalleryUtilities::getRequestVariables('subView') == 'core.UserLogin');
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	global $gallery;

	/* Get the description from the current subView */
	$subViewName = GalleryUtilities::getRequestVariables('subView');
	if (empty($subViewName)) {
	    $subViewName = 'core.ItemEdit';
	}
	list ($ret, $subView) = GalleryView::loadView($subViewName);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $description) = $subView->getViewDescription();
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $description);
    }

    /**
     * @see GalleryView::getViewType
     */
    function getViewType() {
	return VIEW_TYPE_ADMIN;
    }
}
?>
