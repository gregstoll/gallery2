<?php
/**
 * Usage: php trim-po.php xx_YY.po
 * Prints a copy of xx_YY.po, omitting all translations that match xx.po.
 * If not of form xx_YY.po or xx.po does not exist then trim any translations
 * where msgid == msgstr (applies mainly to en_*).
 * Both cases also print warnings for any translation hints that are not
 * handled in this translation (and will appear in the application).
 */
$path = $argv[1];
if (preg_match('#^/cygdrive/(\w+)/(.*)$#', trim($path), $matches)) {
    /* Cygwin and Window PHP filesystem function don't play nice together. */
    $path = $matches[1] . ':\\' . str_replace('/', '\\', $matches[2]);
}

$langpath = preg_replace('{(..)_..\.po$}', '$1.po', $path);

if ($langpath == $path || !file_exists($langpath)) {
    if ($langpath != $path && !in_array(basename($langpath), array('en.po', 'zh.po'))) {
	fwrite(stdErr(), "\nWarning: $path without $langpath\n");
    }
    list ($po, $header) = readPo($path);
    print $header;
    foreach ($po as $id => $data) {
	$isFuzzy = strpos($data['before'], ', fuzzy') !== false;
	checkMessageForValidity($id, $data['msgstr'], $isFuzzy, $path);
	if (substr($id, 5) != substr($data['msgstr'], 6)) {
	    print $data['before'] . $id . $data['msgstr'] . "\n";
	}
    }
    exit;
}

list ($po, $header) = readPo($path);
list ($langpo) = readPo($langpath);

print $header;
foreach ($po as $id => $data) {
    $isFuzzy = strpos($data['before'], ', fuzzy') !== false;
    checkMessageForValidity($id, $data['msgstr'], $isFuzzy, $path);
    if (!isset($langpo[$id]) || $langpo[$id]['msgstr'] != $data['msgstr']) {
	print $data['before'] . $id . $data['msgstr'] . "\n";
    }
}

function isValidUtf8($string) {
    /*
     * Additional tests here:
     * http://dev.jpmullan.com/snippets/check_utf8.php
     */
    $valid = false;
    /*
     * We are declaring non-strings to be invalid, even if their string
     * representation is valid.
     */
    if (is_string($string)) {
	if (!$string) {
	    /* The empty string is always valid */
	    $valid = true;
	} else if (preg_match('/^.{1}/usS', $string)) {
	    if (!preg_match("/[\xC0\xC1\xF5-\xFF]/S", $string)) {
		/*
		 * A string must pass both regexes to be valid.
		 *
		 * See
		 * http://us.php.net/manual/en/reference.pcre.pattern.modifiers.php 
		 * for the /u modifier trick.
		 *
		 * Theoretically preg_match /u does not fail on five or six
		 * octet sequences, but they are not displayable in any
		 * browser.  See the wikipedia page for why each of these
		 * characters is specifically illegal.
		 * http://en.wikipedia.org/wiki/UTF-8 
		 */
		$valid = true;
	    }
	}
    }
    return $valid;
}

function checkStringForBadUtf8($string, $path) {
    if (!isValidUtf8($string)) {
	$printableString = preg_replace('/([^\x20-\x7e])/e', '"\\\\\\x" . dechex(ord("${1}"))', $string);
	fwrite(stdErr(),
	       "\nWarning: Translation contains invalid UTF-8"
	       . " \"$printableString\" in file $path\n");
    }
}

function checkMessageForValidity($msgid, $msgstr, $isFuzzy, $path) {
    $string = _parsePoLinesForMessageString($msgid);
    checkStringForHtml($string, 'msgid', $path);
    checkStringForBadUtf8($string, $path);
    if (!$isFuzzy) {
	$string = _parsePoLinesForMessageString($msgstr);
	checkStringForHtml($string, 'msgstr', $path);
	checkStringForBadUtf8($string, $path);
    }
}

function checkStringForHtml($string, $type, $path) {
    static $ltRegExpPattern, $gtRegExpPattern;
    if (empty($ltRegExpPattern)) {
	$allowedHtmlTags = array('b', 'i', 'strong', 'tt');

	$openTags = implode('>|', $allowedHtmlTags) . '>';
	$closeTags = '/' . implode('>|/', $allowedHtmlTags) . '>';
	$ltRegExpPattern = '#<(?!' . $openTags . '|' . $closeTags . ')#';

	$openTags = '<' . implode('|<', $allowedHtmlTags);
	$closeTags = '</' . implode('|</', $allowedHtmlTags);
	$gtRegExpPattern = '#(?<!' . $openTags . '|' . $closeTags . ')>#';
    }

    if (preg_match($ltRegExpPattern, $string)) {
	fwrite(stdErr(), "\nWarning: Translation contains < (should be &lt;) in $type "
		       . "\"$string\" in file $path\n");
    }
    
    if (preg_match($gtRegExpPattern, $string)) {
	fwrite(stdErr(), "\nWarning: Translation contains > (should be &gt;) in $type "
		       . "\"$string\" in file $path\n");
    }

    if (strpos($string, '&') !== false) {
	/* Can't use look-ahead assertion of variable length. Therefore exploding on &. */
	$ampStrings = explode('&', $string);
	array_shift($ampStrings);
	foreach ($ampStrings as $ampString) {
	    if (!preg_match('/^[A-Za-z0-9#]{2,9};/', $ampString)) {
		fwrite(stdErr(), "\nWarning: Translation contains & (should be &amp;) in $type "
			       . "\"$string\" in file $path\n");
	    }
	}
    }
}

function _parsePoLinesForMessageString($poLines) {
    $lines = explode("\n", $poLines);
    $message = '';
    foreach ($lines as $line) {
	if (preg_match('/^[^"]*"(.*)"/', $line, $matches)) {
	    $message .= $matches[1];
	}
    }
    return $message;
}

function readPo($path) {
    $header = $data = array();
    $lines = file($path);
    for ($line = 'a'; $lines && trim($line); $header[] = $line) {
	$line = array_shift($lines);
    }
    $id = $str = false;
    $key = $value = $before = '';
    while ($lines) {
	$line = array_shift($lines);
	if (!$id && substr($line, 0, 5) == 'msgid') {
	    $id = true;
	} else if ($id && substr($line, 0, 6) == 'msgstr') {
	    $str = true;
	} else if ($id && $str && !trim($line)) {
	    $data[$key] = array('msgstr' => $value, 'before' => $before);
	    $id = $str = false;
	    $key = $value = $before = '';
	    continue;
	}
	if ($str) {
	    $value .= $line;
	} else if ($id) {
	    $key .= $line;
	} else {
	    $before .= $line;
	}
    }
    if ($key && $value) {
	$data[$key] = array('msgstr' => $value, 'before' => $before);
    }
    return array($data, implode('', $header));
}

function stdErr() {
    static $stdErr;
    if (!defined('STDERR')) {
	/* Already defined for CLI but not for CGI */
	$stdErr = fopen('php://stderr', 'w');
	define('STDERR', $stdErr);
    }
    return STDERR;
}
?>
