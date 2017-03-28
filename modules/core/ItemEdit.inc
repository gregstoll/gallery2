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
 * This controller will handle the editing of an item
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17679 $
 */
class ItemEditController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	$editPlugin = GalleryUtilities::getRequestVariables('editPlugin');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* Make sure we have permission to edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'core.edit');
	if ($ret) {
	    return array($ret, null);
	}

	/* Check to see if we have a preferred source */
	list ($ret, $preferredTable) =
	    GalleryCoreApi::fetchPreferredsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null);
	}
	$preferred = empty($preferredTable) ? null : array_shift($preferredTable);

	/* Load the thumbnail */
	list ($ret, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null);
	}
	$thumbnail = empty($thumbnails) ? null : array_shift($thumbnails);

	/* Get all the edit options */
	list ($ret, $optionInstances) =
	    ItemEditOption::getAllOptions($editPlugin, $item, $thumbnail);
	if ($ret) {
	    return array($ret, null);
	}

	/* Load the correct edit plugin */
	list ($ret, $plugin) =
	    GalleryCoreApi::newFactoryInstanceById('ItemEditPlugin', $editPlugin);
	if ($ret) {
	    return array($ret, null);
	}
	if (!isset($plugin)) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	$status = array();
	list ($ret, $error, $status['editMessage'], $requiresProgressBar) =
	    $plugin->handleRequest($form, $item, $preferred);
	if ($ret) {
	    return array($ret, null);
	}
	if (!empty($requiresProgressBar)) {
	    $results['delegate']['view'] = 'core.ProgressBar';
	}

	/* Now let each option process its data */
	if (!isset($status['warning'])) {
	    $status['warning'] = array();
	}
	if (isset($form['action']['save'])) {
	    $progressBarOptions = array();
	    foreach ($optionInstances as $option) {
		if ($requiresProgressBar || $option->requiresProgressBar($form)) {
		    $progressBarOptions[] = $option;
		} else {
		    list ($ret, $optionErrors, $optionWarnings) =
			$option->handleRequestAfterEdit($form, $item, $preferred);
		    if ($ret) {
			return array($ret, null);
		    }

		    $error = array_merge($error, $optionErrors);
		    $status['warning'] = array_merge($status['warning'], $optionWarnings);
		}
	    }

	    if (empty($error) && $progressBarOptions) {
		$templateAdapter =& $gallery->getTemplateAdapter();
		$templateAdapter->registerTrailerCallback(
		    array($this, 'runProgressBarOptions'),
		    array($progressBarOptions, $form, $item, $preferred, $status, $editPlugin));
		$results['delegate']['view'] = 'core.ProgressBar';
	    }
	}

	if (empty($results['delegate'])) {
	    /* It's not a progress bar view */
	    if (empty($error)) {
		$results['redirect'] = array('view' => 'core.ItemAdmin',
					     'subView' => 'core.ItemEdit',
					     'editPlugin' => $editPlugin,
					     'itemId' => $item->getId());
	    } else {
		$results['delegate']['view'] = 'core.ItemAdmin';
		$results['delegate']['subView'] = 'core.ItemEdit';
		$results['delegate']['editPlugin'] = $editPlugin;
	    }
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    function runProgressBarOptions($options, $form, $item, $preferred, $status, $editPlugin) {
	global $gallery;
	$templateAdapter =& $gallery->getTemplateAdapter();

	$error = array();
	foreach ($options as $option) {
	    $gallery->guaranteeTimeLimit(60);

	    list ($ret, $optionErrors, $optionWarnings) =
		$option->handleRequestAfterEdit($form, $item, $preferred);
	    if ($ret) {
		return $ret;
	    }

	    $error = array_merge($error, $optionErrors);
	    $status['warning'] = array_merge($status['warning'], $optionWarnings);
	}
	$session =& $gallery->getSession();
	$session->putStatus($status);

	$redirect = array();
	$redirect['view'] = 'core.ItemAdmin';
	$redirect['subView'] = 'core.ItemEdit';
	$redirect['itemId'] = $item->getId();
	$redirect['editPlugin'] = $editPlugin;

	$urlGenerator =& $gallery->getUrlGenerator();
	$templateAdapter->completeProgressBar($urlGenerator->generateUrl($redirect));

	return null;
    }
}

/**
 * This view will show options to edit an item
 */
class ItemEditView extends GalleryView {

