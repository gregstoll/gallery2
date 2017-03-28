#!/usr/bin/php -f
<?php
/*
 * PHP script to extract strings from all the files and print
 * to stdout for use with xgettext.
 *
 * This script is based on the perl script provided with the Horde project
 * http://www.horde.org/.  As such, it inherits the license from the
 * original version.  You can find that license here:
 *
 * http://cvs.horde.org/co.php/horde/COPYING?r=2.1
 *
 * I'm not exactly sure what the license restrictions are in this case,
 * but I want to give full credit to the original authors:
 *
 * Copyright 2000-2002 Joris Braakman <jbraakman@yahoo.com>
 * Copyright 2001-2002 Chuck Hagenbuch <chuck@horde.org>
 * Copyright 2001-2002 Jan Schneider <jan@horde.org>
 *
 * We've modified the script somewhat to make it work cleanly with the
 * way that Gallery embeds internationalized text, so let's tack on our
 * own copyrights.
 *
 * Copyright (C) 2002-2008 Bharat Mediratta <bharat@menalto.com>
 *
 * $Id: extract.php 17580 2008-04-13 00:38:13Z tnalmdal $
 */

if (!empty($_SERVER['SERVER_NAME'])) {
    errorExit("You must run this from the command line\n");
}
if (!function_exists('token_get_all')) {
    errorExit("PHP tokenizer required.\n"
	    . "Must use a PHP binary that is NOT built with --disable-tokenizer\n");
}

$exts = '(class|php|inc|css|html|tpl)';
$idEmitted = false;
$strings = array();
array_shift($_SERVER['argv']);
foreach ($_SERVER['argv'] as $moduleDir) {
    if (preg_match('#^/cygdrive/(\w+)/(.*)$#', trim($moduleDir), $matches)) {
	/* Cygwin and Window PHP filesystem function don't play nice together. */
	$moduleDir = $matches[1] . ':\\' . str_replace('/', '\\', $matches[2]);
    }
    if (!is_dir($moduleDir)) {
	continue;
    }
    chdir($moduleDir);
    find('.');

    $oldStringsRaw = "$moduleDir/po/strings.raw";
    if (file_exists($oldStringsRaw)) {
	$lines = file($oldStringsRaw);
	if (preg_match('/^#.*Id/', $lines[0])) {
	    print $lines[0];
	    $idEmitted = true;
	}
    }
}

if (!$idEmitted) {
    print '# $' . 'Id$' . "\n";
}
foreach ($strings as $string => $otherFiles) {
    print $string;
    if (!empty($otherFiles)) {
	print ' /* also in: ' . implode(' ', $otherFiles) . ' */';
    }
    print "\n";
}

/**
 * Recursive go through subdirectories
 */
function find($dir) {
    if ($dh = opendir($dir)) {
	$listing = $subdirs = array();
	while (($file = readdir($dh)) !== false) {
	    if ($file == '.' || $file == '..') {
		continue;
	    }
	    $listing[] = $file;
	}
	closedir($dh);
	sort($listing);
	global $exts;
	$dir = ($dir == '.') ? '' : ($dir . '/');
	foreach ($listing as $file) {
	    $filename = $dir . $file;
	    if (is_dir($filename)) {
		/* Don't parse unit tests */
		if ($file != 'test') {
		    $subdirs[] = $filename;
		}
	    } else if (preg_match('/\.' . $exts . '$/', $file)) {
		extractStrings($filename);
	    }
	}
	foreach ($subdirs as $dir) {
	    find($dir);
	}
    }
}

/**
 * Grab all translatable strings in a file into $strings array
 */
