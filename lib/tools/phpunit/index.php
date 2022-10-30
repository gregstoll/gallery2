<?php

/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

/**
 * Script for running unit tests
 * @package Gallery
 * @subpackage PHPUnit
 */
define('G2_SUPPORT_URL_FRAGMENT', '../../support/');

require '../../support/security.inc';
require '../../../bootstrap.inc';
require_once '../../../init.inc';

@date_default_timezone_set(date_default_timezone_get());
$testReportDir = $gallery->getConfig('data.gallery.base') . 'test/';
$priorRuns     = array();
$glob          = glob("${testReportDir}*");

if ($glob) {
	foreach ($glob as $filename) {
		if (preg_match('/run-(\d+).html$/', $filename, $matches)) {
			$priorRuns[] = array(
				'key'  => $matches[1],
				'size' => filesize($filename),
				'date' => strftime('%Y/%m/%d-%H:%M:%S', filectime($filename)),
			);
		}
	}
}

if (!empty($_GET['run'])) {
	list($action, $run) = explode(':', $_GET['run']);

	$run     = substr($run, 0, strspn($run, '0123456789'));
	$runFile = "${testReportDir}run-$run.html";

	switch ($action) {
		case 'frame':
			include __DIR__ . '/runframe.tpl';

			exit;

		case 'show':
			if (file_exists($runFile)) {
				readfile($runFile);
			} else {
				echo "<H1>No prior run with id $run</H1>";
			}

			exit;

		case 'deleteall':
			foreach ($priorRuns as $pr) {
				unlink("${testReportDir}run-$pr[key].html");
			}

			header('Location: index.php');

			exit;

		break;

		case 'delete':
			if (file_exists($runFile)) {
				unlink($runFile);
			}

			header('Location: index.php');

			break;
	}
}

/**
 * Load up main.php so that tests that want _GalleryMain can get to it.  Do it now, though so that
 * we do not mangle the $gallery object during test runs.
 *
 * @todo figure out a way to only do this if we are going to run a test that wants to exercise
 *       _GalleryMain
 */
ob_start();

require_once '../../../main.php';

ob_end_clean();

/**
 *
 * This is an output interceptor that allows us to save the HTML output from our test run in the
 * g2data directory.  We only save the output when there is a filter value set which indicates that
 * there is an actual test run in progress.
 */
function PhpUnitOutputInterceptor($message) {
	global $gallery,  $testReportDir;

	if (isset($_GET['filter'])) {
		if (!file_exists($testReportDir)) {
			mkdir($testReportDir);
		}

		static $fd;

		if (!isset($fd)) {
			$fd = fopen("${testReportDir}run-" . date('YmdHis') . '.html', 'wb+');
		}

		static $replaced;

		if (empty($replaced) && strstr($message, '<!--base_href-->')) {
			$replaced = true;
			fwrite(
				$fd,
				str_replace(
					'<!--base_href-->',
					@sprintf(
						'<base href="http://%s:%d/%s/">',
						$_SERVER['HTTP_HOST'],
						$_SERVER['SERVER_PORT'],
						dirname($_SERVER['SCRIPT_NAME'])
					),
					$message
				)
			);
		} else {
			fwrite($fd, $message);
		}
	}

	return $message;
}

@ini_set('output_buffering', 0);
ob_start('PhpUnitOutputInterceptor', 256);

require_once 'phpunit.inc';
require_once 'GalleryTestCase.class';
require_once 'GalleryImmediateViewTestCase.class';
require_once 'GalleryControllerTestCase.class';
require_once 'GalleryViewTestCase.class';
require_once 'ItemAddPluginTestCase.class';
require_once 'ItemEditPluginTestCase.class';
require_once 'ItemEditOptionTestCase.class';
require_once 'CodeAuditTestCase.class';
require_once 'MockObject.class';
require_once 'UnitTestPlatform.class';
require_once 'UnitTestStorage.class';
require_once 'UnitTestPhpVm.class';
require_once 'UnitTestUrlGenerator.class';
require_once 'MockTemplateAdapter.class';
require_once 'UnitTestTemplate.class';
require_once 'UnitTestRepository.class';
require_once 'UnitTestRepositoryUtilities.class';

