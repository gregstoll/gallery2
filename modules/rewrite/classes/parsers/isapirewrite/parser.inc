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

GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteParser.class');

/* Status code used by the PHP Path Info parser */
define('REWRITE_STATUS_NO_ISAPI_REWRITE', 31);
define('REWRITE_STATUS_HTTPDINI_MISSING', 32);
define('REWRITE_STATUS_HTTPDINI_CANT_READ', 33);
define('REWRITE_STATUS_HTTPDINI_CANT_WRITE', 34);

/**
 * This URL rewrite parser provides ISAPI_Rewrite support for short URLs.
 *
 *   http://www.isapirewrite.com/
 *
 * @package Rewrite
 * @subpackage Parsers
 * @version $Revision: 17580 $
 * @author Douglas Cau <douglas@cau.se>
 */
class IsapiRewriteParser extends RewriteParser {

    function IsapiRewriteParser() {
	$this->_setParserId('isapirewrite');
	$this->_setParserType('preGallery');
	$this->_setUrlGeneratorId('IsapiRewriteUrlGenerator');
    }

    /**
     * @see RewriteParser::saveActiveRules
     */
    function saveActiveRules($activeRules=null, $upgradeModule=null) {
	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');
	return IsapiRewriteHelper::saveActiveRules($this, $activeRules, $upgradeModule);
    }

    /**
     * @see RewriteParser::saveAccessList
     */
    function saveAccessList($accessList, $allowEmptyReferer) {
	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');
	return IsapiRewriteHelper::saveAccessList($accessList, $allowEmptyReferer);
    }

    /**
     * @see RewriteParser::needsConfiguration
     */
    function needsConfiguration() {
	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');
	return IsapiRewriteHelper::needsConfiguration($this);
    }

    /**
     * @see RewriteParser::isValidRule
     */
    function isValidRule($rule, $activeRule=null) {
	$isValid = parent::isValidRule($rule, $activeRule);
	if (!$isValid) {
	    return false;
	}

	if (isset($rule['conditions'])) {
	    foreach ($rule['conditions'] as $condition) {
		/*
		 * ISAPI_Rewrite supports only certain test strings:
		 * http://www.isapirewrite.com/docs/#RewriteCond
		 */
		if (!in_array($condition['test'], array(
			    'REQUEST_URI', 'QUERY_STRING', 'REQUEST_METHOD', 'VERSION'))
			&& strncmp($condition['test'], 'HTTP:', 5) !== 0) {
		    return false;
		}
	    }
	}

	if ((isset($rule['pattern']) && substr($rule['pattern'], 0, 1) == '%')
		|| (isset($activeRule['pattern']) && substr($activeRule['pattern'], 0, 1) == '%')) {
	    return false;
	}

	return true;
    }

    /**
     * @see RewriteParser::getErrorMessage
     */
    function getErrorMessage($code, $rewriteModule=null) {
	if (!isset($rewriteModule)) {
	    list($ret, $rewriteModule) = GalleryCoreApi::loadPlugin('module', 'rewrite');
	    if ($ret) {
		return array($ret, null);
	    }
	}

	list ($ret, $message) = parent::getErrorMessage($code, $rewriteModule);
	if ($ret) {
	    return array($ret, null);
	}

	if (!isset($message)) {
	    switch ($code) {
	    case REWRITE_STATUS_HTTPDINI_CANT_READ:
		$message = $rewriteModule->translate('Cannot read the httpd.ini file');
		break;
	    case REWRITE_STATUS_HTTPDINI_CANT_WRITE:
		$message = $rewriteModule->translate('Cannot write to httpd.ini file');
		break;
	    case REWRITE_STATUS_HTTPDINI_MISSING:
		$message = $rewriteModule->translate('Cannot write to httpd.ini file, please ' .
		    'create it.');
		break;
	    default:
		return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	    }
	}

	return array(null, $message);
    }

