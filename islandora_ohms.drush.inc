<?php

/**
 * @file
 * Implementation of Drush hooks.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_ohms_drush_command() {
	$items = array();

	$items['islandora_ohms_generate_mods'] = array(
		'aliases' => array('iogm'),
		'description' => 'Preprocess the OHMS XML files to generate MODS metadata.',
		'callback' => 'drush_islandora_ohms_generate_mods',
		'arguments' => array(
			'target' => array(
				'description' => 'The target directory to scan.',
				'required' => 'true',
			),
		),
		'options' => array(
			'recursive' => array(
				'description' => 'Scan subdirectories',
			),
			'mods-stylesheet' => array(
				'description' => 'Path to MODS XSLT',
			),
			'dc-stylesheet' => array(
				'description' => 'Path to MODS XSLT',
			),
			'toc-separator' => array(
				'description' => 'String to use as table of contents item separator',
			),
			'force' => array(
				'description' => 'Overwrite existing .MODS.xml files',
			)
		)
	);

	$items['islandora_ohms_batch_preprocess'] = array(
		'aliases' => array('iobp'),
		'callback' => 'drush_islandora_ohms_batch_preprocess',
		'description' => 'Preprocess the OHMS XML files into batch queue entries',
		'drupal dependencies' => array(
			'islandora_batch',
			'islandora_ohms',
		),
		'arguments' => array(
			'target' => array(
				'description' => 'The target directory to scan.',
				'required' => 'true',
			),
		),
		'options' => array(
			'recursive' => array(
				'description' => 'Scan subdirectories',
			),
			'namespace' => array(
				'description' => 'The namespace for objects created by this command.',
				'required' => TRUE,
			),
			'parent' => array(
				'description' => 'The collection to which the generated items should be added.',
				'required' => TRUE,
			),
		),
		'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
	);

	return $items;
}

/**
 * @param type $target
 * @param type $recursive
 * @return Iterator|SplFileInfo[]
 */
function get_iterator($target, $recursive) {
	$filter = function(SplFileInfo $item) {
		if ($item->getExtension() !== 'xml') {
			return false;
		}
		return !preg_match('/\.(DC|MODS)\.xml$/', $item->getFilename());
	};

	if ($recursive) {
		$dir = new RecursiveDirectoryIterator($target);
		$iter = new RecursiveIteratorIterator($dir);
		$filter = new CallbackFilterIterator($iter, $filter);
		return $filter;
	}
	
	$dir = new DirectoryIterator($target);
	$rex = new CallbackFilterIterator($dir, $filter);
	return $rex;
}

function generate_mods(SplFileInfo $item, XSLTProcessor $xsl, $force) {
	$modsFile = preg_replace('/(?<!\.MODS)\.xml$/', '.MODS.xml', $item->getPathname());
	if (file_exists($modsFile) && !$force) {
		drupal_set_message("Skipping {$modsFile}.");
		return;
	}

	$mods = islandora_ohms_apply_stylesheet($item->getPathname(), $xsl);
	file_put_contents($modsFile, $mods->saveXML());
}

function generate_dc(SplFileInfo $item, XSLTProcessor $xsl, $force) {
	$modsFile = preg_replace('/(?<!\.DC)\.xml$/', '.DC.xml', $item->getPathname());
	if (file_exists($modsFile) && !$force) {
		drupal_set_message("Skipping {$modsFile}.");
		return;
	}

	$mods = islandora_ohms_apply_stylesheet($item->getPathname(), $xsl);
	file_put_contents($modsFile, $mods->saveXML());
}

function drush_islandora_ohms_generate_mods($target) {
	module_load_include('inc', 'islandora_ohms', 'includes/utilities');

	$recursive = drush_get_option('recursive', false);
	$modsXslFile = drush_get_option('mods-stylesheet', dirname(__FILE__) . '/xsl/ohms2mods.xsl');
	$dcXslFile = drush_get_option('dc-stylesheet', dirname(__FILE__) . '/xsl/ohms2dc.xsl');

	$force = drush_get_option('force', false);
	$separator = drush_get_option('toc-separator', ' -- ');

	if (!file_exists($target)) {
		return drush_set_error('Target directory does not exist ' . $target);
	}

	$iterator = get_iterator($target, $recursive);

	$modsXsl = islandora_ohms_get_stylesheet($modsXslFile, array(
		'toc-separator' => $separator
	));

	$dcXsl = islandora_ohms_get_stylesheet($dcXslFile, array(
		'toc-separator' => $separator
	));


	foreach ($iterator as $item) {
		$name = $item->getFilename();
		if ($name[0] === '.') {
			continue;
		}
		drupal_set_message($item->getPathname());
		generate_mods($item, $modsXsl, $force);
		generate_dc($item, $dcXsl, $force);
	}
}

function drush_islandora_ohms_batch_preprocess($target) {
	drupal_static_reset('islandora_get_tuque_connection');
	$connection = islandora_get_tuque_connection();
	$parameters = array(
		'target' => $target,
		'namespace' => drush_get_option('namespace'),
		'parent' => drush_get_option('parent'),
		'recursive' => drush_get_option('recursive', false),
		'parent_relationship_uri' => drush_get_option('parent_relationship_uri', 'info:fedora/fedora-system:def/relations-external#'),
		'parent_relationship_pred' => drush_get_option('parent_relationship_pred', 'isMemberOfCollection'),
	);
	module_load_include('inc', 'islandora_ohms', 'IslandoraOhmsBatchPreprocess');
	$preprocessor = new IslandoraOhmsBatchPreprocessor($connection, $parameters);
	$preprocessed = islandora_batch_handle_preprocessor($preprocessor);
}
