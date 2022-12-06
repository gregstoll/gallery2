<?php

// PHP framework for testing, based on the design of "JUnit".
// Written by Fred Yankowski <fred@ontosys.com>
//            OntoSys, Inc  <http://www.OntoSys.com>
// $Id: phpunit.inc 20957 2009-12-16 04:57:07Z mindless $
// Copyright (c) 2000 Fred Yankowski
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
if (version_compare(phpversion(), '5.3', '>=')) {
	// Ignore these warnings for PHP 5.3 in Gallery 2.3
	error_reporting(E_ALL & ~E_STRICT);
} else {
	error_reporting(E_ALL);
}

/*
 * interface Test {
 *    function run(&$aTestResult);
 *    function countTestCases();
 * }
 */
function trace($msg) {
	return;

	echo $msg;
	flush();
}

function PHPUnit_error_handler($errno, $errstr, $errfile, $errline) {
	global $PHPUnit_testRunning;

	/*
	 * Smarty smarty_core_write_file does @unlink() on Windows platforms without doing a
	 * file_exists. Ignore these errors.
	 */
	if (error_reporting() == 0 && DIRECTORY_SEPARATOR == '\\') {
		global $gallery;

		if (isset($gallery)) {
			$templateDir = $gallery->getConfig('data.smarty.templates_c');

			if (strpos($errstr, 'unlink(' . $templateDir) !== false) {
				return;
			}
		}
	}

	$message = '<B>PHP ERROR:</B> ' . $errstr;

	if (function_exists('debug_backtrace')) {
		$traceSource = debug_backtrace();

		foreach ($traceSource as $traceEntry) {
			// Trace with call_user_func may miss file/line in an entry
			$buf = ' <B>in</B> ' . (isset($traceEntry['file']) ? $traceEntry['file'] : '??');

			if (isset($traceEntry['class'], $traceEntry['function'])) {
				$buf .= " ($traceEntry[class]::$traceEntry[function]) ";
			} elseif (isset($traceEntry['function'])) {
				$buf .= " ($traceEntry[function]) ";
			}

			$buf    .= ' <B>at line</B> ' . (isset($traceEntry['line']) ? $traceEntry['line'] : '??');
			$trace[] = $buf;
		}

		$message .= join('<BR>', $trace);
	} else {
		$message .= ' <B>in</B> ' . $errfile . ' <B>at line</B> ' . $errline;
	}

	$PHPUnit_testRunning->fail($message);
}

class PHPUnitException {
	// Emulate a Java exception, sort of...
	public $message;
	public $type;

	public function __construct($message, $type = 'FAILURE') {
		$this->message = $message;
		$this->type    = $type;
	}

	public function getMessage() {
		return $this->message;
	}

	public function getType() {
		return $this->type;
	}
}

/**
 * A set of assert methods.
 *
 * @category   Testing
 * @package    PHPUnit
 * @author     Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @copyright  2002-2007 Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @license    http://www.opensource.org/licenses/bsd-license.php  BSD License
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PHPUnit
 * @since      Class available since Release 1.0.0
 */
class Assert {
	/**
	 * @var    boolean
	 * @access private
	 */
	public $_looselyTyped = false;

	/**
	 * construct.
	 *
	 * @param  boolean $boolean
	 * @access public
	 */
	public function __construct($boolean, $message = 0) {
		if (!$boolean) {
			$this->fail($message);
		}
	}

	/**
	 * Asserts that a haystack contains a needle.
	 *
	 * @param  mixed
	 * @param  mixed
	 * @param  string
	 * @access public
	 * @since  Method available since Release 1.1.0
	 */
	public function assertContains($needle, $haystack, $message = null) {
		if (is_string($needle) && is_string($haystack)) {
			$this->assertTrue(strpos($haystack, $needle) !== false, $message);
		} elseif (is_array($haystack) && !is_object($needle)) {
			$this->assertTrue(in_array($needle, $haystack), $message);
		} else {
			$this->fail('Unsupported parameter passed to assertContains().');
		}
	}

	/**
	 * Asserts that a haystack does not contain a needle.
	 *
	 * @param  mixed
	 * @param  mixed
	 * @param  string
	 * @access public
	 * @since  Method available since Release 1.1.0
	 */
	public function assertNotContains($needle, $haystack, $message = null) {
		if (is_string($needle) && is_string($haystack)) {
			$this->assertFalse(strpos($haystack, $needle) !== false, $message);
		} elseif (is_array($haystack) && !is_object($needle)) {
			$this->assertFalse(in_array($needle, $haystack), $message);
		} else {
			$this->fail('Unsupported parameter passed to assertNotContains().');
		}
	}

