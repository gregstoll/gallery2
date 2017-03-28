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

if (php_sapi_name() == 'cli') {
    /* We don't apply this security check to command-line PHP */
    return;
}

if (!defined('G2_SUPPORT_URL_FRAGMENT')) {
    print 'Error: Missing value for G2_SUPPORT_URL_FRAGMENT';
    exit;
}

require_once(dirname(__FILE__) . '/GallerySetupUtilities.class');

GallerySetupUtilities::startSession();

/* The $baseUrl global is used by other scripts to figure out where to find our CSS */
$config = GallerySetupUtilities::getGalleryConfig();
if (!empty($config) && $config['galleryBaseUrl'] != '') {
    $baseUrl = $config['galleryBaseUrl'] . 'lib/support/';
} else {
    $baseUrl = G2_SUPPORT_URL_FRAGMENT;
}

/*
 * If the config, or setup.password is missing, main.php will try to send us to the installer.
 * We don't want that to happen, so check for ourselves first.
 */
$options = array();
if (empty($config)) {
    render('missingConfig', $options);
} else if (empty($config['setup.password'])) {
    render('missingSetupPassword', $options);
} else if (!GallerySetupUtilities::isSessionAuthenticated()) {
    $baseDir = defined('GALLERY_CONFIG_DIR') ?
	GALLERY_CONFIG_DIR : dirname(dirname(dirname(__FILE__)));
    $authFile = $baseDir . '/login.txt';
    $options['attempts'] = GallerySetupUtilities::getLoginAttempts();
    $options['error'] = ($options['attempts'] === false);
    $options['authFile'] = sprintf(
	'%s%s%s', basename(dirname($authFile)), DIRECTORY_SEPARATOR, basename($authFile));
    $options['baseDir'] = basename($baseDir) . DIRECTORY_SEPARATOR;
    $options['key'] = GallerySetupUtilities::getAuthenticationKey();
    if (empty($options['key'])) {
	$options['key'] = GallerySetupUtilities::generateAuthenticationKey();
	GallerySetupUtilities::setAuthenticationKey($options['key']);
    }

    if ($options['error'] || $options['attempts'] >= G2_SUPPORT_MAX_LOGIN_ATTEMPTS) {
	if (!file_exists($authFile)) {
	    $options['authFileErrors']['missing'] = 1;
	} else if (!is_readable($authFile)) {
	    $options['authFileErrors']['unreadable'] = 1;
	} else {
	    $authKeyFromFile = trim(join('', file($authFile)));
	    if ($authKeyFromFile == $options['key']) {
		GallerySetupUtilities::authenticateThisSession();
		GallerySetupUtilities::redirectBackToSelf();
		exit;
	    } else {
		$options['authFileErrors']['mismatch'] = 1;
	    }
	}
	render('loginTxtForm', $options);
    } else if (!empty($_POST['password'])) {
	if ($_POST['password'] == $config['setup.password']) {
	    GallerySetupUtilities::authenticateThisSession();
	    GallerySetupUtilities::redirectBackToSelf();
	    exit;
	} else {
	    $options['attempts']++;
	    if (!GallerySetupUtilities::setLoginAttempts($options['attempts'])) {
		$options['error'] = true;
	    }

	    if ($options['error'] || $options['attempts'] >= G2_SUPPORT_MAX_LOGIN_ATTEMPTS) {
		if ($options['attempts'] >= G2_SUPPORT_MAX_LOGIN_ATTEMPTS) {
		    $ret = GallerySetupUtilities::notifySiteAdministrator();
		    /* swallow return code; we can't do anything with it */
		}
		render('loginTxtForm', $options);
	    } else {
		$options['wrongPassword'] = 1;
		render('passwordForm', $options);
	    }
	}
    } else {
	if (isset($_POST['password'])) {
	    $options['missingPassword'] = 1;
	}
	render('passwordForm', $options);
    }
}

if (GallerySetupUtilities::isSessionAuthenticated()) {
    /* This is the only safe way out of this include.  Everything else aborts now */
    return;
}

/* Unless we're properly authenticated, this is the end of the line */
exit;

