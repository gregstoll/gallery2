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
GalleryCoreApi::requireOnce('modules/notification/classes/GalleryCoreEventNotifications.class');

/**
 * Event Notification by Method.
 * Manages subscription and notification of custom events.
 *
 * @package Notification
 * @author Zimzat <zimzat@zimzat.com>
 * @author Tim Almdal <tnalmdal@shaw.ca>
 * @version $Revision: 18172 $
 */
class NotificationModule extends GalleryModule {
    function NotificationModule() {
	global $gallery;
	$this->setId('notification');
	$this->setName($gallery->i18n('Notification'));
	$this->setDescription(
		$gallery->i18n('Allow users to be notified of events by subscription.'));
	$this->setGroup('gallery', $gallery->i18n('Gallery'));
	$this->setVersion('1.0.3');
	$this->setRequiredCoreApi(array(7, 37));
	$this->setRequiredModuleApi(array(3, 9));
	$this->setCallbacks('getSiteAdminViews|getUserAdminViews|getItemLinks|getItemAdminViews');
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {
	/* Email Method */
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'NotificationHandler_1_0', 'NotificationMethodEmail', 'NotificationMethodEmail',
	    'modules/notification/classes/NotificationMethodEmail.class', 'notification', null);
	if ($ret) {
	    return $ret;
	}

	/* Entity Delete and Save Handler */
	$handlerPath = 'modules/notification/classes/NotificationItemStateChangeHandler.class';
	$ret = GalleryCoreApi::registerFactoryImplementation('GalleryEventListener',
	    'NotificationItemStateChangeHandler', 'NotificationItemStateChangeHandler',
	    $handlerPath, 'notification', array('GalleryEntity::delete', 'GalleryEntity::save'));
	if ($ret) {
	    return $ret;
	}

	/* Item Added Option Handler */
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddOption', 'NotificationItemsAddedOption', 'NotificationItemsAddedOption',
	    'modules/notification/NotificationItemsAddedOption.inc', 'notification', null, 9);
	if ($ret) {
	    return $ret;
	}

