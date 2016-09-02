<?php
/**
 * @file
 * This is the template file for the object page for ohms file.
 *
 * @TODO: add documentation about file and available variables
 */
?>
<div class="islandora-ohms-metadata">
	<?php print $description; ?>

	<?php if ($bio !== null): ?>
		<img class='tn' src="<?php echo $tn; ?>">
		<h2>About</h2>
  		<p><?php echo $bio->nodeValue; ?></p>
	<?php endif; ?>

	<?php if ($abstract !== null): ?>
  	<h2>Description</h2>
  	<p><?php echo $abstract->nodeValue; ?></p>
	<?php endif; ?>

	<h2>People Referenced</h2>
	<ul class="referenced-people">
		<?php foreach ($names as $name): ?>
  		<li><?php echo $name; ?></li>
		<?php endforeach; ?>
	</ul>

	<?php if ($parent_collections): ?>
  	<div>
  		<h2><?php print t('In collections'); ?></h2>
  		<ul>
			  <?php foreach ($parent_collections as $collection): ?>
				<li><?php print l($collection->label, "islandora/object/{$collection->id}"); ?></li>
			  <?php endforeach; ?>
  		</ul>
  	</div>
	<?php endif; ?>
</div>
