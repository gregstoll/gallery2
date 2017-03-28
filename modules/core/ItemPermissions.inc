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
 * This controller will handle the changes in the permission of an item
 * @package GalleryCore
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ItemPermissionsController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* Make sure we have permission to change permissions of this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.changePermissions');
	if ($ret) {
	    return array($ret, null);
	}

	$applyToChildren = isset($form['applyToSubItems']);
	$status = $error = array();
	if (isset($form['action']['deleteGroupPermission'])) {
	    /* Figure out which one we're working with */
	    $deleteGroupPermission = array_keys($form['action']['deleteGroupPermission']);
	    $index = array_pop($deleteGroupPermission);

	    /* Handle delete group perm actions */
	    list ($groupId, $permissionId) = explode(',', $form['group']['delete'][$index]);
	    $ret = GalleryCoreApi::removeGroupPermission($itemId, $groupId,
							 $permissionId, $applyToChildren);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Figure out where to redirect upon success */
	    $redirect['view'] = 'core.ItemAdmin';
	    $redirect['subView'] = 'core.ItemPermissions';
	    $redirect['itemId'] = $itemId;
	    $status['deletedGroupPermission'] = 1;

	    /* Stuff the values back into the form for easy re-adding */
	    $redirect['form[group][permission]'] = $permissionId;
	    list ($ret, $group) = GalleryCoreApi::loadEntitiesById($groupId, 'GalleryGroup');
	    if (!$ret) {
		$redirect['form[group][groupName]'] = $group->getGroupName();
	    }
	    $verifySelfPermissions = true;
	} else if (isset($form['action']['deleteUserPermission'])) {
	    /* Figure out which one we're working with */
	    $deleteUserPermission = array_keys($form['action']['deleteUserPermission']);
	    $index = array_pop($deleteUserPermission);

	    /* Handle delete user perm actions */
	    list ($userId, $permissionId) = explode(',', $form['user']['delete'][$index]);
	    $ret = GalleryCoreApi::removeUserPermission($itemId, $userId,
							$permissionId, $applyToChildren);
	    if ($ret) {
		return array($ret, null);
	    }

	    /* Figure out where to redirect upon success */
	    $redirect['view'] = 'core.ItemAdmin';
	    $redirect['subView'] = 'core.ItemPermissions';
	    $redirect['itemId'] = $itemId;
	    $status['deletedUserPermission'] = 1;

	    /* Stuff the values back into the form for easy re-adding */
	    $redirect['form[user][permission]'] = $permissionId;
	    list ($ret, $user) = GalleryCoreApi::loadEntitiesById($userId, 'GalleryUser');
	    if (!$ret) {
		$redirect['form[user][userName]'] = $user->getUserName();
	    }
	    if ($userId == $gallery->getActiveUserId()) {
		$verifySelfPermissions = true;
	    }
	} else if (isset($form['action']['addUserPermission'])) {

	    /* Handle add user permission actions */
	    if (empty($form['user']['userName'])) {
		$error[] = 'form[error][user][missingUser]';
	    } else {
		/* Validate the user */
		list ($ret, $user) = GalleryCoreApi::fetchUserByUserName($form['user']['userName']);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
			$error[] = 'form[error][user][invalidUser]';
		    } else {
			return array($ret, null);
		    }
		}
	    }

	    /* Validate the permission */
	    $permission = $form['user']['permission'];
	    list ($ret, $allPermissions) = GalleryCoreApi::getPermissionIds();
	    if ($ret) {
		return array($ret, null);
	    }
	    if (empty($allPermissions[$permission])) {
		$error[] = 'form[error][user][invalidPermission]';
	    }

	    if (empty($error)) {
		/* Don't add the permission if it already exists */
		list ($ret, $hasIt) =
		    GalleryCoreApi::hasPermission($itemId, $user->getId(), $permission);
		if ($ret) {
		    return array($ret, null);
		}
		if ($hasIt) {
		    $error[] = 'form[error][user][alreadyHadPermission]';
		}
	    }

	    if (empty($error)) {
		$ret = GalleryCoreApi::addUserPermission($itemId, $user->getId(),
							 $permission, $applyToChildren);
		if ($ret) {
		    return array($ret, null);
		}

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'core.ItemPermissions';
		$redirect['itemId'] = $itemId;
		$redirect['form[user][userName]'] = $user->getUserName();
		$status['addedUserPermission'] = 1;
	    }
	} else if (isset($form['action']['addGroupPermission'])) {

	    /* Handle add group permission actions */
	    if (empty($form['group']['groupName'])) {
		$error[] = 'form[error][group][missingGroup]';
	    } else {
		/* Validate the group */
		list ($ret, $group) =
		    GalleryCoreApi::fetchGroupByGroupName($form['group']['groupName']);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
			$error[] = 'form[error][group][invalidGroup]';
		    } else {
			return array($ret, null);
		    }
		}
	    }

	    /* Validate the permission */
	    $permission = $form['group']['permission'];
	    list ($ret, $allPermissions) = GalleryCoreApi::getPermissionIds();
	    if ($ret) {
		return array($ret, null);
	    }
	    if (empty($allPermissions[$permission])) {
		$error[] = 'form[error][group][invalidPermission]';
	    }

	    if (empty($error)) {
		/* Don't add the permission if it already exists */
		list ($ret, $hasIt) =
		    GalleryCoreApi::hasPermission($itemId, $group->getId(), $permission);
		if ($ret) {
		    return array($ret, null);
		}
		if ($hasIt) {
		    $error[] = 'form[error][group][alreadyHadPermission]';
		}
	    }

	    if (empty($error)) {
		$ret = GalleryCoreApi::addGroupPermission($itemId, $group->getId(),
							  $permission, $applyToChildren);
		if ($ret) {
		    return array($ret, null);
		}

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'core.ItemPermissions';
		$redirect['itemId'] = $itemId;
		$redirect['form[group][groupName]'] = $group->getGroupName();
		$status['addedGroupPermission'] = 1;
	    }
	} else if (isset($form['action']['changeOwner'])) {
	    if (empty($form['owner']['ownerName'])) {
		$error[] = 'form[error][owner][missingUser]';
	    } else {
		$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
		if ($ret) {
		    return array($ret, null);
		}

		/* Validate the user */
		list ($ret, $user) =
		    GalleryCoreApi::fetchUserByUserName($form['owner']['ownerName']);
		if ($ret) {
		    if ($ret->getErrorCode() & ERROR_MISSING_OBJECT) {
			$error[] = 'form[error][owner][invalidUser]';
		    } else {
			return array($ret, null);
		    }
		}
	    }

	    if (empty($error)) {
		list ($ret, $permissions) =
		    GalleryCoreApi::fetchPermissionsForItems(array($itemId), $item->getOwnerId());
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($itemId);
		if ($ret) {
		    return array($ret, null);
		}

		list ($ret, $item) = $item->refresh();
		if ($ret) {
		    return array($ret, null);
		}

		$userId = $user->getId();
		$item->setOwnerId($userId);
		$item->setSerialNumber($form['serialNumber']);
		$ret = $item->save();
		if ($ret) {
		    GalleryCoreApi::releaseLocks($lockId);
		    return array($ret, null);
		}

		$ret = GalleryCoreApi::releaseLocks($lockId);
		if ($ret) {
		    return array($ret, null);
		}

		$applyOwnerToChildren = isset($form['applyOwnerToSubItems']);
		foreach ($permissions[$itemId] as $permission => $unused) {
		    $ret = GalleryCoreApi::addUserPermission($itemId, $userId,
							     $permission, $applyOwnerToChildren);
		    if ($ret) {
			return array($ret, null);
		    }
		}

		/* change the owner recursively for the descendents */
		if ($applyOwnerToChildren) {
		    list ($ret, $descendentIds) =
			GalleryCoreApi::fetchDescendentItemIds($item, null, null, 'core.all');

		    /*
		     * Process these descendents in chunks since we may have thousands of
		     * items and we don't want to give the database a heart attack.
		     */
		    $chunkSize = 200;
		    while (!empty($descendentIds)) {
			$chunk = array_splice($descendentIds, 0, $chunkSize);
			$gallery->guaranteeTimeLimit(60);

			list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($chunk);
			if ($ret) {
			    return array($ret, null);
			}

			list ($ret, $descendents) =
			    GalleryCoreApi::loadEntitiesById($chunk, 'GalleryItem');
			if ($ret) {
			    return array($ret, null);
			}
			foreach ($descendents as $item) {
			    $item->setOwnerId($userId);
			    $ret = $item->save();
			    if ($ret) {
				GalleryCoreApi::releaseLocks($lockId);
				return array($ret, null);
			    }
			}
			$ret = GalleryCoreApi::releaseLocks($lockId);
			if ($ret) {
			    return array($ret, null);
			}
		    }
		}

		/* Figure out where to redirect upon success */
		$redirect['view'] = 'core.ItemAdmin';
		$redirect['subView'] = 'core.ItemPermissions';
		$redirect['itemId'] = $itemId;
		$status['changedOwner'] = 1;
	    }
	}

	if (isset($verifySelfPermissions)) {
	    /*
	     * Make sure we don't remove our own ability to change permissions on this item.
	     * If this was a recursive remove we may lose permissions on subitems.
	     */
	    list ($ret, $canEdit) = GalleryCoreApi::hasItemPermission($itemId, 'core.edit');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!$canEdit) {
		$ret = GalleryCoreApi::addUserPermission($itemId, $gallery->getActiveUserId(),
							 'core.edit', false);
		if ($ret) {
		    return array($ret, null);
		}
		$status['addedBackSelfPermission'] = 1;
	    }
	    list ($ret, $canChange) =
		GalleryCoreApi::hasItemPermission($itemId, 'core.changePermissions');
	    if ($ret) {
		return array($ret, null);
	    }
	    if (!$canChange) {
		$ret = GalleryCoreApi::addUserPermission($itemId, $gallery->getActiveUserId(),
							 'core.changePermissions', false);
		if ($ret) {
		    return array($ret, null);
		}
		$status['addedBackSelfPermission'] = 1;
	    }
	}

	if (empty($error)) {
	    /*
	     * Try compacting.  Ignore lock timeouts here; if we failed this time we'll try
	     * again next time.
	     */
	    $ret = GalleryCoreApi::maybeCompactAccessLists();
	    if ($ret && !($ret->getErrorCode() & ERROR_LOCK_TIMEOUT)) {
		return array($ret, null);
	    }
	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.ItemAdmin';
	    $results['delegate']['subView'] = 'core.ItemPermissions';
	}
	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will prompt for permission settings of an item
 */