    /**
     * @see RewriteParser::loadTestResultsTemplate
     */
    function loadTestResultsTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');
	return IsapiRewriteHelper::loadTestResultsTemplate($template, $form);
    }


    /**
     * @see RewriteParser::handleTestResultsRequest
     */
    function handleTestResultsRequest($form) {
	$error = $status = array();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null, null);
	}

	if (isset($form['force']['test'])) {
	    $ret = GalleryCoreApi::setPluginParameter(
		'module', 'rewrite', 'isapirewrite.forced', true);
	    if ($ret) {
		return array($ret, null, null);
	    }
	}

	$status['saved'] = 1;
	return array(null, $error, $status);
    }

    /**
     * @see RewriteParser::loadAdminParserTemplate
     */
    function loadAdminParserTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');
	return IsapiRewriteHelper::loadAdminRewriteTemplate($template, $form);
    }

    /**
     * @see RewriteParser::handleTestResultsRequest
     */
    function handleAdminParserRequest($form) {
	$error = $status = array();

	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');
	global $gallery;
	$platform =& $gallery->getPlatform();

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null, null);
	}

	if (empty($form['httpdini'])) {
	    $error[] = 'form[error][' . REWRITE_STATUS_EMPTY_VALUE . ']';
	    return array(null, $error, $status);
	}

	$path = $form['httpdini'];
	$path = rtrim($form['httpdini'], "\\");

	if (!$platform->is_dir($path)) {
	    $error[] = 'form[error][invalidDir]';
	    return array(null, $error, $status);
	}

	if (!$platform->is_writeable($path . '\httpd.ini', 'w')) {
	    $error[] = 'form[error][' . REWRITE_STATUS_HTTPDINI_CANT_WRITE . ']';
	    return array(null, $error, $status);
	}

	$ret = GalleryCoreApi::setPluginParameter(
	    'module', 'rewrite', 'isapirewrite.httpdini', $path);
	if ($ret) {
	    return array($ret, null, null);
	}

	if (GalleryUtilities::isEmbedded()) {
	    if (empty($form['embeddedLocation'])) {
		$error[] = 'form[error][' . REWRITE_STATUS_EMPTY_VALUE . ']';
		return array(null, $error, $status);
	    }

	    list ($ret, $code) = IsapiRewriteHelper::saveEmbedConfig(
		array('embeddedLocation' => $form['embeddedLocation']), $this,
		GalleryUtilities::getRequestVariables('controller') != 'SetupRewriteController');
	    if ($ret) {
		return array($ret, null, null);
	    }

	    if ($code != REWRITE_STATUS_OK) {
		$error[] = 'form[error][' . $code . ']';
	    }
	}

	if (empty($error)) {
	    $status['saved'] = 1;
	}

	return array(null, $error, $status);
    }

    /**
     * @see RewriteParser::needsEmbedConfig
     */
    function needsEmbedConfig() {
	list ($ret, $value) = GalleryCoreApi::getPluginParameter(
	    'module', 'rewrite', 'isapirewrite.embeddedLocation');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, empty($value));
    }

    /**
     * @see RewriteParser::fetchEmbedConfig
     */
    function fetchEmbedConfig() {
	list ($ret, $value) = GalleryCoreApi::getPluginParameter(
	    'module', 'rewrite', 'isapirewrite.embeddedLocation');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, array('embeddedLocation' => $value));
    }

    /**
     * @see RewriteParser::fetchEmbedConfig
     */
    function saveEmbedConfig($param) {
	GalleryCoreApi::requireOnce(
	    'modules/rewrite/classes/parsers/isapirewrite/IsapiRewriteHelper.class');

	list ($ret, $code) = IsapiRewriteHelper::saveEmbedConfig($param, $this, true);
	if ($ret) {
	    return array($ret, null, null);
	}

	return array(
	    null, $code, ($code != REWRITE_STATUS_OK) ? $this->getErrorMessage($code) : null);
    }
}
?>