	/**
	 * Asserts that two variables are equal.
	 *
	 * @param  mixed
	 * @param  mixed
	 * @param  string
	 * @param  mixed
	 * @access public
	 */
	public function assertEquals($expected, $actual, $message = null, $delta = 0) {
		if ((is_array($actual) && is_array($expected))
			|| (is_object($actual) && is_object($expected))
		) {
			if (is_object($actual) && is_object($expected)) {
				$actual   = (array)$actual;
				$expected = (array)$expected;
			}

			if (is_array($actual) && is_array($expected)) {
				$actualForSort   = $actual;
				$expectedForSort = $expected;
				$actual          = $this->recursive_ksort($actualForSort);
				$expected        = $this->recursive_ksort($expectedForSort);
			}

			if ($this->_looselyTyped) {
				$actual   = $this->_convertToString($actual);
				$expected = $this->_convertToString($expected);
			}

			$serializedActual   = serialize($actual);
			$serializedExpected = serialize($expected);

			/*
			 *			$message = sprintf(
			 *				'%sEXPECTED: %s <br>ACTUAL: %s',
			 *				!empty($message) ? $message . ' <br>' : '<br>',
			 *				$serializedExpected,
			 *				$serializedActual
			 *			);
			 */
			if ($serializedActual !== $serializedExpected) {
				return $this->failNotEquals($expected, $actual, $message);

				//return $this->fail($message);
			}
		} elseif (is_numeric($actual) && is_numeric($expected)) {
			$message = sprintf(
				'%sexpected %s%s, <br>ACTUAL: %s',
				!empty($message) ? $message . ' <br>' : '<br>',
				$expected,
				($delta != 0) ? ('+/- ' . $delta) : '',
				$actual
			);

			if (!($actual >= ($expected - $delta) && $actual <= ($expected + $delta))) {
				return $this->fail($message);
			}
		} else {
			$message = sprintf(
				'%sEXPECTED: %s <br>ACTUAL: %s',
				!empty($message) ? $message . ' <br>' : '<br>',
				$expected,
				$actual
			);

			if ($actual !== $expected) {
				return $this->fail($message);
			}
		}
	}

	/**
	 * Asserts that two variables are not equal.
	 *
	 * @param  mixed
	 * @param  mixed
	 * @param  string
	 * @param  mixed
	 * @access public
	 */
	public function assertNotEquals($expected, $actual, $message = null, $delta = 0) {
		if ((is_array($actual) && is_array($expected))
			|| (is_object($actual) && is_object($expected))
		) {
			if (is_array($actual) && is_array($expected)) {
				$actualForSort   = $actual;
				$expectedForSort = $expected;
				$actual          = $this->recursive_ksort($actualForSort);
				$expected        = $this->recursive_ksort($expectedForSort);
			}

			if ($this->_looselyTyped) {
				$actual   = $this->_convertToString($actual);
				$expected = $this->_convertToString($expected);
			}

			$serializedActual   = serialize($actual);
			$serializedExpected = serialize($expected);

			/*
			 *			$message = sprintf(
			 *				'%sEXPECTED: %s <br>ACTUAL: %s',
			 *				!empty($message) ? $message . ' <br>' : '<br>',
			 *				$serializedExpected,
			 *				$serializedActual
			 *			);
			 */
			if ($serializedActual == $serializedExpected) {
				return $this->failNotEquals($expected, $actual, $message);

				//return $this->fail($message);
			}
		} elseif (is_numeric($actual) && is_numeric($expected)) {
			$message = sprintf(
				'%sexpected %s %s <br>ACTUAL: %s',
				!empty($message) ? $message . ' <br>' : '<br>',
				$expected,
				($delta != 0) ? ('+/- ' . $delta) : '',
				$actual
			);

			if (($actual >= ($expected - $delta) && $actual <= ($expected + $delta))) {
				return $this->fail($message);
			}
		} else {
			$message = sprintf(
				'%sEXPECTED: %s <br>ACTUAL: %s',
				!empty($message) ? $message . ' <br>' : '<br>',
				$expected,
				$actual
			);

			if ($actual == $expected) {
				return $this->fail($message);
			}
		}
	}

	/**
	 * Asserts that a variable is not NULL.
	 *
	 * @param  mixed
	 * @param  string
	 * @access public
	 */
	public function assertNotNull($actual, $message = null) {
		$message = sprintf(
			'%sEXPECTED: NOT NULL <br>ACTUAL: NULL',
			!empty($message) ? $message . ' <br>' : '<br>'
		);

		if (null === $actual) {
			return $this->fail($message);
		}
	}

