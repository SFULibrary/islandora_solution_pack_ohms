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

		// TN datastream.
		if ($tn = $this->getTn()) {
			$this->ingestDatastream($tn);
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
		if (file_exists($mods_file_path)) {
			$mods = $this->constructDatastream('MODS', 'M');
			$mods->setContentFromFile($mods_file_path, FALSE);
		} else {
			// In the absence of a poper MODS file, generate a minimal one.
			$mods = $this->constructDatastream('MODS', 'M');
			$mods->content = <<<EOQ
<mods xmlns='http://www.loc.gov/mods/v3'>
  <titleInfo>
    <title>{$file_name}</title>
  </titleInfo>
</mods>
EOQ;
		}
		$mods->mimetype = 'application/xml';
		$mods->label = 'MODS metadata';
		return $mods;
	}

}
