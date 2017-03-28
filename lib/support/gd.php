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
 * This script is useful to gather information about a specific PHP
 * environment. The output is an array that can be appended to the
 * GdFunctionalityMatrix.inc in the gd module's phpunit tests. This is used
 * by the phpunit tests to test the GdToolkit functionality in different PHP
 * environments in a single installation.
 *
 * @version $Revision: 17580 $
 * @package Gd
 * @subpackage PHPUnit
 *
 * @author Ernesto Baschny <ernst@baschny.de>
 */
if (!defined('G2_SUPPORT')) { return; }

/*
 * Gets a lot of information about our GD installation and return it as a
 * giant string, which can be eval'ed to an array.
 */
function getGdLibraryInfo() {
    if (! extension_loaded('gd')) {
	return '';
    }

    /* Get GD version from phpinfo or gd_info */
    if (function_exists('gd_info')) {
	$gdInfo = gd_info();
	$matchString = $gdInfo['GD Version'];
	$matcherVersion = '/([\d\.]+)(\s+or\s+higher)?/i';
	$matcherBundled = '/bundled/i';
    } else {
	ob_start();
	phpinfo(8);
	$matchString = ob_get_contents();
	$matcherVersion = '/\bgd\s+version\b[^\d\n\r]+?([\d\.]+)(\s+or\s+higher)?/i';
	$matcherBundled = '/\bgd\s+version\b[^\d\n\r]+?bundled/i';
	ob_end_clean();
    }
    if (preg_match($matcherVersion, $matchString, $matches)) {
	$gdVersion = $matches[1];
    } else {
	$gdVersion = 0;
    }
    if (isset($matches[2])) {
	$gdVersion = sprintf('>%s', $gdVersion);
    }
    $isGdBundled = 0;
    if (preg_match($matcherBundled, $matchString)) {
	$isGdBundled = 1;
    }

    /* Find out supported mime types */
    $mimeChecks = array(
	array(
	    'mimeType' => 'image/gif',
	    'value' => defined('IMG_GIF') ? IMG_GIF : '',
	    'functions' => array('imageCreateFromGif', 'imageGif')
	),
	array(
	    'mimeType' => 'image/jpeg',
	    'value' => defined('IMG_JPEG') ? IMG_JPEG : '',
	    'functions' => array('imageCreateFromJpeg', 'imageJpeg')
	),
	array(
	    'mimeType' => 'image/png',
	    'value' => defined('IMG_PNG') ? IMG_PNG : '',
	    'functions' => array('imageCreateFromPng', 'imagePng')
	),
	array(
	    'mimeType' => 'image/vnd.wap.wbmp',
	    'value' => defined('IMG_WBMP') ? IMG_WBMP : '',
	    'functions' => array('imageCreateFromWbmp', 'imageWbmp')
	),
	array(
	    'mimeType' => 'image/x-xpixmap',
	    'value' => defined('IMG_XPM') ? IMG_XPM : '',
	    'functions' => array('imageCreateFromXpm', 'imageXpm')
	),
	array(
	    'mimeType' => 'image/x-xbitmap',
	    'value' => defined('IMG_XBM') ? IMG_XBM : '',
	    'functions' => array('imageCreateFromXbm', 'imageXbm')
	),
    );
    $mimeTypes = array();
    foreach ($mimeChecks as $check) {
	$ok = true;
	foreach ($check['functions'] as $fct) {
	    if (! function_exists($fct)) {
		$ok = false;
	    }
	}
	if ($ok && ! ($check['value'] & imageTypes())) {
	    $ok = false;
	}
	if ($ok) {
	    $mimeTypes[] = $check['mimeType'];
	}
    }

    $out = '';
    $out .= '$gdEnvironments[] = array(' . "\n";
    $name = sprintf('%s|%s%s|%s',
		 phpversion(),
		 $gdVersion,
		 ($isGdBundled ? '-bundled' : '-external'),
		 PHP_OS
		);

    $out .= "\t" . sprintf('\'name\' => \'%s\',', $name) . "\n";
    $out .= "\t" . sprintf('\'phpVersion\' => \'%s\',', phpversion()) . "\n";
    $out .= "\t" . sprintf('\'gdVersion\' => \'%s\',', $gdVersion) . "\n";
    $out .= "\t" . sprintf('\'gdBundled\' => %s,', $isGdBundled) . "\n";

    $imageTypes = 0;
    if (function_exists('imageTypes')) {
	$imageTypes = imageTypes();
    }
    $out .= "\t" . sprintf('\'imageTypes\' => %s,', $imageTypes) . "\n";

    if (function_exists('gd_info')) {
	$gdInfo = gd_info();
	$out .= "\t" . '\'gd_info\' => array(' . "\n";
	foreach ($gdInfo as $field => $value) {
	    $out .= "\t\t" . sprintf('\'%s\' => \'%s\',', $field, $value) . "\n";
	}
	$out .= "\t" . '),' . "\n";
    }

    /* Check which constants are defined */
    $constants = get_defined_constants();
    $out .= "\t" . '\'constants\' => array(' . "\n";
    foreach ($constants as $constant => $value) {
	if (! preg_match('/^(IMAGE|IMG|GD|PHP)/', $constant)) { continue; }
	if (! is_int($value)) {
	    $value = sprintf('\'%s\'', $value);
	}
	$out .= "\t\t" . sprintf('\'%s\' => %s,', $constant, $value) . "\n";
    }
    $out .= "\t" . '),' . "\n";

    $out .= "\t" . '\'mimeTypes\' => array(' . "\n";
    foreach ($mimeTypes as $mimeType) {
	$out .= "\t\t" . sprintf('\'%s\',', $mimeType) . "\n";
    }
    $out .= "\t" . '),' . "\n";

    ob_start();
    phpinfo(8);
    $phpinfo = ob_get_contents();
    ob_end_clean();
    $phpinfo = htmlspecialchars($phpinfo);
    $phpinfo = preg_replace('/\'/', '\\\'', $phpinfo);
    $out .= "\t" . sprintf('\'phpinfo(8)\' => \'%s\',', $phpinfo) . "\n";

    /* Functions defined in this GD module */
    $functions = get_extension_funcs('gd');
    $out .= "\t" . '\'functions\' => array(' . "\n";
    foreach ($functions as $fct) {
	$out .= "\t\t" . sprintf('\'%s\' => true,', $fct) . "\n";
    }

    $otherFunctions = array(
	'getimagesize',
	'image_type_to_extension',
	'image_type_to_mime_type',
	'iptcembed',
	'iptcparse',
    );
    foreach ($otherFunctions as $fct) {
	if (! function_exists($fct)) { continue; }
	$out .= "\t\t" . sprintf('\'%s\' => true,', $fct) . "\n";
    }
    $out .= "\t" . '),' . "\n";
    $out .= ');' . "\n";
    return $out;
}

$gdInfo = getGdLibraryInfo();

?>
<html>
  <head>
    <title>Gallery Support | GD Library Info</title>
    <link rel="stylesheet" type="text/css" href="<?php print $baseUrl ?>support.css"/>
  </head>
  <body>
    <div id="content">
      <div id="title">
	<a href="../../">Gallery</a> &raquo;
	<a href="<?php generateUrl('index.php') ?>">Support</a> &raquo; GD Library Info
      </div>
      <?php if ($gdInfo == ''): ?>
      <h2>No GD library found.</h2>
      <?php else: ?>
      <h2>This information might be useful for the GD module developers:</h2>
      <pre style="padding-left: 20px"><?php echo $gdInfo; ?></pre>
      <?php endif; ?>
    </div>
  </body>
</html>
