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

$tmpdir = __DIR__ . '/tmp_dbxml_' . mt_rand(1, 30000);

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

function generateEntityDbXml() {
	global $smarty;

	$entityXmlFiles = glob('tmp/classxml/*.xml');

	if (empty($entityXmlFiles)) {
		return;
	}

	foreach ($entityXmlFiles as $xmlFile) {
		$p        = new XmlParser();
		$root     = $p->parse($xmlFile);
		$base     = basename($xmlFile);
		$base     = preg_replace('/\.[^\.]*$/', '', $base);
		$tmpFile  = "tmp/dbxml/$base.xml";
		$origFile = "$base.xml";

		$membersBase = $root[0]['child'];
		$schema      = array(
			'name'  => $root[0]['child'][2]['child'][0]['content'],
			'major' => $root[0]['child'][2]['child'][1]['content'],
			'minor' => (!empty($root[0]['child'][2]['child'][2]['content']) ? $root[0]['child'][2]['child'][2]['content'] : 0),
		);

		$members    = array();
		$keys       = array();
		$indexes    = array();
		$requiresId = false;

		foreach ($membersBase as $child) {
			switch ($child['name']) {
				case 'MEMBER':
					$member = array(
						'name'   => $child['child'][0]['content'],
						'type'   => $child['child'][1]['content'],
						'ucName' => ucfirst($child['child'][0]['content']),
						'lcType' => strtolower($child['child'][1]['content']),
					);

					for ($i = 2; $i < count($child['child']); $i++) {
						switch ($child['child'][$i]['name']) {
							case 'MEMBER-SIZE':
											$member['size'] = $child['child'][$i]['content'];

								break;

							case 'INDEXED':
								$indexes[]                                           = array(
									'columns' => array($member['name']),
								);
									$member[strtolower($child['child'][$i]['name'])] = 1;

								break;

							case 'UNIQUE':
								$keys[]                                          = array(
									'columns' => array($member['name']),
								);
								$member[strtolower($child['child'][$i]['name'])] = 1;

								break;

							case 'PRIMARY':
								$keys[]            = array(
									'columns' => array($member['name']),
									'primary' => 1,
								);
								$member['primary'] = 1;

								break;

							case 'ID':
							case 'LINKED':
								$member[strtolower($child['child'][$i]['name'])] = 1;

								break;

							case 'REQUIRED':
								$member['required'] = array();

								if (isset($child['child'][$i]['attrs']['EMPTY'])) {
									$member['required']['empty'] = $child['child'][$i]['attrs']['EMPTY'];
								} else {
									$member['required']['empty'] = 'disallowed';
								}

								break;

							case 'DEFAULT':
								$member['default'] = $child['child'][$i]['content'];

								break;

							case 'MEMBER-EXTERNAL-ACCESS':
								// Not relevant for storage layer
								break;

							default:
								print 'Unknown member type: ' . $child['child'][$i]['name'] . '\n';
						}
					}

					if (empty($member['size'])) {
						$member['size'] = 'MEDIUM';
					}

					$members[] = $member;

					break;

				case 'KEY':
					$key = array();

					foreach ($child['child'] as $column) {
						$key['columns'][] = $column['content'];
					}
					$key['primary'] = isset($child['attrs']['PRIMARY']) && $child['attrs']['PRIMARY'] == 'true';
					$keys[]         = $key;

					break;

				case 'INDEX':
					$index = array();

					foreach ($child['child'] as $column) {
						$index['columns'][] = $column['content'];
					}
					$index['primary'] = isset($child['attrs']['PRIMARY']) && $child['attrs']['PRIMARY'] == 'true';

					$indexes[] = $index;

					break;

				case 'REQUIRES-ID':
					$requiresId = true;
					$keys[]     = array(
						'columns' => array('id'),
						'primary' => 1,
					);

					break;
			}
		}

		$smarty->assign('root', $root);
		$smarty->assign('schema', $schema);
		$smarty->assign('members', $members);
		$smarty->assign('keys', $keys);
		$smarty->assign('indexes', $indexes);
		$smarty->assign('requiresId', $requiresId);
		$smarty->assign('isMap', false);
		$new = $smarty->fetch('dbxml.tpl');

		$fd = fopen($tmpFile, 'w');
		fwrite($fd, $new);
		fclose($fd);
	}
}

