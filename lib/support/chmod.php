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

if (!defined('G2_SUPPORT')) { return; }

/* Commands */
define('CMD_CHMOD_MODULES_AND_THEMES_DIR', 'chmodModulesAndThemesDir');
define('CMD_ADVANCED', 'advanced');
define('CMD_CHMOD_PLUGIN_DIR', 'chmodPluginDir');
define('CMD_CHMOD_GALLERY_DIR', 'chmodGalleryDir');
define('CMD_CHMOD_STORAGE_DIR', 'chmodStorageDir');
define('CMD_CHMOD_LOCALE_DIR', 'chmodLocaleDir');
/* For get/post input sanitation */
require_once(dirname(__FILE__) . '/../../modules/core/classes/GalleryUtilities.class');

$DEFAULT_FOLDER_PERMISSIONS = PermissionBits::fromString('555');
$DEFAULT_FILE_PERMISSIONS = PermissionBits::fromString('444');

$status = array();
$ret = null;

/* The permission bit sets that we accept / handle. */
$permissionBitSets = getPermissionSets();
/* Gather a complete list of plugins in this installation. */
$plugins = getPluginList();

/* Process inputs and set some variables to default values */

$path = getRequestVariable('path');
if (empty($path)) {
    $path = getGalleryStoragePath();
} else {
    /*
     * $path is used in a chmod() call and we output the path in the HTML.
     * Just do some very basic sanitation.
     */
    GalleryUtilities::sanitizeInputValues($path);
}
/* Some basic sanitation */
$path = str_replace('..', '', $path);
if (!file_exists($path)) {
    /* TODO: add open_basedir check */
    $status['error'][] = "Folder or file '$path' does not exist!";
}

/* Permissions (format e.g. 755644, split after 3 characters to get 755 and 644)*/
$permissions = (string)getRequestVariable('permissions');
if (empty($permissions)) {
    $permissions = $DEFAULT_FOLDER_PERMISSIONS->getAsString() .
    		   $DEFAULT_FILE_PERMISSIONS->getAsString();
}
if (strlen($permissions) != 6) {
    $status['error'][] =
	"Unknown permissions '$permissions'! Aborting action and resetting permissions.";
}
if (empty($status['error'])) {
    $folderPermissions = PermissionBits::fromString(substr($permissions, 0, 3));
    $filePermissions = PermissionBits::fromString(substr($permissions, 3, 3));
    if (!$folderPermissions->isValid()) {
	$status['error'][] =
	    'Invalid folder permissions! Aborting action and resetting permissions.';
	$folderPermissions = $DEFAULT_FOLDER_PERMISSIONS;
    }
    if (!$filePermissions->isValid()) {
	$status['error'][] = 'Invalid file permissions! Aborting action and resetting permissions.';
	$filePermissions = $DEFAULT_FILE_PERMISSIONS;
    }
} else {
    $folderPermissions = $DEFAULT_FOLDER_PERMISSIONS;
    $filePermissions = $DEFAULT_FILE_PERMISSIONS;
}

/************************************************************
 * Main program section
 ************************************************************/

printPageWithoutFooter($plugins, $path, $filePermissions, $folderPermissions, $permissionBitSets);