	/**
	 * Asserts that a variable is NULL.
	 *
	 * @param  mixed
	 * @param  string
	 * @access public
	 */
	public function assertNull($actual, $message = null) {
		$message = sprintf(
			'%sEXPECTED: NULL <br>ACTUAL: NOT NULL',
			!empty($message) ? $message . ' <br>' : '<br>'
		);

		if (null !== $actual) {
			return $this->fail($message);
		}
	}

	/**
	 * Asserts that a condition is true.
	 *
	 * @param  boolean
	 * @param  string
	 * @access public
	 */
	public function assertTrue($condition, $message = null) {
		$message = sprintf(
			'%sEXPECTED: TRUE <br>ACTUAL: FALSE',
			!empty($message) ? $message . ' <br>' : '<br>'
		);

		if (!$condition) {
			return $this->fail($message);
		}
	}

	/**
	 * Asserts that a condition is false.
	 *
	 * @param  boolean
	 * @param  string
	 * @access public
	 */
	public function assertFalse($condition, $message = null) {
		$message = sprintf(
			'%sEXPECTED: FALSE <br>ACTUAL: TRUE',
			!empty($message) ? $message . ' <br>' : '<br>'
		);

		if ($condition) {
			return $this->fail($message);
		}
	}

	/**
	 * Asserts that a string matches a given regular expression.
	 *
	 * @param  string
	 * @param  string
	 * @param  string
	 * @access public
	 */
	public function assertRegExp($pattern, $string, $message = null) {
		$message = sprintf(
			'%s"%s" does not match pattern "%s"',
			!empty($message) ? $message . ' <br>' : '<br>',
			$string,
			$pattern
		);

		if (!preg_match($pattern, $string)) {
			return $this->fail($message);
		}
	}

	/**
	 * Asserts that a string does not match a given regular expression.
	 *
	 * @param  string
	 * @param  string
	 * @param  string
	 * @access public
	 * @since  Method available since Release 1.1.0
	 */
	public function assertNotRegExp($pattern, $string, $message = null) {
		$message = sprintf(
			'%s"%s" matches pattern "%s"',
			!empty($message) ? $message . ' <br>' : '<br>',
			$string,
			$pattern
		);

		if (preg_match($pattern, $string)) {
			return $this->fail($message);
		}
	}

	/**
	 * Asserts that a variable is of a given type.
	 *
	 * @param  string          $expected
	 * @param  optional string $message
	 * @access public
	 */
	public function assertType($expected, $actual, $message = null) {
		return $this->assertEquals(
			$expected,
			gettype($actual),
			$message
		);
	}

	/**
	 * Converts a value to a string.
	 *
	 * @access private
	 */
	protected function _convertToString($value) {
		foreach ($value as $k => $v) {
			if (is_array($v)) {
				$value[$k] = $this->_convertToString($value[$k]);
			} else {
				settype($value[$k], 'string');
			}
		}

		return $value;
	}

	/**
	 * @param  boolean $looselyTyped
	 * @access public
	 */
	public function setLooselyTyped($looselyTyped) {
		if (is_bool($looselyTyped)) {
			$this->_looselyTyped = $looselyTyped;
		}
	}

	/**
	 * Recursively 'ksorts' an array.
	 *
	 * @param  array $fixArray
	 * @access protected
	 */
	protected function recursive_ksort($fixArray) {
		$sortedArray = $fixArray;
		ksort($sortedArray);

		foreach ($sortedArray as $key => $value) {
			if (is_array($value)) {
				$targetArray       = $sortedArray[$key];
				$sortedArray[$key] = $this->recursive_ksort($targetArray);
			}
		}

		return $sortedArray;
	}

	public function assertEqualsMultilineStrings(
		$string0,
		$string1,
		$message = null
	) {
		$lines0 = explode("\n", $string0);
		$lines1 = explode("\n", $string1);

		if (sizeof($lines0) != sizeof($lines1)) {
			$this->failNotEquals(
				sizeof($lines0) . ' line(s)',
				sizeof($lines1) . ' line(s)',
				'expected',
				$message
			);
		}

		for ($i = 0; $i < sizeof($lines0); $i++) {
			$this->assertEquals(
				trim($lines0[$i]),
				trim($lines1[$i]),
				'line ' . ($i + 1) . ' of multiline strings differ. ' . $message
			);
		}
	}