    /**
     * Returns sizes for "maxlength" in forms
     * @todo This function is duplicated in ItemEditCaptions.inc, so it needs to be consolidated
     *       somehow.
     * @param integer $value is the value from 
     * @return size for "maxlength" in form
     * @access private
     */
    function _getSizesForMaxlength($value) {
	switch ($value) {
	case STORAGE_SIZE_SMALL:
	    $size = 32;
	    break;

	case STORAGE_SIZE_MEDIUM:
	    $size = 128;
	    break;

	case STORAGE_SIZE_LARGE:
	    $size = 255;
	    break;
	}
	
	return $size;
    }
    
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$editPlugin = GalleryUtilities::getRequestVariables('editPlugin');
	
	list ($ret, $item, $wasSpecified) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	/* Make sure we have permission to edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'core.edit');
	if ($ret) {
	    return array($ret, null);
	}

	/* Load the thumbnail */
	list ($ret, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	if ($ret) {
	    return array($ret, null);
	}
	if (!empty($thumbnails)) {
	    list ($ret, $thumbnail) = GalleryCoreApi::rebuildDerivativeCacheIfNotCurrent(
						      $thumbnails[$item->getId()]->getId());
	    if ($ret) {
		/* Ignore thumbnail errors so we can edit items with broken thumbnail */
	    }
	} else {
	    $thumbnail = null;
	}

	/* Get the edit plugins that support this item type */
	list ($ret, $allPluginIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('ItemEditPlugin');
	if ($ret) {
	    return array($ret, null);
	}

	$pluginInstances = array();
	foreach (array_keys($allPluginIds) as $pluginId) {
	    list ($ret, $plugin) =
		GalleryCoreApi::newFactoryInstanceById('ItemEditPlugin', $pluginId);
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($plugin->isSupported($item, $thumbnail)) {
		$pluginInstances[$pluginId] = $plugin;
	    }
	}
	
	/* Define the first edit plugin as the default in case we need to fall back. */
	$editPluginIds = array_keys($pluginInstances);
	$defaultEditPlugin = $editPluginIds[0];

	/*
	 * If the plugin is empty get it from the session.  If it's empty there, default to the
	 * first plugin we find.  Either way, save the user's preference in the session.
	 */
	$session =& $gallery->getSession();
	$editPluginSessionKey = 'core.view.ItemEdit.editPlugin.' . get_class($item);
	if (empty($editPlugin)) {
	    $editPlugin = $session->get($editPluginSessionKey);
	    if (empty($editPlugin) || !in_array($editPlugin, array_keys($pluginInstances))) {
		$editPlugin = $defaultEditPlugin;
	    }
	}
	/* Input validation of the given editPlugin id. */
	if (!isset($pluginInstances[$editPlugin])) {
	    $editPlugin = $defaultEditPlugin;
	}
	$session->put($editPluginSessionKey, $editPlugin);

	/* Get display data for all plugins */
	$plugins = array();
	foreach ($pluginInstances as $pluginId => $plugin) {
	    list ($ret, $title) =  $plugin->getTitle();
	    if ($ret) {
		return array($ret, null);
	    }
	    $plugins[] = array('title' => $title,
			       'id' => $pluginId,
			       'isSelected' => ($pluginId == $editPlugin));
	}

	/* Record our item serial number in the form so that all plugins can use it */
	$form['serialNumber'] = $item->getSerialNumber();

	$ItemEdit = array();
	$ItemEdit['editPlugin'] = $editPlugin;
	$ItemEdit['plugins'] = $plugins;
	$ItemEdit['itemTypeNames'] = $item->itemTypeName();
	$ItemEdit['showEditThumbnail'] = $thumbnail != null;

	list ($ret, $ItemEdit['isAdmin']) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	/* Let the plugin load its template data */
	list ($ret, $ItemEdit['pluginFile'], $ItemEdit['pluginL10Domain']) =
	    $pluginInstances[$editPlugin]->loadTemplate($template, $form, $item, $thumbnail);
	if ($ret) {
	    return array($ret, null);
	}

	/* Get all the edit options */
	list ($ret, $optionInstances) =
	    ItemEditOption::getAllOptions($editPlugin, $item, $thumbnail);
	if ($ret) {
	    return array($ret, null);
	}

	/* Now let all options load their template data */
	$ItemEdit['options'] = array();
	foreach ($optionInstances as $option) {
	    list ($ret, $entry['file'], $entry['l10Domain']) =
		$option->loadTemplate($template, $form, $item, $thumbnail);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!empty($entry['file'])) {
		$ItemEdit['options'][] = $entry;
	    }
	}

	list ($ret, $entityInfo) = GalleryCoreApi::describeEntity('GalleryItem');
	if ($ret) {
	    return array($ret, null);
	}

	$ItemEdit['fieldLengths'] = array();
	$ItemEdit['fieldLengths']['title'] =
	     $this->_getSizesForMaxLength($entityInfo['GalleryItem']['members']['title']['size']);

	$template->setVariable('ItemEdit', $ItemEdit);
	$template->setVariable('controller', 'core.ItemEdit');
	return array(null, array('body' => 'modules/core/templates/ItemEdit.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	$itemTypeNames = $item->itemTypeName(true);

	return array(null,
		     $core->translate(array('text' => 'edit %s', 'arg1' => $itemTypeNames[1])));
    }
}

/**
 * Interface for plugins to the ItemEdit view and controller
 * @abstract
 */
class ItemEditPlugin {
    /**
     * Does this plugin support the given item type?
     *
     * @param GalleryItem $item
     * @param GalleryDerivative $thumbnail item's thumbnail
     * @return boolean true if it's supported
     */
    function isSupported($item, $thumbnail) {
	return false;
    }

    /**
     * Load the template with data from this plugin
     * @see GalleryView::loadTemplate
     *
     * @param GalleryTemplate $template
     * @param array $form the form values
     * @param GalleryItem $item
     * @param GalleryDerivative $thumbnail item's thumbnail
     * @return array GalleryStatus a status code
     *               string the path to a template file to include
     *               string localization domain for the template file
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Let the plugin handle the incoming request
     * @see GalleryController::handleRequest
     *
     * @param array $form the form values
     * @param GalleryItem $item
     * @param GalleryDerivative $preferred item's preferred derivative, if there is one
     * @return array GalleryStatus a status code
     *               array error messages
     *               string localized status message
     *               boolean true if progress bar is needed
     */
    function handleRequest($form, &$item, &$preferred) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED),
		     null, null, null);
    }

    /**
     * Return a localized title for this plugin, suitable for display to the user
     *
     * @return array GalleryStatus a status code
     *               string localized title
     */
    function getTitle() {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null);
    }

    /**
     * Check to see if a given operation is available for any of a set of mime types
     *
     * @param string $operation the operation (eg. 'rotate' or 'scale')
     * @param array $mimeTypes
     * @return array GalleryStatus a status code
     *               bool true if any of the mime types are supported
     * @access protected
     */
    function _checkForOperation($operation, $mimeTypes) {
	foreach (array_unique($mimeTypes) as $mimeType) {
	    list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation($mimeType, $operation);
	    if ($ret) {
		return array($ret, null);
	    }

	    if (isset($toolkit)) {
		break;
	    }
	}

	return array(null, isset($toolkit));
    }
}

