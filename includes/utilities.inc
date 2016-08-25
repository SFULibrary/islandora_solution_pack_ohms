<?php

/**
 * @file
 * Utility functions for Islandora OHMS.
 */

/**
 * Load a DOM document from a file on disk.
 *
 * @param string $path
 *   Path to the XML document.
 *
 * @return \DOMDocument
 *   The parsed DOM document.
 *
 * @throws Exception
 *   If libxml cannot read the xml file.
 */
function islandora_ohms_get_dom($path) {
  $errors = libxml_use_internal_errors(TRUE);

  $dom = new DOMDocument();
  if (!$dom->load($path)) {
    libxml_use_internal_errors($errors);
    throw islandora_ohms_xml_exception();
  }

  libxml_use_internal_errors($errors);
  return $dom;
}

/**
 * Generate an exception from the libxml errors and return it.
 *
 * @return \Exception
 *   Exception with the errors as the message.
 */
function islandora_ohms_xml_exception() {
  $m = "";
  foreach (libxml_get_errors() as $e) {
    $m .= "{$e->file}:{$e->line}:{$e->column} {$e->message}\n";
  }
  return new Exception($m);
}

/**
 * Get an XSL Processor.
 *
 * @param string $path
 *   Location of the xsl file on disk.
 * @param array $opts
 *   Parameters to the xsl.
 *
 * @return \XSLTProcessor
 *   The generated processor with the options in place.
 *
 * @throws Exception
 *   If libxml cannot parse the XSL.
 */
function islandora_ohms_get_stylesheet($path, $opts = array()) {
  $errors = libxml_use_internal_errors(TRUE);
  $dom = islandora_ohms_get_dom($path);
  $xsl = new XSLTProcessor();
  $result = $xsl->importStylesheet($dom);
  if ($result === FALSE) {
    libxml_use_internal_errors($errors);
    throw islandora_ohms_xml_exception();
  }
  foreach ($opts as $name => $value) {
    $xsl->setParameter(NULL, $name, $value);
  }
  libxml_use_internal_errors($errors);
  return $xsl;
}

/**
 * Apply an XSL to an XML File.
 *
 * @param string $path
 *   Path to the xml file on disk.
 * @param XSLTProcessor $xsl
 *   The parsed stylesheet to run.
 *
 * @return DOMDocument
 *   The processed XML document.
 *
 * @throws Exception
 *   For any libxml error.
 */
function islandora_ohms_apply_stylesheet($path, XSLTProcessor $xsl) {
  $errors = libxml_use_internal_errors(TRUE);
  $dom = islandora_ohms_get_dom($path);
  $result = $xsl->transformToDoc($dom);
  if ($result === FALSE) {
    libxml_use_internal_errors($errors);
    throw islandora_ohms_xml_exception();
  }
  libxml_use_internal_errors($errors);
  return $result;
}

/**
 * Get a directory iterator.
 *
 * @param string $target
 *   Path to the directory.
 * @param bool $recursive
 *   If true, then the subdirectories will be processed.
 *
 * @return Iterator|SplFileInfo[]
 *   An iterator of file info objects.
 */
function islandora_ohms_get_iterator($target, $recursive) {
  $filter = function () {
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