	/* Notification module integration */
	$events[] = array('NotificationEvent_1_0', 'ActivatePluginNotification',
			  'ActivatePluginNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::ActivatePlugin'));
	$events[] = array('NotificationEvent_1_0', 'DeactivatePluginNotification',
			  'DeactivatePluginNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::DeactivatePlugin'));
	$events[] = array('NotificationEvent_1_0', 'ErrorNotification', 'ErrorNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::Error'));
	$events[] = array('NotificationEvent_1_0', 'ItemOrderNotification',
			  'ItemOrderNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::ItemOrder'));
	$events[] = array('NotificationEvent_1_0', 'LoginNotification', 'LoginNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::Login'));
	$events[] = array('NotificationEvent_1_0', 'FailedLoginNotification',
			  'FailedLoginNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::FailedLogin'));
	$events[] = array('NotificationEvent_1_0', 'LogoutNotification', 'LogoutNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::Logout'));
	$events[] = array('NotificationEvent_1_0', 'RemovePermissionNotification',
			  'RemovePermissionNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::RemovePermission'));
	$events[] = array('NotificationEvent_1_0', 'UninstallPluginNotification',
			  'UninstallPluginNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::UninstallPlugin'));
	$events[] = array('NotificationEvent_1_0', 'AlbumStructureChangeNotification',
			  'AlbumStructureChangeNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::ViewableTreeChange'));
	$events[] = array('NotificationEvent_1_0', 'SaveNotification', 'SaveNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('GalleryEntity::save'));
	$events[] = array('NotificationEvent_1_0', 'DeleteNotification', 'DeleteNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('GalleryEntity::delete'));
	$events[] = array('NotificationEvent_1_0', 'ItemAddedNotification',
			  'ItemAddedNotification',
			  'modules/notification/classes/GalleryCoreEventNotifications.class',
			  array('Gallery::itemAdded'));
	foreach ($events as $entry) {
	    $ret = GalleryCoreApi::registerFactoryImplementation($entry[0], $entry[1], $entry[2],
							      $entry[3], 'notification', $entry[4]);
	    if ($ret) {
		return $ret;
	    }
	}

	/* Get a list of events that we need to subscribe to */
	list ($ret, $hints) = NotificationHelper::getActiveSubscriptionsEvents();
	$this->setParameter('_activeListeners', serialize($hints));
	if ($ret) {
	    return $ret;
	}

	/* Generic Notification Router */
	$handlerPath = 'modules/notification/classes/NotificationRouter.class';
	$ret = GalleryCoreApi::registerFactoryImplementation('GalleryEventListener',
	    'NotificationRouter', 'NotificationRouter',
	    $handlerPath, 'notification', empty($hints) ? null : $hints, 4);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array(
	    'name' => $this->translate('Notification'),
	    'view' => 'notification.NotificationSiteAdmin')));
    }

    /**
     * @see GalleryModule::getUserAdminViews();
     */
    function getUserAdminViews($user) {
	$views = array();

	$userId = $user->getId();
	list ($ret, $showView) = $this->_showWatch($userId);
	if ($ret) {
	    return array($ret, null);
	}

	if ($showView) {
	    $views[] = array('name' => $this->translate('Watch List'),
			     'view' => 'notification.NotificationUserAdmin',
	    );
	}

	return array(null, $views);
    }

    /**
     * @see GalleryModule::getItemAdminViews();
     */
    function getItemAdminViews($item) {
	list ($ret, $showView) = $this->_showWatch();
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, !$showView ? array()
				      : array(array('name' => $this->translate('Watch'),
					      'view' => 'notification.NotificationItemAdmin')));
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions, $userId) {
	$links = array();

	list ($ret, $showView) = $this->_showWatch($userId);
	if ($ret) {
	    return array($ret, null);
	}

	if ($showView) {
	    foreach ($items as $item) {
		$links[$item->getId()][] =
			array('text' => $this->translate('Watch'),
			      'params' => array('view' => 'core.ItemAdmin',
						'subView' => 'notification.NotificationItemAdmin',
						'itemId' => $item->getId()));
	    }
	}

	return array(null, $links);
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion, $statusMonitor) {
	global $gallery;
	$storage =& $gallery->getStorage();

	if (empty($currentVersion)) {
	    $currentVersion = '0';
	}

	if (version_compare($currentVersion, '1.0.0', '<')) {
	    $ret = $this->_createNotificationLockId('id.eventMapLock');
	    if ($ret) {
		return $ret;
	    }
	    $ret = $this->_createNotificationLockId('id.hintsLock');
	    if ($ret) {
		return $ret;
	    }

	    if ($currentVersion != '0') {
		$enableMap = array();
		list ($ret, $results) =
		    $storage->search('SELECT COUNT(*) FROM [NotificationSubscriptions]', array());
		if ($ret) {
		    return $ret;
		}
		$count = $results->nextResult();

		$select = array('eventClass', 'userId', 'methodClass', 'filter');

		$offset = 0;
		$blockCount = 200;
		$options = array('limit' => array('count' => $blockCount));
		$rowsRead = 0;

		list($ret, $eventMapLock) = $this->getParameter('id.eventMapLock');
		if ($ret) {
		    return $ret;
		}
		list ($ret, $eventMapLockId) = GalleryCoreApi::acquireWriteLock($eventMapLock);
		if ($ret) {
		    return $ret;
		}

		while ($rowsRead < $count) {
		    $gallery->guaranteeTimeLimit(30);
		    $options['limit']['offset'] = $offset;
		    list ($ret, $results) = GalleryCoreApi::getMapEntry('NotificationSubscriptions',
									$select, array(), $options);
		    if ($ret) {
			return $ret;
		    }

		    $rowsRead += $results->resultCount();
		    if ($results->resultCount() == 0) {
			break;
		    }
		    while ($row = $results->nextResult()) {
			$newRow = array('userId' => $row[1],
					'handlerName' => 'NotificationMethodEmail');
			$notificationName = $row[0] == 'NotificationEventItemAdd'
								? 'ItemAddedNotification'
								: 'CommentAddNotification';
			$newRow['notificationName'] = $notificationName;
			$newRow['itemId'] = (int)substr($row[3], 2);
			$ret = GalleryCoreApi::addMapEntry('SubscriptionMap', $newRow);
			if ($ret) {
			    return $ret;
			}
			$enableMap[$notificationName]['NotificationMethodEmail'] =
							array('enabled' => true, 'public' => true);
		    }
		    $offset += $blockCount;
		}
		
		$ret = $this->setParameter('_enabledMap', serialize($enableMap));
		if ($ret) {
		    return $ret;
		}
	    }
	}

	return null;
    }

    /**
     * Create the event map lock id
     * @param string $lockName Name of the lock id to create
     * @return GalleryStatus
     * @access private
     */
    function _createNotificationLockId($lockName) {
	global $gallery;
	$storage =& $gallery->getStorage();

	list ($ret, $lockId) = $storage->getUniqueId();
	if ($ret) {
	    return $ret;
	}

	$ret = $this->setParameter($lockName, $lockId);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * Determime if the watch link is to be displayed for this item
     * @param int $userId The user id to validate against
     * @return boolean true to show the link
     * @access private
     */
    function _showWatch($userId=null) {
	list ($ret, $isAnonymous) = GalleryCoreApi::isAnonymousUser($userId);
	if ($ret) {
	    return array($ret, null);
	}
	
	$show = false;
	if (!$isAnonymous) {
	    list ($ret, $notifications) = NotificationHelper::getEnabledNotifications();
	    if ($ret) {
		return array($ret, null);
	    }
	    $show = count($notifications) > 0;
	}
	
	return array(null, $show);
    }
}
?>