class ItemPermissionsView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}
	$itemId = $item->getId();

	/* Make sure we have permission to edit this item */
	$ret = GalleryCoreApi::assertHasItemPermission($itemId, 'core.edit');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $canChange) =
	    GalleryCoreApi::hasItemPermission($itemId, 'core.changePermissions');
	if ($ret) {
	    return array($ret, null);
	}

	$form['serialNumber'] = $item->getSerialNumber();

	if ($form['formName'] == 'ItemPermissions') {
	    /* Complain if we have any invalid data */
	} else {
	    /*
	     * First time around, load the form with item data.  Note that
	     * userName and groupName can be passed in to this form so don't
	     * initialize them unless they don't exist.
	     */
	    if (empty($form['user']['userName'])) {
		$form['user']['userName'] = '';
	    }

	    if (empty($form['user']['permission'])) {
		$form['user']['permission'] = '';
	    }

	    if (empty($form['group']['groupName'])) {
		$form['group']['groupName'] = '';
	    }

	    if (empty($form['group']['permission'])) {
		$form['group']['permission'] = '';
	    }

	    $form['owner']['ownerName'] = '';
	    $form['formName'] = 'ItemPermissions';
	}

	/* Get all available permissions */
	list ($ret, $allPermissions) = GalleryCoreApi::getPermissionIds();
	if ($ret) {
	    return array($ret, null);
	}
	ksort($allPermissions);

	/* Get all permissions for the item. */
	list ($ret, $permissions) =
	    GalleryCoreApi::fetchAllPermissionsForItem($itemId, true);
	if ($ret) {
	    return array($ret, null);
	}

	/* Figure out all the unique user/group ids and load those */
	$userAndGroupEntityIds = array();
	foreach ($permissions as $permission) {
	    if (!empty($permission['userId'])) {
		$userAndGroupEntityIds[$permission['userId']] = 1;
	    }
	    if (!empty($permission['groupId'])) {
		$userAndGroupEntityIds[$permission['groupId']] = 1;
	    }
	}

	list ($ret, $userAndGroupEntities) = GalleryCoreApi::loadEntitiesById(
		array_keys($userAndGroupEntityIds), array('GalleryUser', 'GalleryGroup'));
	if ($ret) {
	    return array($ret, null);
	}

	/* Convert them into a hash map by entity id */
	foreach ($userAndGroupEntities as $entity) {
	    $userAndGroupEntityMap[$entity->getId()] = (array)$entity;
	}

	/* Figure out the admin group id */
	list ($ret, $adminGroupId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.adminGroup');
	if ($ret) {
	    return array($ret, null);
	}

	/*
	 * Now create the separate user and group permission maps.
	 *
	 * Silently ignore any permissions that we come across that aren't part
	 * of the permission registry.  They may be permission associated with
	 * modules that are not currently active.
	 */
	$userPermissions = $groupPermissions = array();
	foreach ($permissions as $permission) {
	    $permissionId = $permission['permission'];
	    if (!empty($permission['userId']) && isset($allPermissions[$permissionId])) {
		list ($ret, $subPermissions) = GalleryCoreApi::getSubPermissions($permissionId);
		if ($ret) {
		    return array($ret, null);
		}

		$userPermissions[] = array(
		    'permission' => array('id' => $permissionId,
					  'description' => $allPermissions[$permissionId]),
		    'user' => $userAndGroupEntityMap[$permission['userId']],
		    'deleteList' => $subPermissions);
	    }

	    if (!empty($permission['groupId']) && isset($allPermissions[$permissionId])) {
		if ($permission['groupId'] != $adminGroupId) {
		    list ($ret, $subPermissions) = GalleryCoreApi::getSubPermissions($permissionId);
		    if ($ret) {
			return array($ret, null);
		    }
		} else {
		    $subPermissions = array();
		}

		$groupPermissions[] =
		    array('permission' => array('id' => $permissionId,
						'description' => $allPermissions[$permissionId]),
			  'group' => $userAndGroupEntityMap[$permission['groupId']],
			  'deleteList' => $subPermissions);
	    }
	}

	/* Figure out the owner */
	list ($ret, $owner) = GalleryCoreApi::loadEntitiesById($item->getOwnerId(), 'GalleryUser');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup();
	if ($ret) {
	    return array($ret, null);
	}

	/* Figure out what we can display on the form */
	$can['changePermissions'] = $canChange;
	$can['changeOwner'] = $isAdmin;
	$can['applyToSubItems'] = $item->getCanContainChildren();

	$ItemPermissions['owner'] = (array)$owner;
	$ItemPermissions['can'] = $can;
	$ItemPermissions['userPermissions'] = $userPermissions;
	$ItemPermissions['groupPermissions'] = $groupPermissions;
	$ItemPermissions['allPermissions'] = $allPermissions;

	$template->setVariable('ItemPermissions', $ItemPermissions);
	$template->setVariable('controller', 'core.ItemPermissions');
	return array(null, array('body' => 'modules/core/templates/ItemPermissions.tpl'));
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $item) = $this->getItem();
	if ($ret) {
	    return array($ret, null);
	}

	$itemTypeNames = $item->itemTypeName(true);

	return array(null, $core->translate(array('text' => 'edit %s permissions',
						  'arg1' => $itemTypeNames[1])));
    }
}
?>
