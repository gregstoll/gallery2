<?php

/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */
GalleryCoreApi::requireOnce('modules/core/classes/GalleryRepositoryUtilities.class');
GalleryCoreApi::requireOnce('lib/tools/repository/classes/RepositoryObject.class');
GalleryCoreApi::requireOnce('lib/tools/repository/classes/RepositoryIndex.class');

/**
 * Controller for the repository browser.
 *
 * @author Jozef Selesi <selesi at gmail dot com>
 *
 * @version $Revision: 17580 $
 * @package RepositoryTools
 * @subpackage Controllers
 * @module RepositoryManagerController
 */
class RepositoryManagerControllerAndView extends RepositoryControllerAndView {
	/**
	 * Displays the contents of the repository.
	 *
	 * @return GalleryStatus a status code
	 */
	public function browse() {
		global $gallery;

		$urlGenerator =& $gallery->getUrlGenerator();

		// Get repository plugin meta data.
		$index = new RepositoryIndex();

		$index->init($gallery->getConfig('repository.path'));

		list($ret, $pluginInfo) = $index->getPluginInfo();

		if ($ret) {
			return $ret;
		}

		$data['pluginInfo'] = $pluginInfo;

		// Add action links.
		$upgradeablePlugins = array();

		foreach ($data['pluginInfo'] as $indexId => $plugin) {
			$data['pluginInfo'][$indexId]['actions']['remove'] = $urlGenerator->generateUrl(
				array(
					'controller' => 'RepositoryManager',
					'action'     => 'removePlugin',
					'pluginId'   => $plugin['id'],
					'pluginType' => $plugin['type'],
				)
			);
		}

		if (count($upgradeablePlugins) > 0) {
			$data['upgradeRepositoryLink'] = $urlGenerator->generateUrl(
				array(
					'controller' => 'PackagePlugin',
					'action'     => 'packagePlugins',
					'filter'     => sprintf('/%s/', implode('|', $upgradeablePlugins)),
				)
			);
		}

		$ret = $this->showView('RepositoryManager', $data);

		if ($ret) {
			return $ret;
		}

		return null;
	}

	/**
	 * Removes the specified plugin from the repository.
	 *
	 * For now, versions are ignored - all versions of the plugin are removed.
	 */
	public function removePlugin() {
		global $gallery;

		list($pluginId, $pluginType) = GalleryUtilities::getRequestVariables('pluginId', 'pluginType');

		if (empty($pluginId) || empty($pluginType)) {
			return GalleryCoreApi::error(
				ERROR_BAD_PARAMETER,
				__FILE__,
				__LINE__,
				sprintf(
					'pluginId [%s] and/or pluginType [%s] not set.',
					$pluginId,
					$pluginType
				)
			);
		}

		// Remove 'pluginId-*' files from 'repository/pluginType'.
		$removedFiles        = array();
		$platform            =& $gallery->getPlatform();
		$repositoryPluginDir = $gallery->getConfig('repository.path') . $pluginType . '/';

		if ($dir = $platform->opendir($repositoryPluginDir)) {
			while (false !== ($file = readdir($dir))) {
				if (preg_match(sprintf('/^%s/', $pluginId), $file)) {
					$platform->unlink($repositoryPluginDir . $file);

					$removedFiles[] = $file;
				}
			}
		}

		$this->showView(
			'PackageRemovalResults',
			array(
				'files' => $removedFiles,
			)
		);

		return null;
	}
}