	public function _formatValue($value, $class = '') {
		if (is_object($value)) {
			if (method_exists($value, 'toString')) {
				$translateValue = htmlspecialchars($value->toString());
			} else {
				$translateValue = htmlspecialchars(serialize($value));
			}
		} elseif (is_array($value)) {
			$translateValue = '<pre>' . htmlspecialchars(var_export($value, 1)) . '</pre>';
		} else {
			$translateValue = htmlspecialchars($value);
		}

		$htmlValue = "<code class=\"$class\">" . $translateValue . '</code>';

		if (is_bool($value)) {
			$htmlValue = $value ? '<i>true</i>' : '<i>false</i>';
		} elseif (null === $value) {
			$htmlValue = '<i>null</i>';
		}

		$htmlValue .= '&nbsp;&nbsp;&nbsp;<span class="typeinfo">';
		$htmlValue .= 'type:' . gettype($value);
		$htmlValue .= is_object($value) ? ', class:' . get_class($value) : '';
		$htmlValue .= '</span>';

		return $htmlValue;
	}

	public function failNotEquals($expected, $actual, $message = null) {
		// Private function for reporting failure to match.
		$str = $message ? ($message . ' <br>') : '';

		//$str .= "($expected_label/actual)<br>";
		$str .= sprintf(
			'EXPECTED:<br>%s<br><br>ACTUAL:<br>%s',
			$this->_formatValue($expected, 'expected'),
			$this->_formatValue($actual, 'actual')
		);

		return $this->fail($str);
	}
}

class Assert_X {
	public function __construct($boolean, $message = 0) {
		if (!$boolean) {
			$this->fail($message);
		}
	}

	/**
	 * Asserts that a condition is true or false.
	 *
	 * @param  boolean
	 * @param  string
	 * @access public
	 */
	public function assert($condition, $message = null) {
		$ret = 'null';

		if (!empty($message)) {
			$string = trim($message) . ' ';
		}

		if ($condition === 'true') {
			$message = sprintf('%sexpected TRUE, actual FALSE', $string);

			$ret = $this->_assertHelperTrue($condition, $message);
		} elseif ($condition === 'false') {
			$message = sprintf('%sexpected FALSE, actual TRUE', $string);

			$ret = $this->_assertHelperFalse($condition, $message);
		}

		if ($ret != 'null') {
			$this->fail($ret);
		}
	}

	/**
	 * Helps assert that a condition is true.
	 *
	 * @param  boolean
	 * @param  string
	 * @access private
	 */
	private function _assertHelperTrue($condition, $message = null) {
		$message = sprintf(
			'%sexpected TRUE, actual FALSE',
			!empty($message) ? $message . ' ' : ''
		);

		if (!$condition) {
			return $message;
		}

		return 'null';
	}

	/**
	 * Helps assert that a condition is false.
	 *
	 * @param  boolean
	 * @param  string
	 * @access private
	 */
	private function _assertHelperFalse($condition, $message = null) {
		$message = sprintf(
			'%sexpected FALSE, actual TRUE',
			!empty($message) ? $message . ' ' : ''
		);

		if (!$condition) {
			return $message;
		}

		return 'null';
	}

	protected function recursive_ksort($fixArray) {
		$sortedArray = $fixArray;
		ksort($sortedArray);

		foreach ($sortedArray as $key => $value) {
			if (is_array($value)) {
				$targetArray       = $sortedArray[$key];
				$sortedArray[$key] = $this->recursive_ksort($targetArray);
			}
		}

		return $sortedArray;
	}

	public function assertEquals($expected, $actual, $message = 0) {
		if (gettype($expected) != gettype($actual)) {
			$this->failNotEquals($expected, $actual, 'expected', $message);

			return;
		}

		if (is_array($actual) && is_array($expected)) {
			$actual   = $this->recursive_ksort($actual);
			$expected = $this->recursive_ksort($expected);
		}

		if (is_object($expected)) {
			if (get_class($expected) != get_class($actual)) {
				$this->failNotEquals($expected, $actual, 'expected', $message);

				return;
			}

			if (method_exists($expected, 'equals')) {
				if (!$expected->equals($actual)) {
					$this->failNotEquals($expected, $actual, 'expected', $message);
				}

				// no further tests after equals()
				return;
			}
		}

		if ((null === $expected) != (null === $actual)) {
			$this->failNotEquals($expected, $actual, 'expected', $message);

			return;
		}

		$itemsAreEqual = ($expected === $actual);

		if ($itemsAreEqual) {
			return;
		}

		$this->failNotEquals($expected, $actual, 'expected', $message);
	}

	public function assertRegexp($regexp, $actual, $message = false) {
		if (!preg_match($regexp, $actual)) {
			$this->failNotEquals($regexp, $actual, 'pattern', $message);
		}
	}

