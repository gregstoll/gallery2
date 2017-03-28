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
 * This controller saves changes to settings on a per-user basis.
 * @package Notification
 * @subpackage UserInterface
 * @author Zimzat <zimzat@zimzat.com>
 * @author Tim Almdal <tnalmdal@shaw.ca>
 * @version $Revision: 18132 $
 */
class NotificationUserAdminController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$status = $error = array();

	/* Check that the user is logged in */
	list ($ret, $userId) = NotificationHelper::getUserIdCheck();
	if ($ret) {
	    return array($ret, null);
	}

	/* Is the user an administrator */
	list ($ret, $isAdminGroup) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	$returnOption = 'redirect';
	if (!empty($form['action']['save'])) {
	    foreach ($form['notifications'] as $i => $notification) {
		$notificationName = $notification['name'];
		list ($ret, $instance) = GalleryCoreApi::newFactoryInstanceById(
							'NotificationEvent_1_0', $notificationName);
		if ($ret) {
		    return array($ret, null);
		}

		$permission = $instance->getPermission();
		if (!$isAdminGroup && empty($permission)) {
		    continue;
		}

		foreach ($notification['items'] as $formItem) {
		    if (empty($formItem['oldSubscribed']) ^ empty($formItem['subscribed'])) {
			list ($ret, $hints) =
			    GalleryCoreApi::getFactoryDefinitionHints('NotificationEvent_1_0',
								      $notificationName);
			if ($ret) {
			    return array($ret, null);
			}
			if (empty($formItem['subscribed'])) {
			    $ret = NotificationHelper::unsubscribe($userId, $formItem['itemId'],
					     $hints[0], $notificationName, $notification['handler'],
					     !empty($formItem['recursive']));
			} else {
			    $itemId = (int)$formItem['itemId'];
			    $authorized = $isAdminGroup;
			    $item = null;
			    if (!empty($itemId)) {
				list ($ret, $authorized) =
				    GalleryCoreApi::hasItemPermission($itemId, 'core.view');
				if ($ret) {
				    return array($ret, null);
				}
				/* Avoid information disclosure, act as if the item didn't exist. */
				if (!$authorized) {
				    return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
				}
				if ($permission != 'core.view') {
				    list ($ret, $authorized) =
					GalleryCoreApi::hasItemPermission($itemId, $permission);
				    if ($ret) {
					return array($ret, null);
				    }
				}
				list ($ret, $item) =
				    GalleryCoreApi::loadEntitiesById($itemId, 'GalleryItem');
				if ($ret) {
				    return array($ret, null);
				}
			    }
			    if ($authorized) {
				$ret = NotificationHelper::subscribe($userId, $item,
					     $hints[0], $notificationName, $notification['handler'],
					     !empty($notification['recursive']));
			    } else {
				$error[] = 'form[error][notifications][' . $i . '][notAuthorized]';
			    }
			}
		    }
		    if ($ret) {
			return array($ret, null);
		    }
		}
	    }

	    if (empty($error)) {
		/* Alert of the changes we have done. */
		$status['saved'] = 1;
	    } else {
		$returnOption = 'delegate';
	    }
	}

	$results[$returnOption] = array('view' => 'core.UserAdmin',
					'subView' => 'notification.NotificationUserAdmin');
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view shows all event / method settings on a per-user basis.
 */
