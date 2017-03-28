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

GalleryCoreApi::requireOnce('modules/notification/classes/NotificationHelper.class');

/**
 * This controller handles event / method settings on a site-wide basis.
 * @package Notification
 * @subpackage UserInterface
 * @author Zimzat <zimzat@zimzat.com>
 * @author Tim Almdal <tnalmdal@shaw.ca>
 * @version $Revision: 17802 $
 */
class NotificationSiteAdminController extends GalleryController {
    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$storage =& $gallery->getStorage();

	$error = $status = array();

	/* Check if logged in and is admin */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$returnOption = 'redirect';

	if (!empty($form['action']['save'])) {
	    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'notification');
	    if ($ret) {
		return array($ret, null);
	    }

	    list ($ret, $notifications, $eventMapLockId) =
		NotificationHelper::getEnabledNotifications(true);
	    if ($ret) {
		return array($ret, null);
	    }
	    $inputRows = 0;
	    foreach ($form['notificationMap'] as $i => $notificationMap) {
		if (empty($notificationMap['currentName'])
			&& empty($notificationMap['notificationName'])) {
		    continue;
		}

		if (!empty($notificationMap['notificationName'])
			&& empty($notificationMap['handler'])) {
		    $error[] = 'form[error][notificationMap][' . $i . '][noEventHandler]';
		    continue;
		}

		$inputRows++;
		if (empty($notificationMap['currentName'])) {
		    $ret = $this->_addHandlerEntry($notifications, $notificationMap);
		    if ($ret) {
			return array($ret, null);
		    }
		} else if (empty($notificationMap['notificationName'])) {
		    $this->_removeHandlerEntry($notifications, $notificationMap);
		} else {
		    $notificationName = $notificationMap['notificationName'];
		    $handlerId = $notificationMap['handler'];
		    if ($notificationMap['currentName'] != $notificationName
			    || $notificationMap['currentHandler'] != $handlerId) {
			$this->_removeHandlerEntry($notifications, $notificationMap);
			$ret = $this->_addHandlerEntry($notifications, $notificationMap);
			if ($ret) {
			    return array($ret, null);
			}
		    } else {
			$notifications[$notificationName][$handlerId]['public'] =
			    !empty($notificationMap['public']);
			$notifications[$notificationName][$handlerId]['enabled'] =
			    !empty($notificationMap['enabled']);
		    }
		}
	    }

	    if (empty($error)) {
		$ret = GalleryCoreApi::setPluginParameter('module', 'notification', '_enabledMap',
							  serialize($notifications));
		if (empty($ret)) {
		    /* Alert the user of the changes we have done. */
		    $status['saved'] = 1;
		}
	    } else {
		$returnOption = 'delegate';
		$displayRows = $inputRows + 1;
	    }

	    $ret = GalleryCoreApi::releaseLocks($eventMapLockId);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$results[$returnOption] = array('view' => 'core.SiteAdmin',
					'subView' => 'notification.NotificationSiteAdmin');
	if ($returnOption == 'delegate') {
	    $results['delegate']['form[displayRows]'] = $displayRows;
	}
	$results['status'] = $status;
	$results['error'] = $error;
	return array(null, $results);
    }

    /**
     * Add the event/handler combination to the available notifications
     *
     * @param array $notifications The array of enabled event/handler combinations
     * @param array $notificationMap the current form entry defining an event/handler combinations
     * @return array GalleryStatus
     * @private
     */
    function _addHandlerEntry(&$notifications, $notificationMap) {
	$notificationName = $notificationMap['notificationName'];
	$handlerId = $notificationMap['handler'];

	list ($ret, $handler) =
	    GalleryCoreApi::newFactoryInstanceById('NotificationHandler_1_0', $handlerId);
	if ($ret) {
	    return $ret;
	}

	$notifications[$notificationName][$handlerId] =
			array('public' => !empty($notificationMap['public']),
			      'enabled' => !empty($notificationMap['enabled']));

	return null;
    }

    /**
     * Remove the event/handler combination to the available notifications
     *
     * @param array $notifications The array of enabled event/handler combinations
     * @param array $notificationMap the current form entry defining an event/handler combinations
     * @private
     */
    function _removeHandlerEntry(&$notifications, $notificationMap) {
	$notificationName = $notificationMap['currentName'];
	$handlerId = $notificationMap['currentHandler'];

	unset($notifications[$notificationName][$handlerId]);
	
	if (empty($notifications[$notificationName])) {
	    unset($notifications[$notificationMap['currentName']]);
	}
    }
}

/**
 * This view shows all the event / method settings on a site-wide basis.
 */
class NotificationSiteAdminView extends GalleryView {
    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$template->style('modules/notification/data/grids.css');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $definedEvents) = NotificationHelper::getDefinedEvents(true);
	if ($ret) {
	    return array($ret, null);
	}
	$template->setVariable('definedEvents', $definedEvents);

	list ($ret, $handlers) = NotificationHelper::getDefinedHandlers(true);
	if ($ret) {
	    return array($ret, null);
	}
	$template->setVariable('eventHandlers', $handlers);

	if ($form['formName'] != 'NotificationSiteAdmin') {
	    $ret = $this->_initializeForm($form, $definedEvents, $handlers);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$template->setVariable('controller', 'notification.NotificationSiteAdmin');

	return array(null,
		     array('body' => 'modules/notification/templates/NotificationSiteAdmin.tpl'));
    }

    /**
     * Helper function to initialize the form
     * @param array $form the form values
     * @param array $events the defined notification events
     * @param array $handlers the defined notification handlers
     * @return GalleryStatus
     * @access private
     */
    function _initializeForm(&$form, $events, $handlers) {
	$form['formName'] = 'NotificationSiteAdmin';

	list ($ret, $notifications) = NotificationHelper::getEnabledNotifications();
	if ($ret) {
	    return $ret;
	}

	$form['notificationMap'] = array();
	$i = 1;
	foreach ($notifications as $notificationName => $handlers) {
	    foreach($handlers as $handler => $enabledEvent) {
		$form['notificationMap'][$i++] = array('currentName' => $notificationName,
				      'notificationName' => $notificationName,
				      'description' => $events[$notificationName]['description'],
				      'isGlobal' => $events[$notificationName]['global'],
				      'currentHandler' => $handler,
				      'handler' => $handler,
				      'public' => $enabledEvent['public'],
				      'enabled' => $enabledEvent['enabled']);
	    }
	}

	$form['totalRows'] = count($form['notificationMap']) + 20;
	$form['displayRows'] = count($notifications) + 1;
	return null;
    }
}
?>
