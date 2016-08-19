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
			'stylesheet' => array(
				'description' => 'Path to XSLT',
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
	if ($recursive) {
		$dir = new RecursiveDirectoryIterator($target);
		$iter = new RecursiveIteratorIterator($dir);
		$rex = new RegexIterator($iter, '/(?<!\.MODS)\.xml$/', RegexIterator::MATCH);
		return $rex;
	}
	$dir = new DirectoryIterator($target);
	$rex = new RegexIterator($dir, '/(?<!\.MODS)\.xml$/', RegexIterator::MATCH);
	return $rex;
}

function get_xsltproc($stylesheet, $opts) {
	$dom = new DOMDocument();
	if (!$dom->load($stylesheet)) {
		$m = "";
		foreach (libxml_get_errors() as $e) {
			$m .= "{$e->file}:{$e->line}:{$e->column} {$e->message}\n";
		}
		throw new Exception($m);
	}
	$xsl = new XSLTProcessor();
	$xsl->importStylesheet($dom);
	foreach ($opts as $name => $value) {
		$xsl->setParameter(null, $name, $value);
	}
	return $xsl;
}

function apply_xsltproc($xsl, $target) {
	$dom = new DOMDocument();
	if (!$dom->load($target)) {
		$m = "";
		foreach (libxml_get_errors() as $e) {
			$m .= "{$e->file}:{$e->line}:{$e->column} {$e->message}\n";
		}
		throw new Exception($m);
	}
	$result = $xsl->transformToDoc($dom);
	if ($result === false) {
		$m = "";
		foreach (libxml_get_errors() as $e) {
			$m .= "{$e->file}:{$e->line}:{$e->column} {$e->message}\n";
		}
		throw new Exception($m);
	}
	return $result;
}

function drush_islandora_ohms_generate_mods($target) {
	$defaultXsl = $path = dirname(__FILE__) . '/xsl/ohms2mods.xsl';

	$recursive = drush_get_option('recursive', false);
	$stylesheet = drush_get_option('stylesheet', $defaultXsl);
	$force = drush_get_option('force', false);
	$separator = drush_get_option('toc-separator', ' -- ');

	if (!file_exists($target)) {
		return drush_set_error('Target directory does not exist ' . $target);
	}

	$iterator = get_iterator($target, $recursive);
	libxml_use_internal_errors(true);

	try {
		$xslt_proc = get_xsltproc($stylesheet, array(
			'toc-separator' => $separator
		));
	} catch (Exception $e) {
		$m = "Cannot load stylesheet {$stylesheet}:\n {$e->getMessage()}";
		return drush_set_error($m);
	}

	foreach ($iterator as $item) {
		$name = $item->getFilename();
		if ($name[0] === '.') {
			continue;
		}
		// $item is an SplFileInfo object
		drupal_set_message($item->getPathName());
		$modsFile = preg_replace('/(?!\.MODS)\.xml$/', '.MODS.xml', $item->getPathname());
		if (file_exists($modsFile) && !$force) {
			drupal_set_message("Skipping {$modsFile}.");
			continue;
		}

		$mods = apply_xsltproc($xslt_proc, $item->getPathname());
		file_put_contents($modsFile, $mods->saveXML());
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
