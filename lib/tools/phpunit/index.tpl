<!DOCTYPE html>
<html>
<head>
	<title>Gallery Unit Tests</title>
	<!--base_href-->
	<script type="text/javascript" src="../../../lib/yui/utilities.js"></script>
	<link rel="stylesheet" type="text/css" href="stylesheet.css"/>

	<script type="text/javascript">
	function toggle(id) {
		var display = YAHOO.util.Dom.getStyle(id, 'display');
		YAHOO.util.Dom.setStyle(id, 'display', display == 'block' ? 'none' : 'block');
	}
	function setFilter(value) {
		document.forms[0].filter.value=value;
	}
	function reRun() {
		setFilter(failedTestFilter);
		document.forms[0].submit();
	}
	function skip(i) {
	}
	function updateProgressBar(title, description, percentComplete, timeRemaining, memoryInfo) {
		/**
		* Dummy updateProgressBar method
		* in case some unit tests actually writes the script to call (ItemAddFromServerTest)
		*/
	}
	</script>
</head>
<body>
	<?php
	/*
	* Returns the exact bytes value from a php.ini setting
	* Copied from PHP.net's manual entry for ini_get()
	* Applied fix from https://stackoverflow.com/a/45648879/891636
	*/
	function getBytes($val) {
		$val = substr(trim($val), 0, -1);
		$last = substr($val, -1);

		switch ($last) {
			case 'g':
			case 'G':
				$val *= 1024;
				// Fall Through
			case 'm':
			case 'M':
				$val *= 1024;
				// Fall Through
			case 'k':
			case 'K':
				$val *= 1024;
		}

		return $val;
	}
	?>
	<?php if (!isset($compactView)): ?>
		<div id="status" style="position: absolute; right: 0; top: 0; background: white; display: none">
			<div class="header">
				Run Status
				<div style="position: absolute; right: 0px; display: inline">
					<img class="toggle" onclick="hideStatus()" src="cancel.png">
				</div>
			</div>
			<div class="body">
				<?php $memLim = ini_get('memory_limit'); ?>
				<?php $memLimVal = getBytes($memLim); ?>
				Pass: <span id="pass_count">&nbsp;</span>, Fail <span id="fail_count">&nbsp;</span>, Skip: <span id="skip_count">&nbsp;</span>, Total: <span id="total_count">&nbsp;</span> <br>
				Elapsed time: <span id="elapsed_time">&nbsp;</span> <br>
				Estimated time remaining: <span id="estimated_time_remaining">&nbsp;</span> <br>
				Memory Usage: <span id="used_memory">&nbsp;</span> (<?php echo (0 < $memLimVal) ? $memLim . " allowed" : "Unlimited"; ?>)
			</div>
			<div id="show_more" class="header toggle">
				<img src="add.png" onclick="showMoreStatus()">
			</div>
			<div id="more" class="body" style="display: none">
				Test running: <span id="test_running">none</span> <br>
				Last update: <span id="last_update_interval">not running</span> <br>
			</div>
			<div id="show_less" class="header toggle" style="display: none">
				<img src="../../../modules/icons/iconpacks/silk/delete.png" onclick="showLessStatus()">
			</div>
		</div>
	</div>

	<h1>Gallery Unit Tests</h1>
	<div class="section">
		<p>
			<a href="../../../">Return to Gallery</a>
		</p>
		<p>
			This is the Gallery test framework.  We will use this to verify
			that the Gallery code is functioning properly.  It will help us
			identify bugs in the code when we add new features, port to new
			systems, or add support for new database back ends.  All the
			tests should pass with a green box that says <b>PASSED</b> in it).
		</p>
	</div>

	<?php if (!$isSiteAdmin): ?>
		<h2> <span class="error">ERROR!</span> </h2>
		<div class="section">
			You are not logged in as a Gallery site administrator so you are
			not allowed to run the unit tests.
			[<a href="../../../main.php?g2_view=core.UserAdmin&g2_subView=core.UserLogin&g2_return=<?php echo $_SERVER['REQUEST_URI']?>">login</a>]
		</div>
	<?php endif; ?>

	<?php if (sizeof($incorrectDevEnv) > 0): ?>
		<div style="float: right; width: 500px; border: 2px solid red; padding: 3px">
			<h2 style="margin: 0px"> Development Environment Warning </h2>
			<div style="margin-left: 5px">
				The following settings in your development environment are not correct.
				See the <a href="http://codex.gallery2.org/index.php/Gallery2:Developer_Guidelines#PHP_Settings">
				G2 Development Environment</a> page for more information
			</div>
			<br>
			<table border="0" class="details">
				<tr>
					<th> PHP Setting </th>
					<th> Actual Value </th>
					<th> Expected Value(s) </th>
				</tr>
				<?php foreach (array_keys($incorrectDevEnv) as $key): ?>
					<tr>
						<td> <?php echo $key ?> </td>
						<td> <?php echo wordwrap($incorrectDevEnv[$key][1], 45, "<br>", true) ?> </td>
						<td> <?php echo join(' <b>or</b> ', $incorrectDevEnv[$key][0]) ?> </td>
					</tr>
				<?php endforeach; ?>
			</table>
		</div>
	<?php endif; ?>

	<h2>Filter (<span onclick="toggle('help_and_examples')" class="fakelink">Help/Examples</span>) </h2>
	<div class="section">
		<form>
			<?php if (isset($sessionKey)): ?>
				<input type="hidden" name="<?php echo $sessionKey?>" value="<?php echo $sessionId ?>"/>
			<?php endif; ?>

			<input type="text" name="filter" size="60" value="<?php echo $displayFilter ?>"
			id="filter" style="margin-top: 0.3em; margin-bottom: 0.3em"/>
			<?php if (!isset($_GET['filter'])): ?>
				<script type="text/javascript"> document.getElementById('filter').focus(); </script>
			<?php endif; ?>

			<br>
			<h2>
			</h2>
			<div id="help_and_examples" style="display: none">
				Enter a regular expression string to restrict testing to classes containing
				that text in their class name or test method.  If you use an exclamation before a
				module/class/test name(s) encapsulated in parenthesis and separated with bars, this will
				exclude the matching tests. Use ":#-#" to restrict which matching tests are actually run.
				You can also specify multiple spans with ":#-#,#-#,#-#".
				Append ":1by1" to run tests one-per-request; automatic refresh stops when a test fails.

				<ul id="filter_examples_list">
					<li>
						<a href="javascript:setFilter('AddCommentControllerTest.testAddComment')">
							AddCommentControllerTest.testAddComment
						</a>
					</li>
					<li>
						<a href="javascript:setFilter('AddCommentControllerTest.testAdd')">AddCommentControllerTest.testAdd</a>
					</li>
					<li>
						<a href="javascript:setFilter('AddCommentControllerTest')">AddCommentControllerTest</a>
					</li>
					<li>
						<a href="javascript:setFilter('comment')">comment</a>
					</li>
					<li>
						<a href="javascript:setFilter('!(comment)')">!(comment)</a>
					</li>
					<li>
						<a href="javascript:setFilter('!(comment|core)')">!(comment|core)</a>
					</li>
					<li>
						<a href="javascript:setFilter('comment:1-3')">comment:1-3</a>
					</li>
					<li>
						<a href="javascript:setFilter('comment:3-')">comment:3-</a>
					</li>
					<li>
						<a href="javascript:setFilter('comment:-5')">comment:-5</a>
					</li>
					<li>
						<a href="javascript:setFilter('comment:1-3,6-8,10-12')">comment:1-3,6-8,10-12</a>
					</li>
					<li>
						<a href="javascript:setFilter('comment:-3,4-')">comment:-3,4-</a>
					</li>
					<li>
						<a href="javascript:setFilter('core:1by1')">core:1by1</a>
					</li>
				</ul>
			</div>
		</form>
	</div>

	<?php
	$activeCount = 0;
	foreach ($moduleStatusList as $moduleId => $moduleStatus) {
		if (!empty($moduleStatus['active'])) {
			$activeCount++;
		}
	}
	?>
	<h2>
		<span onclick="toggle('modules_listing')" class="fakelink">Modules (<?php printf("%d active, %d total", $activeCount, sizeof($moduleStatusList)); ?>)</span>
	</h2>
	<div class="section" style="width: 100%">
		<table cellspacing="1" cellpadding="1" border="0"
		width="800" class="details" id="modules_listing" style="display: none">
		<tr>
			<th> Module Id </th>
			<th> Active </th>
			<th> Installed </th>
		</tr>
		<?php foreach ($moduleStatusList as $moduleId => $moduleStatus): ?>
			<tr>
				<td style="width: 100px">
					<?php echo $moduleId ?>
				</td>
				<td style="width: 100px">
					<?php echo !empty($moduleStatus['active']) ? "active" : "not active" ?>
				</td>
				<td style="width: 100px">
					<?php echo !empty($moduleStatus['available']) ? "installed" : "not available" ?>
				</td>
			</tr>
		<?php endforeach; ?>
	</table>