function generateMapDbXml() {
	global $smarty;

	$xmlFile = '../Maps.xml';

	if (!file_exists($xmlFile)) {
		return;
	}

	$p        = new XmlParser();
	$root     = $p->parse($xmlFile);
	$base     = basename($xmlFile);
	$base     = preg_replace('/\.[^\.]*$/', '', $base);
	$origFile = "$base.xml";

	foreach ($root[0]['child'] as $map) {
		$origMapName = $map['child'][0]['content'];

		/*
		 * NOTE!  Keep this in sync with the similar block in extractClassXml.pl
		 * and generate-entities.php
		 */
		$mapName = $origMapName;
		$mapName = preg_replace('/^Gallery/', '', $mapName);
		// Shorten some table names to fit Oracle's 30 char name limit..
		$mapName = str_replace('Preferences', 'Prefs', $mapName);
		$mapName = str_replace('Toolkit', 'Tk', $mapName);
		$mapName = str_replace('TkOperation', 'TkOperatn', $mapName);

		$schema  = array(
			'name'  => $mapName,
			'major' => $map['child'][1]['child'][0]['content'],
			'minor' => $map['child'][1]['child'][1]['content'],
		);
		$tmpFile = "tmp/dbxml/$origMapName.xml";

		$members    = array();
		$keys       = array();
		$indexes    = array();
		$requiresId = false;

		for ($j = 2; $j < count($map['child']); $j++) {
			$child = $map['child'][$j];

			switch ($child['name']) {
				case 'MEMBER':
					$member = array(
						'name'   => $child['child'][0]['content'],
						'type'   => $child['child'][1]['content'],
						'ucName' => ucfirst($child['child'][0]['content']),
						'lcType' => strtolower($child['child'][1]['content']),
					);

					for ($i = 2; $i < count($child['child']); $i++) {
						switch ($child['child'][$i]['name']) {
							case 'MEMBER-SIZE':
											$member['size'] = $child['child'][$i]['content'];

								break;

							case 'INDEXED':
								$indexes[]                                           = array(
									'columns' => array($member['name']),
								);
									$member[strtolower($child['child'][$i]['name'])] = 1;

								break;

							case 'UNIQUE':
								$keys[]                                          = array(
									'columns' => array($member['name']),
								);
								$member[strtolower($child['child'][$i]['name'])] = 1;

								break;

							case 'PRIMARY':
								$keys[]            = array(
									'columns' => array($member['name']),
									'primary' => 1,
								);
								$member['primary'] = 1;

								break;

							case 'REQUIRED':
								$member['required'] = array();

								if (isset($child['child'][$i]['attrs']['EMPTY'])) {
									$member['required']['empty'] = $child['child'][$i]['attrs']['EMPTY'];
								} else {
									$member['required']['empty'] = 'disallowed';
								}

								break;

							case 'DEFAULT':
								$member['default'] = $child['child'][$i]['content'];

								break;

							case 'MEMBER-EXTERNAL-ACCESS':
								// Not relevant for storage layer
								break;

							default:
								print 'Unknown member type: ' . $child['child'][$i]['name'] . '\n';
						}
					}

					if (empty($member['size'])) {
						$member['size'] = 'MEDIUM';
					}

					$members[] = $member;

					break;

				case 'KEY':
					$key = array();

					foreach ($child['child'] as $column) {
						$key['columns'][] = $column['content'];
					}
					$key['primary'] = isset($child['attrs']['PRIMARY']) && $child['attrs']['PRIMARY'] == 'true';
					$keys[]         = $key;

					break;

				case 'INDEX':
					$index = array();

					foreach ($child['child'] as $column) {
						$index['columns'][] = $column['content'];
					}
					$index['primary'] = isset($child['attrs']['PRIMARY']) && $child['attrs']['PRIMARY'] == 'true';

					$indexes[] = $index;

					break;
			}

			$smarty->assign('root', $root);
			$smarty->assign('schema', $schema);
			$smarty->assign('members', $members);
			$smarty->assign('keys', $keys);
			$smarty->assign('indexes', $indexes);
			$smarty->assign('requiresId', $requiresId);
			$smarty->assign('isMap', true);
			$new = $smarty->fetch('dbxml.tpl');

			$fd = fopen($tmpFile, 'w');
			fwrite($fd, $new);
			fclose($fd);
		}
	}
}

generateEntityDbXml();
generateMapDbXml();

// Clean up the cheap and easy way
if (file_exists($tmpdir)) {
	system("rm -rf $tmpdir");
}
