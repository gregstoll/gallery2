		<?php newRow(); ?>
		<td class="<?php echo modifier('plugin'); ?>">
			<?php echo $plugin; ?>
		</td>
		<td style="background-color:#<?php echo percentColor($pluginData['percentDone']); ?>">
			<?php echo $pluginData['percentDone']; ?>%
		</td>

		<?php
		if (isset($pluginData['missing'])) {
			?>
		<td colspan="5" class="<?php echo modifier('missing'); ?>">
		  Missing
		</td>
			<?php
		} else {
			?>
		<td class="<?php echo modifier('total'); ?>">
			<?php echo $pluginData['total']; ?>
		</td>
		<td class="<?php echo modifier('translated'); ?>">
			<?php echo $pluginData['translated']; ?>
		</td>
		<td class="<?php echo modifier('fuzzy'); ?>">
			<?php echo $pluginData['fuzzy']; ?>
		</td>
		<td class="<?php echo modifier('untranslated'); ?>">
			<?php echo $pluginData['untranslated']; ?>
		</td>
		<td class="<?php echo modifier('obsolete'); ?>">
			<?php echo $pluginData['obsolete']; ?>
		</td>
			<?php
		} ?>
