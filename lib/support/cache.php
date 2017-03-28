<?php
if (!defined('G2_SUPPORT')) { return; }

function getCaches() {
    $dirs = array(
	'cached_pages' => array(true, 'clearPageCache', array(), 'Cached HTML pages'),
	'entity' => array(true, 'clearG2dataDir', array('cache/entity'), 'Albums and photo data'),
	'module' => array(true, 'clearG2dataDir', array('cache/module'), 'Module settings'),
	'theme' => array(true, 'clearG2dataDir', array('cache/theme'), 'Theme settings'),
	'template' => array(true, 'clearG2dataDir', array('smarty/templates_c'),
			    'Smarty templates'),
	'tmp' => array(true, 'clearG2dataDir', array('tmp'), 'Temporary directory'),
	'repository' => array(true, 'clearG2dataDir', array('cache/repository'),
			      'Downloadable Plugin Cache'),
	'log' => array(
	    false, 'clearInstallUpgradeLogs', array(),
	    'Install/Upgrade log files <span class="subtext important">' .
	    '(can\'t be recovered!)</span>'),
	'derivative' => array(
	    false, 'clearG2dataDir', array('cache/derivative'),
	    'Thumbnails and resizes <span class="subtext important">(expensive to rebuild)</span>')
	);

    if (!empty($_COOKIE['g2cache'])) {
	$set = array_flip(explode(',', $_COOKIE['g2cache']));
	foreach ($dirs as $key => $ignored) {
	    $dirs[$key][0] = isset($set[$key]);
	}
    }
    return $dirs;
}

function recursiveRmdir($dirname, &$status) {
    $count = 0;
    if (!file_exists($dirname)) {
	return $count;
    }

    if (!($fd = opendir($dirname))) {
	return $count;
    }

    while (($filename = readdir($fd)) !== false) {
	if (!strcmp($filename, '.') || !strcmp($filename, '..')) {
	    continue;
	}
	$path = "$dirname/$filename";

	if (is_dir($path)) {
	    $count += recursiveRmdir($path, $status);
	} else {
	    if (!@unlink($path)) {
		if (!@is_writeable($path)) {
		    $status[] = array('error', "Permission denied removing file $path");
		} else {
		    $status[] = array('error', "Error removing $path");
		}
	    } else {
		$count++;
	    }
	}
    }
    closedir($fd);

    if (!@rmdir($dirname)) {
	$status[] = array('error', "Unable to remove directory $dirname");
    } else {
	$count++;
    }

    return $count;
}

function clearPageCache() {
    global $gallery;
    $storage =& $gallery->getStorage();

    $ret = GalleryCoreApi::removeAllMapEntries('GalleryCacheMap', true);
    if ($ret) {
	$status = array(array('error', 'Error deleting page cache!'));
    } else {
	$status = array(array('info', 'Successfully deleted page cache'));
    }
    $ret = $storage->checkPoint();
    if ($ret) {
	$status[] = array('error', 'Error committing transaction!');
    }

    return $status;
}

function clearG2DataDir($dir) {
    global $gallery;
    $path = $gallery->getConfig('data.gallery.base') . $dir;
    $status = array(array('info', "Deleting dir: $path"));
    $count = recursiveRmdir($path, $status);

    /* Commented this out because it's a little noisy */
    /* $status[] = array('info', "Removed $count files and directories"); */

    if (@mkdir($path)) {
	$status[] = array('info', "Recreating dir: $path");
    } else {
	$status[] = array('error', "Unable to recreate dir: $path");
    }

    return $status;
}

function clearInstallUpgradeLogs() {
    global $gallery;
    $path = $gallery->getConfig('data.gallery.base');
    $status = array();
    $count = 0;
    if ($fd = opendir($path)) {
	while (($filename = readdir($fd)) !== false) {
	    if (preg_match('/^(install|upgrade)_[0-9a-f]+\.log$/', $filename)
		    && is_file($path . $filename)) {
		if (@unlink($path . $filename)) {
		    $count++;
		} else {
		    $status[] = array('error', "Error removing $path$filename");
		}
	    }
	}
	closedir($fd);
    }
    $status[] = array('info', "Removed $count install/upgrade log files");
    return $status;
}

$status = array();
$caches = getCaches();
if (isset($_REQUEST['clear']) && isset($_REQUEST['target'])) {
    require_once(dirname(__FILE__) . '/../../embed.php');
    $ret = GalleryEmbed::init(array('fullInit' => false));
    if ($ret) {
	/* Try to swallow the error, but define a session to make ::done() pass. */
	global $gallery;
	$gallery->initEmptySession();
    }
    $remember = array();
    foreach ($_REQUEST['target'] as $key => $ignored) {
	/* Make sure the dir is legit */
	if (!array_key_exists($key, $caches)) {
	    $status[] = array('error', "Ignoring illegal cache: $key");
	    continue;
	}

	$func = $caches[$key][1];
	$args = $caches[$key][2];
	$status = array_merge($status, call_user_func_array($func, $args));
	$remember[] = $key;
    }
    $ret = GalleryEmbed::done();
    if ($ret) {
	$status[] = array('error', 'Error completing transaction!');
    }
    $_COOKIE['g2cache'] = join(',', $remember);
}
?>
<html>
  <head>
    <title>Gallery Support | Cache Maintenance</title>
    <link rel="stylesheet" type="text/css" href="<?php print $baseUrl ?>support.css"/>
  </head>
  <body>
    <div id="content">
      <div id="title">
	<a href="../../">Gallery</a> &raquo;
	<a href="<?php generateUrl('index.php') ?>">Support</a> &raquo; Cache Maintenance
      </div>
      <h2>
	Gallery caches data on disk to increase performance.
	Occasionally these caches get out of date and need to be deleted.
	Anything in the cache can be deleted safely!  Gallery will
	rebuild anything it needs.
      </h2>

      <?php if (!empty($status)): ?>
      <div class="success">
	<?php foreach ($status as $line): ?>
	<pre class="<?php print $line[0] ?>"><?php print $line[1] ?></pre>
	<?php endforeach; ?>
      </div>
      <?php endif; ?>

      <?php startForm(); ?>
        <p>
	  <?php $caches = getCaches(); ?>
	  <?php foreach ($caches as $key => $info): ?>
	  <input type="checkbox" name="target[<?php print $key ?>]"
            <?php if ($info[0]): ?> checked="checked" <?php endif; ?> />
	  <?php print $info[3] ?> <br/>
	  <?php endforeach; ?>
	  <input type="submit" name="clear" value="Clear Cache"/>
	</p>
      </form>
    </div>
  </body>
</html>
