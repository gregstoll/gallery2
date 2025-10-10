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

// Authors: Jens Tkotz, Bharat Mediratta
define('G2_SUPPORT_URL_FRAGMENT', '../../support/');

require_once __DIR__ . '/../../support/security.inc';
ini_set('magic_quotes_runtime', false);
set_time_limit(0);

$type = 'summary';

if (!empty($_REQUEST['type']) && $_REQUEST['type'] == 'detail') {
	$type = 'detail';
} elseif (php_sapi_name() == 'cli' && $argv[1] == 'detail') {
	$type = 'detail';
}
$precision = isset($_GET['precision']) ? (int)$_GET['precision'] : ($type == 'detail' ? 2 : 1);
$pow       = 10 ** $precision;

$poFiles = findPoFiles('../../..');
list($reportData, $mostRecentPoDate, $totalTranslated) = parsePoFiles($poFiles);

require __DIR__ . '/localization/main_' . $type . '.inc';

exit;

function findPoFiles($dir) {
	$results = array();

	if (!is_dir($dir)) {
		return $results;
	}

	if ($dh = opendir($dir)) {
		while (($file = readdir($dh)) !== false) {
			if ($file == '.' || $file == '..') {
				continue;
			}
			$path = $dir . '/' . $file;

			if (is_dir($path)) {
				$results = array_merge($results, findPoFiles($path));
			} else {
				if (preg_match('/\.po$/', $file)) {
					$results[] = $path;
				}
			}
		}
	}

	return $results;
}

function parsePoFiles($poFiles) {
	/*
	 * Parse each .po file for relevant statistics and gather it together into a
	 * single data structure.
	 */
	global $pow;
	$poData           = $seenPlugins           = $maxMessageCount           = array();
	$mostRecentPoDate = $totalTranslated = 0;

	foreach ($poFiles as $poFile) {
		if (!preg_match('|((?:\w+/)+)po/(\w{2}(?:_\w{2})?)\.po|', $poFile, $matches)) {
			continue;
		}
		list($plugin, $locale) = array($matches[1], $matches[2]);
		$seenPlugins[$plugin]  = 1;
		$stat                  = stat($poFile);

		if ($stat && $stat['mtime'] > $mostRecentPoDate) {
			$mostRecentPoDate = $stat['mtime'];
		}

		$fuzzy = $translated = $untranslated = $obsolete = 0;
		/*
		 * Untranslated:
		 *   msgid "foo"
		 *   msgstr ""
		 *
		 * Translated:
		 *   msgid "foo"
		 *   msgstr "bar"
		 *
		 * Translated:
		 *   msgid "foo"
		 *   msgstr ""
		 *   "blah blah blah"
		 *
		 * Untranslated:
		 *   msgid "foo"
		 *   msgid_plural "foos"
		 *   msgstr[0] ""
		 *   msgstr[1] ""
		 *   msgstr[2] ""
		 *
		 * Translated:
		 *   msgid "foo"
		 *   msgid_plural "foos"
		 *   msgstr[0] "bar1"
		 *   msgstr[1] "bar2"
		 *   msgstr[2] "bar3"
		 *
		 * Translated, Fuzzy:
		 *   # fuzzy
		 *   msgid "foo"
		 *   msgstr "bar"
		 *
		 * Deleted, Fuzzy:
		 *   # fuzzy
		 *   #~ msgid "foo"
		 *   #~ msgstr "bar"
		 *
		 */
		$msgId       = null;
		$nextIsFuzzy = $lastLineWasEmptyMsgStr = $lastLineWasEmptyMsgId = 0;

		foreach (file($poFile) as $line) {
			/*
			 * Scan for:
			 *   msgid "foo bar"
			 *
			 * and:
			 *   msgid ""
			 *   "foo bar"
			 */
			if (preg_match('/^msgid "(.*)"/', $line, $matches)) {
				if (empty($matches[1])) {
					$lastLineWasEmptyMsgId = 1;
				} else {
					$msgId = $line;
				}

				continue;
			}

			/*
			 * Scan for:
			 *   msgid ""
			 *   "foo bar"
			 */
			if ($lastLineWasEmptyMsgId) {
				if (preg_match('/^\s*"(.*)"/', $line, $matches)) {
					$msgId = $line;
				}
				$lastLineWasEmptyMsgId = 0;

				continue;
			}

			if (strpos($line, '#, fuzzy') === 0) {
				$nextIsFuzzy = 1;

				continue;
			}

			if (preg_match('/^#~ msgid "(.*)"/', $line, $matches)) {
				$obsolete++;
				$nextIsFuzzy = 0;
			}

			/*
			 * Scan for:
			 *   msgstr ""
			 *   "foo bar"
			 */
			if ($lastLineWasEmptyMsgStr) {
				if (preg_match('/^\s*".+"/', $line)) {
					if ($nextIsFuzzy) {
						$fuzzy++;
					}
					$translated++;
				} else {
					if ($nextIsFuzzy) {
						echo "ERROR: DISCARD FUZZY for [$locale, $plugin, $msgId]<br>";
					}
					$untranslated++;
				}
				$msgId                  = null;
				$nextIsFuzzy            = 0;
				$lastLineWasEmptyMsgStr = 0;
			}

			/*
			 * Scan for:
			 *   msgstr "foo bar"
			 *
			 * or:
			 *   msgstr ""
			 *   "foo bar"
			 */
			if (!empty($msgId)) {
				if (preg_match('/^msgstr/', $line)) {
					if (preg_match('/^msgstr[\d\[\]]*\s*""\s*$/', $line)) {
						$lastLineWasEmptyMsgStr = 1;
					} else {
						if ($nextIsFuzzy) {
							$fuzzy++;
						}
						$translated++;
						$msgId       = null;
						$nextIsFuzzy = 0;
					}
				}
			}
		}
		// Catch msgstr "" in last line
		if (!empty($msgId) && $lastLineWasEmptyMsgStr) {
			$untranslated++;
		}

		$total = $translated + $untranslated;

		if (empty($total)) {
			$percentDone = $exactPercentDone = 0;
		} else {
			$percentDone      = floor(($translated - $fuzzy) * 100 * $pow / $total) / $pow;
			$exactPercentDone = ($translated - $fuzzy) * 100                        / $total;
		}
		$poData[$locale]['plugins'][$plugin] = array(
			'translated'       => $translated,
			'untranslated'     => $untranslated,
			'total'            => $total,
			'fuzzy'            => $fuzzy,
			'obsolete'         => $obsolete,
			'percentDone'      => $percentDone,
			'exactPercentDone' => $exactPercentDone,
			'name'             => $plugin,
		);
		$totalTranslated                    += $translated - $fuzzy;

		foreach (array('translated', 'untranslated', 'fuzzy', 'obsolete') as $key) {
			if (!isset($summary[$locale][$key])) {
				$summary[$locale][$key] = 0;
			}

			$summary[$locale][$key] += $poData[$locale]['plugins'][$plugin][$key];
		}

		// Keep track of the largest message count we've seen per plugin
		if (empty($maxMessageCount[$plugin]) || $total > $maxMessageCount[$plugin]) {
			$maxMessageCount[$plugin] = $total;
		}
	}

	// Overall total message count
	$overallTotal = array_sum(array_values($maxMessageCount));

	foreach (array_keys($poData) as $locale) {
		$pluginTotal = 0;

		// Fill in any missing locales
		foreach (array_keys($seenPlugins) as $plugin) {
			if (!isset($poData[$locale]['plugins'][$plugin])) {
				$poData[$locale]['plugins'][$plugin]['missing']          = 1;
				$poData[$locale]['plugins'][$plugin]['percentDone']      = 0;
				$poData[$locale]['plugins'][$plugin]['exactPercentDone'] = 0;
				$poData[$locale]['plugins'][$plugin]['name']             = $plugin;
			} else {
				$pluginTotal += $poData[$locale]['plugins'][$plugin]['translated'] - $poData[$locale]['plugins'][$plugin]['fuzzy'];
			}
		}
		uasort($poData[$locale]['plugins'], 'sortByPercentDone');

		// Figure out total percentage
		if (empty($overallTotal)) {
			$poData[$locale]['percentDone'] = $poData[$locale]['exactPercentDone'] = 0;
		} else {
			$poData[$locale]['percentDone']      = floor($pluginTotal * 100 * $pow / $overallTotal) / $pow;
			$poData[$locale]['exactPercentDone'] = $pluginTotal * 100                               / $overallTotal;
		}

		foreach (array('translated', 'untranslated', 'fuzzy', 'obsolete') as $key) {
			$poData[$locale]['summary'][$key] = floor($summary[$locale][$key] * 100 * $pow / $overallTotal) / $pow;
		}
		$poData[$locale]['summary']['total'] = $overallTotal;
	}

	// Sort locales by overall total
	uasort($poData, 'sortByPercentDone');

	return array($poData, $mostRecentPoDate, $totalTranslated);
}

