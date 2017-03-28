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
 * This controller will handle the addition of new items in the gallery
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17589 $
 */
class ItemAddController extends GalleryController {
    /**
     * ItemAddOption instances to use when handling this request.  Only used by test code.
     *
     * @var array (optionId => ItemAddOption) $_optionInstances
     * @access private
     */
    var $_optionInstances;

    /**
     * Tests can use this method to hardwire a specific set of option instances to use.
     * This avoids situations where some of the option instances will do unpredictable
     * things and derail the tests.
     *
     * @param array $optionInstances (optionId => ItemAddOption, ...)
     */
    function setOptionInstances($optionInstances) {
	$this->_optionInstances = $optionInstances;
    }

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$templateAdapter =& $gallery->getTemplateAdapter();

	$addPluginId = GalleryUtilities::getRequestVariables('addPlugin');

	/**
	 * Special case for backwards-compatibility with the webdav module
	 * @todo Remove on next major API change
	 */
	if ($addPluginId == 'ItemAddWebDav') {
	    /* WebDAV used to do a static ::handleRequest() call. We need an instance. */
	    $controller = new ItemAddController();
	} else {
	    $controller = $this;
	}

	list ($ret, $item) = $controller->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* Make sure we have permission to add to this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.addDataItem');
	if ($ret) {
	    return array($ret, null);
	}

	if (!$item->getCanContainChildren()) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	/* Load the correct add plugin */
	list ($ret, $addPlugin) =
	    GalleryCoreApi::newFactoryInstanceById('ItemAddPlugin', $addPluginId);
	if ($ret) {
	    return array($ret, null);
	}

	if (!isset($addPlugin)) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	/**
	 * Special case for backwards-compatibility with the webdav module
	 * @todo Remove on next major API change
	 */
	if ($addPluginId == 'ItemAddWebDav') {
	    /* Don't output any HTML (progress-bar) */
	    ob_start();
	    $ret = $controller->handleRequestWithProgressBar($form, $item, $addPlugin);
	    ob_end_clean();
	    if ($ret) {
		return array($ret, null);
	    }

	    $session =& $gallery->getSession();
	    $results['status'] = $session->getStatus();
	    $results['redirect'] = array('view' => 'core.ItemAdmin',
					 'subView' => 'core.ItemAddConfirmation',
					 'itemId' => $item->getId());
	    $results['error'] = array();
	    return array(null, $results);
	}

	/* Do the actual work in callback of a progress-bar view */
	$templateAdapter->registerTrailerCallback(
		    array($this, 'handleRequestWithProgressBar'),
		    array($form, $item, $addPlugin));