if (empty($status['error'])) {
    $command = trim(getRequestVariable('command'));
    switch ($command) {
    case CMD_ADVANCED:
        /* Advanced Options, allow chmod of any folder / file */
        $ret = chmodRecursively($path, $folderPermissions->getAsInt(),
		     $filePermissions->getAsInt(), time() - 60);
	if (!empty($ret)) {
            $status['error'][] = "Failed to change the filesystem permissions "
		. "of '$path'.";
        } else {
	    $status['message'] = "Successfully changed the filesystem permissions "
		. "of '$path'.";
        }
        break;
    case CMD_CHMOD_MODULES_AND_THEMES_DIR:
        /* Chmod the modules/ and themes/ dir writeable or read-only (not recursively) */
        $mode = getRequestVariable('mode');
        if (!in_array($mode, array('open', 'secure'))) {
            $status['error'][] = "Unknown mode '$mode'. Please try again.";
        } else {
            $ret = chmodModulesAndThemesDir($mode == 'open');
            if (!empty($ret)) {
                $status['error'][] = 'Failed to change the filesystem permissions '
		    . 'of the modules/ and themes/ folder.';
            } else {
            	$status['message'] = 'Successfully changed the filesystem permissions '
		    . 'of the modules/ and the themes/ folder.';
            }
        }
        break;
    case CMD_CHMOD_PLUGIN_DIR:
        /* Chmod a _specific_ plugin (theme or module) writeable or read-only (recursively) */
        $mode = getRequestVariable('mode');

        /* Check the given plugin path against a white list */
        $pluginPath = getRequestVariable('pluginId');
        if (!isset($plugins[$pluginPath])) {
            $status['error'][] = "Unknown plugin path '$pluginPath'.";
        } else if (!in_array($mode, array('open', 'secure'))) {
            $status['error'][] = "Unknown mode '$mode'. Please try again.";
        } else {
            $ret = chmodPluginDir($pluginPath, $mode == 'open');
            if (!empty($ret)) {
                $status['error'][] = "Failed to change the filesystem permissions "
		    . "of the '$pluginPath' folder.";
            } else {
            	$status['message'] = "Successfully changed the filesystem permissions "
		    . "of the '$pluginPath' folder.";
            }
        }

        break;
    case CMD_CHMOD_GALLERY_DIR:
        /* Chmod the whole gallery2 dir writeable or read-only */
        $mode = getRequestVariable('mode');
        if (!in_array($mode, array('open', 'secure'))) {
            $status['error'][] = "Unknown mode '$mode'. Please try again.";
        } else {
            $ret = chmodGalleryDirRecursively($mode == 'open');
            if (!empty($ret)) {
                $status['error'][] = 'Failed to change the filesystem permissions '
		    . 'of the Gallery folder.';
            } else {
            	$status['message'] = 'Successfully changed the filesystem permissions '
		    . 'of the Gallery folder.';
            }
        }
        break;
    case CMD_CHMOD_STORAGE_DIR:
        /* Chmod the entire storage dir writeable */
	$ret = chmodStorageDirRecursively();
	if (!empty($ret)) {
            $status['error'][] = 'Failed to change the filesystem permissions '
		. 'of the storage folder.';
        } else {
            $status['message'] = 'Successfully changed the filesystem permissions '
		. 'of the storage folder.';
        }
        break;
    case CMD_CHMOD_LOCALE_DIR:
        /* Chmod the entire locale dir writeable */
	$ret = chmodLocaleDirRecursively();
	if (!empty($ret)) {
            $status['error'][] = 'Failed to change the filesystem permissions '
		. 'of the locale folder.';
        } else {
            $status['message'] = 'Successfully changed the filesystem permissions '
		. 'of the locale folder.';
        }
        break;
    default:
       /* Just redisplay the page. */
       break;
    }
}
printStatus($status);

printFooter();

/************************************************************
 * Functions and Classes
 ************************************************************/

/**
 * Changes the filesystem permissions of a file or a folder recursively.  Also prints out folder
 * names online on success / error and prints out filenames on error as well.
 *
 * @param string $filename absolute path to folder/file that should be chmod'ed
 * @param int $folderPermissions (octal) new permissions for folders
 * @param int $filePermissions (octal) new permissions for files
 * @param int $start unix timestamp of last webserver/php timeout counter-measure
 * @return null on success, int <> 0 on error
 */
function chmodRecursively($filename, $folderPermissions, $filePermissions, $start) {
    $filename = rtrim($filename, '\\/');
    $error = 0;
    /* Try to prevent timeouts */
    if (time() - $start > 55) {
	if (function_exists('apache_reset_timeout')) {
    	    @apache_reset_timeout();
    	}
    	@set_time_limit(600);
	$start = time();
    }
    /*
     * Have to chmod first before the is_dir check because is_dir does a stat on the
     * file / dir which fails if the permissions are too tight.
     * Chmod to filepermissions since the majority of the chmod() calls will be for
     * files anyway and then change the permissions for folders with a second call.
     */
    if (!@chmod($filename, $filePermissions)) {
    	error("[ERROR]", $filename);
    	$error = 1;
    }
    if (is_dir($filename)) {
    	/* For folders, we change the permissions to the right ones with a second chmod call. */
    	if (!$error) {
            if (!@chmod($filename, $folderPermissions)) {
    	        error("[ERROR]", $filename);
    	        $error = 1;
	    } else {
	        status("[OK]", $filename);
    	    }
        }
    	/*
    	 * Recurse into subdirectories: Open all files / sub-dirs and change the
    	 * permissions recursively.
    	 */
	if ($fd = opendir($filename)) {
	    while (($child = readdir($fd)) !== false) {
		if ($child == '.' || $child == '..') {
		    continue;
		}
		$fullpath = "$filename/$child";
		$ret = chmodRecursively($fullpath, $folderPermissions,
					$filePermissions, $start);
		$error |= $ret;
	    }
	    closedir($fd);
	} else {
	    error("Cannot open directory", $filename);
	    return 1;
	}
    }

    if ($error) {
	return 1;
    }

    return null;
}

