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

GalleryCoreApi::requireOnce('modules/fotokasten/lib/nusoap.inc');

/**
 * This controller sends data to fotokasten.de
 * @package Fotokasten
 * @author Ioana Ciocanescu <service@fotokasten.de>
 * @version $Revision: 17580 $
 */
class PrintPhotosController extends GalleryController {

    /* Server URL configuration */
    var $nserver = '.partner.fotokasten.de';
    var $ftk_soap_path = '/affiliateapi/transfer_basket.php';
    var $server;

    /* Affiliate Configuration */
    var $affiliate_id;
    var $affiliate_id_pass;
    var $method_session = 'registerSession';
    var $method_basket = 'transferBasket';

    /* For testability */
    var $soapclient = 'nusoapclient';

    /**
     * @see GalleryController::handleRequest
     */
    function handleRequest($form) {
	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$itemId = GalleryUtilities::getRequestVariables('itemId');
	if (!empty($itemId)) {
	    $cartItemIds = array($itemId => 1);
	} else {
	    $session =& $gallery->getSession();
	    $cartItemIds = $session->get('fotokasten.cart');
	    $session->remove('fotokasten.cart');
	}
	if (empty($cartItemIds)) {
	    return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER), null);
	}

	/* Load the necessary item data */
	$itemIds = array_keys($cartItemIds);
	list ($ret, $items) = GalleryCoreApi::loadEntitiesById($itemIds, 'GalleryPhotoItem');
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $permissions) = GalleryCoreApi::fetchPermissionsForItems($itemIds);
	if ($ret) {
	    return array($ret, null);
	}
	foreach ($itemIds as $id) {
	    if (empty($permissions[$id]['core.view'])) {
		/* Avoid information disclosure, act as if the item didn't exist. */
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	    }
	}

	list ($ret, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds($itemIds);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $resizes) = GalleryCoreApi::fetchResizesByItemIds($itemIds);
	if ($ret) {
	    return array($ret, null);
	}

	list ($ret, $preferreds) = GalleryCoreApi::fetchPreferredsByItemIds($itemIds);
	if ($ret) {
	    return array($ret, null);
	}

	/* We want to know which are viewable to guests */
	list ($ret, $anonymousUserId) =
	    GalleryCoreApi::getPluginParameter('module', 'core', 'id.anonymousUser');
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $publicPermissions) =
	    GalleryCoreApi::fetchPermissionsForItems($itemIds, $anonymousUserId, false);
	if ($ret) {
	    return array($ret, null);
	}
	/* Assemble all our data */
	$i = 0;
	$entries = $protectedIds = array();
	foreach ($items as $item) {
	    $itemId = $item->getId();

	    if (!isset($permissions[$itemId]['fotokasten.print'])) {
		/* Skip any cart items for which we don't have print permission */
		continue;
	    }

	    $source = isset($preferreds[$itemId]) ? $preferreds[$itemId] : $item;
	    $needSession = !isset($publicPermissions[$itemId]['core.viewSource']);

	    if ($needSession && !isset($foSession)) {
		/*
		 * Get G2 session for fotokasten to access non-public images.
		 * We can't use this session because hijack protection will prevent access
		 * plus the current user could logout before fotokasten retrieves the images.
		 * Create a new session with the rights of current user for fotokasten to use.
		 */
		$foSession = new GallerySession();
		$ret = $foSession->initEmpty(true, $gallery->getActiveUserId());
		if ($ret) {
		    return array($ret, null);
		}
	    }
	    if ($needSession) {
		$sessionParam = array($foSession->getKey() => $foSession->getId());
		$protectedIds[] = $source->getId();
	    } else {
		$sessionParam = array();
	    }

	    /* attributes of the photo: url, thumbnail_url, height and width */
	    $basket_item = array(
		'photo_url' => $urlGenerator->generateUrl(
		    array_merge(array('view' => 'core.DownloadItem', 'itemId' => $source->getId()),
				$sessionParam),
		    array('forceSessionId' => false, 'forceFullUrl' => true,
			  'htmlEntities' => false)),
		'height'        => $source->getHeight(),
		'width'         => $source->getWidth()
	    );

	    if (!isset($thumbnails[$itemId]) || $thumbnails[$itemId]->getPostFilterOperations()) {
		/* Use the source if the thumbnail has a postfilter (like a watermark) */
		$thumbSource = $source;
	    } else {
		$thumbSource = $thumbnails[$itemId];
		if ($needSession) {
		    $protectedIds[] = $thumbSource->getId();
		}
	    }

	    $basket_item['thumbnail_url'] = $urlGenerator->generateUrl(
		array_merge(array('view' => 'core.DownloadItem',
				  'itemId' => $thumbSource->getId()),
			    $sessionParam),
		array('forceSessionId' => false, 'forceFullUrl' => true,
		      'htmlEntities' => false));

	    $basket['item' . ++$i] = $basket_item;
	}
	if (isset($foSession)) {
	    /* Mark this session so that it can be treated specially */
	    $foSession->put('core.isPrintService', $protectedIds);

	    /*
	     * TODO: Would like to enforce a particular session timeout to ensure this session
	     * lasts long enough for fotokasten to retrieve the images.  Maybe also store the
	     * sessionid in this session so we can reuse it for multiple print requests (and
	     * just bump timeout each time).
	     */
	    $ret = $foSession->save();
	    if ($ret) {
		return array($ret, null);
	    }
	}

	/* prepare for connection */
	list ($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'fotokasten');
	if ($ret) {
	    return array($ret, null);
	}
	$this->affiliate_id = $params['affiliateId'];
	$this->affiliate_id_pass = $params['affiliateIdPass'];
	$this->server =
	    array('soapaction'      => 'urn::sopainterop',
		  'endpoint'        => 'http://' . $this->affiliate_id . $this->nserver .
				       $this->ftk_soap_path,
		  'methodNamespace' => 'http://soap.fotokasten.de',
		  'name'            => $this->affiliate_id . $this->nserver
	    );

	/* get fotokasten session */
	$ret = $this->getFotokastenSession();
	if ($ret) {
	    return array($ret, null);
	}

	/* transfer cart to fotokasten */
	list ($ret, $referer) = $this->sendFotokastenCart($basket);
	if ($ret) {
	    return array($ret, null);
	}

	/* add language parameter */
	if (strpos($referer, '?') && !strpos($referer, 'language=')) {
	    list ($ret, $language) = $gallery->getActiveLanguageCode();
	    if (!$ret && !empty($language)) {
		$referer .= '&language=' . substr($language, 0, 2);
	    }
	}

	return array(null, array(
		     'redirect' => array('href' => $referer), /* redirect to shopping basket */
		     'status' => array(),
		     'error' => array()));
    }

    /**
     * Initialize fotokasten session.
     * @return GalleryStatus a status code
     */
    function getFotokastenSession() {
	global $gallery;
	$session =& $gallery->getSession();
	$endpoint = $session->get('fotokasten.endpoint');
	if (!empty($endpoint)) {
	    $this->server['endpoint'] = $endpoint;
	    return null;
	}

	/* initialize soap server */
	$soap = new $this->soapclient($this->server['endpoint']);

	/* register session for customer */
	$method_params['query'] = array('affiliate_id_pass' => $this->affiliate_id_pass,
					'checkout'          => 'selectProduct');

	if ($return = $soap->call($this->method_session, $method_params,
				  $this->server['methodNamespace'], $this->server['soapaction'])) {
	    if ($soap->fault) {
		return GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__,
		    "FAULT: Code {$soap->faultcode} :: String {$soap->faultstring}");
	    } else if ($return['error_code'] != 0 ) {
		$gallery->debug_r($soap->request);
		$gallery->debug_r($return);
		return GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__,
					    $return['error_code']);
	    }
	    /* this endpoint has to be used for all further requests */
	    $session->put('fotokasten.endpoint',
			  $this->server['endpoint']  = $return['endpoint']);
	} else {
	    $gallery->debug_r($soap->debug_str);
	    $gallery->debug_r($soap->request);
	    $gallery->debug_r($soap->response);
	    return GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__, $soap->getError());
	}

	unset($soap);
	return null;
    }

    /**
     * Send basket to fotokasten.
     * @return array GalleryStatus a status code
     *               string url for redirect to shopping basket
     */
    function sendFotokastenCart($basket) {
	global $gallery;

	/* create new client with new endpoint */
	$soap = new $this->soapclient($this->server['endpoint']);

	/* transfer basket */
	$method_params['request'] = array('affiliate_id_pass' => $this->affiliate_id_pass,
					  'basket'            => $basket);

	if ($return = $soap->call($this->method_basket, $method_params,
				  $this->server['methodNamespace'], $this->server['soapaction'])) {
	    if ($soap->fault) {
		return array(GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__,
		    "FAULT: Code {$soap->faultcode} :: String {$soap->faultstring}"), null);
	    } else if ($return['error_code'] != 0) {
		$gallery->debug_r($soap->request);
		$gallery->debug_r($return);
		return array(GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__,
						  $return['error_code']), null);
	    }
	    /* Link for user Browser to show shopping basket */
	    $referer = $return['browserlink'];
	} else {
	    $gallery->debug_r($soap->debug_str);
	    $gallery->debug_r($soap->request);
	    $gallery->debug_r($soap->response);
	    return array(GalleryCoreApi::error(ERROR_UNKNOWN, __FILE__, __LINE__,
					      $soap->getError()), null);
	}

	unset($soap);
	return array(null, $referer);
    }
}
?>
