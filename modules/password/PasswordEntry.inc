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
 * Verify password entry
 * @package Password
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @author Jess Martin <jmartin@cs.unc.edu>
 * @version $Revision: 17588 $
 */
class PasswordEntryController extends GalleryController {
    /**
     * ValidationPlugin instances to use when handling this request.  Only used by test code.
     * @var array $_plugins (array of GalleryValidationPlugin)
     * @access private
     */
    var $_pluginInstances;

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$status = $error = array();

	if (isset($form['action']['password'])
		&& isset($form['itemId']) && isset($form['password'])) {
	    $itemId = $form['itemId'];
	    list ($ret, $canSee) = GalleryCoreApi::hasItemPermission($itemId, 'core.view');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!$canSee) {
		/* Allow access without core.view if this item is also hidden */
		list ($ret, $hiddenInterface) =
		    GalleryCoreApi::newFactoryInstance('HiddenInterface_1_0');
		if ($ret) {
		    return array($ret, null);
		}
		if (isset($hiddenInterface)) {
		    list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
		    if (!$ret) {
			list ($ret, $canSee) = $hiddenInterface->isHidden($item);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}
	    }

	    /* Check the password entered against the actual password */
	    list ($ret, $hashedPassword) = GalleryCoreApi::getPluginParameter(
		    'module', 'password', 'password', $itemId);
	    if ($ret) {
		return array($ret, null);
	    }
	    GalleryUtilities::unsanitizeInputValues($form['password'], false);
	    $isCorrect = $canSee && $hashedPassword
		&& GalleryUtilities::isCorrectPassword($form['password'], $hashedPassword);

	    /* Prepare for validation */
	    $options = array('pass' => $isCorrect);
	    list ($ret, $options['level']) =
		GalleryCoreApi::getPluginParameter('module', 'password', 'validation.level');
	    if ($ret) {
		return array($ret, null);
	    }
	    if ($options['level'] == 'MEDIUM') {
		$options['key'] = 'password.PasswordEntry.' . $itemId;
	    }
	    if ($options['level'] == 'OFF') {
		$plugins = array();
	    } else if (isset($this->_pluginInstances)) {
		$plugins = $this->_pluginInstances;
	    } else {
		list ($ret, $plugins) =
		    GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
		if ($ret) {
		    return array($ret, null);
		}
		foreach (array_keys($plugins) as $pluginId) {
		    list ($ret, $plugins[$pluginId]) = GalleryCoreApi::newFactoryInstanceById(
						       'GalleryValidationPlugin', $pluginId);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    /* Let each plugin do its verification */
	    foreach ($plugins as $plugin) {
		list ($ret, $pluginErrors, $continue) =
		    $plugin->performValidation($form, $options);
		if ($ret) {
		    return array($ret, null);
		}
		$error = array_merge($error, $pluginErrors);
		if (!$continue) {
		    break;
		}
	    }

	    if (empty($error) && $isCorrect) {
		GalleryCoreApi::addPermissionToSession($itemId);

		$results['redirect']['view'] = 'core.ShowItem';
		$results['redirect']['itemId'] = $itemId;
	    } else if (empty($error)) {
		$error[] = 'form[error][incorrectPassword]';
	    }
	}
	if (!isset($results['redirect'])) {
	    $results['delegate']['view'] = 'password.PasswordEntry';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * View that shows user a password entry box, allowing them to the enter the password for an item.
 */
class PasswordEntryView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	if ($form['formName'] != 'PasswordEntry') {
	    $form['formName'] = 'PasswordEntry';
	    $form['itemId'] = GalleryUtilities::getRequestVariables('itemId');
	}

	list ($ret, $form['validationLevel']) =
	    GalleryCoreApi::getPluginParameter('module', 'password', 'validation.level');
	if ($ret) {
	    return array($ret, null);
	}

	$template->setVariable('controller', 'password.PasswordEntry');
	return array(null, array('body' => 'modules/password/templates/PasswordEntry.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'password');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $core->translate('Password Entry'));
    }
}
?>
