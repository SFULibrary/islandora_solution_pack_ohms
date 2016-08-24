<?php

/**
 * Load a DOM document from a file on disk
 *
 * @param string $path
 *
 * @return \DOMDocument
 * @throws Exception
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
 */
function islandora_ohms_xml_exception() {
  $m = "";
  foreach (libxml_get_errors() as $e) {
    $m .= "{$e->file}:{$e->line}:{$e->column} {$e->message}\n";
  }
  return new Exception($m);
}

/**
 *
 * @param string $path location of the xsl file on disk
 * @param array $opts parameters to the xsl
 *
 * @return \XSLTProcessor
 * @throws Exception
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
 *
 * @param string $path to the xml file on disk
 * @param XSLTProcessor $xsl the stylesheet to run
 *
 * @return DOMDocument
 * @throws Exception
 */
function islandora_ohms_apply_stylesheet($path, XSLTProcessor$xsl) {
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
 *
 * @param type $target
 * @param type $recursive
 *
 * @return Iterator|SplFileInfo[]
 */
function islandora_ohms_get_iterator($target, $recursive) {
  $filter =
  function (SplFileInfo$item) {
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

