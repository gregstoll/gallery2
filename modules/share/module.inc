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
 * Implementation of the Share module
 *
 * @package Share
 * @author Karl Newman
 * @version $Revision: 1910 $
 */
class ShareModule extends GalleryModule
{

	function __construct()
	{
		global $gallery;

		$this->setId('share');
		$this->setName($gallery->i18n('Share'));
		$this->setDescription($gallery->i18n('Share link to Gallery pictures on social media sites'));
		$this->setVersion('1.0.1'); /* Update upgrade() function below too */
		$this->_templateVersion = 1;
		$this->setGroup('export', $gallery->i18n('Export'));
		$this->setCallbacks('getSiteAdminViews');
		$this->setRequiredCoreApi(array(7, 2));
		$this->setRequiredModuleApi(array(3, 0));
	}

	/**
	 * @see GalleryModule::getSiteAdminViews
	 */
	function getSiteAdminViews()
	{
		return array(
			null,
			array(
				array(
					'name' => $this->translate('Share'),
					'view' => 'share.ShareSiteAdmin',
				),
			),
		);
	}

	/**
	 * @see GalleryModule::upgrade
	 */
	function upgrade($currentVersion, $x1 = NULL)
	{
		switch ($currentVersion) {
			case null:
				/* Initial install */

				foreach (array(
					'onlyWhenLoggedIn' => false,
					'addThisAccountId' => 'addThisAccountId',
				) as $key => $value) {
					$ret = $this->setParameter($key, $value);
					if ($ret) {
						return $ret;
					}
				}
			/* keep going */

			case '1.0.0':
			case '1.0.1': /* Fixed typo in block template */
			default:
		}

		return null;
	}
}

?>
