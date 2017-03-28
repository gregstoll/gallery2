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
 * This ItemAddOption sends a Notification event for the newly created item.
 * @package Notification
 * @subpackage UserInterface
 * @author Zimzat <zimzat@zimzat.com>
 * @author Tim Almdal <tnalmdal@shaw.ca>
 * @version $Revision: 17580 $
 */
class NotificationItemsAddedOption extends ItemAddOption {

    /**
     * @see ItemAddOption::isAppropriate
     */
    function isAppropriate() {
	return array(null, true);
    }

    /**
     * @see ItemAddOption::handleRequestAfterAdd
     */
    function handleRequestAfterAdd($form, $items) {
	/* Tell the world we were successful */
	$itemAddEvent = GalleryCoreApi::newEvent('Gallery::ItemAdded');
	$itemAddEvent->setData($items);
	list ($ret) = GalleryCoreApi::postEvent($itemAddEvent);
	if ($ret) {
	    return array($ret, null, null);
	}

	return array(null, array(), array());
    }
}
?>
