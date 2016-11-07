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
   * Batch injest parameters.
   *
   * @var array
   */
  protected $params;

  /**
   * Constructor.
   *
   * @param IslandoraTuque $connection
   *   Connection object.
   * @param string $path
   *   Path to the object.
   * @param array $params
   *   Parameters for ingestion.
   */
  public function __construct(IslandoraTuque $connection, $path, array $params) {
    parent::__construct(NULL, $connection->repository);
    $this->filePath = $path;
    $this->params = $params;
  }

  /**
   * Assemble the base datastreams.
   *
   * @return int
   *   One of the ISLANDORA_BATCH_STATE__* constants.
   */
  public function batchProcess() {
    // OBJ datastream.
    $obj = $this->constructDatastream('OBJ', 'M');
    $obj->label = 'OBJ Datastream';
    $obj->mimetype = 'application/xml';
    $obj->setContentFromFile($this->filePath);
    $this->ingestDatastream($obj);

    $tn = $this->getTn();
    if ($tn) {
      $this->ingestDatastream($tn);
    }

    $media = $this->getMedia();
    if ($media) {
      $this->ingestDatastream($media);
    }

    $tr = $this->getTranscript();
    if ($tr) {
      $this->ingestDatastream($tr);
    }
    
    $bio = $this->getBio();
    if($bio) {
      $this->ingestDatastream($bio);
    }
    
    $names = $this->getNames();
    if($names) {
      $this->ingestDatastream($names);
    }

    $dom = new DOMDocument();
    $dom->load($this->filePath);
    $xpath = new DOMXPath($dom);
    $titles = $xpath->query("/ROOT/record/title");
    if ($titles->length > 0) {
      $this->label = $titles->item(0)->nodeValue;
    }
    else {
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

  /**
   * Generate a thumbnail data stream for the object.
   *
   * @return DataStream|null
   *   The generated datastream, ready for ingest or null if one wasn't found.
   */
  private function getTn() {
    $pathinfo = pathinfo($this->filePath);
    $dir_name = $pathinfo['dirname'];
    $file_name = $pathinfo['filename'];
    $basename = basename($file_name, '.xml');
    $tn_file_name = $dir_name . DIRECTORY_SEPARATOR . $basename . '.TN.jpg';

    if (file_exists($tn_file_name)) {
      $tn = $this->constructDatastream('TN');
      $tn->setContentFromFile($tn_file_name);
      $tn->mimetype = 'image/jpeg';
      $tn->label = 'Thumbnail';
      return $tn;
    }
    return NULL;
  }

  /**
   * Generate a transcript data stream for the object.
   *
   * @return DataStream|null
   *   The generated datastream, ready for ingest or null if one wasn't found.
   */
  private function getTranscript() {
    $pathinfo = pathinfo($this->filePath);
    $dir_name = $pathinfo['dirname'];
    $file_name = $pathinfo['filename'];
    $basename = basename($file_name, '.xml');
    $tr_file_name = $dir_name . DIRECTORY_SEPARATOR . $basename . '.TRANSCRIPT.pdf';
    if (file_exists($tr_file_name)) {
      $tr = $this->constructDatastream('TRANSCRIPT');
      $tr->setContentFromFile($tr_file_name);
      $tr->mimetype = 'application/pdf';
      $tr->label = 'Transcript';
      return $tr;
    }
    return NULL;
  }

  /**
   * Generate a transcript data stream for the object.
   *
   * @return DataStream|null
   *   The generated datastream, ready for ingest or null if one wasn't found.
   */
  private function getBio() {
    $pathinfo = pathinfo($this->filePath);
    $dir_name = $pathinfo['dirname'];
    $file_name = $pathinfo['filename'];
    $basename = basename($file_name, '.xml');
    $tr_file_name = $dir_name . DIRECTORY_SEPARATOR . $basename . '.BIO.txt';
    if (file_exists($tr_file_name)) {
      $tr = $this->constructDatastream('BIO');
      $tr->setContentFromFile($tr_file_name);
      $tr->mimetype = 'text/plain';
      $tr->label = 'Biography';
      return $tr;
    }
    return NULL;
  }

  /**
   * Generate a transcript data stream for the object.
   *
   * @return DataStream|null
   *   The generated datastream, ready for ingest or null if one wasn't found.
   */
  private function getNames() {
    $pathinfo = pathinfo($this->filePath);
    $dir_name = $pathinfo['dirname'];
    $file_name = $pathinfo['filename'];
    $basename = basename($file_name, '.xml');
    $tr_file_name = $dir_name . DIRECTORY_SEPARATOR . $basename . '.NAMES.csv';
    if (file_exists($tr_file_name)) {
      $tr = $this->constructDatastream('NAMES');
      $tr->setContentFromFile($tr_file_name);
      $tr->mimetype = 'text/plain';
      $tr->label = 'Names Referenced';
      return $tr;
    }
    return NULL;
  }

  /**
   * Generate a media data stream for the object.
   *
   * @return DataStream|null
   *   The generated datastream, ready for ingest or null if one wasn't found.
   */
  private function getMedia() {
    $pathinfo = pathinfo($this->filePath);
    $dir_name = $pathinfo['dirname'];
    $file_name = $pathinfo['filename'];
    $basename = basename($file_name, '.xml');

    $media_extensions = array('mp3', 'mp4', 'avi');
    foreach ($media_extensions as $ext) {
      $path = "{$dir_name}/{$basename}.{$ext}";
      if (file_exists($path)) {
        $media = $this->constructDatastream('MEDIA');
        $media->setContentFromFile($path);
        $media->mimetype = mime_content_type($path);
        $media->label = 'Media file';
        return $media;
      }
    }
    return NULL;
  }

}