	public function assertEqualsMultilineStrings(
		$string0,
		$string1,
		$message = null
	) {
		$lines0 = explode("\n", $string0);
		$lines1 = explode("\n", $string1);

		if (sizeof($lines0) != sizeof($lines1)) {
			$this->failNotEquals(
				sizeof($lines0) . ' line(s)',
				sizeof($lines1) . ' line(s)',
				'expected',
				$message
			);
		}

		for ($i = 0; $i < sizeof($lines0); $i++) {
			$this->assertEquals(
				trim($lines0[$i]),
				trim($lines1[$i]),
				'line ' . ($i + 1) . ' of multiline strings differ. ' . $message
			);
		}
	}

	public function _formatValue($value, $class = '') {
		if (is_object($value)) {
			if (method_exists($value, 'toString')) {
				$translateValue = htmlspecialchars($value->toString());
			} else {
				$translateValue = htmlspecialchars(serialize($value));
			}
		} elseif (is_array($value)) {
			$translateValue = '<pre>' . htmlspecialchars(var_export($value, 1)) . '</pre>';
		} else {
			$translateValue = htmlspecialchars($value);
		}

		$htmlValue = "<code class=\"$class\">" . $translateValue . '</code>';

		if (is_bool($value)) {
			$htmlValue = $value ? '<i>true</i>' : '<i>false</i>';
		} elseif (null === $value) {
			$htmlValue = '<i>null</i>';
		}

		$htmlValue .= '&nbsp;&nbsp;&nbsp;<span class="typeinfo">';
		$htmlValue .= 'type:' . gettype($value);
		$htmlValue .= is_object($value) ? ', class:' . get_class($value) : '';
		$htmlValue .= '</span>';

		return $htmlValue;
	}

	public function failNotEquals($expected, $actual, $expected_label, $message = 0) {
		// Private function for reporting failure to match.
		$str = $message ? ($message . ' <br>') : '';

		//$str .= "($expected_label/actual)<br>";
		$str .= sprintf(
			'%s<br>%s',
			$this->_formatValue($expected, 'expected'),
			$this->_formatValue($actual, 'actual')
		);
		$this->fail($str);
	}
}

// implements Test
class TestCase extends Assert {
	/* Defines context for running tests.  Specific context -- such as
	instance variables, global variables, global state -- is defined
	by creating a subclass that specializes the setUp() and
	tearDown() methods.  A specific test is defined by a subclass
	that specializes the runTest() method. */
	public $fName;
	public $fClassName;
	public $fModuleId;
	public $fResult;
	public $fExceptions = array();
	public $fTiming;
	public $fLifeCycle = array();
	public $fSkipped   = false;

	public function __construct($name) {
		$this->fName = $name;
	}

	public function setModuleId($moduleId) {
		$this->fModuleId = $moduleId;
	}

	public function getModuleId() {
		return $this->fModuleId;
	}

	public function run(&$testResult, $ranges) {
		/* Run this single test, by calling the run() method of the
		TestResult object which will in turn call the runBare() method
		of this object.  That complication allows the TestResult object
		to do various kinds of progress reporting as it invokes each
		test.  Create/obtain a TestResult object if none was passed in.
		Note that if a TestResult object was passed in, it must be by
		reference. */
		$this->fResult = $testResult;

		$testResult->run($this, $ranges);

		$this->fResult = 0;

		return $testResult;
	}

	public function classname() {
		if (isset($this->fClassName)) {
			return $this->fClassName;
		}

		return get_class($this);
	}

	public function countTestCases() {
		return 1;
	}

	public function runTest() {
		// Saved ref to current TestCase, so that the error handler can access it.
		$GLOBALS['PHPUnit_testRunning'] =& $this;
		$old_handler                    = set_error_handler('PHPUnit_error_handler');

		// errors will now be handled by our error handler
		$name = $this->name();

		if (!method_exists($this, $name)) {
			$this->error("Method '$name' does not exist");
		} else {
			$ret = $this->$name();

			if ($ret) {
				global $gallery;

				/*
				 * DA NOTE: Array check below is a hack to allow linksinthehead to pass
				 * TODO Remove and resolve real issue.
				 */
				if (is_array($ret)) {
					$ret = implode('', $ret);

					if (!empty(trim($ret))) {
						$this->fail('failed: ' . $ret . '<br>' . $gallery->getDebugBuffer());
					}
				} else {
					$this->fail($ret->getAsHtml() . $gallery->getDebugBuffer());
				}
			}
		}

		// Remove PHPUnit_error_handler AND any handlers added by test
		while (true) {
			// Call set/restore just to see what the current error handler is:
			$old_handler = set_error_handler('PHPUnit_error_handler');
			restore_error_handler();

			// Remove current error handler:
			restore_error_handler();

			// If we just removed PHPUnit_error_handler then previous state is restored.
			// If all error handlers are gone then test must have called restore_error_handler
			// (preferably they do not do that).
			if ($old_handler == 'PHPUnit_error_handler' || empty($old_handler)) {
				break;
			}
		}

		// Original code:
		//if (isset($old_handler)) {
		// revert to prior error handler
		//    set_error_handler($old_handler);
		//} else {
		//    restore_error_handler();
		//}
		$PHPUnit_testRunning = null;
	}