function PhpUnitGalleryMain(&$testSuite, $filter) {
	$ret = GalleryInitFirstPass();

	if ($ret) {
		return $ret;
	}

	$ret = GalleryInitSecondPass();

	if ($ret) {
		return $ret;
	}

	// Set the appropriate charset in our HTTP header
	if (!headers_sent()) {
		header('Content-Type: text/html; charset=UTF-8');
	}

	global $gallery;

	// Configure our url Generator, find the correct base URL
	$urlGenerator = new GalleryUrlGenerator();

	$ret = $urlGenerator->init('index.php');

	if ($ret) {
		return $ret;
	}

	$urlDir       = str_replace('lib/tools/phpunit/', '', $urlGenerator->getCurrentUrlDir());
	$path         = substr($urlDir, strlen($urlGenerator->makeUrl('/')) - 1);
	$urlGenerator = new GalleryUrlGenerator();

	$ret = $urlGenerator->init($path . GALLERY_MAIN_PHP);

	if ($ret) {
		return $ret;
	}

	$gallery->setUrlGenerator($urlGenerator);

	/*
	 * Commit our transaction here because we are going to have a new
	 * transaction for every test.
	 */
	$storage =& $gallery->getStorage();

	$ret = $storage->commitTransaction();

	if ($ret) {
		return $ret;
	}

	/*
	 * Use assertUserIsSiteAdministrator instead of isUserInSiteAdminGroup to make sure the admin
	 * session has not expired.
	 */
	$ret = GalleryCoreApi::assertUserIsSiteAdministrator();

	if ($ret && ($ret->getErrorCode() & ERROR_PERMISSION_DENIED)) {
		$isSiteAdmin = false;
	} elseif ($ret) {
		return $ret;
	} else {
		$isSiteAdmin = true;
	}

	if ($isSiteAdmin && $filter !== false) {
		// Load the test cases for every active module.
		list($ret, $moduleStatusList) = GalleryCoreApi::fetchPluginStatus('module');

		if ($ret) {
			return $ret;
		}

		$suiteArray = array();

		$gallery->guaranteeTimeLimit(120);

		foreach ($moduleStatusList as $moduleId => $moduleStatus) {
			$modulesDir = GalleryCoreApi::getCodeBasePath('modules/');

			if (empty($moduleStatus['active'])) {
				continue;
			}

			$testDir     = $modulesDir . $moduleId . '/test/phpunit';
			$suiteArray += loadTests($moduleId, $testDir, $filter);
		}

		// Add repository tools tests.
		$suiteArray += loadTests(
			'repositorytools',
			__DIR__ . '/../repository/test/phpunit',
			$filter
		);

		$keys = array_keys($suiteArray);
		natcasesort($keys);

		foreach ($keys as $className) {
			$testSuite->addTest($suiteArray[$className]);
		}
	}

	return null;
}

function loadTests($moduleId, $testDir, $filter) {
	global $gallery;

	$moduleArray = array();
	$platform    =& $gallery->getPlatform();

	if ($platform->file_exists($testDir)
		&& $platform->is_dir($testDir)
		&& $dir = $platform->opendir($testDir)
	) {
		if (empty($filter)) {
			$filterRegexp = '.*';
		} else {
			$filterRegexp = $filter;
		}

		while (($file = $platform->readdir($dir)) != false) {
			if (preg_match('/(.*Test).class$/', $file, $matches)) {
				if (!strncmp($matches[1], '.#', 2)) {
					// Ignore Emacs backup files
					continue;
				}

				include_once $testDir . '/' . $file;

				$className = $matches[1];

				if (class_exists($className)
					&& GalleryUtilities::isA(new $className(null), 'GalleryTestCase')
				) {
					$moduleArray[$className] = new TestSuite($className, $moduleId, $filterRegexp);
				}
			}
		}

		$platform->closedir($dir);
	}

	return $moduleArray;
}

