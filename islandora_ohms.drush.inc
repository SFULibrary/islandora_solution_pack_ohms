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

  $items['islandora_ohms_generate_metadata'] = array(
    'aliases' => array('iogm'),
    'description' => 'Preprocess the OHMS XML files to generate MODS metadata.',
    'callback' => 'drush_islandora_ohms_generate_metadata',
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
      ),
    ),
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
 * Get a directory iterator.
 *
 * @param string $target
 *   Path to the target directory.
 * @param bool $recursive
 *   True if the directory should be searched recursively.
 *
 * @return Iterator|SplFileInfo[]
 *   The iterator for the directory structure.
 */
function get_iterator($target, $recursive) {
  $filter = function (SplFileInfo $item) {
    if ($item->getExtension() !== 'xml') {
      return FALSE;
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

/**
 * Generate a MODS file from an OHMS XML file.
 *
 * @param SplFileInfo $item
 *   The iterator item.
 * @param XSLTProcessor $xsl
 *   The XSL to transform the XML.
 * @param bool $forceIf true, overwrite an existing MODS file.
 *   If true, overwrite an existing MODS file.
 */
function generate_mods(SplFileInfo $item, XSLTProcessor $xsl, $force) {
  $mods_path = preg_replace('/(?<!\.MODS)\.xml$/', '.MODS.xml', $item->getPathname());
  if (file_exists($mods_path) && !$force) {
    drupal_set_message("Skipping {$mods_path}.");
    return;
  }

  $mods = islandora_ohms_apply_stylesheet($item->getPathname(), $xsl);
  file_put_contents($mods_path, $mods->saveXML());
}

/**
 * Generate a DC file from an OHMS XML file.
 *
 * @param SplFileInfo $item
 *   The iterator item.
 * @param XSLTProcessor $xsl
 *   The XSL to transform the XML.
 * @param bool $forceIf true, overwrite an existing MODS file.
 *   If true, overwrite an existing MODS file.
 */
function generate_dc(SplFileInfo $item, XSLTProcessor $xsl, $force) {
  $dc_path = preg_replace('/(?<!\.DC)\.xml$/', '.DC.xml', $item->getPathname());
  if (file_exists($dc_path) && !$force) {
    drupal_set_message("Skipping {$dc_path}.");
    return;
  }

  $mods = islandora_ohms_apply_stylesheet($item->getPathname(), $xsl);
  file_put_contents($dc_path, $mods->saveXML());
}

/**
 * Generate the metadata for an OHMS XML file.
 *
 * @param string $target
 *   Path to the XML file.
 *
 * @return bool
 *   True or false for success or failure.
 */
function drush_islandora_ohms_generate_metadata($target) {
  module_load_include('inc', 'islandora_ohms', 'includes/utilities');

  $recursive = drush_get_option('recursive', FALSE);
  $mods_xsl_path = drush_get_option('mods-stylesheet', dirname(__FILE__) . '/xsl/ohms2mods.xsl');
  $dc_xsl_path = drush_get_option('dc-stylesheet', dirname(__FILE__) . '/xsl/ohms2dc.xsl');

  $force = drush_get_option('force', FALSE);
  $separator = drush_get_option('toc-separator', ' -- ');

  if (!file_exists($target)) {
    return drush_set_error('Target directory does not exist ' . $target);
  }

  $iterator = get_iterator($target, $recursive);

  $mods_xsl = islandora_ohms_get_stylesheet($mods_xsl_path, array(
      'toc-separator' => $separator,
    ));

  $dc_xsl = islandora_ohms_get_stylesheet($dc_xsl_path, array(
      'toc-separator' => $separator,
    ));

  foreach ($iterator as $item) {
    $name = $item->getFilename();
    if ($name[0] === '.') {
      continue;
    }
    drupal_set_message($item->getPathname());
    generate_mods($item, $mods_xsl, $force);
    generate_dc($item, $dc_xsl, $force);
  }
  return TRUE;
}

/**
 * Drush callback function to preprocess a directory for batch ingest.
 *
 * @param string $target
 *   Path to the directory.
 *
 * @return bool
 *   True or false for success or failure.
 */
function drush_islandora_ohms_batch_preprocess($target) {
  drupal_static_reset('islandora_get_tuque_connection');
  $connection = islandora_get_tuque_connection();
  $parameters = array(
    'target' => $target,
    'namespace' => drush_get_option('namespace'),
    'parent' => drush_get_option('parent'),
    'recursive' => drush_get_option('recursive', FALSE),
    'parent_relationship_uri' => drush_get_option('parent_relationship_uri', 'info:fedora/fedora-system:def/relations-external#'),
    'parent_relationship_pred' => drush_get_option('parent_relationship_pred', 'isMemberOfCollection'),
  );
  module_load_include('inc', 'islandora_ohms', 'IslandoraOhmsBatchPreprocess');
  $preprocessor = new IslandoraOhmsBatchPreprocessor($connection, $parameters);
  $preprocessed = islandora_batch_handle_preprocessor($preprocessor);
  return TRUE;
}
