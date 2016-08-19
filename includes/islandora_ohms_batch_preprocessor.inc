<?php

class IslandoraOhmsBatchPreprocessor extends IslandoraBatchPreprocessor {

	/**
	 * @param type $target
	 * @param type $recursive
	 * @return Iterator|SplFileInfo[]
	 */
	private function get_iterator($target, $recursive) {
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

	public function preprocess() {
		$target = $this->parameters['target'];
		$recursive = $this->parameters['recursive'];
		$queue = [];
		$iterator = $this->get_iterator($target, $recursive);
		module_load_include('inc', 'islandora_ohms', 'IslandoraOhmsBatchObject');
		
		foreach($iterator as $item) {
			$name = $item->getFilename();
			if ($name[0] === '.') {
				continue;
			}
			drupal_set_message($item->getPathname());
			$obj = new IslandoraOhmsBatchObject(
				$this->connection,
				$item->getPathname(),
				$this->parameters
			);
			$this->addToDatabase($obj);
			drupal_set_message('Item id ' . $obj->getBatchId());
			$queue[] = $obj;
		}
		return $queue;
	}

}
