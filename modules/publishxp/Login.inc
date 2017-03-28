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
 * Controller to initialize the process of publishing photos.
 * Allows the user to log in to Gallery to begin the process of uploading
 * photos through Windows XP's publishing mechanism.
 *
 * @package PublishXp
 * @subpackage UserInterface
 * @author Timothy Webb <tiwebb@cisco.com>
 * @version $Revision: 17580 $
 */

GalleryCoreApi::requireOnce('modules/publishxp/PublishXpLogin.inc');

/**
 * Obsolete.  This class is kept around to maintain backwards compatibility with older versions of
 * the PublishXp module, since Windows desktops will have view=publishxp.Login in their registry.
 */
class LoginView extends PublishXpLoginView {
}
?>