</div>
<?php endif; /* compactView */ ?>

<?php if ($priorRuns): ?>
	<h2>
		<span onclick="toggle('prior_runs')" class="fakelink">Prior Runs (<?php echo count($priorRuns)?>)</span>
	</h2>
	<div id="prior_runs" style="display: none">
		<table cellspacing="1" cellpadding="1" border="0"	width="800" class="details">
			<tr>
				<th> Date </th>
				<th> File Size </th>
				<th> Action </th>
			</tr>
			<?php foreach ($priorRuns as $run): ?>
				<tr>
					<td style="width: 100px">
						<a href="index.php?run=frame:<?php echo $run['key']?>"><?php echo $run['date'] ?></a>
					</td>
					<td style="width: 100px">
						<?php echo $run['size'] ?> bytes
					</td>
					<td style="width: 100px">
						<a href="index.php?run=delete:<?php echo $run['key']?>">delete</a>
					</td>
				</tr>
			<?php endforeach; ?>
			<tr>
				<td colspan="3">
					<center>
						<h3> <a href="?run=deleteall:">Delete All</a></h3>
					</center>
				</td>
			</tr>
		</table>
	</div>
<?php endif; /* $priorRuns */ ?>

<h2>Test Results</h2>

<table cellspacing="1" cellpadding="1" border="0" width="90%" align="CENTER" class="details">
	<tr><th>#</th><th>Module</th><th>Class</th><th>Function</th><th>Status</th><th>Time</th></tr>
	<?php $i = 0;
	foreach ($testSuite->fTests as $testClass):
	foreach ($testClass->fTests as $test): $i++;
	if (isset($testOneByOne) && $testOneByOne != $i) continue; ?>
	<tr id="testRow<?php echo $i ?>">
		<td><?php echo $i ?></td>
		<td><?php echo $test->getModuleId() ?></td>
		<td><?php echo $test->classname() ?></td>
		<td><?php echo $test->name() ?></td>
		<td><a href="#fail<?php echo $i ?>" style="display:none">FAILED</a>&nbsp;</td><td>&nbsp;</td>
	</tr><?php endforeach; endforeach; $totalTests = $i;?>
