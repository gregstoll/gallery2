<?php
define('G2_SUPPORT_URL_FRAGMENT', '');
require_once(dirname(__FILE__) . '/security.inc');
ob_start();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<?php
/* Tell other scripts we passed security.inc ok */
define('G2_SUPPORT', true);
if (!empty($_SERVER['QUERY_STRING'])) {
    foreach (array('phpinfo', 'cache', 'gd', 'chmod', 'import') as $script) {
    	/*
    	 * Don't use isset($_GET[$script]) since we want to allow for GET args could collide
    	 * with the above mentioned script names
    	 */
	if ($_SERVER['QUERY_STRING'] == $script ||
	        strncmp($_SERVER['QUERY_STRING'], $script . '&', strlen($script)+1) == 0) {
	    include(dirname(__FILE__) . '/' . $script . '.php');
	    $results = ob_get_contents();
	    ob_end_clean();
	    print $results;
	    return;
	}
    }
}
function generateUrl($uri, $print=true) {
    /* If session.use_trans_sid is on then it will add the session id. */
    if (!GallerySetupUtilities::areCookiesSupported() && !ini_get('session.use_trans_sid')) {
	$sid = session_name() . '=' . session_id();
	$uri .= (!strpos($uri, '?') ? '?' : '&amp;') . $sid;
    }
    if ($print) {
	print $uri;
    }
    return $uri;
}
?>
<html>
  <head>
    <title>Gallery Support</title>
    <link rel="stylesheet" type="text/css" href="<?php print $baseUrl ?>support.css"/>
  </head>

  <body>
    <div id="content">
      <div id="title">
        <a href="../../">Gallery</a> &raquo; Support
      </div>

      <h2>
	This is a collection of scripts that you use to troubleshoot problems with
	your Gallery installation.
      </h2>

      <h2>
        <a href="<?php generateUrl('index.php?phpinfo') ?>">PHP Info</a>
      </h2>
      <p class="description">
        PHP configuration information
      </p>
      <hr class="faint" />

      <h2>
        <a href="<?php generateUrl('index.php?cache') ?>">Cache Maintenance</a>
      </h2>
      <p class="description">
        Delete files from the Gallery data cache
      </p>
      <hr class="faint" />

      <h2>
        <a href="<?php generateUrl('index.php?chmod') ?>">Filesystem Permissions</a>
      </h2>
      <p class="description">
        Change the filesystem permissions of your Gallery and your storage folder.
      </p>
      <hr class="faint" />

      <h2>
        <a href="<?php generateUrl('index.php?gd') ?>">GD</a>
      </h2>
      <p class="description">
        Information about your GD configuration
      </p>
      <hr class="faint" />

      <h2>
        <a href="<?php generateUrl('index.php?import') ?>">Import Database</a>
      </h2>
      <p class="description">
        Restore your Gallery database from an export that was made from the site administration
        maintenance screen or from the Database Backup step of the Gallery upgrader.
      </p>
      <hr class="faint" />

      <h2>
	<?php
	  require_once('../../embed.php');
	  $ret = GalleryEmbed::init(array('fullInit' => false, 'noDatabase' => true));
	  /* Ignore error */
	  $url = GalleryUrlGenerator::appendParamsToUrl('../../' . GALLERY_MAIN_PHP,
		  array('view' => 'core.UserAdmin', 'subView' => 'core.UserRecoverPasswordAdmin'));
	?>
	<a href="<?php print $url ?>">Reset User Password</a>
      </h2>
      <p class="description">
	Set new password for any user.  Can be used to regain access to an administrator
	account when the "forgot password" feature cannot be used due to invalid/missing
	email address or other email problems.
      </p>
    </div>
  </body>
</html>