class GalleryTestResult extends TestResult {
	public $_totalElapsed        = 0;
	public $_testsFailed         = 0;
	public $_testsRunThenSkipped = 0;

	public function __construct() {
		parent::__construct();
	}

	public function report() {
		// report result of test run
		global $compactView;

		$nRun      = $this->countTests();
		$nFailures = $this->failureCount();
		echo '<script text="text/javascript">completeStatus();</script>';

		if ($nFailures) {
			echo "</ol>\n";
		}

		if (!isset($compactView)) {
			echo '<script type="text/javascript">';
			echo 'function setTxt(i,t) { document.getElementById(i).firstChild.nodeValue=t; }';
			printf("setTxt('testTime','%2.4f');", $this->_totalElapsed);
			printf("setTxt('testCount','%s test%s');", $nRun, ($nRun == 1) ? '' : 's');

			if ($this->_testsRunThenSkipped) {
				printf(
					"setTxt('runThenSkip',' (of those, %s skipped)');",
					$this->_testsRunThenSkipped
				);
			}

			printf(
				"setTxt('testFailCount','%s test%s');",
				$this->_testsFailed,
				($this->_testsFailed == 1) ? '' : 's'
			);

			printf(
				"setTxt('testErrorCount','%s error%s');",
				$nFailures,
				($nFailures == 1) ? '' : 's'
			);

			printf("setTxt('testReport', '%s');", $this->_getTestResultRecord());
			printf('setUsername("NAME_PLACEHOLDER", getUsernameFromCookie());');
			echo "document.getElementById('testSummary').style.display='block';</script>\n";
		}

		if ($nFailures == 0) {
			return;
		}

		$failures  = $this->getFailures();
		$newFilter = array();

		foreach ($failures as $failure) {
			$newFilter[$failure->getClassName() . '.' . $failure->getTestName()] = 1;
		}

		printf(
			'<script type="text/javascript">var failedTestFilter="(%s)$";%s</script>',
			implode('|', array_keys($newFilter)),
			"document.getElementById('runBrokenButton').style.display='block';"
		);
	}

	public function _getTestResultRecord() {
		global $gallery;

		$storage    =& $gallery->getStorage();
		$translator =& $gallery->getTranslator();

		list($ret, $params) = GalleryCoreApi::fetchAllPluginParameters('module', 'core');

		if ($ret) {
			return $ret->getAsHtml();
		}

		list($ret, $moduleStatusList) = GalleryCoreApi::fetchPluginStatus('module');

		if ($ret) {
			return $ret->getAsHtml();
		}

		$notes = array();

		foreach ($moduleStatusList as $moduleId => $moduleStatus) {
			if ($moduleId == 'multiroot') {
				// This module is never active
				continue;
			}

			if (empty($moduleStatus['active'])) {
				$notes[] = "-$moduleId";
			}
		}

		$ourUName      = php_uname();
		$explodedUName = explode(' ', $ourUName);
		$webserver     = GalleryUtilities::getServerVar('SERVER_SOFTWARE');
		$php           = 'PHP ' . phpversion();
		$database      = $storage->getAdoDbType() . ' ' . $storage->getVersion();
		$OS            = array_shift($explodedUName);
		$locking       = $params['lock.system'];
		$language      = $translator->_languageCode;
		$owner         = 'NAME_PLACEHOLDER';
		$count         = $this->countTests();
		$failed        = $this->_testsFailed;
		$date          = date('Y-m-d', time());
		$buf           = sprintf(
			'|%s||%s||%s||%s||%s||%s||%s||%d||%d||%s||%s',
			$webserver,
			$php,
			$database,
			$OS,
			$locking,
			$language,
			$owner,
			$count,
			$failed,
			$date,
			join(' ', $notes)
		);

		return $buf;
	}