	// expect override
	public function setUp($x1 = null) {
		//print("TestCase::setUp()<br>\n");
		$this->fLifeCycle['setUp'] = 1;

		return null;
	}

	public function tearDown() {
		// possible override
		//print("TestCase::tearDown()<br>\n");
		$this->fLifeCycle['tearDown'] = 1;

		return null;
	}

	////////////////////////////////////////////////////////////////
	// protected
	protected function _createResult() {
		// override this to use specialized subclass of TestResult
		return new TestResult();
	}

	public function fail($message = 0) {
		//printf("TestCase::fail(%s)<br>\n", ($message) ? $message : '');

		/* JUnit throws AssertionFailedError here.  We just record the
		failure and carry on */
		$this->fExceptions[] = new PHPUnitException($message, 'FAILURE');
	}

	public function error($message) {
		/* report error that requires correction in the test script
		itself, or (heaven forbid) in this testing infrastructure */
		$this->fExceptions[] = new PHPUnitException($message, 'ERROR');

		// [does not work]
		$this->fResult->stop();
	}

	public function failed() {
		reset($this->fExceptions);

		foreach ($this->fExceptions as $key => $exception) {
			if ($exception->type == 'FAILURE') {
				return true;
			}
		}

		return false;
	}

	public function errored() {
		reset($this->fExceptions);

		foreach ($this->fExceptions as $key => $exception) {
			if ($exception->type == 'ERROR') {
				return true;
			}
		}

		return false;
	}

	public function elapsed() {
		list($startMicro, $startSecs) = explode(' ', $this->fTiming['start']);
		list($stopMicro, $stopSecs)   = explode(' ', $this->fTiming['stop']);

		return ($stopSecs - $startSecs) + ($stopMicro - $startMicro);
	}

	public function getExceptions() {
		return $this->fExceptions;
	}

	public function name() {
		return $this->fName;
	}

	public function skip() {
		$this->fSkipped = true;
	}

	public function wasSkipped() {
		return $this->fSkipped;
	}

	public function runBare() {
		global $gallery;

		$this->fTiming['start'] = microtime();

		$ret = $this->setUp();

		if ($ret) {
			$this->fail($ret->getAsHtml() . $gallery->getDebugBuffer());
		}

		parent::__construct(
			!empty($this->fLifeCycle['setUp']),
			'TestSuite setUp() method did not call parent::setUp()'
		);

		if (!($this->errored() || $this->failed())) {
			$this->runTest();
		}

		$ret = $this->tearDown();

		if ($ret) {
			$this->fail($ret->getAsHtml() . $gallery->getDebugBuffer());
		}

		parent::__construct(
			!empty($this->fLifeCycle['tearDown']),
			'TestSuite tearDown() method did not call parent::tearDown()'
		);

		$this->fTiming['stop'] = microtime();
	}
}

// implements Test
class TestSuite {
	// Compose a set of Tests (instances of TestCase or TestSuite) and run them all.
	public $fTests = array();
	public $fClassname;
	public $fModuleId;

	public function __construct($classname = false, $moduleid = false, $filter = null) {
		// Find all methods of the given class whose name starts with
		// "test" and add them to the test suite.
		if (empty($classname)) {
			return;
		}

		$this->fClassname = $classname;
		$this->fModuleId  = $moduleid;
		$this->fFilter    = $filter;
		$names            = get_class_methods($classname);

		foreach ($names as $key => $method) {
			if (preg_match('/^test/', $method)) {
				$test = new $classname($method);

				if (strcasecmp($method, $classname) == 0 || is_subclass_of($test, $method)) {
					// Ignore the given method name since it is a constructor:
					//   it is the name of our test class
					//   or the name of a superclass of our test class.
					// (This code smells funny. Anyone got a better way?)
				} else {
					if (substr($this->fFilter, 0, 2) == '!(' & substr($this->fFilter, -1, 1) == ')'
					) {
						$newFilter = substr($this->fFilter, 1);

						if (!isset($this->fFilter)
							|| !preg_match("/{$newFilter}/i", "$moduleid.$classname.$method")
						) {
							$test->setModuleId($this->fModuleId);
							$this->addTest($test);
						}
					} else {
						if (!isset($this->fFilter)
							|| preg_match("/{$this->fFilter}/i", "$moduleid.$classname.$method")
						) {
							$test->setModuleId($this->fModuleId);
							$this->addTest($test);
						}
					}
				}
			}
		}
	}