/**
 * Comparison function to be called by uasort elsewhere. The given params are arrays expected to
 * have at least a key of 'percentDone' with a numeric value. Optionally the arrays may have
 * two more keys: 'missing' and 'name'. Main sort is by 'percentDone' DESC. But if one of the
 * given params has a key 'missing' and the other not, the entry with 'missing' is sorted last,
 * regardless of percentage. When two entries are otherwise equal and both have a 'name' entry,
 * then the sort is by 'name' ASC.
 *
 * @param array $a first entry to sort
 * @param array $b second entry to sort
 * @return int   -1, 0, +1, depending on the comparision
 */
function sortByPercentDone($a, $b) {
	if (isset($a['missing']) && !isset($b['missing'])) {
		return 1;
	}

	if (isset($b['missing']) && !isset($a['missing'])) {
		return -1;
	}

	if ($a['exactPercentDone'] == $b['exactPercentDone']) {
		if (isset($a['name'], $b['name'])) {
			return ($a['name'] < $b['name']) ? -1 : 1;
		}

		return 0;
	}

	return ($a['exactPercentDone'] < $b['exactPercentDone']) ? 1 : -1;
}

function percentColor($percent) {
	$border = 50;

	if ($percent < $border) {
		$color = dechex(255 - $percent * 2) . '0000';
	} else {
		$color = '00' . dechex(55 + $percent * 2) . '00';
	}

	if (strlen($color) < 6) {
		$color = '0' . $color;
	}

	return $color;
}

function newRow() {
	$count =& getRowCount();
	$count++;
}

function &getRowCount() {
	static $count;

	if (!isset($count)) {
		$count = 0;
	}

	return $count;
}

function modifier($string) {
	$count =& getRowCount();

	if ($count % 2) {
		return $string . '_light';
	}

	return $string . '_dark';
}