	public function _startTest($test) {
		if ($this->fRunTests == 1) {
			echo '<script text="text/javascript">showStatus();</script>';
		}

		printf('<script type="text/javascript">runningTest("%s");</script>', $test->name());
		flush();
	}

	public function _endTest(&$test) {
		$failure    = '';
		$extra      = '';
		$usedMemory = (function_exists('memory_get_usage')) ? memory_get_usage() : '"unknown"';

		if ($test->wasSkipped()) {
			global $compactView;

			if (isset($compactView)) {
				return;
			}

			$class   = 'Skipped';
			$text    = 'r.cells[4].lastChild.nodeValue="SKIPPED";';
			$extra   = 'r.className="skip";';
			$elapsed = '0.0000';
			$cmd     = "updateStats(0, 0, 1, $usedMemory)";

			if (!empty($test->fLifeCycle['setUp'])) {
				// Test was started, then test skipped itself
				$this->_testsRunThenSkipped++;
			}
		} else {
			$elapsed              = sprintf('%2.4f', $test->elapsed());
			$this->_totalElapsed += $elapsed;

			if ($test->failed()) {
				$class    = 'Failure';
				$text     = 'r.cells[4].firstChild.style.display="inline";';
				$failure  = $this->_testsFailed++ ? '' : '<h2>Failure Details</h2><ol>';
				$failure .= '<li><a href="?filter=' .
				urlencode(
					'^' . $test->getModuleId() . '.' . $test->classname() . '.' .
					$test->name() . '$'
				) .
				'" name="fail' . $this->fRunTests . '">' . $test->classname() . '.' .
				$test->name() . "</a></li><ul>\n";

				foreach ($test->getExceptions() as $exception) {
					$failure .= '<li>' . $exception->getMessage() . "</li>\n";
				}

				$failure .= "</ul>\n";
				$cmd      = "updateStats(0, 1, 0, $usedMemory)";
			} else {
				$class = 'Pass';
				$text  = 'r.cells[4].lastChild.nodeValue="PASSED";';

				global $testOneByOne;

				if (isset($testOneByOne)) {
					$i = $testOneByOne + 1;
					$x = substr($_GET['filter'], 0, strrpos($_GET['filter'], ':') + 1);
					echo '<meta http-equiv="refresh" content="0; index.php?filter=' .
					"$x$i-$i" . '&amp;onebyone=true"/>';
				}

				$cmd = "updateStats(1, 0, 0, $usedMemory)";
			}
		}

		echo '<script type="text/javascript">r=document.getElementById(\'testRow'
		. $this->fRunTests . "');$extra";
		echo "r.cells[4].className='$class';$text";
		echo "r.cells[5].firstChild.nodeValue='$elapsed';$cmd;</script>\n$failure";
		flush();
	}
}

define('FILTER_MAX', 1000000);

