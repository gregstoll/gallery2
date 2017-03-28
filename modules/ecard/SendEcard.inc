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

GalleryCoreApi::requireOnce('modules/ecard/classes/EcardHelper.class');

/**
 * Send photo as eCard
 * @package Ecard
 * @subpackage UserInterface
 * @author Nico Kaiser <nico@siriux.net>
 * @author Dariush Molavi <dari@nukedgallery.net>
 * @version $Revision: 17580 $
 */
class SendEcardController extends GalleryController {

    /**
     * ValidationPlugin instances to use when handling this request.  Only used by test code.
     *
     * @var array $_plugins (array of GalleryValidationPlugin)
     * @access private
     */
    var $_pluginInstances;

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$session =& $gallery->getSession();
	$redirect = $status = $error = array();

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'ecard.send');
	if ($ret) {
	    return array($ret, null);
	}

	if (isset($form['action']['send']) || isset($form['action']['preview'])) {
	    /* Validate form */
	    if (empty($form['to'])) {
		$error[] = 'form[error][to][missing]';
	    } else if (!GalleryUtilities::isValidEmailString($form['to'])) {
		$error[] = 'form[error][to][invalid]';
	    }
	    if (empty($form['from'])) {
		$error[] = 'form[error][from][missing]';
	    } else if (!GalleryUtilities::isValidEmailString($form['from'])) {
		$error[] = 'form[error][from][invalid]';
	    }
	    if (empty($form['text'])) {
		$error[] = 'form[error][text][missing]';
	    }

	    list ($ret, $useValidationPlugins) = EcardHelper::useValidationPlugins();
	    if ($ret) {
		return array($ret, null);
	    }
	    if (isset($form['action']['send']) && $useValidationPlugins) {
		if (isset($this->_pluginInstances)) {
		    $pluginInstances = $this->_pluginInstances;
		} else {
		    list ($ret, $pluginInstances) =
			GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
		    if ($ret) {
			return array($ret, null);
		    }
		    foreach (array_keys($pluginInstances) as $pluginId) {
			list ($ret, $pluginInstances[$pluginId]) =
			    GalleryCoreApi::newFactoryInstanceById('GalleryValidationPlugin',
								   $pluginId);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}

		/* Let each plugin do its verification */
		foreach ($pluginInstances as $plugin) {
		    list ($ret, $pluginErrors, $continue) = $plugin->performValidation($form);
		    if ($ret) {
			return array($ret, null);
		    }
		    $error = array_merge($error, $pluginErrors);
		    if (!$continue) {
			break;
		    }
		}
	    }
	}
	if (isset($form['action']['send']) && empty($error)) {
	    GalleryCoreApi::requireOnce('lib/pear/mime.php');

	    list ($ret, $config) = GalleryCoreApi::fetchAllPluginParameters('module', 'ecard');
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $resize) = EcardHelper::getSmallestSize($itemId);
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $path) = $resize->fetchPath();
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $anonymousUserId) = GalleryCoreApi::getAnonymousUserId();
	    if ($ret) {
		return array($ret, null);
	    }
	    list ($ret, $canLink) =
		GalleryCoreApi::hasItemPermission($itemId, 'core.view', $anonymousUserId);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Get pseudo filename and attach image */
	    list ($ret, $filename) = GalleryUtilities::getPseudoFileName($resize);
	    if ($ret) {
		return array($ret, null);
	    }
	    $urlGenerator =& $gallery->getUrlGenerator();
	    $link = $canLink ? $urlGenerator->generateUrl(
		    array('view' => 'core.ShowItem', 'itemId' => $itemId),
		    array('forceFullUrl' => true, 'forceSessionId' => false)) : '';

	    $data = $form;
	    $data['header'] = EcardHelper::replaceKeywords($config['header'], $form);
	    $data['footer'] = EcardHelper::replaceKeywords($config['footer'], $form);
	    $data['image'] = $filename;
	    $data['link'] = $link;

	    $dataPlain = $data;
	    GalleryUtilities::unsanitizeInputValues($dataPlain, false);
	    $dataPlain['image'] = $data['image'];

	    $data['header'] = nl2br($data['header']);
	    $data['footer'] = nl2br($data['footer']);
	    $data['text'] = nl2br($data['text']);

	    $headers = array(
		'from'    => $config['from'] ? $config['from']
					     : $form['fromName'] . ' <' . $form['from'] . '>',
		'to'      => $form['to'],
		'subject' => EcardHelper::replaceKeywords($config['subject'], $form)
		);

	    if (!empty($config['bcc'])) {
		$headers['bcc'] = $config['bcc'];
	    }
	    GalleryUtilities::unsanitizeInputValues($headers, false);

	    $mime = new Mail_mime("\n");
	    $platform =& $gallery->getPlatform();

	    $messagePlain = $platform->file_get_contents(
		    dirname(__FILE__) . '/templates/Message.txt');
	    $messagePlain = EcardHelper::replaceKeywords($messagePlain, $dataPlain);
	    $mime->setTXTBody($messagePlain);

	    /* Either add attachment (plain) or add HTML image (html) */
	    if ($config['format'] == 'plain') {
		$mime->addAttachment($path, $resize->getMimeType(), $filename);
	    } else {
		$messageHtml = $platform->file_get_contents(
		    dirname(__FILE__) . '/templates/Message.html');
		$messageHtml = EcardHelper::replaceKeywords($messageHtml, $data);
		$mime->setHTMLBody($messageHtml);
		if ($resize->canBeViewedInline()) {
		    $mime->addHTMLImage($path, $resize->getMimeType(), $filename);
		} else {
		    $mime->addAttachment($path, $resize->getMimeType(), $filename);
		}
	    }

	    $body = $mime->get(array(
		'html_charset'  => 'UTF-8',
		'text_charset'  => 'UTF-8',
		'text_encoding' => '8bit',
		'head_charset'  => 'UTF-8'
		)
	    );

	    $tmp = array('From' => $headers['from']);
	    if (isset($headers['bcc'])) {
		$tmp['bcc'] = $headers['bcc'];
	    }
	    $hdrs = '';

	    foreach ($mime->headers($tmp) as $key => $value) {
		$hdrs .= $key . ': ' . $value . "\n";
	    }
	    for ($buf = '', $i = 0; $i < strlen($headers['subject']); $i++) {
		$val = ord($headers['subject']{$i});
		$buf .= ($val > 127) ? '=' . dechex($val) : $headers['subject']{$i};
	    }
	    if ($buf != $headers['subject']) {
		/* Encode subject for UTF-8 display if any extended characters are present */
		$headers['subject'] = '=?utf-8?q?' . $buf . '?=';
	    }
	    $success = $platform->mail($headers['to'], $headers['subject'], $body, $hdrs);
	    if (!$success) {
		return array(GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__,
		     'Could not send mail to ' . $headers['to']), null);
	    }

	    /* Put sender data into session */
	    $session->put('ecard.from', $form['from']);
	    if (isset($form['fromName'])) {
		$session->put('ecard.fromName', $form['fromName']);
	    }

	    /* Send the user to a confirmation page, for now */
	    $redirect['view'] = 'ecard.Confirmation';
	    $redirect['itemId'] = (int)$itemId;
	    $status['sent'] = 1;
	} else if (isset($form['action']['cancel'])) {
	    /* Where to go on a cancel?  Back to viewing the item. */
	    $redirect['view'] = 'core.ShowItem';
	    $redirect['itemId'] = (int)$itemId;
	}

	/* Prepare our results */
	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'ecard.SendEcard';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }

    /**
     * Tests can use this method to hardwire a specific set of plugin instances to use.
     * This avoids situations where some of the option instances will do unpredictable
     * things and derail the tests.
     *
     * @param array $pluginInstances of GalleryValidationPlugin
     */
    function setPluginInstances($pluginInstances) {
	$this->_pluginInstances = $pluginInstances;
    }
}

