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
 * This view will show the availiable site-admin options
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17769 $
 */
class SiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	/**
	 * Make sure we have adequate permissions.
	 * @todo Remove _permissionCheck() since the check is done in main.php now.
	 *       (r17170 obsoleted the check here (from r16894). Keep the assert here of course. 
	 */
	$ret = $this->_permissionCheck(GalleryCoreApi::assertUserIsSiteAdministrator());
	if ($ret) {
	    return array($ret[0], $ret[1]);
	}

	/* Load the module list */
	list ($ret, $moduleStatus) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
	    return array($ret, null);
	}

	/* Get a list of all the admin views for each module */
	$subViewChoices = array();
	ksort($moduleStatus);

	/*
	 * We show all site admin links to the Modules page because it will hide any links for
	 * modules that are not active.
	 */
	$subView = GalleryUtilities::getRequestVariables('subView');
	$showAllSiteAdminViews = ($subView == 'core.AdminPlugins');
	if ($showAllSiteAdminViews) {
	    /* Ensure translations available even for uninstalled plugins */
	    GalleryCoreApi::installTranslationsForLocale();
	}

	foreach ($moduleStatus as $moduleId => $status) {
	    if (empty($status['active']) && !$showAllSiteAdminViews) {
		continue;
	    }

	    /* Get the selected module's admin view */
	    if (in_array('getSiteAdminViews', explode('|', $status['callbacks'])) ||
		$showAllSiteAdminViews) {
		if ($showAllSiteAdminViews) {
		    list ($ret, $module) =
			GalleryCoreApi::loadPlugin('module', $moduleId, true, false);
		    if ($ret) {
			if ($ret->getErrorCode() & ERROR_BAD_PARAMETER) {
			    /* Module is missing or malformed, so ignore it */
			    continue;
			}
			return array($ret, null);
		    }

		    if ($ret) {
			return array($ret, null);
		    }
		} else {
		    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', $moduleId);
		    if ($ret) {
			if ($ret->getErrorCode() & ERROR_PLUGIN_VERSION_MISMATCH) {
			    continue;
			}
			return array($ret, null);
		    }
		}

		list ($ret, $moduleViews) = $module->getSiteAdminViews();
		if ($ret) {
		    return array($ret, null);
		}

		$group = $module->getGroup();
		/**
		 * Pre 2.2, we only accepted a 'view', which we made the subView to
		 * core.SiteAdmin.  In 2.2, we now let them specify extra parameters
		 * so we create a separate 'view' which contains all the url parameters
		 * to form a complete SiteAdmin view (except that we force view to
		 * be core.SiteAdmin).
		 *
		 * TODO: change the API to allow the module to return this form directly
		 * so that we don't have to massage it like this.
		 */
		$currentSubView = GalleryUtilities::getRequestVariables('subView');
		foreach (array_keys($moduleViews) as $i) {
		    $outer = array('name' => $moduleViews[$i]['name']);
		    if (isset($moduleViews[$i]['group'])) {
			$outer['group'] = $moduleViews[$i]['group'];
			unset($moduleViews[$i]['group']);
			$outer['groupLabel'] = $moduleViews[$i]['groupLabel'];
			unset($moduleViews[$i]['groupLabel']);
		    } else {
			$outer['group'] = $group['group'];
			$outer['groupLabel'] = $group['groupLabel'];
		    }

		    $inner = $moduleViews[$i];
		    unset($inner['name']);
		    $inner['subView'] = $inner['view'];
		    $inner['view'] = 'core.SiteAdmin';
		    $outer['view'] = $inner;

		    if ($inner['subView'] == $currentSubView) {
			/* This is probably our selected view */
			$selected = true;
			foreach ($inner as $key => $value) {
			    if ($key == 'view') {
				continue;
			    }
			    if (GalleryUtilities::getRequestVariables($key) != $value) {
				$selected = false;
				break;
			    }
			}
			$outer['selected'] = $selected;
		    }
		    $moduleViews[$i] = $outer;
		}

		$subViewChoices = array_merge($subViewChoices, $moduleViews);
	    }
	}

	/* Sort and group */
	$subViewGroups = $groupLabels = array();
	foreach ($subViewChoices as $adminView) {
	    $group = $adminView['group'];
	    $subViewGroups[$group][] = $adminView;

	    if (!empty($adminView['groupLabel'])) {
		$label = $adminView['groupLabel'];
		if (!isset($groupLabels[$group][$label])) {
		    $groupLabels[$group][$label] = 1;
		} else {
		    $groupLabels[$group][$label]++;
		}
	    }
	}

	/* Set labels */
	foreach (array_keys($subViewGroups) as $group) {
	    $data =& $subViewGroups[$group];
	    usort($data, array($this, 'nameSort'));
	    if (isset($groupLabels[$group])) {
		$tmp = -1;
		foreach ($groupLabels[$group] as $label => $count) {
		    if ($count > $tmp) {
			$tmp = $count;
			$data[0]['groupLabel'] = $label;
		    }
		}
	    } else {
		$data[0]['groupLabel'] = ucwords($data[0]['group']);
	    }
	}
	usort($subViewGroups, array($this, 'groupSort'));

	/* If we have a specific sub view, render it now */
	$subViewName = GalleryUtilities::getRequestVariables('subView');
	if ($subViewName == 'core.SiteAdmin') {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	} else if (empty($subViewName)) {
	    $subViewName = 'core.AdminCore';
	} else if ($subViewName == 'core.AdminModules') {
	    /*
	     * In 2.2, we changed core.AdminModules and core.AdminPlugins.  Remap the name
	     * here to avoid breaking any existing modules.
	     * TODO: Remove this when GalleryCoreApi is at 8.0+
	     */
	    $subViewName = 'core.AdminPlugins';
	}

	list ($ret, $subView) = GalleryView::loadView($subViewName);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $results) = $subView->loadTemplate($template, $form);
	if ($ret) {
	    return array($ret, null);
	}

	/* Our sub view may have set some hints, like the encoding type */
	if ($template->hasVariable('SiteAdmin')) {
	    $SiteAdmin =& $template->getVariableByReference('SiteAdmin');
	} else {
	    $SiteAdmin = array();
	}

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	/* Set up my view parameters */
	$SiteAdmin['subViewGroups'] = $subViewGroups;
	$SiteAdmin['subViewName'] = $subViewName;
	$SiteAdmin['viewBodyFile'] = $results['body'];
	$SiteAdmin['viewL10Domain'] = $subView->getL10Domain();

	$template->setVariable('SiteAdmin', $SiteAdmin);
	$template->title($module->translate('Gallery Site Administration'));
	return array(null, array('body' => 'modules/core/templates/SiteAdmin.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	/* Get the description from the current subView */
	$subViewName = GalleryUtilities::getRequestVariables('subView');
	if (empty($subViewName)) {
	    $subViewName = 'core.AdminCore';
	}
	list ($ret, $subView) = GalleryView::loadView($subViewName);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $description) = $subView->getViewDescription();
	if (!empty($description)) {
	    return array(null, $description);
	}
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	/* Fallback if subView doesn't have a name */
	return array(null, $core->translate('site admin'));
    }

    /**
     * @see GalleryView::getViewType
     */
    function getViewType() {
	return VIEW_TYPE_ADMIN;
    }

    function groupSort($a, $b) {
	static $groupOrder;
	if (!isset($groupOrder)) {
	    /* gallery first, toolkits second, other last */
	    $groupOrder = array('gallery' => 1, 'toolkits' => 2, '' => 3, 'other' => 4);
	}
	$ag = $a[0]['group'];
	$bg = $b[0]['group'];
	$ao = isset($groupOrder[$ag]) ? $groupOrder[$ag] : $groupOrder[''];
	$bo = isset($groupOrder[$bg]) ? $groupOrder[$bg] : $groupOrder[''];
	if ($ao != $bo) {
	    return ($ao > $bo) ? 1 : -1;
	}
	$ag = isset($a[0]['groupLabel']) ? $a[0]['groupLabel'] : $ag;
	$bg = isset($b[0]['groupLabel']) ? $b[0]['groupLabel'] : $bg;
	return ($ag > $bg) ? 1 : -1;
    }

    function nameSort($a, $b) {
	$an = $a['name'];
	$bn = $b['name'];
	if ($an == $bn) {
	    return 0;
	}
	return ($an > $bn) ? 1 : -1;
    }
}
?>
