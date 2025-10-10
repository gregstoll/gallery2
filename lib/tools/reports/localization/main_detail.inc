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
	View: <a href="?type=summary"> summary </a> | <b> detail </b>
	<table class="report">
		<?php $index = 0; ?>
		<?php
		foreach ($reportData as $locale => $localeData) {
			?>
			<?php include __DIR__ . '/row_header_detail.inc'; ?>
			<?php $index++; ?>
		<tr>
		  <td rowspan="<?php echo sizeof($localeData['plugins']); ?>" class="dark">
			<?php echo $index; ?> 
		  </td>
		  <td rowspan="<?php echo sizeof($localeData['plugins']); ?>" class="dark">
			<?php echo $locale; ?>
		  </td>
		  <td rowspan="<?php echo sizeof($localeData['plugins']); ?>" class="locale_percent"
		  style="background-color:#<?php echo percentColor($localeData['percentDone']); ?>">
			<?php echo $localeData['percentDone']; ?>%
		  </td>
			<?php $keys = array_keys($localeData['plugins']); ?>
			<?php $plugin = $keys[0]; ?>
			<?php $pluginData = $localeData['plugins'][$plugin]; ?>
			<?php include __DIR__ . '/plugin_rowdata.inc'; ?>
		</tr>

			<?php
			for ($i = 1; $i < sizeof($localeData['plugins']); $i++) {
				?>
		<tr>
				<?php $keys = array_keys($localeData['plugins']); ?>
				<?php $plugin = $keys[$i]; ?>
				<?php $pluginData = $localeData['plugins'][$plugin]; ?>
				<?php include __DIR__ . '/plugin_rowdata.inc'; ?>
		</tr>
				<?php
			} ?>
			<?php
		}
		?>
	</table>
	  </center>
	</body>
</html>
