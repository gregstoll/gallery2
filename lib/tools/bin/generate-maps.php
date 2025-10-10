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
ini_set('error_reporting', 2047);

if (!empty($_SERVER['SERVER_NAME'])) {
	echo "You must run this from the command line\n";

	exit(1);
}

require_once __DIR__ . '/XmlParser.inc';

require_once __DIR__ . '/../../smarty/Smarty.class.php';

$tmpdir = __DIR__ . '/tmp_maps_' . mt_rand(1, 30000);

if (file_exists($tmpdir)) {
	echo "Tmp dir already exists: $tmpdir\n";

	exit(1);
}

if (!mkdir($tmpdir)) {
	echo "Unable to make tmp dir: $tmpdir\n";

	exit(1);
}

$smarty                  = new Smarty();
$smarty->compile_dir     = $tmpdir;
$smarty->error_reporting = error_reporting();
$smarty->debugging       = true;
$smarty->use_sub_dirs    = false;
$smarty->template_dir    = __DIR__;

$xmlFile = 'Maps.xml';

if (!file_exists($xmlFile)) {
	echo "Missing Maps.xml, can't continue.\n";
	cleanExit(1);
}

$p    = new XmlParser();
$root = $p->parse($xmlFile);

$maps = array();

foreach ($root[0]['child'] as $map) {
	$mapName = $map['child'][0]['content'];

	for ($j = 2; $j < count($map['child']); $j++) {
		$child = $map['child'][$j];

		if ($child['name'] == 'MEMBER') {
			$member = array(
				'name' => $child['child'][0]['content'],
				'type' => 'STORAGE_TYPE_' . $child['child'][1]['content'],
			);

			if (!empty($child['child'][2]['name'])
				&& $child['child'][2]['name'] == 'MEMBER-SIZE'
			) {
				$member['size'] = 'STORAGE_SIZE_' . $child['child'][2]['content'];
			} else {
				$member['size'] = 'STORAGE_SIZE_MEDIUM';
			}

			for ($k = 2; $k < count($child['child']); $k++) {
				if (!empty($child['child'][$k]['name'])) {
					$elem = $child['child'][$k];

					if ($elem['name'] == 'PRIMARY' || $elem['name'] == 'REQUIRED') {
						if ($elem['name'] != 'REQUIRED' || empty($elem['attrs']['EMPTY'])
							|| $elem['attrs']['EMPTY'] != 'allowed'
						) {
							$member['notNull'] = true;
						} else {
							$member['notNullEmptyAllowed'] = true;
						}

						break;
					}
				}
			}

			$maps[$mapName][] = $member;
		}
	}
}

$smarty->assign('maps', $maps);
$smarty->assign('mapName', $mapName);
$new = $smarty->fetch('maps.tpl');

// Windows leaves a CR at the end of the file
$new = rtrim($new, "\r");

$fd = fopen('Maps.inc', 'w');
fwrite($fd, $new);
fclose($fd);

// Done
cleanExit(0);

function cleanExit($status = 0) {
	// Clean up the cheap and easy way
	global $tmpdir;

	if (file_exists($tmpdir)) {
		system("rm -rf $tmpdir");
	}

	exit($status);
}
