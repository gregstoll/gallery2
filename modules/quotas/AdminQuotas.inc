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
 * Options to add/remove/edit User/Group Quotas
 * @package Quotas
 * @subpackage UserInterface
 * @author Robert Balousek <volksport@users.sf.net>
 * @version $Revision: 17580 $
 */
class AdminQuotasController extends GalleryController {

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	$results = $status = $error = array();
	$group = $user = null;
	if (!empty($form['text']['groupName'])) {
	    list ($ret, $group) = GalleryCoreApi::fetchGroupByGroupName($form['text']['groupName']);
	    if ($ret) {
		if (!($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
	    }
	}
	if (!empty($form['text']['userName'])) {
	    list ($ret, $user) = GalleryCoreApi::fetchUserByUserName($form['text']['userName']);
	    if ($ret) {
		if (!($ret->getErrorCode() & ERROR_MISSING_OBJECT)) {
		    return array($ret, null);
		}
	    }
	}

	GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');

	if (isset($form['action']['group']['createFromText'])) {
	    if (empty($form['text']['groupName'])) {
		$error[] = 'form[error][text][noGroupSpecified]';
	    } else if (is_null($group)) {
		$error[] = 'form[error][text][noSuchGroup]';
	    } else {
		list ($ret, $quotaExists, $groupQuota) =
		    GalleryQuotasHelper::fetchGroupQuota($group->getId());
		if ($ret) {
		    return array($ret, null);
		}
		if (isset($groupQuota['id'])) {
		    $error[] = 'form[error][text][GroupQuotaExists]';
		}
	    }
	    if (empty($error)) {
		/* Show the "create group quota" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminCreateGroupQuota';
		$redirect['groupId'] = $group->getId();
	    }
	} else if (isset($form['action']['user']['createFromText'])) {
	    if (empty($form['text']['userName'])) {
		$error[] = 'form[error][text][noUserSpecified]';
	    } else if (is_null($user)) {
		$error[] = 'form[error][text][noSuchUser]';
	    } else {
		list ($ret, $quotaExists, $userQuota) =
		    GalleryQuotasHelper::fetchUserQuota($user->getId());
		if ($ret) {
		    return array($ret, null);
		}
		if (isset($userQuota['id'])) {
		    $error[] = 'form[error][text][UserQuotaExists]';
		}
	    }
	    if (empty($error)) {
		/* Show the "create user quota" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminCreateUserQuota';
		$redirect['userId'] = $user->getId();
	    }
	} else if (isset($form['action']['group']['editFromText'])) {

	    if (empty($form['text']['groupName'])) {
		$error[] = 'form[error][text][noGroupSpecified]';
	    } else if ($group == null) {
		$error[] = 'form[error][text][noSuchGroup]';
	    } else {
		list ($ret, $quotaExists, $groupQuota) =
		    GalleryQuotasHelper::fetchGroupQuota($group->getId());
		if ($ret) {
		    return array($ret, null);
		}
		if (!isset($groupQuota['id'])) {
		    $error[] = 'form[error][text][noSuchGroupQuota]';
		}
	    }
	    if (empty($error)) {
		/* Show the "edit group quota" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminEditGroupQuota';
		$redirect['groupId'] = $group->getId();
	    }

	} else if (isset($form['action']['user']['editFromText'])) {

	    if (empty($form['text']['userName'])) {
		$error[] = 'form[error][text][noUserSpecified]';
	    } else if ($user == null) {
		$error[] = 'form[error][text][noSuchUser]';
	    } else {
		list ($ret, $quotaExists, $userQuota) =
		    GalleryQuotasHelper::fetchUserQuota($user->getId());
		if ($ret) {
		    return array($ret, null);
		}
		if (!isset($userQuota['id'])) {
		    $error[] = 'form[error][text][noSuchUserQuota]';
		}
	    }
	    if (empty($error)) {
		/* Show the "edit user quota" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminEditUserQuota';
		$redirect['userId'] = $user->getId();
	    }

	} else if (isset($form['action']['group']['deleteFromText'])) {

	    if (empty($form['text']['groupName'])) {
		$error[] = 'form[error][text][noGroupSpecified]';
	    } else if ($group == null) {
		$error[] = 'form[error][text][noSuchGroup]';
	    } else {
		list ($ret, $quotaExists, $groupQuota) =
		    GalleryQuotasHelper::fetchGroupQuota($group->getId());
		if ($ret) {
		    return array($ret, null);
		}
		if (!isset($groupQuota['id'])) {
		    $error[] = 'form[error][text][noSuchGroupQuota]';
		}
	    }
	    if (empty($error)) {
		/* Show the "delete group quota" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminDeleteGroupQuota';
		$redirect['groupId'] = $group->getId();
	    }

	} else if (isset($form['action']['user']['deleteFromText'])) {

	    if (empty($form['text']['userName'])) {
		$error[] = 'form[error][text][noUserSpecified]';
	    } else if ($user == null) {
		$error[] = 'form[error][text][noSuchUser]';
	    } else {
		list ($ret, $quotaExists, $userQuota) =
		    GalleryQuotasHelper::fetchUserQuota($user->getId());
		if ($ret) {
		    return array($ret, null);
		}
		if (!isset($userQuota['id'])) {
		    $error[] = 'form[error][text][noSuchUserQuota]';
		}
	    }
	    if (empty($error)) {
		/* Show the "delete user quota" view */
		$redirect['view'] = 'core.SiteAdmin';
		$redirect['subView'] = 'quotas.AdminDeleteUserQuota';
		$redirect['userId'] = $user->getId();
	    }

	}

	if (!empty($redirect)) {
	    $results['redirect'] = $redirect;
	} else {
	    $results['delegate']['view'] = 'core.SiteAdmin';
	    $results['delegate']['subView'] = 'quotas.AdminQuotas';
	}

	$results['status'] = $status;
	$results['error'] = $error;

	return array(null, $results);
    }
}

/**
 * This view will show available options to add/remove/edit User/Group Quotas
 */
class AdminQuotasView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	GalleryCoreApi::requireOnce('modules/quotas/classes/GalleryQuotasHelper.class');

	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();
	if ($ret) {
	    return array($ret, null);
	}