</table>

<div id="testSummary" style="display:none">
	<h2>Summary</h2>

	<p><span id="testTime">&nbsp;</span> seconds elapsed</p>
	<p><span id="testCount">&nbsp;</span> run<span id="runThenSkip">&nbsp;</span></p>
	<p><span id="testFailCount">&nbsp;</span> failed
		with <span id="testErrorCount">&nbsp;</span></p>
		<p><a href="http://codex.gallery2.org/Gallery2:Test_Matrix#Unit_Tests">Test Matrix Entry</a>:
			<br><b><span id="testReport">&nbsp;</span></b>
			<br>(<span><a href="javascript:changeUsername()">change username</a></span>)
		</p>
		<script type="text/javascript">
		function getUsernameFromCookie() {
			var dc = document.cookie;
			if (dc) {
				var m = dc.match(/g2_phpunit_username=(.*?);/);
				if (m && m.length == 2) {
					return m[1];
				}
			}
			return 'NAME_PLACEHOLDER';
		}

		function setCookie(key, value) {
			document.cookie = key + '=' + escape(value) +
			'; expires=Sunday, January 17, 2038 4:00:00 PM';
		}

		function setUsername(oldUsername, newUsername) {
			var report = document.getElementById('testReport').firstChild;
			report.nodeValue = report.nodeValue.replace('\|' + oldUsername + '\|', '|' + newUsername + '|');
			setCookie('g2_phpunit_username', newUsername);
		}

		function changeUsername() {
			setUsername(getUsernameFromCookie(), prompt('What is your username?'));
		}

		function showStatus() {
			document.getElementById("status").style.display = 'block';
			window.onscroll = function() {
				new YAHOO.util.Anim(
					'status',
					{ top: { to: YAHOO.util.Dom.getDocumentScrollTop() } },
					.5, YAHOO.util.Easing.easeIn).animate();
				};
				running = true;
				setTimeout('updateMoreBox()', 0);
			}

			function completeStatus() {
				if (failCount > 0) {
					YAHOO.util.Dom.addClass('status', 'fail');
				} else {
					YAHOO.util.Dom.addClass('status', 'pass');
				}
				running = false;
				runningTest('none');
			}

			function updateStats(pass, fail, skip, usedMemory, force) {
				if (pass || force) {
					passCount += pass;
					passCountEl.innerHTML = passCount;
				}
				if (fail || force) {
					failCount += fail;
					failCountEl.innerHTML = failCount;
				}
				if (skip || force) {
					skipCount += skip;
					skipCountEl.innerHTML = skipCount;
				}
				usedMemoryEl.innerHTML = Math.round(usedMemory / (1024 * 1024)) + 'M';

				var completedCount = passCount + failCount + skipCount;
				var elapsed = (new Date().getTime() / 1000) - startTime;
				var completionPercent = completedCount / totalCount;
				var estimatedTotalTime = elapsed / completionPercent;
				var estimatedRemainingTime = (1 - completionPercent) * estimatedTotalTime;
				estimatedRemainingTime = Math.round(estimatedRemainingTime);
				estimatedTimeRemainingEl.innerHTML = estimatedRemainingTime + " seconds";
				elapsedEl.innerHTML = Math.round(elapsed) + " seconds";
				lastUpdateTime = new Date().getTime() / 1000;
			}

			function updateMoreBox() {
				var lastUpdateEl = document.getElementById('last_update_interval');
				var testRunningEl = document.getElementById('test_running');

				var lastText = 'not running';
				var runningText = 'none';
				if (running) {
					var now = new Date().getTime() / 1000;
					lastText = Math.round(100 * (now - lastUpdateTime)) / 100 + ' seconds ago';
				}

				lastUpdateEl.innerHTML = lastText;
				testRunningEl.innerHtml = runningText;

				if (running) {
					setTimeout('updateMoreBox()', 500 + Math.random() * 500);
				}
			}

			function hideStatus() {
				YAHOO.util.Dom.setStyle('status', 'display', 'none');
			}

			function showMoreStatus() {
				YAHOO.util.Dom.setStyle('show_more', 'display', 'none');
				YAHOO.util.Dom.setStyle('more', 'display', 'block');
				YAHOO.util.Dom.setStyle('show_less', 'display', 'block');
			}

			function showLessStatus() {
				YAHOO.util.Dom.setStyle('show_more', 'display', 'block');
				YAHOO.util.Dom.setStyle('more', 'display', 'none');
				YAHOO.util.Dom.setStyle('show_less', 'display', 'none');
			}

			function runningTest(testName) {
				document.getElementById('test_running').innerHTML = testName;
			}

			var startTime = new Date().getTime() / 1000;
			var passCount = failCount = skipCount = 0;
			var totalCount = <?php echo $totalTests; ?>;
			var passCountEl = document.getElementById('pass_count');
			var failCountEl = document.getElementById('fail_count');
			var skipCountEl = document.getElementById('skip_count');
			var estimatedTimeRemainingEl = document.getElementById('estimated_time_remaining');
			var elapsedEl = document.getElementById('elapsed_time');
			var usedMemoryEl = document.getElementById('used_memory');
			var running = false;
			document.getElementById('total_count').innerHTML = totalCount;
			updateMoreBox();
			updateStats(0, 0, 0, 0, 1);
			</script>

			<input type="button" onclick="reRun();" value="Re-run broken tests"
			id="runBrokenButton" style="display:none"/>
			<p>
				<a href="../../../">Return to Gallery</a>
			</p>
		</div>

		<?php
			$result = new GalleryTestResult();
			$testSuite->run($result, $range);
			$result->report();
		?>
	</body>
</html>
