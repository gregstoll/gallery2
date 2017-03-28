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

GalleryCoreApi::requireOnce('modules/password/classes/PasswordHelper.class');

/**
 * ItemEditOption for adding password protection to an item or album
 * @package Password
 * @subpackage UserInterface
 * @author Alan Harder <alan.harder@sun.com>
 * @author Jess Martin <jmartin@cs.unc.edu>
 * @version $Revision: 17657 $
 */
class PasswordOption extends ItemEditOption {

    /**
     * @see ItemEditOption::isAppropriate
     */
    function isAppropriate($item, $thumbnail) {
	list ($ret, $canChange) =
	    GalleryCoreApi::hasItemPermission($item->getId(), 'core.changePermissions');
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * Password protected items are not fully protected, but their descendants are. Only show
	 * the option for non-album items that already are password protected.
	 */
	list ($ret, $hasPassword) = PasswordHelper::hasPassword($item);
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $canChange && ($hasPassword || $item->getCanContainChildren()));
    }

    /**
     * @see ItemEditOption::loadTemplate
     */
    function loadTemplate(&$template, &$form, $item, $thumbnail) {
	$form['PasswordOption']['hasPassword'] = $item->hasOnLoadHandler('Password');
	$form['PasswordOption']['isAlbum'] = $item->getCanContainChildren();
	return array(null, 'modules/password/templates/PasswordOption.tpl', 'modules_password');
    }

    /**
     * @see ItemEditOption::handleRequestAfterEdit
     */
    function handleRequestAfterEdit($form, &$item, &$preferred) {
	$error = $warning = array();
	$useProgressBar = $item->getCanContainChildren();

	$ret = GalleryCoreApi::assertHasItemPermission($item->getId(), 'core.changePermissions');
	if ($ret) {
	    return array($ret, null, null);
	}

	if (!empty($form['PasswordOption']['remove'])) {
	    $ret = PasswordHelper::removePassword($item, $useProgressBar);
	    if ($ret) {
		return array($ret, null, null);
	    }
	} else if (!empty($form['PasswordOption']['password1'])
		|| !empty($form['PasswordOption']['password2'])) {
	    if ($form['PasswordOption']['password1'] == $form['PasswordOption']['password2']) {
		GalleryUtilities::unsanitizeInputValues($form['PasswordOption']['password1'],
							false);
		GalleryCoreApi::requireOnce('modules/password/classes/PasswordHelper.class');
		$ret = PasswordHelper::setPassword($item, $form['PasswordOption']['password1'],
						   $useProgressBar);
		if ($ret) {
		    return array($ret, null, null);
		}
	    } else {
		/* Throw an error because both passwords do not match */
		$error[] = 'form[error][PasswordOption][mismatch]';
	    }
	}

	return array(null, $error, $warning);
    }

    /**
     * @see ItemEditOption::requiresProgressBar
     */
    function requiresProgressBar($form) {
	/* Progress bar if adding new password or removing password from an album */
	return isset($form['PasswordOption']['progressBar']) && (
		($form['PasswordOption']['progressBar'] == 'add'
		 && !empty($form['PasswordOption']['password1'])
		 && !empty($form['PasswordOption']['password2'])
		 && $form['PasswordOption']['password1'] == $form['PasswordOption']['password2'])
		||
		($form['PasswordOption']['progressBar'] == 'remove'
		 && !empty($form['PasswordOption']['remove']))
	    );
    }
}
?>
