<?php
class IslandoraOhmsBatchPreprocessor extends IslandoraBatchPreprocessor {

  public function preprocess() {
    module_load_include('inc', 'islandora_ohms', 'includes/utilities');

    $target = $this->parameters['target'];
    $recursive = $this->parameters['recursive'];
    $queue = [];
    $iterator = islandora_ohms_get_iterator($target, $recursive);
    module_load_include('inc', 'islandora_ohms', 'IslandoraOhmsBatchObject');

    foreach ($iterator as $item) {
      $name = $item->getFilename();
      if ($name[0] === '.') {
        continue;
      }
      drupal_set_message($item->getPathname());
      $obj = new IslandoraOhmsBatchObject(
        $this->connection, $item->getPathname(), $this->parameters
      );
      $this->addToDatabase($obj);
      drupal_set_message('Item id ' . $obj->getBatchId());
      $queue[] = $obj;
    }
    return $queue;
  }
}