/**
 * This view will show an eCard form
 */
class SendEcardView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;
	$session =& $gallery->getSession();

	/* Load our item */
	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $config) = GalleryCoreApi::fetchAllPluginParameters('module', 'ecard');
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'SendEcard') {
	    $form['formName'] = 'SendEcard';
	    $form['to'] = $form['toName'] = $form['text'] = '';
	    $form['from'] = $session->get('ecard.from');
	    $form['fromName'] = $session->get('ecard.fromName');
	    if (empty($form['from'])) {
		list ($ret, $isGuest) = GalleryCoreApi::isAnonymousUser();
		if ($ret) {
		    return array($ret, null);
		}
		if (!$isGuest) {
		    $user = $gallery->getActiveUser();
		    $form['from'] = $user->getEmail();
		    $form['fromName'] = $user->getFullName();
		}
	    }
	} else {
	    foreach (array('to', 'toName', 'from', 'fromName', 'text') as $key) {
		if (empty($form[$key])) {
		    $form[$key] = '';
		}
	    }
	}

	$SendEcard = array();
	$SendEcard['itemId'] = $item->getId();
	$SendEcard['item'] = (array)$item;
	$SendEcard['emailFrom'] = $config['from'] ? $config['from']
				: $form['fromName'] . ' &lt;' . $form['from'] . '&gt;';

	list ($ret, $resize) = EcardHelper::getSmallestSize($item->getId());
	if ($ret) {
	    return array($ret, null);
	}

	$SendEcard['resize'] = (array)$resize;
	$SendEcard['subject'] = EcardHelper::replaceKeywords($config['subject'], $form);
	if (!empty($config['bcc'])) {
	    $SendEcard['bcc'] = $config['bcc'];
	}
	$SendEcard['header'] = EcardHelper::replaceKeywords($config['header'], $form);
	$SendEcard['footer'] = EcardHelper::replaceKeywords($config['footer'], $form);

	/* Check if we should use validation plugins */
	$plugins = array();
	list ($ret, $useValidationPlugins) = EcardHelper::useValidationPlugins();
	if ($ret) {
	    return array($ret, null);
	}
	if ($useValidationPlugins) {
	    list ($ret, $allPluginIds) =
		GalleryCoreApi::getAllFactoryImplementationIds('GalleryValidationPlugin');
	    if ( $ret) {
		return array($ret, null);
	    }

	    /* Let each plugin load its template data */
	    foreach (array_keys($allPluginIds) as $pluginId) {
		list ($ret, $plugin) =
		    GalleryCoreApi::newFactoryInstanceById('GalleryValidationPlugin', $pluginId);
		if ($ret) {
		    return array($ret, null);
		}
		list ($ret, $data['file'], $data['l10Domain']) = $plugin->loadTemplate($form);
		if ($ret) {
		    return array($ret, null);
		}
		if (isset($data['file'])) {
		    $plugins[] = $data;
		}
	    }
	}

	$SendEcard['plugins'] = $plugins;

	$template->setVariable('SendEcard', $SendEcard);
	$template->setVariable('controller', 'ecard.SendEcard');

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'ecard');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('Send eCard'));
	return array(null, array('body' => 'modules/ecard/templates/SendEcard.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'ecard');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('Send eCard'));
    }
}
?>