class NotificationUserAdminView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	/* Check that the user is logged in */
	list ($ret, $userId) = NotificationHelper::getUserIdCheck();
	if ($ret) {
	    return array($ret, null);
	}

	/* Is the user an administrator */
	list ($ret, $isAdminGroup) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'NotificationUserAdmin') {
	    $ret = $this->_initializeForm($form, $isAdminGroup, $userId);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$template->setVariable('controller', 'notification.NotificationUserAdmin');
	return array(null,
		     array('body' => 'modules/notification/templates/NotificationUserAdmin.tpl'));
    }

    /**
     * Helper function to initialize the form
     * @param array $form the form values
     * @param boolean $isAdminGroup flag for whether the uers is in the Admin group or not.
     * @param string $userId
     * @return GalleryStatus
     */
    function _initializeForm(&$form, $isAdminGroup, $userId) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$form['formName'] = 'NotificationUserAdmin';
	list ($ret, $notifications) = NotificationHelper::getEnabledNotifications();
	if ($ret) {
	    return $ret;
	}

	/* Subscriptions */
	list ($ret, $subscriptions) = NotificationHelper::getSubscriptions($userId);
	if ($ret) {
	    return $ret;
	}

	$form['notifications'] = array();
	$i = 0;
	foreach ($notifications as $notificationName => $notification) {
	    list ($ret, $instance) =
		GalleryCoreApi::newFactoryInstanceById('NotificationEvent_1_0', $notificationName);
	    if ($ret) {
		return $ret;
	    }

	    list ($ret, $description) = $instance->getDescription();
	    if ($ret) {
		return $ret;
	    }

	    foreach ($notification as $handlerName => $status) {
		if (empty($status['enabled'])
			|| (empty($status['public']) && !$isAdminGroup)) {
		    continue;
		}

		list ($ret, $handler) = GalleryCoreApi::newFactoryInstance(
							   'NotificationHandler_1_0', $handlerName);
		if ($ret) {
		    return $ret;
		}

		list ($ret, $handlerDescription) = $handler->getDescription();
		if ($ret) {
		    return $ret;
		}

		/*
		 * If there are no subscriptions then check to see if the user has permission to
		 * subscribe to this event.
		 */
		if (!isset($subscriptions[$notificationName])) {
		    if (!$instance->isGlobal() || !$isAdminGroup) {
			/* If it is an item related event then we don't display it here */
			continue;
		    }

		    $form['notifications'][++$i] = array('name' => $notificationName,
					  'description' => $description,
					  'handler' => $handlerName,
					  'handlerDescription' => $handlerDescription,
					  'items' => array('1' => array('itemId' => 0,
									'title' => '',
									'subscribed' => false)));
		} else {
		    $form['notifications'][++$i] = array('name' => $notificationName,
					  'description' => $description,
					  'handler' => $handlerName,
					  'handlerDescription' => $handlerDescription);
		    $itemIds = array();
		    /* Do the global subscriptions first */
		    foreach ($subscriptions[$notificationName] as $itemId) {
			if (empty($itemId)) {
			    $form['notifications'][$i]['items'][1] = array('itemId' => 0, 
						'subscribed' => true, 'title' => '', 'url' => '');
			} else {
			    $itemIds[] = $itemId;
			}
		    }

		    if (count($itemIds) > 0) {
		    	/* Filter out items that we (no longer) have access to */
			$permission = $instance->getPermission();
			list ($ret, $itemPermissions) =
			    GalleryCoreApi::fetchPermissionsForItems($itemIds);
			if ($ret) {
			    return $ret;
			}
			$filteredItemIds = array();
			foreach ($itemIds as $itemId) {
			    if (!(empty($permission)
				    || empty($itemPermissions[$itemId]['core.view'])
				    || empty($itemPermissions[$itemId][$permission]))) {
				$filteredItemIds[] = $itemId;
			    }
			}
			$itemIds = $filteredItemIds;

			if (!empty($itemIds)) {
			    list ($ret, $items) =
				GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryItem');
			    if ($ret) {
				return $ret;
			    }
			    $j = 0;
			    foreach ($items as $item) {
				$itemUrl = $urlGenerator->generateUrl(
				    array('view' => 'core.ShowItem', 'itemId' => $item->getId()));
				$form['notifications'][$i]['items'][++$j] =
				    array('itemId' => $item->getId(), 'subscribed' => true,
					  'url' => $itemUrl, 'title' => $item->getTitle());
			    }
			}
		    }
		}
	    }

	}
	return null;
    }
}
?>