	public function addTest($test) {
		// Add TestCase or TestSuite to this TestSuite
		$this->fTests[] = $test;
	}

	public function run(&$testResult, $ranges) {
		/* Run all TestCases and TestSuites comprising this TestSuite,
		accumulating results in the given TestResult object. */
		reset($this->fTests);

		foreach ($this->fTests as $na => $test) {
			if ($testResult->shouldStop()) {
				break;
			}

			$test->run($testResult, $ranges);
		}
	}

	public function countTestCases() {
		/* Number of TestCases comprising this TestSuite (including those
		in any constituent TestSuites) */
		$count = 0;
		reset($this->fTests);

		foreach ($this->fTests as $na => $test_case) {
			$count += $test_case->countTestCases();
		}

		return $count;
	}
}

class TestFailure {
	/* Record failure of a single TestCase, associating it with the
	exception that occurred */
	public $fFailedTestName;
	public $fFailedClassName;
	public $fException;

	public function __construct(&$test, &$exception) {
		$this->fFailedClassName = $test->classname();
		$this->fFailedTestName  = $test->name();
		$this->fException       = $exception;
	}

	public function getExceptions() {
		// deprecated
		return array($this->fException);
	}

	public function getException() {
		return $this->fException;
	}

	public function getTestName() {
		return $this->fFailedTestName;
	}

	public function getClassName() {
		return $this->fFailedClassName;
	}
}

class TestResult {
	// Collect the results of running a set of TestCases.
	public $fFailures       = array();
	public $fErrors         = array();
	public $fRunTests       = 0;
	public $fUnskippedTests = 0;
	public $fStop           = false;

	public function __construct() {}

	public function _endTest(&$test) {
		// protected

		/* specialize this for end-of-test action, such as progress
		reports  */
	}

	public function addError($test, $exception) {
		$this->fErrors[] = new TestFailure($test, $exception);
	}

	public function addFailure($test, $exception) {
		$this->fFailures[] = new TestFailure($test, $exception);
	}

	public function getFailures() {
		return $this->fFailures;
	}

	public function run(&$test, $ranges) {
		// Run a single TestCase in the context of this TestResult
		$count    = ++$this->fRunTests;
		$skipFlag = true;

		foreach ($ranges as $tempRange) {
			if ($count >= $tempRange[0] && $count <= $tempRange[1]) {
				$skipFlag = false;
			}
		}

		if ($skipFlag) {
			$test->skip();
		}

		$this->_startTest($test);

		if (!$test->wasSkipped()) {
			$this->fUnskippedTests++;
			$test->runBare();
		}

		// this is where JUnit would catch AssertionFailedError
		$exceptions = $test->getExceptions();
		reset($exceptions);

		foreach ($exceptions as $key => $exception) {
			if ($exception->type == 'ERROR') {
				$this->addError($test, $exception);
			} elseif ($exception->type == 'FAILURE') {
				$this->addFailure($test, $exception);
			}
		}

		//    if ($exceptions)
		//      $this->fFailures[] = new TestFailure(&$test, &$exceptions);
		$this->_endTest($test);
	}

	public function countTests() {
		return $this->fUnskippedTests;
	}

	public function shouldStop() {
		return $this->fStop;
	}

	public function _startTest($test) {
		// protected
		// specialize this for start-of-test actions
	}

	public function stop() {
		// set indication that the test sequence should halt
		$fStop = true;
	}

	public function errorCount() {
		return count($this->fErrors);
	}

	public function failureCount() {
		return count($this->fFailures);
	}

	public function countFailures() {
		// deprecated
		return $this->failureCount();
	}
}

class TextTestResult extends TestResult {
	// Specialize TestResult to produce text/html report
	public function __construct() {
		// call superclass constructor
		parent::__construct();
	}