function startForm($action='', $name=null) {
    print '<form method="POST" action="' . $action . (isset($name) ? "\" name=\"$name" : '') . '">';
    if (!GallerySetupUtilities::areCookiesSupported()) {
	print '<input type="hidden" name="' . session_name() . '" value="' . session_id() . '"/>';
    }
}

function render($renderType, $options=array()) {
    global $baseUrl;
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
                      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
  <head>
    <title>Security Check</title>
    <link rel="stylesheet" type="text/css" href="<?php print $baseUrl ?>support.css"/>
  </head>
  <body>
     <div id="content">
       <div id="title">
         Security Check
       </div>

       <h2>
	  You are attempting to access a secure section of this Gallery installation.  You can't
	  proceed until you pass the security check.
       </h2>

       <?php if ($renderType == 'missingConfig'): ?>
       <div class="error">
	 You must create a config.php file in the Gallery directory before
	 you can continue configuring the application.  Use the
	 <a href="../../install">installer</a> to create one.
       </div>
       <?php endif; ?>

       <?php if ($renderType == 'missingSetupPassword'): ?>
       <div class="error">
	 You must enter a setup password in your gallery/config.php file in order
	 to be able to set up Gallery. If your config.php is empty, you should run
	 the <a href="../../install">installer</a> to install your Gallery.
       </div>
       <?php endif; ?>

       <?php if ($renderType == 'passwordForm'): ?>
	 <?php startForm(); ?>
         <?php if (!empty($options['attempts'])): ?>
         <div class="warning">
           Failed login attempts: <?php print $options['attempts'] ?> out of a total of
          <?php print G2_SUPPORT_MAX_LOGIN_ATTEMPTS; ?>.
         </div>
         <?php endif; ?>

	 <p>
	   In order to verify you, we require you to enter your Gallery setup password.  This is
	   the password you entered when you installed Gallery.  It can be found in your
	   gallery/config.php file like this:
	 </p>
         <pre>
           $gallery-&gt;setConfig('setup.password', '<b>your password here</b>');
         </pre>
         <p>
	   Password:
	   <input type="password" name="password"/>
	   <script type="text/javascript">document.forms[0]['password'].focus();</script>
	   <input type="submit" value="Verify Me"/>
         </p>
         <?php if (isset($options['missingPassword'])): ?>
         <div class="warning">
           Please enter a password!
         </div>
         <?php endif; ?>
       </form>
       <?php endif; ?>

       <?php if ($renderType == 'loginTxtForm'): ?>
	 <?php startForm(); ?>
	 <p>
           <span class="error"><b>
	   <?php if (!empty($options['error'])): ?>
	   There was a problem connecting to the database.
	   <?php else: ?>
           You have exceeded the maximum attempts to login by entering a password.
	   <?php endif; ?>
	   </b></span>

           To prevent somebody from hacking your website, we now
           require you to complete a stricter authentication
           process.  You must create a text file called
	   <strong>login.txt</strong> in the gallery2 directory on your
	   webserver (that's the same directory where main.php is
	   located) and put specific text in it.  If you used the preinstaller
	   to install Gallery, then you may have to use it again to set your directory
           permissions so that you can create the login.txt file.
	 </p>
	 <div class="info">
	   Create this file: <b><?php print $options['baseDir'] ?>login.txt</b> <br/>
	   Put this in the file: <b><?php print $options['key']; ?></b>
	 </div>
	 <?php if (!empty($options['authFileErrors'])): ?>
	 <div class="error">
	   <?php if (!empty($options['authFileErrors']['missing'])): ?>
	     The login.txt file does not exist.
	   <?php elseif (!empty($options['authFileErrors']['unreadable'])): ?>
             The login.txt file exists, but is not readable by the webserver.
             <br/> Try: <b>chmod 644 login.txt</b>
           <?php elseif (!empty($options['authFileErrors']['mismatch'])): ?>
             The password in your login.txt file does not match the key above!
           <?php endif; ?>
	 </div>
         <?php endif; ?>
	 <p>
	   <?php if (!empty($options['error'])): ?>
	   <?php endif; ?>
           <input type="submit" value="Verify me"/>
         </p>
       </form>
       <?php endif; ?>
     </div>
  </body>
</html>
<?php
}
?>
