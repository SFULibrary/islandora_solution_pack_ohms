<?php

/**
 * @file
 * Implementation of classes from the batch ingest framework.
 */

/**
 * Batch object class.
 */
class IslandoraOhmsBatchObject extends IslandoraBatchObject {

	/**
	 * Path to the OHMS XML file for the object.
	 *
	 * @var string
	 */
	protected $filePath;

	/**
	 * Batch injest parameters
	 *
	 * @var array
	 */
	protected $params;

	/**
	 * Constructor.
	 */
	public function __construct(IslandoraTuque $connection, $path, $params) {
		parent::__construct(NULL, $connection->repository);
		$this->filePath = $path;
		$this->params = $params;
	}

	/**
	 * Assemble the base datastreams.
	 */
	public function batchProcess() {
		// OBJ datastream.
		$obj = $this->constructDatastream('OBJ', 'M');
		$obj->label = 'OBJ Datastream';
		$obj->mimetype = 'application/xml';
		$obj->setContentFromFile($this->filePath);
		$this->ingestDatastream($obj);

		$mods = $this->getMods();
		$this->ingestDatastream($mods);
		
		$dc = $this->getDc();
		$this->ingestDatastream($dc);
    
    $tn = $this->getTn();
    if($tn) {
      $this->ingestDatastream($tn);
    }

		$dom = new DOMDocument();
		$dom->loadXML($dc->content);
		$xpath = new DOMXPath($dom);
		$xpath->registerNamespace('dc', 'http://purl.org/dc/elements/1.1/');
		$titles = $xpath->query("//dc:title");
		if ($titles->length > 0) {
			$this->label = $titles->item(0)->nodeValue;
		} else {
			$this->label = '[No object label available]';
		}

		$this->addRelationships();

		// Indicate that this object is ready to be ingested.
		return ISLANDORA_BATCH_STATE__DONE;
	}

	/**
	 * Add relationships.
	 */
	public function addRelationships() {
		$this->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $this->params['parent']);
		$this->models = 'islandora:sp_ohms';
	}

	/**
	 * Get a list of resources.
	 */
	public function getResources() {
		return array();
	}

	private function getMods() {
		$pathinfo = pathinfo($this->filePath);
		$dir_name = $pathinfo['dirname'];
		$file_name = $pathinfo['filename'];
		$mods_file_path = $dir_name . DIRECTORY_SEPARATOR . $file_name . '.MODS.xml';
		$mods = $this->constructDatastream('MODS', 'M');
		$mods->setContentFromFile($mods_file_path);
		$mods->mimetype = 'application/xml';
		$mods->label = 'MODS metadata';
		return $mods;
	}

	private function getDc() {
		$pathinfo = pathinfo($this->filePath);
		$dir_name = $pathinfo['dirname'];
		$file_name = $pathinfo['filename'];
		$mods_file_path = $dir_name . DIRECTORY_SEPARATOR . $file_name . '.DC.xml';
		$mods = $this->constructDatastream('DC', 'M');
		$mods->setContentFromFile($mods_file_path);
		$mods->mimetype = 'application/xml';
		$mods->label = 'MODS metadata';
		return $mods;
	}
  
  private function getTn() {    
		$pathinfo = pathinfo($this->filePath);
		$dir_name = $pathinfo['dirname'];
		$file_name = $pathinfo['filename'];
    $basename = basename($file_name, '.xml');
    $tn_file_name = $dir_name . DIRECTORY_SEPARATOR . $basename . '.TN.jpg';

    if(file_exists($tn_file_name)) {
      $tn = $this->constructDatastream('TN', 'M');
      $tn->setContentFromFile($tn_file_name);
      $tn->mimetype = 'image/jpeg';
      $tn->label = 'Thumbnail';
      return $tn;
    }
    return null;
  }
}