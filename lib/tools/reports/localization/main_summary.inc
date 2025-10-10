<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
	<title> Gallery 2: Localization Status Report (<?php echo strftime('%x'); ?>)</title>
	<link rel="stylesheet" type="text/css" href="localization/style.css"/>
  </head>
	<body>
	  <h1> Localization Status Report for Gallery 2 </h1>
	  <h3> Generated: <?php echo strftime('%x %X'); ?> <br/>
	   Most Recent PO: <?php echo strftime('%x %X', $mostRecentPoDate); ?> <br/>
	   Total Translated Strings: <?php echo $totalTranslated; ?> </h3>

	  <center>
	View: <b> summary </b> | <a href="?type=detail"> detail </a>
	<table class="report">
		<?php $index = 0; ?>
		<?php
		foreach ($reportData as $locale => $localeData) {
			?>
			<?php
			if (!($index % 20)) {
				?>
				<?php include __DIR__ . '/row_header_summary.inc'; ?>
				<?php
			} ?>
			<?php $index++; ?>
		<tr>
		  <td class="dark">
			<?php echo $index; ?> 
		  </td>
		  <td class="dark">
			<?php echo $locale; ?>
		  </td>
		  <td class="locale_percent" style="background-color:#
			<?php
			echo percentColor($localeData['percentDone'])
			?>
		  ">
			<?php echo $localeData['percentDone']; ?>%
		  </td>
			<?php $summaryData = $localeData['summary']; ?>
		  <td class="<?php echo modifier('total'); ?>">
			<?php echo $summaryData['total']; ?>
		  </td>
		  <td class="<?php echo modifier('translated'); ?>">
			<?php echo $summaryData['translated']; ?>%
		  </td>
		  <td class="<?php echo modifier('fuzzy'); ?>">
			<?php echo $summaryData['fuzzy']; ?>%
		  </td>
		  <td class="<?php echo modifier('untranslated'); ?>">
			<?php
			echo '<span title="overall untranslated including missing *.po files">'
			. (100 - $summaryData['translated'])
			. '%</span> (<span title="untranslated in existing *.po files">'
			. $summaryData['untranslated'] . '%</span>)'; ?>
		  </td>
		  <td class="<?php echo modifier('obsolete'); ?>">
			<?php echo $summaryData['obsolete']; ?>%
		  </td>
		</tr>
			<?php
		}
		?>
	</table>
	  </center>
	</body>
</html>
