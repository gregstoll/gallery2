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

$tmpdir = __DIR__ . '/tmp_entities_' . mt_rand(1, 30000);

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

// Grab all G2 XML from entity class files

$xml  = '<!DOCTYPE classes SYSTEM "' .
	"../../../../lib/tools/dtd/GalleryClass2.1.dtd\">\n";
$xml .= "<classes>\n";

if (!$dh = opendir('.')) {
	echo "Unable to opendir(.)\n";
	cleanExit(1);
}

$files = array();

while (($file = readdir($dh)) !== false) {
	if (preg_match('/\.class$/', $file)) {
		$files[] = $file;
	}
}
closedir($dh);
sort($files);
$classXml = '';

foreach ($files as $file) {
	$snippet = getXml($file);

	if ($snippet) {
		$classXml .= "<class>\n" . join("\n", $snippet) . "\n</class>\n";
	}
}

if (empty($classXml)) {
	// Nothing to do
	cleanExit(0);
}

$xml .= $classXml;
$xml .= "</classes>\n";

$entitiesXml = "$tmpdir/Entities.xml";

if (!$fp = fopen($entitiesXml, 'wb')) {
	echo "Unable to write to $entitiesXml\n";
	cleanExit(1);
}
fwrite($fp, $xml);
fclose($fp);

if (system("xmllint --valid --noout $entitiesXml", $retval)) {
	echo "System error: $retval\n";
	cleanExit();
}

$p    = new XmlParser();
$root = $p->parse($entitiesXml);

$entities = array();

foreach ($root[0]['child'] as $entity) {
	$entityName       = $entity['child'][0]['content'];
	$parentEntityName = $entity['child'][1]['content'];

	$j = 3;

	if ($entity['child'][$j]['name'] == 'REQUIRES-ID') {
		$j++;
	}

	$entities[$entityName]['members'] = array();
	$entities[$entityName]['linked']  = array();

	for (; $j < count($entity['child']); $j++) {
		$member = $entity['child'][$j];
		$name   = $member['child'][0]['content'];

		$entities[$entityName]['members'][$name]['type'] = 'STORAGE_TYPE_' . $member['child'][1]['content'];
		$entities[$entityName]['members'][$name]['type'] = 'STORAGE_TYPE_' . $member['child'][1]['content'];

		for ($k = 2; $k < count($member['child']); $k++) {
			if (!empty($member['child'][$k]['name'])) {
				switch ($member['child'][$k]['name']) {
					case 'MEMBER-SIZE':
						$entities[$entityName]['members'][$name]['size'] = $size = 'STORAGE_SIZE_' . $member['child'][$k]['content'];

						break;

					case 'ID':
						$entities[$entityName]['members'][$name]['type'] .= '| STORAGE_TYPE_ID';

						break;

					case 'LINKED':
						$entities[$entityName]['linked'][] = $name;

						break;

					case 'REQUIRED':
					case 'PRIMARY':
						$elem = $member['child'][$k];

						if ($elem['name'] != 'REQUIRED' || empty($elem['attrs']['EMPTY'])
							|| $elem['attrs']['EMPTY'] != 'allowed'
						) {
							$entities[$entityName]['members'][$name]['notNull'] = true;
						} else {
							$entities[$entityName]['members'][$name]['notNullEmptyAllowed'] = true;
						}

						break;

					case 'MEMBER-EXTERNAL-ACCESS':
						switch (trim($member['child'][$k]['content'])) {
							case 'READ':
								$entities[$entityName]['members'][$name]['external-access'] = 'EXTERNAL_ACCESS_READ';

								break;

							case 'WRITE':
								$entities[$entityName]['members'][$name]['external-access'] = 'EXTERNAL_ACCESS_WRITE';

								break;

							case 'FULL':
								$entities[$entityName]['members'][$name]['external-access'] = 'EXTERNAL_ACCESS_FULL';

								break;

							default:
								printf(
									'Unknown value for member-external-access "%s"\n',
									$member['child'][$k]['content']
								);
						}

						break;
				}
			}
		}
	}

	$entities[$entityName]['parent'] = $parentEntityName;
	$entities[$entityName]['module'] = basename(dirname(realpath('.')));
}

$smarty->assign('entities', $entities);
$new = $smarty->fetch('entities.tpl');

// Windows leaves a CR at the end of the file
$new = rtrim($new, "\r");

$fd = fopen('Entities.inc', 'w');
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

function getXml($filename) {
	$results = array();

	if ($fp = fopen($filename, 'rb')) {
		while (!feof($fp)) {
			$line = fgets($fp, 4096);

			if (preg_match('/@g2(.*)/', $line, $matches)) {
				$results[] = $line = $matches[1];

				/*
				 * NOTE!  Keep this in sync with the similar block in extractClassXml.pl
				 * and generate-dbxml.php
				 */
				if (preg_match('{<class-name>(.*)</class-name>}', $line, $matches)) {
					$schemaName = $matches[1];
					$schemaName = preg_replace('/^Gallery/', '', $schemaName);
					// Shorten some table names to fit Oracle's 30 char name limit..
					$schemaName = preg_replace('/Preferences/', 'Prefs', $schemaName);
					$schemaName = preg_replace('/Toolkit/', 'Tk', $schemaName);
					$schemaName = preg_replace('/TkOperation/', 'TkOperatn', $schemaName);
				}

				if (preg_match('{<schema>}', $line)) {
					$results[] = "   <schema-name>$schemaName</schema-name>";
				}
			}
		}
		fclose($fp);
	}

	return $results;
}