function extractStrings($filename) {
    global $strings;
    $strings["\n/* $filename */"] = array();
    $startSize = count($strings);
    $localStrings = array();
    $data = file_get_contents($filename);

    /*
     * class|inc|php are module and core PHP files.
     * Parse .html as PHP for installer/upgrader templates/*.html files.
     * Parse .css as PHP for modules/colorpack/packs/{name}/color.css files.
     */
    if (preg_match('/\.(class|inc|php|css|html)$/', $filename)) {
	/* Tokenize PHP code and process to find translate( or i18n( or _( calls */
	$tokens = token_get_all($data);
	for ($i = 0; $i < count($tokens); $i++) {
	    if (is_array($tokens[$i]) && $tokens[$i][0] == T_STRING
		    && in_array($tokens[$i][1], array('translate', '_translate', 'i18n', '_'))
		    && $tokens[$i + 1] === '(') {
		/* Found a function call for translation, process the contents */
		for ($i += 2; is_array($tokens[$i]) && $tokens[$i][0] == T_WHITESPACE; $i++) { }
		if (is_array($tokens[$i]) && $tokens[$i][0] == T_VARIABLE) {
		    /* Skip translate($variable) */
		    continue;
		}
		for ($buf = '', $parenCount = $ignore = 0; $i < count($tokens); $i++) {
		    /* Fill $buf with translation params; so end at , or ) not in a nested () */
		    if (!$parenCount && ($tokens[$i] === ')' || $tokens[$i] === ',')) {
			break;
		    }
		    if ($ignore && $parenCount == $ignore
			    && ($tokens[$i] === ',' || $tokens[$i] === ')')) {
			$ignore = false;
		    }
		    if ($tokens[$i] === '(') {
			$parenCount++;
		    } else if ($tokens[$i] === ')') {
			$parenCount--;
		    }
		    if (is_array($tokens[$i]) && $tokens[$i][0] == T_CONSTANT_ENCAPSED_STRING) {
			$lastString = $tokens[$i][1];
		    }
		    if (!$ignore) {
			$buf .= is_array($tokens[$i]) ? $tokens[$i][1] : $tokens[$i];
		    }
		    if (is_array($tokens[$i]) && $tokens[$i][0] == T_DOUBLE_ARROW
			    && (substr($lastString, 1, 3) === 'arg'
				|| substr($lastString, 1, 5) === 'count')) {
			/*
			 * Convert 'argN' => code to 'argN' => null so we don't eval that code.
			 * Add 'null' to $buf now, then ignore content until next , or ) not in
			 * a deeper nested ().
			 */
			$buf .= 'null';
			$ignore = $parenCount;
		    }
		}
		$param = eval('return ' . $buf . ';');
		if (is_string($param)) {
		    /* Escape double quotes and newlines */
		    $text = strtr($param, array('"' => '\"', "\r\n" => '\n', "\n" => '\n'));
		    $string = 'gettext("' . $text . '")';
		    if (!isset($strings[$string])) {
			$strings[$string] = array();
		    } else if (!isset($localStrings[$string])) {
			$strings[$string][] = $filename;
		    }
		    $localStrings[$string] = true;
		} else if (is_array($param)) {
		    foreach (array('text', 'one', 'many') as $key) {
			if (isset($param[$key])) {
			    /* Escape double quotes and newlines */
			    $param[$key] = strtr($param[$key],
						 array('"' => '\\"', "\r\n" => '\n', "\n" => '\n'));
			}
		    }
		    if (isset($param['one'])) {
			$string = 'ngettext("' . $param['one'] . '", "' . $param['many'] . '")';
		    } else {
			$string = 'gettext("' . $param['text'] . '")';
		    }
		    if (isset($param['cFormat'])) {
			$string = '/* xgettext:'
				. ($param['cFormat'] ? '' : 'no-') . "c-format */\n$string";
		    }
		    if (!empty($param['hint'])) {
			$string = '// HINT: ' . str_replace("\n", "\n// ", $param['hint'])
				. "\n$string";
		    }
		    if (!isset($strings[$string])) {
			$strings[$string] = array();
		    } else if (!isset($localStrings[$string])) {
			$strings[$string][] = $filename;
		    }
		    $localStrings[$string] = true;
		}
	    }
	}
    } else if (preg_match_all('/{\s*g->(?:text|changeInDescendents)\s+.*?[^\\\\]}/s',
			      $data, $matches)) {
	/* Use regexp to process tpl files for {g->text ..} and {g->changeInDescendents ..} */
	foreach ($matches[0] as $string) {
	    $text = $one = $many = null;

	    /*
	     * Ignore translations of the form:
	     *   text=$foo
	     * as we expect those to be variables containing values that
	     * have been marked elsewhere with the i18n() function.
	     */
	    if (preg_match('/\stext=\$/', $string)) {
		continue;
	    }

	    /* text=..... */
	    if (preg_match('/\stext="(.*?[^\\\\])"/s', $string, $matches)) {
		$text = $matches[1];
	    } else if (preg_match("/text='(.*?)'/s", $string, $matches)) {
		$text = str_replace('"', '\"', $matches[1]);    /* Escape double quotes */
	    }

	    /* one=..... */
	    if (preg_match('/\sone="(.*?[^\\\\])"/s', $string, $matches)) {
		$one = $matches[1];
	    } else if (preg_match("/\sone='(.*?)'/s", $string, $matches)) {
		$one = str_replace('"', '\"', $matches[1]);    /* Escape double quotes */
	    }

	    /* many=..... */
	    if (preg_match('/\smany="(.*?[^\\\\])"/s', $string, $matches)) {
		$many = $matches[1];
	    } else if (preg_match("/\smany='(.*?)'/s", $string, $matches)) {
		$many = str_replace('"', '\"', $matches[1]);    /* Escape double quotes */
	    }

	    /* Hint for translators */
	    $translatorHint = preg_match('/\shint=((["\']).*?[^\\\\]\2)/s', $string, $matches)
		? eval('return ' . $matches[1] . ';') : '';

	    /* c-format hint for xgettext */
	    $cFormatHint = preg_match('/\sc[Ff]ormat=(true|false)/s', $string, $matches)
		? '/* xgettext:' . ($matches[1] == 'false' ? 'no-' : '') . "c-format */\n" : '';

	    /* Pick gettext() or ngettext() and escape newlines */
	    if (isset($text)) {
		$string = 'gettext("' . strtr($text, array("\r\n" => '\n', "\n" => '\n')) . '")';
	    } else if (isset($one) && isset($many)) {
		$string = 'ngettext("' . strtr($one, array("\r\n" => '\n', "\n" => '\n')) . '", "'
			. strtr($many, array("\r\n" => '\n', "\n" => '\n')) . '")';
	    } else {
		/* Parse error */
		$string = str_replace("\n", '\n> ', $string);
		errorExit("extract.php parse error: $filename:\n> $string\n");
	    }

	    if ($cFormatHint) {
		$string = $cFormatHint . $string;
	    }
	    if ($translatorHint) {
		$string = "// HINT: $translatorHint\n$string";
	    }
	    if (!isset($strings[$string])) {
		$strings[$string] = array();
	    } else if (!isset($localStrings[$string])) {
		$strings[$string][] = $filename;
	    }
	    $localStrings[$string] = true;
	}
    }
    if (count($strings) == $startSize) {
	unset($strings["\n/* $filename */"]);
    }
}

function errorExit($message) {
    $stderr = fopen('php://stderr', 'w');
    fwrite($stderr, $message);
    exit(1);
}
?>