	/* Load some standard form parameters */
	if ($form['formName'] != 'AdminQuotas') {
	    $form['text']['groupName'] = '';
	    $form['text']['userName'] = '';
	    $form['formName'] = 'AdminQuotas';
	}

	if (empty($form['list']['group']['page'])) {
	    $form['list']['group']['page'] = 1;
	}
	if (empty($form['list']['user']['page'])) {
	    $form['list']['user']['page'] = 1;
	}

	list ($ret, $totalGroupCount) = GalleryQuotasHelper::fetchQuotaGroupCount();
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $totalUserCount) = GalleryQuotasHelper::fetchQuotaUserCount();
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['group']['count'] = $totalGroupCount;
	$form['list']['group']['pageSize'] = $totalGroupCount > 10 ? 10 : $totalGroupCount + 2;

	/* Figure out our max pages, make sure our current page fits in it */
	$form['list']['group']['maxPages'] =
	    ceil($form['list']['group']['count'] / $form['list']['group']['pageSize']);
	if ($form['list']['group']['page'] > $form['list']['group']['maxPages']) {
	    $form['list']['group']['page'] = $form['list']['group']['maxPages'];
	}

	/* Calculate the next/back pages */
	$form['list']['group']['nextPage'] =
	    min($form['list']['group']['page']+1, $form['list']['group']['maxPages']);
	$form['list']['group']['backPage'] = max(1, $form['list']['group']['page']-1);

	list ($ret, $groupNames) =
	    GalleryQuotasHelper::fetchQuotaGroupIdList($form['list']['group']['pageSize'],
						     (($form['list']['group']['page'] - 1) *
						       $form['list']['group']['pageSize']));
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['groupNames'] = array();
	foreach ($groupNames as $groupId => $values) {
	    $form['list']['groupNames'][$groupId]['groupName'] = $values['name'];
	    list ($ret,
		  $form['list']['groupNames'][$groupId]['quotaSize'],
		  $form['list']['groupNames'][$groupId]['quotaUnit']) =
		GalleryQuotasHelper::humanReadableFromKilobytes($values['quota']);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$form['list']['user']['count'] = $totalUserCount;
	$form['list']['user']['pageSize'] = $totalUserCount > 10 ? 10 : $totalUserCount + 2;

	/* Figure out our max pages, make sure our current page fits in it */
	$form['list']['user']['maxPages'] =
	    ceil($form['list']['user']['count'] / $form['list']['user']['pageSize']);
	if ($form['list']['user']['page'] > $form['list']['user']['maxPages']) {
	    $form['list']['user']['page'] = $form['list']['user']['maxPages'];
	}

	/* Calculate the next/back pages */
	$form['list']['user']['nextPage'] =
	    min($form['list']['user']['page']+1, $form['list']['user']['maxPages']);
	$form['list']['user']['backPage'] = max(1, $form['list']['user']['page']-1);

	list ($ret, $userNames) =
	    GalleryQuotasHelper::fetchQuotaUserIdList($form['list']['user']['pageSize'],
						     (($form['list']['user']['page'] - 1) *
						       $form['list']['user']['pageSize']));
	if ($ret) {
	    return array($ret, null);
	}

	$form['list']['userNames'] = array();
	foreach ($userNames as $userId => $values) {
	    $form['list']['userNames'][$userId]['userName'] = $values['name'];
	    list ($ret,
		  $form['list']['userNames'][$userId]['quotaSize'],
		  $form['list']['userNames'][$userId]['quotaUnit']) =
		GalleryQuotasHelper::humanReadableFromKilobytes($values['quota']);
	    if ($ret) {
		return array($ret, null);
	    }
	}

	$AdminQuotas = array();
	$AdminQuotas['totalGroupCount'] = $totalGroupCount;
	$AdminQuotas['totalUserCount'] = $totalUserCount;

	$template->setVariable('AdminQuotas', $AdminQuotas);
	$template->setVariable('controller', 'quotas.AdminQuotas');
	return array(null, array('body' => 'modules/quotas/templates/AdminQuotas.tpl'));
    }
}
?>
