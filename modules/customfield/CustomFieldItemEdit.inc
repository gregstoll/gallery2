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

GalleryCoreApi::requireOnce('modules/customfield/classes/CustomFieldHelper.class');

/**
 * This plugin will handle assigning values to custom fields
 * @package CustomField
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class CustomFieldItemEdit extends ItemEditPlugin {

    /**
     * @see ItemEditPlugin::handleRequest
     */
    function handleRequest($form, &$item, &$preferred) {
	$status = null;
	$error = array();
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'customfield');
	if ($ret) {
	    return array($ret, null, null, null);
	}

	if (isset($form['cfAdmin'])) {
	    list ($ret, $adminStatus) = CustomFieldHelper::handleAdminAction($form, $item->getId());
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    foreach ($adminStatus as $key => $tmp) {
		switch ($key) {
		case 'saved':
		    $status = $module->translate('Display settings saved successfully');
		    break;
		case 'added':
		    $status = $module->translate('New field added successfully');
		    break;
		case 'moved':
		    $status = $module->translate('Field moved successfully');
		    break;
		case 'removed':
		    $status = $module->translate('Field removed successfully');
		    break;
		case 'picklist':
		    $status = $module->translate('Picklist updated successfully');
		    break;
		case 'error':
		    $status = '<div class="giError">' .
			$module->translate('Field name already in use') . '</div>';
		    break;
		default:
		    $status = $key;
		}
	    }
	} else if (isset($form['action']['save'])) {
	    list ($ret, $markup) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    if ($markup == 'html') {
		/* Strip malicious content if html markup allowed */
		foreach ($form['fields'] as $key => $value) {
		    $form['fields'][$key] = GalleryUtilities::htmlSafe($value, true);
		}
	    }

	    $ret = CustomFieldHelper::saveFieldValues($item, $form['fields']);
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'customfield');
	    if ($ret) {
		return array($ret, null, null, null);
	    }
	    $status = $module->translate('Changes saved successfully');
	}

	return array(null, $error, $status, false);
    }

    /**
     * @see ItemEditPlugin::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	global $gallery;
	$session =& $gallery->getSession();
	$tmp = $session->get(CUSTOM_FIELD_SESSION_KEY);
	$adminMode = (!empty($tmp) && $tmp == $item->getId()) ? 1 : 0;

	if ($form['formName'] != 'CustomFieldItemEdit') {
	    $form['formName'] = 'CustomFieldItemEdit';
	    $form['itemId'] = $item->getId();
	}

	switch ($adminMode) {
	case 1:
	    $ret = CustomFieldHelper::loadAdminForm($form, $item->getId());
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $tpl = 'modules/customfield/templates/CustomFieldItemAdmin.tpl';
	    break;
	default:
	    $set = GalleryUtilities::isA($item, 'GalleryAlbumItem') ? 'album'
		 : (GalleryUtilities::isA($item, 'GalleryPhotoItem') ? 'photo' : 'common');
	    list ($ret, $tmp, $param, $isContainerSettings) =
		CustomFieldHelper::fetchFieldValues(array($item), null, $set);
	    if ($ret) {
		return array($ret, null, null);
	    }
	    $form['fields'] = $tmp[$item->getId()];
	    foreach (array('common' => 1, $set => 1) as $s => $tmp) {
		foreach ($param[$item->getId()][$s] as $it) {
		    if (!empty($it['choices'])) {
			$form['choices'][$it['field']] = $it['choices'];
		    }
		}
	    }
	    if ($set == 'album') {
		$form['adminFlag'] = $isContainerSettings[$item->getId()] ? 1 : 2;
	    }
	    $tpl = 'modules/customfield/templates/CustomFieldItemEdit.tpl';
	}
	$template->setVariable('controller', 'customfield.CustomFieldItemEdit');
	return array(null, $tpl, 'modules_customfield');
    }

    /**
     * @see ItemEditPlugin::isSupported
     */
    function isSupported($item, $thumbnail) {
	if (GalleryUtilities::isA($item, 'GalleryAlbumItem')) {
	    return true;
	}
	list ($ret, $param) = CustomFieldHelper::loadParameters($item->getParentId());
	if ($ret) {
	    return false;
	}
	if (!empty($param['common']) ||
	    (GalleryUtilities::isA($item, 'GalleryPhotoItem') && !empty($param['photo']))) {
	    return true;
	}
	return false;
    }

    /**
     * @see ItemEditPlugin::getTitle
     */
    function getTitle() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'customfield');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $module->translate('Custom Fields'));
    }
}
?>