/**
 * Returns the predefined / acceptable permission bit sets for folders and files
 * as strings. Use as-is for HTML output, convert to integer (octdec) for chmod().
 *
 * @return array(array(string folder permission) )
 */
function getPermissionSets() {
    $permissionSets = array();

    $permissionSets[] = array(PermissionBits::fromString("777"),
    			      PermissionBits::fromString("666"));
    $permissionSets[] = array(PermissionBits::fromString("555"),
    			      PermissionBits::fromString("444"));
    $permissionSets[] = array(PermissionBits::fromString("755"),
    			      PermissionBits::fromString("644"));
    return $permissionSets;
}

function getGalleryStoragePath() {
    $config = GallerySetupUtilities::getGalleryConfig();
    return $config['data.gallery.base'];
}

/**
 * Class to represent a set of filesystem permission bits, eg. 0755 with a few convenience methods.
 */
class PermissionBits {
    /**
     * Bits in octal integer representation, e.g. 0755
     */
    var $_bits;

    /**
     * Constructor
     * @param int $bits permission bits in decimal integer representation, eg. octdec(0755)
     */
    function PermissionBits($bits) {
    	$this->_bits = decoct($bits);
    }

    /**
     * Returns a new PermissionBits object
     * @param string $bitsAsString permission set in string representation, e.g. "755"
     * @return PermissionBits object
     * @static
     */
    function fromString($bitsAsString) {
    	$bitsAsString = (string)$bitsAsString;
    	if (strlen($bitsAsString) && $bitsAsString{0} != '0') {
    	    $bitsAsString = '0' . $bitsAsString;
    	}
    	return new PermissionBits(octdec($bitsAsString));
    }

    function getAsString() {
    	return (string)$this->_bits;
    }

    /**
     * For use with chmod()
     * @return int the permission set as decimal integer
     */
    function getAsInt() {
    	return octdec($this->_bits);
    }

    /**
     * Returns a concise description of this permission set
     * @XXX rethink the whole concept, maybe just show a owner/group/world vs. r+w+x matrix
     */
    function getDescription() {
    	switch (intval($this->_bits, 8)) {
    	    case 0777:
		return 'Read + Write + Execute for Everyone';
	    case 0555:
		return 'Read + Execute for Everyone';
	    case 0666:
		return 'Read And Write for Everyone';
	    case 0444:
		return 'Read Only for Everyone, Including Owner';
	    case 0755:
		return 'Read + Execute for Everyone, Plus Write for Owner';
	    case 0644:
		return 'Read And Write for Owner, Read for Everyone Else';
	    default:
	        /* No description available */
	        return null;
    	}
    }

    function getAsDescriptiveString() {
    	return $this->getAsString() . ' (' . $this->getDescription() . ' )';
    }

    function equals($permissionBits) {
        return $this->getAsInt() == $permissionBits->getAsInt();
    }

    function isValid() {
    	$description = $this->getDescription();
    	return !empty($description);
    }
}

/* Functions which control the HTML output of the page. */
$errorBoxOpen = 0;
function status($msg, $obj) {
    openErrorBox();
    printf("$msg&nbsp;<b>%s</b><br/>", wordwrap($obj, 85, "<br/>&nbsp;&nbsp;&nbsp;", true));
}

function error($msg, $obj) {
    openErrorBox();
    print '<span class="error">';
    printf("$msg&nbsp;<b>%s</b><br/>", wordwrap($obj, 85, "<br/>&nbsp;&nbsp;&nbsp;", true));
    print '</span>';
}

function isModulesOrThemesDirWriteable() {
    return is_writeable(GallerySetupUtilities::getConfigDir() . '/modules/') &&
        is_writeable(GallerySetupUtilities::getConfigDir() . '/themes/');
}

/**
 * Make the themes/ and modules/ dir writeable or read-only
 * @param boolean $makeItWriteable true to make the dirs writeable, false to make them read-only
 * @return null on success, non 0 integer on error
 */
