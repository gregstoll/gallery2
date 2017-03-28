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
 * The implementation of the cart module
 *
 * @package Cart
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 18172 $
 */
class CartModule extends GalleryModule {

    function CartModule() {
	global $gallery;

	$this->setId('cart');
	$this->setName($gallery->i18n('Cart'));
	$this->setDescription($gallery->i18n('Shopping Cart Module'));
	$this->setVersion('1.0.12'); /* Update upgrade() function below too */
	$this->_templateVersion = 1;
	$this->setGroup('commerce', $gallery->i18n('Commerce'));
	$this->setCallbacks('getItemLinks');
	$this->setRequiredCoreApi(array(7, 27));
	$this->setRequiredModuleApi(array(3, 9));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion) {
	global $gallery;

	if (empty($currentVersion)) {
	    $currentVersion = '0';
	}
	list ($ret, $coreParams) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');
	if ($ret) {
	    return $ret;
	}

	switch ($currentVersion) {
	case '0':
	    /* Register our permission */
	    $ret = GalleryCoreApi::registerPermission($this->getId(), 'cart.add',
						      $gallery->i18n('[cart] Add to cart'));
	    if ($ret) {
		return $ret;
	    }
	    /* Give everybody cart permission by default */
	    $gallery->guaranteeTimeLimit(300);
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'], 'cart.add', true);
	    if ($ret) {
		return $ret;
	    }
	    break;

	case '0.8':
	case '0.8.1':
	case '0.8.2':
	case '0.8.3':
	case '0.8.4':
	case '0.8.5':
	case '0.8.6':
	case '0.8.7':
	case '0.8.8':
	    $storage =& $gallery->getStorage();
	    $query = '
	      DROP TABLE [CartMap]
	    ';
	    $ret = $storage->execute($query);
	    if ($ret) {
		return $ret;
	    }
	    $query = '
	      DELETE FROM [Schema]
	      WHERE [Schema::name] = \'CartMap\'
	    ';
	    $ret = $storage->execute($query);
	    if ($ret) {
		return $ret;
	    }

	case '0.8.9':
	case '0.8.10':
	case '0.9.1':
	case '0.9.2':
	case '0.9.3':
	case '0.9.4':
	case '0.9.5':
	case '0.9.6':
	case '1.0.0':
	    /* Register our permission */
	    $ret = GalleryCoreApi::registerPermission($this->getId(), 'cart.add',
						      $gallery->i18n('[cart] Add to cart'));
	    if ($ret) {
		return $ret;
	    }
	    /* Give everybody cart permission by default */
	    $ret = GalleryCoreApi::addGroupPermission(
		$coreParams['id.rootAlbum'], $coreParams['id.everybodyGroup'], 'cart.add', true);
	    if ($ret) {
		return $ret;
	    }

	case '1.0.1':
	case '1.0.2':
	case '1.0.3':
	case '1.0.4':
	case '1.0.5':
	case '1.0.6':
	    /* .mo file migration */
	case '1.0.7':
	case '1.0.8':
	    /* Added CartPluginInterface_1_1 */
	case '1.0.9':
	case '1.0.10':
	case '1.0.11':

	case 'end of upgrade path':
	    break;

	default:
	    return GalleryCoreApi::error(ERROR_BAD_PLUGIN, __FILE__, __LINE__,
					sprintf('Unknown module version %s', $currentVersion));
	}

	return null;
    }

    /**
     * @see GalleryModule::getItemLinks
     */
    function getItemLinks($items, $wantsDetailedLinks, $permissions) {
	$links = array();
	foreach ($items as $item) {
	    if (isset($permissions[$item->getId()]['cart.add'])) {
		$itemTypeNames = array_merge($item->itemTypeName(), $item->itemTypeName(false));
		if (false) {
		    /* Specific translations: */ _('Add Album To Cart'); _('Add Photo To Cart');
		}
		$links[$item->getId()][] =
		    array('text' => $this->_translate(
				array('text' => 'Add %s To Cart',
				      'arg1' => $itemTypeNames[0]), $itemTypeNames[2]),
			  'params' => array('controller' => 'cart.AddToCart',
					    'itemId' => $item->getId(), 'return' => true));
	    }
	}

	return array(null, $links);
    }
}
?>