	return array(null, array('delegate' => array('view' => 'core.ProgressBar'),
				 'status' => array(), 'error' => array()));
    }

    /**
     * Handles the add item request and is expected to be called as a progress-bar callback.
     * @param array $form
     * @param GalleryItem $item The container to which we're adding child-items
     * @param ItemAddPlugin $addPlugin The plugin that handles this add item request
     * @return GalleryStatus
     */
    function handleRequestWithProgressBar($form, $item, $addPlugin) {
	global $gallery;
	$templateAdapter =& $gallery->getTemplateAdapter();
	$urlGenerator =& $gallery->getUrlGenerator();
	$phpVm = $gallery->getPhpVm();
	$session =& $gallery->getSession();

	$startTime = $phpVm->time();
	/* Auto-redirect if we complete the request within this period. Else show the continueURL */
	$autoRedirectSeconds = 15;

	list ($ret, $this->_coreModule) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return $ret;
	}
	$templateAdapter->updateProgressBar($this->_coreModule->translate('Adding items'), '', 0);

	$error = array();
	$addPluginId = GalleryUtilities::getRequestVariables('addPlugin');

	list ($ret, $lockId) = GalleryCoreApi::acquireReadLock($item->getId());
	if ($ret) {
	    return $ret;
	}

	/* Start the add process */
	list ($ret, $error, $status) = $addPlugin->handleRequest($form, $item, $this);
	if ($ret) {
	    GalleryCoreApi::releaseLocks($lockId);
	    return $ret;
	}

	if (empty($error)) {
	    list ($ret, $error) = $this->postprocessItems($form, $status);
	    if ($ret) {
		GalleryCoreApi::releaseLocks($lockId);
		return $ret;
	    }
	}

	$ret = GalleryCoreApi::releaseLocks($lockId);
	if ($ret) {
	    return $ret;
	}

	if (!empty($error)) {
	    /** @todo Should we remove all added items in case of a late form validation error? */
	    if (!empty($status['addedFiles'])) {
		$error[] = 'form[error][itemsAddedDespiteFormErrors]';
	    }
	    $session->put('itemAdd.error', $error);

	    $doRedirect = true;
	    $continueUrl = $urlGenerator->generateUrl(
		array('view' => 'core.ItemAdmin', 'subView' => 'core.ItemAdd',
		      'addPlugin' => $addPluginId, 'itemId' => $item->getId()),
		array('forceFullUrl' => true));
	} else {
	    $session->putStatus($status);
	    $doRedirect = ($phpVm->time() - $startTime) <= $autoRedirectSeconds;

	    if (empty($status['addedFiles'])) {
		/*
		 * Append all form parameters for the next view request.  Some plugins submit a
		 * first form to the controller only to forward the request to the view which might
		 * depend on the same form parameters.
		 */
		$continueUrl = $urlGenerator->generateUrl(
		    array('view' => 'core.ItemAdmin', 'subView' => 'core.ItemAdd',
			  'addPlugin' => $addPluginId, 'itemId' => $item->getId(), 'form' => $form),
		    array('forceFullUrl' => true));
		$templateAdapter->updateProgressBar(
		    $this->_coreModule->translate('Adding items'), '', 1);
	    } else {
		$continueUrl = $urlGenerator->generateUrl(
		    array('view' => 'core.ItemAdmin', 'subView' => 'core.ItemAddConfirmation',
			  'itemId' => $item->getId()), array('forceFullUrl' => true));
	    }
	}

	$templateAdapter->completeProgressBar($continueUrl, $doRedirect);

	return null;
    }

    /**
     * Do post-processing which includes extracting archive-items and letting all ItemAddOption
     * instances handle the added items.
     *
     * If called from an ItemAddPlugin, the plugin should stop adding items if $error is non-empty.
     *
     * @param array $form
     * @param array $status An array including the list of all added items
     * @see ItemAddPlugin::handleRequest() for the structure of the $status array
     * @return array GalleryStatus
     *               array $error request parameter errors
     */
    function postprocessItems($form, &$status) {
	global $gallery;
	$this->_templateAdapter =& $gallery->getTemplateAdapter();
	$this->_storage =& $gallery->getStorage();

	if (!isset($this->_coreModule)) {
	    list ($ret, $this->_coreModule) = GalleryCoreApi::loadPlugin('module', 'core');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$this->_processingItemsMessage = $this->_coreModule->translate('Processing items');

	if (!isset($this->_optionInstances)) {
	    list ($ret, $this->_optionInstances) = ItemAddOption::getAllAddOptions();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	if (!isset($this->_extractionToolkitMap)) {
	    list ($ret, $extractToolkits) =
		GalleryCoreApi::getToolkitOperationMimeTypes('extract');
	    if ($ret) {
		return array($ret, null);
	    }
	    $this->_extractionToolkitMap = array();
	    foreach ($extractToolkits as $mimeType => $toolkitList) {
		if (!empty($toolkitList)) {
		    list ($ret, $this->_extractionToolkitMap[$mimeType]) =
			GalleryCoreApi::getToolkitByOperation($mimeType, 'extract');
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }
	}

	if (empty($status['addedFiles'])
		|| empty($this->_extractionToolkitMap) && empty($this->_optionInstances)) {
	    /* Nothing to do */
	    return array(null, array());
	}

	$this->_templateAdapter->updateProgressBar($this->_processingItemsMessage, '', 0);

	$ret = $this->_storage->checkPoint();
	if ($ret) {
	    return array($ret, null);
	}
	$gallery->guaranteeTimeLimit(60);

	if (!isset($this->_processedItems)) {
	    $this->_processedItems = array();
	}
	$errors = array();

	/* The number of items a ItemAddOption should be able to process in less than 30 seconds */
	$batchSize = 20;

	/* Extract all archive-type items and call the ItemAddOption instances for postprocessing */
	$itemsToProcess = $itemsToProcessKeyMap = array();
	$i = 0;
	do {
	    $file =& $status['addedFiles'][$i];
	    if (empty($file['id']) || isset($this->_processedItems[$i])) {
		/* We couldn't add this file for whatever reason or it has been processed already */
		continue;
	    }
	    list ($ret, $addedItem) = GalleryCoreApi::loadEntitiesById($file['id'], 'GalleryItem');
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Check if we should extract individual files out of an archive */
	    if (GalleryUtilities::isA($addedItem, 'GalleryDataItem')
		    && isset($this->_extractionToolkitMap[$addedItem->getMimeType()])) {
		list ($ret, $extractedItems) = $this->_extractAndAddFiles(
		    $addedItem, $this->_extractionToolkitMap[$addedItem->getMimeType()]);
		if ($ret) {
		    return array($ret, null);
		}
		$ret = GalleryCoreApi::deleteEntityById($addedItem->getId(), 'GalleryItem');
		if ($ret) {
		    return array($ret, null);
		}
		/*
		 * Remove this element from the status and use array_merge to append the extracted
		 * items and to reindex the whole array to fill the gap we just created.
		 */
		unset($status['addedFiles'][$i--]);
		$status['addedFiles'] = array_merge($status['addedFiles'], $extractedItems);

		$gallery->guaranteeTimeLimit(30);
		$ret = $this->_storage->checkPoint();
		if ($ret) {
		    return array($ret, null);
		}
	    } else {
		/* This is not an archive, add it to our array of item objects */
		$itemsToProcess[] = $addedItem;
		/*
		 * We can't index $itemsToProcess directly by $i because some options expect it to
		 * be indexed from 0..n without holes.
		 */
		$itemsToProcessKeyMap[] = $i;
	    }

	    if (count($itemsToProcess) % $batchSize == 0
		    || !isset($status['addedFiles'][$i+1]) && count($itemsToProcess)) {
		/* Allow ItemAddOptions to process added item(s) */
		$optionNumber = 0;
		foreach ($this->_optionInstances as $option) {
		    $this->_templateAdapter->updateProgressBar(
			$this->_processingItemsMessage, '',
			$optionNumber++ / count($this->_optionInstances));
		    $gallery->guaranteeTimeLimit(60);

		    list ($ret, $optionErrors, $optionWarnings) =
			$option->handleRequestAfterAdd($form, $itemsToProcess);
		    if ($ret) {
			return array($ret, null);
		    }

		    $errors = array_merge($errors, $optionErrors);
		    /* For each item, put the items warnings into our status array */
		    foreach ($optionWarnings as $j => $messages) {
			$key = $itemsToProcessKeyMap[$j];
			if (!isset($status['addedFiles'][$key]['warnings'])) {
			    $status['addedFiles'][$key]['warnings'] = array();
			}
			$status['addedFiles'][$key]['warnings'] =
			    array_merge($status['addedFiles'][$key]['warnings'], $messages);
		    }

		    $ret = $this->_storage->checkPoint();
		    if ($ret) {
			return array($ret, null);
		    }
		}

		foreach ($itemsToProcessKeyMap as $j) {
		    $this->_processedItems[$j] = 1;
		}
		$itemsToProcess = $itemsToProcessKeyMap = array();
		$gallery->guaranteeTimeLimit(60);
		$this->_templateAdapter->updateProgressBar($this->_processingItemsMessage, '', 1);
	    }
	} while (isset($status['addedFiles'][++$i]));

	$this->_templateAdapter->updateProgressBar($this->_processingItemsMessage, '', 1);

	return array(null, $errors);
    }

    /**
     * Extract files from an archive item and add new items to the same album.
     * @param GalleryDataItem $archiveItem archive
     * @param GalleryToolkit $toolkit toolkit that supports extract operation
     * @return array GalleryStatus a status code
     *               array of array('fileName' => '..', 'id' => ##, 'warnings' => array of string)
     * @access private
     */
    function _extractAndAddFiles($archiveItem, $toolkit) {
	global $gallery;
	$this->_platform =& $gallery->getPlatform();

	$this->_extractingArchiveMessage = $this->_coreModule->translate('Extracting archive');
	$this->_templateAdapter->updateProgressBar($this->_extractingArchiveMessage, '', 0);
	$gallery->guaranteeTimeLimit(120);

	$parentId = $archiveItem->getParentId();
	list ($ret, $hasAddAlbumPermission) =
	    GalleryCoreApi::hasItemPermission($parentId, 'core.addAlbumItem');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $file) = $archiveItem->fetchPath();
	if ($ret) {
	    return array($ret, null);
	}

	$base = $this->_platform->tempnam($gallery->getConfig('data.gallery.tmp'), 'tmp_');
	$tmpDir = $base . '.dir';
	if (!$this->_platform->mkdir($tmpDir)) {
	    return array(GalleryCoreApi::error(ERROR_PLATFORM_FAILURE), null);
	}

	list ($ret) = $toolkit->performOperation($archiveItem->getMimeType(), 'extract',
						 $file, $tmpDir, array());
	if ($ret) {
	    @$this->_platform->recursiveRmdir($tmpDir);
	    @$this->_platform->unlink($base);
	    return array($ret, null);
	}

	/*
	 * If archive title matches the filename or base filename then name new items
	 * with the same strategy; otherwise just use the archive title.
	 */
	$archiveTitle = $archiveItem->getTitle();
	$archiveName = $archiveItem->getPathComponent();
	list ($archiveBase) = GalleryUtilities::getFileNameComponents($archiveName);
	if ($archiveTitle == $archiveName) {
	    $titleMode = 'file';
	} else if ($archiveTitle == $archiveBase) {
	    $titleMode = 'base';
	} else {
	    $titleMode = 'archive';
	}

	$this->_templateAdapter->updateProgressBar($this->_extractingArchiveMessage, '', 0.1);
	$gallery->guaranteeTimeLimit(30);

	$addedFiles = array();
	$ret = $this->_recursiveAddDir(
	    $tmpDir, $parentId, $addedFiles, $archiveItem, $titleMode, $hasAddAlbumPermission);
	@$this->_platform->recursiveRmdir($tmpDir);
	@$this->_platform->unlink($base);
	if ($ret) {
	    return array($ret, null);
	}

	$this->_templateAdapter->updateProgressBar($this->_extractingArchiveMessage, '', 1);

	return array(null, $addedFiles);
    }

    /**
     * Recursively add files from extracted archive.
     * @return GalleryStatus a status code
     * @access private
     */
    function _recursiveAddDir($dir, $parentId, &$addedFiles, &$archiveItem, $titleMode,
			      $canAddAlbums) {
	global $gallery;

	$list = array();
	$dh = $this->_platform->opendir($dir);
	while (($file = $this->_platform->readdir($dh)) !== false) {
	    if ($file != '.' && $file != '..') {
		$list[] = $file;
	    }
	}
	$this->_platform->closedir($dh);

	foreach ($list as $filename) {
	    $path = "$dir/$filename";
	    if ($this->_platform->is_dir($path)) {
		if ($canAddAlbums) {
		    $title = $filename;
		    GalleryUtilities::sanitizeInputValues($title);
		    list ($ret, $album) = GalleryCoreApi::createAlbum(
			$parentId, $filename, $title, '', '', '');
		    if ($ret) {
			return $ret;
		    }
		    list ($ret, $lockId) = GalleryCoreApi::acquireReadLock($album->getId());
		    if ($ret) {
			return $ret;
		    }
		    $ret = $this->_recursiveAddDir($path, $album->getId(), $addedFiles,
			$archiveItem, $titleMode, $canAddAlbums);
		    if ($ret) {
			GalleryCoreApi::releaseLocks($lockId);
			return $ret;
		    }
		    $ret = GalleryCoreApi::releaseLocks($lockId);
		    if ($ret) {
			return $ret;
		    }
		    $newItem =& $album;
		} else {
		    /*
		     * Flattening folder structure since we're not allowed to create albums.
		     * Adding files but ignoring directories.
		     */
		    $ret = $this->_recursiveAddDir(
			$path, $parentId, $addedFiles, $archiveItem, $titleMode, $canAddAlbums);
		    if ($ret) {
			return $ret;
		    }
		    $newItem = null;
		}
	    } else {
		list ($ret, $mimeType) = GalleryCoreApi::getMimeType($filename);
		if ($ret) {
		    return $ret;
		}
		if ($titleMode == 'file') {
		    $title = $filename;
		    GalleryUtilities::sanitizeInputValues($title);
		} else if ($titleMode == 'base') {
		    list ($title) = GalleryUtilities::getFileNameComponents($filename);
		    GalleryUtilities::sanitizeInputValues($title);
		} else {
		    $title = $archiveItem->getTitle();
		}
		list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum(
		    $path, $filename, $title, $archiveItem->getSummary(),
		    $archiveItem->getDescription(), $mimeType, $parentId);
		if ($ret) {
		    return $ret;
		}
	    }

	    if ($newItem) {
		$sanitizedFilename = $filename;
		GalleryUtilities::sanitizeInputValues($sanitizedFilename);
		$addedFiles[] = array('fileName' => $sanitizedFilename, 'id' => $newItem->getId(),
				      'warnings' => array());
	    }
	    if (count($addedFiles) % 10 == 0) {
		/* The percentage isn't accurate at all, we just keep the visual feedback going */
		$this->_templateAdapter->updateProgressBar($this->_extractingArchiveMessage, '',
		    0.1 + 0.9 * count($addedFiles) / (count($list) + count($addedFiles)));
		$gallery->guaranteeTimeLimit(30);
		$ret = $this->_storage->checkPoint();
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }
}

/**
 * This view will show the selected plugin for adding items to the gallery
 */
class ItemAddView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$session =& $gallery->getSession();

	$addPlugin = GalleryUtilities::getRequestVariables('addPlugin');

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* Make sure we have permission to add to this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.addDataItem');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	/* Get all the add plugins */
	list ($ret, $allPluginIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('ItemAddPlugin');
	if ($ret) {
	    return array($ret, null);
	}

	$pluginInstances = array();
	foreach (array_keys($allPluginIds) as $pluginId) {
	    list ($ret, $plugin) =
		GalleryCoreApi::newFactoryInstanceById('ItemAddPlugin', $pluginId);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $isAppropriate) = $plugin->isAppropriate();
	    if ($ret) {
		return array($ret, null);
	    }

	    if ($isAppropriate) {
		$pluginInstances[$pluginId] = $plugin;
	    }
	}

	/* Get all the add options */
	list ($ret, $optionInstances) = ItemAddOption::getAllAddOptions();
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * If the plugin is empty get it from the session.  If it's empty there,
	 * default to the first plugin we find.  Either way, save the user's
	 * preference in the session.
	 */
	$addPluginSessionKey = 'core.view.ItemAdd.addPlugin.' . get_class($item);
	if (empty($addPlugin) || !isset($pluginInstances[$addPlugin])) {
	    $addPlugin = $session->get($addPluginSessionKey);
	    if (empty($addPlugin) || !isset($pluginInstances[$addPlugin])) {
		$ids = array_keys($pluginInstances);
		$addPlugin = $ids[0];
	    }
	}
	$session->put($addPluginSessionKey, $addPlugin);

	$errors = $session->get('itemAdd.error');
	if (!empty($errors)) {
	    $session->remove('itemAdd.error');
	    /* Same logic as in main.php */
	    foreach ($errors as $error) {
		GalleryUtilities::putRequestVariable($error, 1);
	    }
	}

	/* Get display data for all plugins */
	$plugins = array();
	foreach ($pluginInstances as $pluginId => $plugin) {
	    list ($ret, $title) =  $plugin->getTitle();
	    if ($ret) {
		return array($ret, null);
	    }
	    $plugins[] = array('title' => $title,
			       'id' => $pluginId,
			       'isSelected' => ($pluginId == $addPlugin));
	}

	$ItemAdd = array();
	$ItemAdd['addPlugin'] = $addPlugin;
	$ItemAdd['plugins'] = $plugins;
	$ItemAdd['isAdmin'] = $isAdmin;

	/* Let the plugin load its template data */
	list ($ret, $ItemAdd['pluginFile'], $ItemAdd['pluginL10Domain']) =
	    $pluginInstances[$addPlugin]->loadTemplate($template, $form, $item);
	if ($ret) {
	    return array($ret, null);
	}

	/* Now let all options load their template data */
	$ItemAdd['options'] = array();
	foreach ($optionInstances as $option) {
	    list ($ret, $entry['file'], $entry['l10Domain']) =
		$option->loadTemplate($template, $form, $item);
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!empty($entry['file'])) {
		$ItemAdd['options'][] = $entry;
	    }
	}

	/* Make sure that we've got some toolkits */
	list ($ret, $operations) = GalleryCoreApi::getToolkitOperations('image/jpeg');
	if ($ret) {
	    return array($ret, null);
	}

	$ItemAdd['hasToolkit'] = false;
	for ($i = 0; $i < sizeof($operations); $i++) {
	    if ($operations[$i]['name'] == 'thumbnail') {
		$ItemAdd['hasToolkit'] = true;
		break;
	    }
	}

	$template->setVariable('ItemAdd', $ItemAdd);
	$template->setVariable('controller', 'core.ItemAdd');
	return array(null,
		     array('body' => 'modules/core/templates/ItemAdd.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}
	return array(null, $core->translate('add items'));
    }
}


/**
 * Interface for plugins to the ItemAdd view and controller.
 * Plugins provide alternate ways to add items into Gallery.
 * @abstract
 */
class ItemAddPlugin {

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
    function loadTemplate(&$template, &$form, $item) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Let the plugin handle the incoming request
     * @see GalleryController::handleRequest
     *
     * @param array $form the form values
     * @param GalleryItem $item
     * @param GalleryItemAddController $addController A reference to the ItemAddController
     *               to be used for the post-processing calls.
     * @return array GalleryStatus a status code
     *               array error messages (request parameter errors). Stop processing on errors.
     *               array status data, 'addedFiles' entry should contain:
     *                                   array(array('fileName' => '...', 'id' => ##,
     *                                               'warnings' => array of strings), ...)
     */
    function handleRequest($form, &$item, &$addController) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Return a localized title for this plugin, suitable for display to the user
     *
     * @return array GalleryStatus a status code
     *               return-array (same as GalleryController::handleRequest)
     */
    function getTitle() {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null);
    }

    /**
     * Is this plugin appropriate at this time?  Default is true.
     *
     * @return array GalleryStatus a status code
     *               boolean true or false
     */
    function isAppropriate() {
	return array(null, true);
    }
}

/**
 * Interface for options to the ItemAdd view and controller.
 * Options allow us to provide extra UI in the views and extra processing in the controller so
 * that we can add new functionality like watermarking, quotas, etc to every ItemAddPlugin
 * @abstract
 */
class ItemAddOption {

    /**
     * Return all the available option plugins
     *
     * @return array GalleryStatus a status code
     *               array ItemAddOption instances
     * @static
     */
    function getAllAddOptions() {
	/* Get all the option plugins */
	list ($ret, $allOptionIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('ItemAddOption');
	if ($ret) {
	    return array($ret, null);
	}

	$optionInstances = array();
	foreach (array_keys($allOptionIds) as $optionId) {
	    list ($ret, $option) =
		GalleryCoreApi::newFactoryInstanceById('ItemAddOption', $optionId);
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $isAppropriate) = $option->isAppropriate();
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
    function loadTemplate(&$template, &$form, $item) {
	return array(null, null, null);
    }

    /**
     * Let the plugin handle the incoming request.  We expect the $items to be locked.
     * @see GalleryController::handleRequest
     *
     * @param array $form the form values
     * @param array $items GalleryDataItems
     * @return array GalleryStatus a status code
     *               array localized error messages.  Attempt to continue processing on errors since
     *                     the items have already been added and post-processing will continue.
     *               array localized warning messages
     */
    function handleRequestAfterAdd($form, $items) {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED), null, null);
    }

    /**
     * Is this option appropriate at this time?
     *
     * @return array GalleryStatus a status code
     *               boolean true or false
     */
    function isAppropriate() {
	return array(null, false);
    }
}
?>