function chmodModulesAndThemesDir($makeItWriteable) {
    $mode = $makeItWriteable ? 0777 : 0555;
    $ret = null;
    foreach (array('/modules/', '/themes/') as $dir) {
    	if (file_exists(GallerySetupUtilities::getConfigDir() . $dir)) {
    	    /* Try to chmod all dirs, even if one fails */
	    if (!@chmod(GallerySetupUtilities::getConfigDir() . $dir, $mode)) {
		error("[ERROR]", GallerySetupUtilities::getConfigDir() . $dir);
	        $ret = 1;
	    }
    	}
    }
    return $ret;
}

function isGalleryDirWriteable() {
    return is_writeable(GallerySetupUtilities::getConfigDir());
}

/**
 * Chmod the whole gallery dir recursively either read-only or writeable
 * @param boolean $makeItWriteable true to make the dirs writeable, false to make them read-only
 * @return null on success, non 0 integer on error
 */
function chmodGalleryDirRecursively($makeItWriteable) {
    /* This is just a wrapper function for the general chmod recursively function */
    $folderMode = $makeItWriteable ? 0777 : 0555;
    $fileMode = $makeItWriteable ? 0666 : 0444;
    return chmodRecursively(GallerySetupUtilities::getConfigDir(), $folderMode, $fileMode,
			    time() - 60);
}

/* Chmod a specific plugin dir recursively */
function chmodPluginDir($pluginPath, $makeItWriteable) {
    /* This is just a wrapper function for the general chmod recursively function */
    $folderMode = $makeItWriteable ? 0777 : 0555;
    $fileMode = $makeItWriteable ? 0666 : 0444;
    return chmodRecursively(GallerySetupUtilities::getConfigDir() . $pluginPath, $folderMode,
			    $fileMode, time() - 60);
}

function chmodStorageDirRecursively() {
    /* This is just a wrapper function for the general chmod recursively function */
    return chmodRecursively(getGalleryStoragePath(), 0777, 0666, time() - 60);
}

function chmodLocaleDirRecursively() {
    /* This is just a wrapper function for the general chmod recursively function */
    return chmodRecursively(getGalleryStoragePath() . 'locale', 0777, 0666, time() - 60);
}

/**
 * @return array (pluginId => boolean writeable, .. )
 */
function getPluginList() {
    /*
     * We don't want to depend on the G2 API here, so just list the folders in
     * modules/, themes/ and in plugins/modules/, plugins/themes/.
     * We prefer being indepdent of the state of G2 over flexibility (e.g. if the
     * user hacked init.inc to set a different plugins dir name).
     */
    $plugins = array();
    foreach (array('/modules/', '/themes/') as $base) {
	if (!file_exists(GallerySetupUtilities::getConfigDir() . $base)) {
	    continue;
	}
	$fh = opendir(GallerySetupUtilities::getConfigDir() . $base);
	if (empty($fh)) {
	  continue;
        }

	/* For each folder in the plugin dir, check if it's writeable */
	while (($folderName = readdir($fh)) !== false) {
	    if ($folderName == '.' || $folderName == '..' || $folderName == '.svn') {
		continue;
	    }
	    $pluginId = $base . trim($folderName);
	    if ((int)is_dir(GallerySetupUtilities::getConfigDir() . $base . $folderName)) {
                $plugins[$pluginId] = (int)is_writeable(
                	GallerySetupUtilities::getConfigDir() . $base . $folderName);
            }
    	}
	closedir($fh);
    }
    ksort($plugins);
    return $plugins;
}

function getRequestVariable($varName) {
    foreach (array($_POST, $_GET) as $requestVars) {
	if (isset($requestVars[$varName])) {
	    return $requestVars[$varName];
	}
    }

    return null;
}

/*
 * Uses JavaScript to print the status / error message at the top of the page
 * even if the page has already been printed.
 */
function printStatus($status) {
    if (!empty($status['error'])) {
	printf('<script type="text/javascript">printErrorMessage(\'%s\');</script>',
	       str_replace(array("\\", "'"), array("\\\\", "\\'"),
	       		   implode('<br/>', $status['error'])));
    }
    if (!empty($status['message'])) {
	printf('<script type="text/javascript">printStatusMessage(\'%s\');</script>',
	       str_replace(array("\\", "'"), array("\\\\", "\\'"), $status['message']));
    }
}

/************************************************************
 * HTML - The Page layout / GUI
 ************************************************************/

/**
 * Prints the whole page including form but without the footer.
 * Call this function, then call chmodRecursively() which will output some HTML,
 * and finally call printFooter();
 */
