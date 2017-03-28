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

GalleryCoreApi::requireOnce('modules/cart/classes/CartHelper.class');

/**
 * View cart contents and available cart actions
 * @package Cart
 * @subpackage UserInterface
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class ViewCartView extends GalleryView {

    /**
     * @see GalleryView::loadTemplate
     */
    function loadTemplate(&$template, &$form) {
	global $gallery;

	$returnItemId = GalleryUtilities::getRequestVariables('itemId');
	$template->setVariable('itemId', $returnItemId);

	list ($ret, $itemList) = CartHelper::loadCartItems();
	if ($ret) {
	    return array($ret, null);
	}
	list ($ret, $cartItemIds) = CartHelper::fetchCartItemCounts();
	if ($ret) {
	    return array($ret, null);
	}

	if ($form['formName'] != 'ViewCart') {
	    /* First time around, initialize the cart counts */
	    foreach ($cartItemIds as $itemId => $count) {
		$form['counts'][$itemId] = $count;
	    }
	    $form['formName'] = 'ViewCart';
	}
	$ViewCart = array();

	/* Find all our cart plugins */
	list ($ret, $plugins) = $this->_loadAllCartPlugins();
	if ($ret) {
	    return array($ret, null);
	}

	/* Apply the plugin filter criteria (item types, required permissions) */
	$items = $supportedItemIds = $thumbnails = $itemTypeNames = array();
	if (!empty($cartItemIds)) {
	    foreach ($itemList as $item) {
		$items[$item->getId()] = (array)$item;

		/* TODO: This is a O(N^3) operation -- probably should be optimized */
		foreach ($plugins as $pluginId => $pluginData) {
		    foreach ($pluginData['supportedItemTypes'] as $type) {
			if ($type == '*' || GalleryUtilities::isA($item, $type)) {
			    $plugins[$pluginId]['supported'][$item->getId()] = 1;
			    $supportedItemIds[$item->getId()] = 1;
			    break;
			}
		    }
		}

		$itemTypeNames[$item->getId()] = $item->itemTypeName();
	    }

	    /* Filter out unsupported actions */
	    $itemIds = array_keys($supportedItemIds);
	    list ($ret, $permissionMap) = GalleryCoreApi::fetchPermissionsForItems($itemIds);
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($plugins as $pluginId => $pluginData) {
		if (empty($pluginData['supported'])) {
		    continue;
		}
		foreach ($pluginData['supported'] as $itemId => $ignored) {
		    $grantedPermissions = 0;
		    foreach ($pluginData['requiredPermissions'] as $permissionId) {
			if (!empty($permissionMap[$itemId][$permissionId])) {
			    $grantedPermissions++;
			}
		    }
		    if ($grantedPermissions == count($pluginData['requiredPermissions'])) {
		    	$plugins[$pluginId]['isAvailable'] = 1;
		    	break;
		    }
		}
	    }

	    /* Fetch thumbnails */
	    list ($ret, $thumbnailList) =
		GalleryCoreApi::fetchThumbnailsByItemIds(array_keys($cartItemIds));
	    if ($ret) {
		return array($ret, null);
	    }
	    foreach ($thumbnailList as $thumbnail) {
		$thumbnails[$thumbnail->getParentId()] = (array)$thumbnail;
	    }
	}
	
	$ViewCart['items'] = $items;
	$ViewCart['itemTypeNames'] = $itemTypeNames;
	$ViewCart['thumbnails'] = $thumbnails;
	$ViewCart['plugins'] = $plugins;
	$ViewCart['controller'] = 'cart.ModifyCart';
	$template->setVariable('ViewCart', $ViewCart);

	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'cart');
	if ($ret) {
	    return array($ret, null);
	}

	$template->title($module->translate('View Your Cart'));
	return array(null,
		     array('body' => 'modules/cart/templates/ViewCart.tpl'));
    }

    /**
     * Load all plugin instances and read their information into a data structure.
     *
     * @return array GalleryStatus,
     *               array string plugin id => array plugin data
     * @access private
     */
    function _loadAllCartPlugins() {
	GalleryCoreApi::requireOnce('modules/cart/classes/CartPluginInterface_1_0.class');
	list ($ret, $basicInterfaceIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('CartPluginInterface_1_0');
	if ($ret) {
	    return array($ret, null);
	}
	GalleryCoreApi::requireOnce('modules/cart/classes/CartPluginInterface_1_1.class');
	list ($ret, $advancedInterfaceIds) =
	    GalleryCoreApi::getAllFactoryImplementationIds('CartPluginInterface_1_1');
	if ($ret) {
	    return array($ret, null);
	}
	$plugins = $instances = array();
	foreach ($basicInterfaceIds as $id => $className) {
	    list ($ret, $instances[$id]) =
		GalleryCoreApi::newFactoryInstance('CartPluginInterface_1_0', $className);
	    if ($ret) {
		return array($ret, null);
	    }
	    $plugins[$id]['requiredPermissions'] = array();
	}
	foreach ($advancedInterfaceIds as $id => $className) {
	    list ($ret, $instances[$id]) =
		GalleryCoreApi::newFactoryInstance('CartPluginInterface_1_1', $className);
	    if ($ret) {
		return array($ret, null);
	    }
	    $plugins[$id]['requiredPermissions'] = $instances[$id]->getRequiredPermissions();
	}

	foreach ($instances as $id => $instance) {
	    list ($ret, $plugins[$id]['actionDisplayName']) = $instance->getActionDisplayName();
	    if ($ret) {
		return array($ret, null);
	    }
	    /*
	     * Figure out what types each plugin can handle so that we can cue the UI
	     * to only send it the ones of the appropriate types.
	     */
	    $plugins[$id]['supportedItemTypes'] = $instances[$id]->getSupportedItemTypes();
	}

	return array(null, $plugins);
    }

    /**
     * @see GalleryView::getViewDescription
     */
    function getViewDescription() {
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'cart');
	if ($ret) {
	    return array($ret, null);
	}

	return array(null, $module->translate('view cart'));
    }
}
?>
