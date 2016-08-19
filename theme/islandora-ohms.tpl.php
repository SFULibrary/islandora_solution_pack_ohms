<?php
/**
 * @file
 * This is the template file for the object page for ohms file
 *
 * @TODO: add documentation about file and available variables
 */
?>

<div id='ohms_body'>
	<?php print $islandora_content; ?>
</div>
<div class="islandora-ohms-metadata">
	<?php print $description; ?>
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
	<?php print $metadata; ?>
</div>
