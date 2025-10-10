#!/usr/bin/php -q
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
 *ten You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */
if (!function_exists('file_put_contents')) {
	// Define file_put_contents if running PHP 4.x
	function file_put_contents($path, $data) {
		if (false === ($file = fopen($path, 'w')) || false === fwrite($file, $data)) {
			return false;
		}
		fclose($file);

		return true;
	}
}

// Prepare the environment if the script was run directly from command line
if (empty($SRCDIR)) {
	ini_set('error_reporting', 2047);

	if (!empty($_SERVER['SERVER_NAME'])) {
		die("You must run this from the command line\n");
	}

	$quiet = false;
	$path  = '';
	array_shift($argv);

	for ($i = 0; $i < count($argv); $i++) {
		if ($argv[$i] === '-q') {
			$quiet = true;
		} elseif ($argv[$i] === '-p') {
			$path = $argv[++$i];

			if (!file_exists($path)) {
				die("The directory '$path' does not exist");
			}

			if (!is_dir($path)) {
				die("The specified path ('$path') is not a directory");
			}

			if (!preg_match('#^(modules|themes)(/\w+)?/?$#', $path)) {
				die("The path '$path' must be a relative path to a plugin (e.g. modules/core)");
			}
		}
	}

	/**
	 * If quiet mode is not enabled, display the message on standard out.
	 * @param string $message Message to display.
	 */
	function quiet_print($message) {
		global $quiet;

		if (!$quiet) {
			echo "$message\n";
		}
	}

	makeManifest($path);
}

function makeManifest($filterPath = '') {
	global $SRCDIR;
	$startTime = time();

	if (empty($SRCDIR)) {
		// Current working directory must be gallery2 folder
		if (!file_exists('modules') && !file_exists('themes')) {
			$baseDir = dirname(dirname(dirname(__DIR__))) . '/';
			chdir($baseDir);
		} else {
			$baseDir = getcwd();
		}
	} else {
		$baseDir = $SRCDIR . '/gallery2/';
		chdir($baseDir);
	}
	// Just so we are consistent lets standardize on Unix path sepearators
	$baseDir = str_replace('\\', '/', $baseDir);
	$baseDir .= substr($baseDir, -1) == '/' ? '' : '/';

	quiet_print('Finding all files...');
	$entries = listSvn($filterPath);

	quiet_print('Sorting...');
	sort($entries);

	// Split into sections
	$sections = array();
	quiet_print('Separating into sections...');

	foreach ($entries as $file) {
		$matches = array();

		if (preg_match('#((modules|layouts|themes)[\\/].*?)[\\/]#', $file, $matches) !== 0) {
			$sections["{$matches[1]}/MANIFEST"][] = $file;
		} else {
			$sections['MANIFEST'][] = $file;
		}
	}

	// Now generate the checksum files
	quiet_print('Generating checksums...');
	$changed = 0;
	$total   = 0;

	foreach ($sections as $manifest => $entries) {
		if (!file_exists($baseDir . $manifest)) {
			$oldLines   = array();
			$oldContent = $oldRevision = '';
			$nl         = DIRECTORY_SEPARATOR == '\\' ? "\r\n" : "\n";
		} else {
			$oldLines    = file($baseDir . $manifest);
			$oldContent  = implode('', $oldLines);
			$nl          = preg_match('/\r\n/', $oldContent) ? "\r\n" : "\n";
			$matches     = array();
			$oldRevision = preg_match('/Revision: (\d+\s*)\$/', $oldLines[0], $matches) ? $matches[1] : '';
		}

		$newContent = '# $Revi' . "sion: $oldRevision\$$nl";
		$newContent .= "# File crc32 crc32(crlf) size size(crlf)  or  R File$nl";

		$deleted = $seen = array();

		foreach ($entries as $entry) {
			list($file, $isBinary) = preg_split('/\@\@/', $entry);
			$relativeFilePath      = $file;
			$file                  = $baseDir . $file;

			if (preg_match('/deleted:(.*)/', $relativeFilePath, $matches)) {
				$deleted[$matches[1]] = true;
			} else {
				$seen[$relativeFilePath] = true;
				$fileHandle              = fopen($file, 'rb');
				$fileSize                = filesize($file);
				$data                    = fread($fileHandle, $fileSize);
				fclose($fileHandle);

				$data_crlf = $data;

				if ($isBinary) {
					$size = $size_crlf = filesize($file);
				} else {
					if (preg_match("/\r\n/", $data)) {
						$data = str_replace("\r\n", "\n", $data);
					} else {
						$data_crlf = str_replace("\n", "\r\n", $data_crlf);
					}
					$size      = strlen($data);
					$size_crlf = strlen($data_crlf);
				}

				$cksum      = crc32($data);
				$cksum_crlf = crc32($data_crlf);
				$newContent .= sprintf(
					"$relativeFilePath\t%u\t%u\t%d\t%d$nl",
					$cksum,
					$cksum_crlf,
					$size,
					$size_crlf
				);
			}
		}

		if (!empty($oldLines)) {
			foreach ($oldLines as $line) {
				if ($line[0] == '#') {
					continue;
				}

				if (preg_match('/^R\t(.*)$/', $line, $matches)) {
					$file = trim($matches[1]);

					if (empty($seen[$file])) {
						$deleted[$file] = true;
					}
				} else {
					preg_match('/^(.+?)\t/', $line, $matches);
					$file = trim($matches[1]);

					if (empty($seen[$file])) {
						$deleted[$file] = true;
					}
				}
			}

			foreach ($deleted as $file => $unused) {
				$newContent .= "R\t$file$nl";
			}
		}

		if ($oldContent != $newContent) {
			file_put_contents($baseDir . $manifest, $newContent);
			$changed++;
		}
		$total++;
	}

	quiet_print(sprintf('Completed in %d seconds', time() - $startTime));
	quiet_print(sprintf("Manifests changed: $changed (total: $total)"));
}

/**
 * Retrieve the SVN Entries
 * @param string $filterpath Path to create retrieve the SVN entries for.
 * @return array List of SVN entries
 */
function listSvn($filterpath) {
	$entries = array();

	$binaryList = array();
	exec("svn propget --non-interactive -R svn:mime-type $filterpath", $output);

	foreach ($output as $line) {
		$parts             = preg_split('/\s-\s/', $line);
		$file              = str_replace('\\', '/', $parts[0]);
		$binaryList[$file] = 1;
	}

	$output = array();
	exec("svn status --non-interactive -v -q $filterpath", $output);

	foreach ($output as $line) {
		$matches = array();

		if (preg_match('/^(.).....\s*\d+\s+[\d|\?]+\s+\S+\s+(.*)$/', $line, $matches) == 0) {
			die("Unexpected SVN status format:\n$line\n");
		}

		if (!file_exists($matches[2])) {
			die("The file '$matches[2]' does not exist");
		}

		if (is_dir($matches[2])) {
			continue;
		}

		if (preg_match('#[\\/]MANIFEST#', $matches[2]) > 0) {
			continue;
		}

		if ($matches[1] == 'M') {
			quiet_print("Warning: $matches[2] is locally modified");
		} elseif (!in_array($matches[1], array(' ', 'D', 'M'))) {
			die("Check {$matches[1]} status for {$matches[2]}");
		}

		$status = $matches[1] === 'D' ? 'deleted:' : '';

		$file      = str_replace('\\', '/', $matches[2]);
		$entries[] = sprintf('%s%s@@%d', $status, $file, isset($binaryList[$file]));
	}

	return $entries;
}

?>
