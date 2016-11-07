<?php

/**
 * @file
 * Derivative-creation functions.
 */

/**
 * Adds a datastream to an object.
 *
 * @param AbstractObject $object
 *   The object to add a datastream to.
 * @param bool $force
 *   Whether derivative generation is being forced or not.
 *
 * @return array
 *   An array describing the outcome of the thumbnail creation.
 */
function islandora_ohms_create_mods(AbstractObject $object) {
  module_load_include('inc', 'islandora_ohms', 'includes/utilities');
  $dsid = 'MODS';
  $has_ds = isset($object[$dsid]);
  if ($has_ds) {
    $mods_stream = $object[$dsid];
  } else {
    $mods_stream = $object->constructDatastream($dsid, 'M');
    $mods_stream->label = $dsid;
    $mods_stream->mimeType = 'application/xml';
  }

  $mods_xsl_path = dirname(dirname(__FILE__)) . '/xsl/ohms2mods.xsl';
  $mods_xsl = islandora_ohms_get_stylesheet($mods_xsl_path, array(
    'toc-separator' => ' -- ',
  ));
  $mods = islandora_ohms_apply_stylesheet($object['OBJ']->content, $mods_xsl);  
  $mods_stream->setContentFromString($mods->saveXML());
  $object->ingestDatastream($mods_stream);
  
  $dc_xsl_path = dirname(dirname(__FILE__)) . '/xsl/ohms2dc.xsl';
  $dc_xsl = islandora_ohms_get_stylesheet($dc_xsl_path);
  $dc = islandora_ohms_apply_stylesheet($object['OBJ']->content, $dc_xsl);
  
  $dc_stream = $object['DC'];
  $dc_stream->setContentFromString($dc->saveXML());
  $object->ingestDatastream($dc_stream);
  
  return array(
    'success' => true,
    'messages' => array(
      array(
        'message' => 'success',
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      )
    ),
  );
}

/**
      $path_to_mods_to_dc_xsl = drupal_get_path('module', 'islandora_batch') . '/transforms/mods_to_dc.xsl';
      $mods_to_dc_xsl = file_get_contents($path_to_mods_to_dc_xsl);
      $dc_content = islandora_solution_pack_xml_apply_xslt($mods_to_dc_xsl, $mods->content);
      $dc = $this->constructDatastream('DC', 'M');
      $dc->mimetype = 'application/xml';
      $dc->label = 'DC Record';
      $dc->setContentFromString($dc_content);
      $this->ingestDatastream($dc);

 */