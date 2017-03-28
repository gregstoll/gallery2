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
 * Settings for Captcha
 * @package Captcha
 * @subpackage UserInterface
 * @author Stefan Ioachim <stefanioachim@gmail.com>
 * @author Bharat Mediratta <bharat@menalto.com>
 * @author Alan Harder <alan.harder@sun.com>
 * @version $Revision: 17580 $
 */
class CaptchaSiteAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$error = $status = array();
	if (isset($form['action']['save'])) {
	    if (!isset($form['failedAttemptThreshold']) ||
		    !is_numeric($form['failedAttemptThreshold'])) {
		/* We don't allow free-form input in the HTML, so this should never happen. */
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }

	    /* Make sure that we're between 0 and 5, then store the value */
	    $form['failedAttemptThreshold'] = min($form['failedAttemptThreshold'], 5);
	    $form['failedAttemptThreshold'] = max($form['failedAttemptThreshold'], 0);
	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'captcha', 'failedAttemptThreshold', $form['failedAttemptThreshold']);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Save configuration for other modules */
	    if (!empty($form['level'])) {
		list ($ret, $optionInstances) = CaptchaAdminOption::getAllOptions();
		if ($ret) {
		    return array($ret, null);
		}
		foreach ($form['level'] as $optionId => $level) {
		    if (!isset($optionInstances[$optionId])) {
			return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
		    }
		    list ($ret, $title, $paramData, $choices) =
			$optionInstances[$optionId]->getOptionData();
		    if ($ret) {
			return array($ret, null);
		    }
		    if (!in_array($level, $choices)) {
			return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
		    }
		    $ret = GalleryCoreApi::setPluginParameter($paramData[0], $paramData[1],
							      $paramData[2], $level);
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'captcha.CaptchaSiteAdmin';
	    $status['saved'] = 1;
	} else if (isset($form['action']['reset'])) {
	    $redirect['view'] = 'core.SiteAdmin';
	    $redirect['subView'] = 'captcha.CaptchaSiteAdmin';
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'captcha.CaptchaSiteAdmin';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * Settings for Captcha
 */
class CaptchaSiteAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load our default values if we didn't just come from this form. */
	if ($form['formName'] != 'CaptchaSiteAdmin') {
	    $form['formName'] = 'CaptchaSiteAdmin';

	    list ($ret, $form['failedAttemptThreshold']) =
		GalleryCoreApi::getPluginParameter('module', 'captcha', 'failedAttemptThreshold');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* Load admin options */
	list ($ret, $optionInstances) = CaptchaAdminOption::getAllOptions();
	if ($ret) {
	    return array($ret, null);
	}
	$optionData = array();
	foreach ($optionInstances as $optionId => $option) {
	    list ($ret, $title, $paramData, $choices) = $option->getOptionData();
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $param) =
		GalleryCoreApi::getPluginParameter($paramData[0], $paramData[1], $paramData[2]);
	    if ($ret) {
		return array($ret, null);
	    }
	    $optionData[$optionId] =
		array('title' => $title, 'value' => $param, 'choices' => $choices);
	}

	$template->setVariable('CaptchaSiteAdmin',
			       array('failedAttemptThresholdList' => range(0, 5),
				     'options' => $optionData));
	$template->setVariable('controller', 'captcha.CaptchaSiteAdmin');

	return array(null,
		     array('body' => 'modules/captcha/templates/CaptchaSiteAdmin.tpl'));
    }
}

/**
 * Interface for other modules to add captcha configuration into captcha site admin.
 *
 * @package Captcha
 * @subpackage UserInterface
 */
class CaptchaAdminOption {

    /**
     * Return all available options.
     *
     * @return array GalleryStatus a status code
     *               array of string optionId => CaptchaAdminOption instance
     * @static
     */
    function getAllOptions() {
	$testOptions = CaptchaAdminOption::testOptions();
	if (isset($testOptions)) {
	    return array(null, $testOptions);
	}

	list ($ret, $optionIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('CaptchaAdminOption');
	if ($ret) {
	    return array($ret, null);
	}

	$optionInstances = array();
	foreach (array_keys($optionIds) as $optionId) {
	    list ($ret, $optionInstances[$optionId]) =
		GalleryCoreApi::newFactoryInstanceById('CaptchaAdminOption', $optionId);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	return array(null, $optionInstances);
    }

    /**
     * Set options to use with unit tests.
     *
     * @param array $options (optional) array of string optionId => CaptchaAdminOption instance
     * @return null or array of string optionId => CaptchaAdminOption test instance
     * @static
     */
    function testOptions($options=null) {
	static $testOptions = null;
	if (isset($options)) {
	    $testOptions = $options;
	}
	return $testOptions;
    }

    /**
     * Get data about this configuration option.
     *
     * @return array GalleryStatus a status code
     *               string localized title for this option
     *               array(pluginType, pluginId, parameterName) to configure
     *               array of string choices (any of HIGH, MEDIUM, LOW, OFF)
     */
    function getOptionData() {
	return array(GalleryCoreApi::error(ERROR_UNIMPLEMENTED),
		     null, null, null);
    }
}
?>