if (isset($_GET['filter'])) {
	$filter = trim($_GET['filter']);

	if (substr($filter, -5) == ':1by1') {
		$testOneByOne   = $compactView   = 1;
		$_GET['filter'] = $filter = substr($filter, 0, -3) . '-1';
	} elseif (!empty($_GET['onebyone'])) {
		$testOneByOne = $compactView = (int)substr($filter, strrpos($filter, '-') + 1);
	}

	$range = array();
	$skip  = explode(',', $filter);

	foreach ($skip as $tempSkip) {
		if (preg_match('/(\d+)-(\d+)/', $tempSkip, $matches)) {
			if ($matches[1] >= 1 && $matches[1] <= FILTER_MAX
				&& $matches[2] >= 1 && $matches[2] <= FILTER_MAX
			) {
				$range[] = array($matches[1], $matches[2]);
				$filter  = trim(preg_replace('/:?\d+-\d+,?/', '', $filter, 1));
			}
		} elseif (preg_match('/(\d+)-/', $filter, $matches)) {
			if ($matches[1] >= 1 && $matches[1] <= FILTER_MAX) {
				$range[] = array($matches[1], FILTER_MAX);
				$filter  = trim(preg_replace('/:?\d+-,?/', '', $filter, 1));
			}
		} elseif (preg_match('/-(\d+)/', $filter, $matches)) {
			if ($matches[1] >= 1 && $matches[1] <= FILTER_MAX) {
				$range[] = array(1, $matches[1]);
				$filter  = preg_replace('/:?-\d+,?/', '', $filter, 1);
			}
		}
	}

	$displayFilter = $filter;

	if (count($range) == 0) {
		$range[] = array(1, FILTER_MAX);
	}

	for ($j = 0; $j < count($range); $j++) {
		if ($j == 0 && $j == (count($range) - 1)) {
			if ($range[$j][0] != 1 || $range[$j][1] != FILTER_MAX) {
				$displayFilter .= sprintf(':%d-%d', $range[$j][0], $range[$j][1]);
			}
		} elseif ($j == 0) {
			$displayFilter .= sprintf(':%d-%d,', $range[$j][0], $range[$j][1]);
		} elseif ($j == (count($range) - 1)) {
			$displayFilter .= sprintf('%d-%d', $range[$j][0], $range[$j][1]);
		} else {
			$displayFilter .= sprintf('%d-%d,', $range[$j][0], $range[$j][1]);
		}
	}
} else {
	$filter        = false;
	$displayFilter = null;
	$range         = array(array(1, FILTER_MAX));
}

$testSuite = new TestSuite();

$ret = PhpUnitGalleryMain($testSuite, $filter);

if ($ret) {
	$ret = $ret;

	echo $ret->getAsHtml();

	echo $gallery->getDebugBuffer();

	return;
}

list($ret, $moduleStatusList) = GalleryCoreApi::fetchPluginStatus('module');

if ($ret) {
	$ret = $ret;

	echo $ret->getAsHtml();

	return;
}

$session = $gallery->getSession();

if (!$session->isUsingCookies()) {
	$sessionKey = GALLERY_FORM_VARIABLE_PREFIX . $session->getKey();
	$sessionId  = $session->getId();
}

/*
 * Use assertUserIsSiteAdministrator instead of isUserInSiteAdminGroup to make sure the admin
 * session has not expired.
 */
$ret = GalleryCoreApi::assertUserIsSiteAdministrator();

if ($ret && ($ret->getErrorCode() & ERROR_PERMISSION_DENIED)) {
	$isSiteAdmin = false;
} elseif ($ret) {
	echo $ret->getAsHtml();

	return;
} else {
	$isSiteAdmin = true;
}

// Check that our dev environment is correct
$incorrectDevEnv       = array();
$desiredErrorReporting = E_ALL & ~E_STRICT;

foreach (array(
	'error_reporting'                => array($desiredErrorReporting),
	'allow_call_time_pass_reference' => array('off', 0),
	'register_globals'               => array('off', 0),
	'display_errors'                 => array('on', 1),
	'allow_url_fopen'                => array('off', 0),
	'include_path'                   => array('/bogus'),
) as $key => $expected) {
	$actual = ini_get($key);

	if (!in_array($actual, $expected)) {
		$incorrectDevEnv[$key] = array($expected, $actual);
	}
}

// Uncomment below to see debug output before tests run

/*
 * print "<pre>";
 * print $gallery->getDebugBuffer();
 * print "</pre>";
 */
require __DIR__ . '/index.tpl';

// Compact any ACLs that were created during this test run
if ($testSuite->countTestCases() > 0) {
	$ret = GalleryCoreApi::compactAccessLists();

	if ($ret) {
		echo $ret->getAsHtml();

		return;
	}
}

$storage =& $gallery->getStorage();

$ret = $storage->commitTransaction();

if ($ret) {
	echo $ret->getAsHtml();

	return;
}

ob_end_flush();