/**
 * Interface for options to the ItemEdit view and controller.
 * Options allow us to provide extra UI in the views and extra processing in the controller so
 * that we can add new functionality to various ItemEditPlugins
 * @abstract
 */
class ItemEditOption {

    /**
     * Return all the available option plugins
     *
     * @param string $editPlugin name of ItemEditPlugin
     * @param GalleryItem $item
     * @param GalleryDerivative $thumbnail
     * @return array GalleryStatus a status code
     *               array ItemEditOption instances
     * @static
     */
    function getAllOptions($editPlugin, $item, $thumbnail) {
	list ($ret, $allOptionIds) =
	    GalleryCoreApi::getAllFactoryImplementationIdsWithHint('ItemEditOption', $editPlugin);
	if ($ret) {
	    return array($ret, null);
	}

	$optionInstances = array();
	foreach (array_keys($allOptionIds) as $optionId) {
	    list ($ret, $option) =
		GalleryCoreApi::newFactoryInstanceById('ItemEditOption', $optionId);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $isAppropriate) = $option->isAppropriate($item, $thumbnail);
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($isAppropriate) {
		$optionInstances[$optionId] = $option;
	    }
	}

	return array(null, $optionInstances);
    }

    /**
     * Load the template with data from this plugin
     * @see GalleryView::loadTemplate
     *
     * @param GalleryTemplate $template
     * @param array $form the form values
     * @param GalleryItem $item
     * @return array GalleryStatus a status code
     *               string the path to a template file to include
     *               string localization domain for the template file
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Let the plugin handle the incoming request.  We expect the $item to be locked.
     * @see GalleryController::handleRequest
     *
     * @param array $form the form values
     * @param GalleryItem $item reference to the item
     * @param GalleryDerivative $preferred reference to preferred derivative
     * @return array GalleryStatus a status code
     *               array error messages
     *               array localized warning messages
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Is this option appropriate at this time?
     *
     * @param GalleryItem $item
     * @param GalleryDerivative $thumbnail
     * @return array GalleryStatus a status code
     *               boolean true or false
     */
    function isAppropriate($item, $thumbnail) {
	return array(null, false);
    }

    /**
     * Will this task run so long that it requires a progress bar?
     *
     * @param array $form the state of the current form
     * @return boolean
     */
    function requiresProgressBar($form) {
	return false;
    }
}
?>