	public function report() {
		// report result of test run
		$nRun      = $this->countTests();
		$nFailures = $this->failureCount();
		$nErrors   = $this->errorCount();
		printf('<p>%s test%s run<br>', $nRun, ($nRun == 1) ? '' : 's');
		printf("%s failure%s<br>\n", $nFailures, ($nFailures == 1) ? '' : 's');
		printf("%s error%s.<br>\n", $nErrors, ($nErrors == 1) ? '' : 's');

		if ($nFailures > 0) {
			echo '<h2>Failures</h2>';
			echo "\n<ol>\n";
			$failures = $this->getFailures();

			foreach ($failures as $i => $failure) {
				$failedTestName = $failure->getTestName();
				printf("<li>%s</li>\n", $failedTestName);
				$exceptions = $failure->getExceptions();
				echo "\n<ul>";

				foreach ($exceptions as $na => $exception) {
					printf("<li>%s</li>\n", $exception->getMessage());
				}

				echo '</ul>';
			}

			echo "</ol>\n";
		}

		if ($nErrors > 0) {
			echo '<h2>Errors</h2>';
			echo "\n<ol>\n";
			reset($this->fErrors);

			foreach ($this->fErrors as $i => $error) {
				$erroredTestName = $error->getTestName();
				printf("<li>%s</li>\n", $failedTestName);
				$exception = $error->getException();
				echo "\n<ul>";
				printf("<li>%s</li>\n", $exception->getMessage());
				echo '</ul>';
			}

			echo "</ol>\n";
		}
	}

	public function _startTest($test) {
		if (phpversion() > '4') {
			printf('%s - %s ', get_class($test), $test->name());
		} else {
			printf('%s ', $test->name());
		}

		flush();
	}

	public function _endTest(&$test) {
		$outcome = $test->failed() ? '<font color="red">FAIL</font>' : '<font color="green">ok</font>';
		printf("$outcome<br>\n");
		flush();
	}
}

// PrettyTestResult created by BJG 17/11/01
// beacuse the standard test result provided looks rubbish.
class PrettyTestResult extends TestResult {
	public $_totalElapsed = 0;

	// Specialize TestResult to produce text/html report
	public function __construct() {
		// call superclass constructor
		parent::__construct();

		echo '<h2>Test Results</h2>';
		echo '<TABLE CELLSPACING="1" CELLPADDING="1" BORDER="0" WIDTH="90%" ALIGN="CENTER" class="details">';
		echo '<TR><TH>#</TH><TH>Module</TH><TH>Class</TH><TH>Function</TH><TH>Success?</TH><TH>Time</TH></TR>';
	}

	public function report() {
		echo '</TABLE>';

		// report result of test run
		$nRun      = $this->countTests();
		$nFailures = $this->countFailures();
		echo '<h2>Summary</h2>';
		printf('<p>%2.4f seconds elapsed</p>', $this->_totalElapsed);
		printf('<p>%s test%s run</p>', $nRun, ($nRun == 1) ? '' : 's');
		printf("<p>%s failure%s.</p>\n", $nFailures, ($nFailures == 1) ? '' : 's');

		if ($nFailures == 0) {
			return;
		}

		$failures  = $this->getFailures();
		$newFilter = array();

		foreach ($failures as $failure) {
			$newFilter[$failure->getClassName() . '.' . $failure->getTestName()] = 1;
		}

		printf(
			'<input type="button" onClick="javascript:reRun(\'%s\');" value="Re-run broken tests">',
			'(' . join('|', array_keys($newFilter)) . ')$'
		);

		echo '<h2>Failure Details</h2>';
		echo "\n<ol>\n";
		reset($failures);

		foreach ($failures as $i => $failure) {
			$failedTestName = $failure->getTestName();
			printf("\n<li>%s</li>\n", $failedTestName);
			$exceptions = $failure->getExceptions();
			echo '<ul>';

			foreach ($exceptions as $na => $exception) {
				printf("\n<li>%s</li>\n", $exception->getMessage());
			}

			echo '</ul>';
		}

		echo "</ol>\n";
	}

	public function _startTest($test) {
		if ($test->wasSkipped()) {
			$style = ' style="text-decoration: line-through;"';
		} else {
			$style = '';
		}

		printf("<TR><TD$style>%d</TD><TD$style>%s </TD><TD$style>%s </TD><TD$style>%s </TD>", $this->fRunTests, $test->getModuleId(), $test->classname(), $test->name());
		flush();
	}

	public function _endTest(&$test) {
		$outcome = $test->failed() ? ' class="Failure">FAILED' : ' class="Pass">PASSED';

		if ($test->wasSkipped()) {
			$outcome = ' class="Skipped">SKIPPED';
			$elapsed = '0.0000';
		} else {
			$elapsed              = sprintf('%2.4f', $test->elapsed());
			$this->_totalElapsed += $elapsed;
		}

		printf("<TD$outcome</TD><TD>$elapsed</TD></TR>");
		flush();
	}
}

class TestRunner {
	// Run a suite of tests and report results.
	public function run(&$suite, $ranges) {
		$result = new TextTestResult();

		$suite->run($result, $ranges);
		$result->report();
	}
}
