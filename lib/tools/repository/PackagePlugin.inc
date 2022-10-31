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
GalleryCoreApi::requireOnce('lib/tools/repository/classes/RepositoryObject.class');
GalleryCoreApi::requireOnce('lib/tools/repository/classes/RepositoryDescriptor.class');
GalleryCoreApi::requireOnce('lib/tools/repository/classes/RepositoryPackage.class');

/**
 * Controller for the plugin packaging page.
 *
 * @author Jozef Selesi <selesi at gmail dot com>
 *
 * @version $Revision: 18677 $
 * @package RepositoryTools
 * @subpackage Controllers
 * @module PackagePluginController
 */
class PackagePluginControllerAndView extends RepositoryControllerAndView {
	/**
	 * Displays the available plugins that can be packaged.
	 *
	 * @return GalleryStatus a status code
	 */
	public function showAvailablePlugins() {
		list($ret, $plugins) = $this->getAvailablePluginList(
			array(
				'package' => array(
					'controller' => 'PackagePlugin',
					'action'     => 'packagePlugin',
				),
			)
		);

		if ($ret) {
			return $ret;
		}

		$ret = $this->showView(
			'PackagePlugin',
			array(
				'formVariablePrefix' => GALLERY_FORM_VARIABLE_PREFIX,
				'plugins'            => $plugins,
			)
		);

		if ($ret) {
			return $ret;
		}

		return null;
	}

	/**
	 * Creates all packages for the selected plugin and displays a summary page when completed.
	 *
	 * @return GalleryStatus a status code
	 */
	public function packagePlugin() {
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

		list($ret, $pluginInfo) = $this->doPackagePlugin($pluginType, $pluginId);

		if ($ret) {
			return $ret;
		}

		$ret = $this->showView(
			'PackageResults',
			array(
				'results' => array($pluginInfo),
			)
		);

		if ($ret) {
			return $ret;
		}

		return null;
	}

	/**
	 * Creates packages for all plugins that match the 'filter' request variable.
	 *
	 * @return GalleryStatus a status code
	 */
	public function packagePlugins() {
		global $gallery;

		$pluginsPackaged = 0;

		// Set default filter if none is provided.
		$filter = GalleryUtilities::getRequestVariables('filter');

		if (strlen($filter) < 3) {
			// Is there a check to see if a regexp is valid?
			$filter = '/.*/';
		}

		list($ret, $plugins) = $this->getAvailablePluginList(array());

		if ($ret) {
			return $ret;
		}

		$packagedPluginList = array();

		foreach ($plugins as $pluginId => $plugin) {
			$gallery->guaranteeTimeLimit(60);

			if (preg_match($filter, $pluginId)) {
				list($ret, $pluginInfo) = $this->doPackagePlugin($plugin['type'], $pluginId);

				if ($ret) {
					return $ret;
				}

				$packagedPluginList[] = $pluginInfo;
			}
		}

		$this->showView(
			'PackageResults',
			array(
				'results' => $packagedPluginList,
			)
		);

		return null;
	}

	/**
	 * Creates all packages for the specified plugin.
	 *
	 * @return array GalleryStatus a status code
	 *               array('pluginId' => ..., 'outputDir' => ...,
	 *                     'packageInfo' => ..., 'errors' => ...)
	 */
	public function doPackagePlugin($pluginType, $pluginId) {
		global $gallery;

		$outputDir = $gallery->getConfig('repository.path');

		list($ret, $plugin) = GalleryCoreApi::loadPlugin($pluginType, $pluginId, true);

		if ($ret) {
			return array($ret, null);
		}

		$outputDir .= sprintf('%ss', $pluginType);
		$descriptor = new RepositoryDescriptor();

		$ret = $descriptor->init($outputDir);

		if ($ret) {
			return array($ret, null);
		}

		$ret = $descriptor->generate($plugin);

		if ($ret) {
			return array($ret, null);
		}

		$overwrite       = GalleryUtilities::getRequestVariables('overwrite');
		$integrityErrors = $descriptor->checkIntegrity();
		$pluginInfo      = array(
			'pluginId'  => $descriptor->getPluginId(),
			'outputDir' => $descriptor->getOutputDir(),
		);

		if (empty($integrityErrors)) {
			$ret = $descriptor->writeToDisk($overwrite, true);

			if ($ret) {
				return array($ret, null);
			}

			$package = new RepositoryPackage();

			$ret = $package->init($descriptor, $descriptor->getOutputDir());

			if ($ret) {
				return array($ret, null);
			}

			list($ret, $packagesWritten) = $package->writeAllPackagesToDisk($overwrite, true);

			if ($ret) {
				return array($ret, null);
			}

			$pluginInfo['packageInfo'] = $packagesWritten;
			$pluginInfo['errors']      = array();
		} else {
			$pluginInfo['packageInfo'] = array(
				'packagesWritten' => array(),
				'packagesSkipped' => array(),
			);

			$pluginInfo['errors'] = $integrityErrors;
		}

		return array(null, $pluginInfo);
	}

	/**
	 * Creates a list of all existing plugins with optional action links.
	 *
	 * @param array $links 'action' => array(UrlGeneratorParameters)
	 * @return array GalleryStatus a status code
	 *           array plugin list
	 */
	public function getAvailablePluginList($links) {
		global $gallery;

		$urlGenerator =& $gallery->getUrlGenerator();
		$data         = array();
		$pluginTypes  = array('module', 'theme');

		foreach ($pluginTypes as $pluginType) {
			list($ret, $plugins) = GalleryCoreApi::fetchPluginStatus($pluginType, true);

			if ($ret) {
				return array($ret, null);
			}

			ksort($plugins);

			foreach ($plugins as $pluginId => $plugin) {
				$data[$pluginId]['type']      = $pluginType;
				$data[$pluginId]['active']    = isset($plugin['active']) ? 'active' : 'inactive';
				$data[$pluginId]['available'] = isset($plugin['available']) ? 'available' : 'unavailable';
				$pluginUrlParameters          = array(
					'pluginId'   => $pluginId,
					'pluginType' => $pluginType,
				);

				foreach ($links as $link => $extraUrlParameters) {
					$data[$pluginId]['links'][$link] = $urlGenerator->generateUrl($extraUrlParameters + $pluginUrlParameters);
				}
			}
		}

		return array(null, $data);
	}
}