function printPageWithoutFooter($plugins, $path, $filePermissions, $folderPermissions, $permissionBitSets) {
    global $baseUrl;
?>
<html>
  <head>
    <title>Gallery Support - Change Filesystem Permissions</title>
    <link rel="stylesheet" type="text/css" href="<?php print $baseUrl ?>support.css"/>
    <style type="text/css">
    </style>
    <script type="text/javascript">
      var plugins = new Array();
      <?php foreach ($plugins as $pluginId => $isOpenForEdit) {
        print "plugins['$pluginId'] = $isOpenForEdit;
        ";
      } ?>

      function setEditOrSecure(pluginId, formObj) {
        if (pluginId == -1) {
	  formObj.mode.value='';
	  formObj.open.disabled = true;
	  formObj.secure.disabled = true;
        } else if (plugins[pluginId]) {
	  formObj.mode.value='secure';
	  formObj.open.disabled = true;
	  formObj.secure.disabled = false;
        } else {
	  formObj.mode.value='open';
	  formObj.open.disabled = false;
	  formObj.secure.disabled = true;
        }
      }

      function printStatusMessage(message) {
        var statusElement = document.getElementById('status');
        statusElement.innerHTML = message + "<a href=\"#details\">[details]</a>";
        statusElement.style.display = 'block';
      }

      function printErrorMessage(message) {
        var errorElement = document.getElementById('error');
        errorElement.innerHTML = message +
          "<br/>Note: Please look at the <a href=\"#details\">[details]</a>. " +
          "You might be able to change the filesystem permissions of the failed directories " +
          "successfully yourself with an FTP program or a command line shell."
        errorElement.style.display = 'block';
      }
    </script>
  </head>

  <body>
    <div id="content">
      <div id="title">
	<a href="../../">Gallery</a> &raquo;
	<a href="<?php generateUrl('index.php') ?>">Support</a> &raquo;
	Change Filesystem Permissions
      </div>
      <h2>
        This tool lets you change the filesystem permissions of files and folders owned
        by the webserver.
      </h2>
      <p>
        All files and folders in your Gallery storage folder are owned by the
        webserver. If you installed Gallery2 by unpacking a .zip or .tar.gz file, then the
        gallery2 folder is probably owned by you which means that you can edit the files
        directly.  However, if you used the preinstaller then your gallery2 directory is
        also owned by the webserver. For more information, see the <b><a
        href="http://codex.gallery2.org/Gallery2:Security">Gallery Security Guide</a>.</b>
      </p>

      <!-- Identifyable placeholders such that we can insert our messages during runtime via JS. -->
      <div id="error" class="error" style="display: none;">
        &nbsp;
      </div>

      <div id="status" class="success" style="display: none;">
        &nbsp;
      </div>

      <hr class="faint"/>

      <?php if (!isModulesOrThemesDirWriteable()): ?>
      <h2>
	<a href="<?php generateUrl('index.php?chmod&amp;command=' . CMD_CHMOD_MODULES_AND_THEMES_DIR
	 . '&amp;mode=open') ?>">Make modules &amp; themes directories writeable</a>
      </h2>
      <p class="description">
	Useful when adding a new module or theme.  This makes your modules and
	themes folders writeable. It only works if you have installed Gallery with the
	pre-installer. Usually you can change the filesystem permissions with your FTP
	program or command line shell.
      </p>
      <?php else: ?>
      <h2>
	<a href="<?php generateUrl('index.php?chmod&amp;command=' . CMD_CHMOD_MODULES_AND_THEMES_DIR
	 . '&amp;mode=secure') ?>">Make modules &amp; themes directories read-only</a>
      </h2>
      <p class="description">
	Useful when you're not going to be making changes by hand. This makes your
	modules and themes folders writeable. Only works if you have installed Gallery
	with the pre-installer. Usually you can change the filesystem permissions with
	your FTP program or command line shell.
      </p>
      <?php endif; ?>

      <hr class="faint"/>

      <?php startForm('index.php?chmod&amp;command=' . CMD_CHMOD_PLUGIN_DIR, 'pluginForm'); ?>
	<h2 id="themeOrModule">
	  Make a specific theme or module editable
	</h2>
	<p class="description">
	  If you want to edit a page template file of a specific module or theme and your
	  Gallery was originally installed with the pre-installer, you might have to make
	  the corresponding plugin folder writeable first.
	</p>
	<p class="description">
	  <select name="pluginId"
	    onchange="setEditOrSecure(this.options[this.selectedIndex].value, this.form)">
	    <option value="-1">&laquo; select a module or theme &raquo;</option>
	    <?php foreach ($plugins as $pluginId => $writeable): ?>
	    <option value="<?php print $pluginId ?>"> <?php print $pluginId ?> </option>
	    <?php endforeach; ?>
	  </select>
	  &nbsp;&nbsp;
	  <input type="hidden" name="mode" value="open"/>
	  <input type="submit" disabled="disabled" name="open" value="Make it open (read/write)"/> |
	  <input type="submit" disabled="disabled" name="secure" value="Make it secure (read-only)"/>
	</p>
      </form>

      <hr class="faint"/>

      <h2><a href="<?php generateUrl('index.php?chmod&amp;command=' . CMD_CHMOD_STORAGE_DIR)
      ?>">Make the data folder read/write</a></h2>
      <p class="description">
        For some reason, your Gallery data folder might no longer be writeable by Gallery itself
        and if that happens, Gallery will usually show a ERROR_PLATFORM_FAILURE. In that case the
        problem might be solved by the above action. If the problem persists, you will have to talk
        to your webhost to get data folder writeable again.
      </p>

      <hr class="faint"/>

      <h2><a href="<?php generateUrl('index.php?chmod&amp;command=' . CMD_CHMOD_LOCALE_DIR)
      ?>">Make the locale folder read/write</a></h2>
      <p class="description">
        If you're localizing Gallery, you may see warnings when you compile up your localization
        since you may not have permissions to copy the the new localized version into your
        g2data/locale folder.  Making the locale folder read/write should solve this problem.
      </p>

      <hr class="faint"/>

      <?php if (isGalleryDirWriteable()): ?>
      <h2><a href="<?php generateUrl('index.php?chmod&amp;command=' . CMD_CHMOD_GALLERY_DIR)
      ?>&amp;mode=open">Make everything read/write</a></h2>
      <p class="description">
        If your Gallery has been installed with the pre-installer, you might have to make the
        whole Gallery directory structure read/write before you can upgrade or delete your
        installation.
      </p>
      <?php else: ?>
      <h2><a href="<?php generateUrl('index.php?chmod&amp;command=' . CMD_CHMOD_GALLERY_DIR)
      ?>&amp;mode=secure">Make everything read-only</a></h2>
      <p class="description">
        If your Gallery has been installed with the pre-installer you may want to change
        all your files back to read-only for a small amount of additional security.
      </p>
      <?php endif; ?>

      <hr class="faint"/>

      <h2>Advanced: Choose the path and the permissions manually</h2>
      <?php startForm('index.php?chmod&amp;command=' . CMD_ADVANCED); ?>
	<p class="description">
	  <b> Path to change: </b>
	  <input type="text" name="path" size="50" value="<?php print $path; ?>"/>
          <br/>
	  <span class="subtext">
	    Gallery folder: <i><?php print GallerySetupUtilities::getConfigDir(); ?></i> <br/>
            Gallery data folder: <i><?php print getGalleryStoragePath(); ?></i> <br/>
	  </span>
          <br/>
          <b> New permissions: </b>
	  <?php
	   foreach ($permissionBitSets as $permissionBitSet):
	       $checked = $permissionBitSet[1]->equals($filePermissions) ? 'checked="checked"' : '';
	       $value = $permissionBitSet[0]->getAsString() . $permissionBitSet[1]->getAsString();
          ?>
	  <br/>
	  <input id="set_<?php print $value?>" type="radio" name="permissions" value="<?php print $value ?>" <?php print $checked ?>>
	    <label for="set_<?php print $value?>">
	      <span class="hasToolTip" title="Files: <?php print $permissionBitSet[1]->getAsString(); ?>, Folders: <?php print $permissionBitSet[0]->getAsString(); ?>"> <?php print $permissionBitSet[1]->getDescription() ?></span>
	    </label>
	  </input>
	  <?php endforeach; ?>
	  <br/><br/>

          <input type="submit" value="Change the Permissions now!"/>
        </p>
      </form>
<?php
} // end function printPageWithoutFooter()

function openErrorBox() {
    global $errorBoxOpen;
    if ($errorBoxOpen) {
      return;
    }
    $errorBoxOpen = 1;
?>
      <a name="details"></a>
      <div id="details" class="results">
        <h2>Details:</h2>
<?php
} // end function openErrorBox() {

function closeErrorBox() {
    global $errorBoxOpen;
    if (!$errorBoxOpen) {
        return;
    }
?>
      </div>
<?php
} // end function closeErrorBox()

function printFooter() {
    closeErrorBox();
?>
    </div>
  </body>
</html>
<?php
} // end function printFooter()
?>
