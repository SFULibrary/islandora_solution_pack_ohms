<?php
function islandora_ohms_build_viewer(&$variables) {
  $path = drupal_get_path('module', 'islandora_ohms');
  require $path . '/lib/ohms-viewer/vendor/autoload.php';
  $islandora_object = $variables['islandora_object'];
  $controller = new \Ohms\ViewerController(
    NULL, $islandora_object['OBJ']->content, islandora_ohms_settings()
  );
  ob_start();
  $controller->route(NULL, NULL, 'Allen Allens', 'minimal');
  $output = ob_get_clean();

  $variables['islandora_content'] = $output;
}

function islandora_ohms_player($object) {
  $content = $object['OBJ']->content;
  $xml = simplexml_load_string($content);
  $clipSource = (string) $xml->record->mediafile->host;
  $playerName = strtolower($clipSource);
  $players = explode(',', variable_get('islandora_ohms_players', 'other,brightcove,kaltura,youtube'));
  if (in_array($playerName, $players)) {
    return $playerName;
  }
  return 'other';
}

function islandora_ohms_assets($playerName) {
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');

  $path = drupal_get_path('module', 'islandora_ohms');
  drupal_add_css($path . '/css/ohms_viewer.css');

  drupal_add_css($path . "/lib/ohms-viewer/skin/jplayer.blue.monday.css");

  $cssPath = $path . '/lib/ohms-viewer/css';
  drupal_add_css($cssPath . "/jquery-ui.toggleSwitch.css");
  drupal_add_css($cssPath . "/jquery-ui-1.8.16.custom.css");
  drupal_add_css($cssPath . "/font-awesome.css");


  $jsPath = $path . '/lib/ohms-viewer/js';
  drupal_add_css($jsPath . "/fancybox_2_1_5/source/jquery.fancybox.css");
  drupal_add_css($jsPath . "/fancybox_2_1_5/source/helpers/jquery.fancybox-buttons.css");
  drupal_add_css($jsPath . "/fancybox_2_1_5/source/helpers/jquery.fancybox-thumbs.css");

  drupal_add_js('//code.jquery.com/jquery-migrate-1.4.1.min.js');
  drupal_add_js('//ajax.googleapis.com/ajax/libs/jqueryui/1.8.18/jquery-ui.min.js');
  drupal_add_js($jsPath . '/jquery-ui.toggleSwitch.js');
  drupal_add_js($jsPath . "/jquery.jplayer.min.js");
  drupal_add_js($jsPath . "/jquery.easing.1.3.js");
  drupal_add_js($jsPath . "/jquery.scrollTo-min.js");
  drupal_add_js($jsPath . "/fancybox_2_1_5/source/jquery.fancybox.pack.js");
  drupal_add_js($jsPath . "/fancybox_2_1_5/source/helpers/jquery.fancybox-buttons.js");
  drupal_add_js($jsPath . "/fancybox_2_1_5/source/helpers/jquery.fancybox-media.js");
  drupal_add_js($jsPath . "/fancybox_2_1_5/source/helpers/jquery.fancybox-thumbs.js");
  drupal_add_js($jsPath . "/viewer.js");
  drupal_add_js($jsPath . '/viewer_' . $playerName . '.js');
  drupal_add_js($jsPath . "/viewer_init.js");
}

/**
 * Implements template_preprocess_hook().
 */
function islandora_ohms_preprocess_islandora_ohms(array & $variables) {
  $islandora_object = $variables['islandora_object'];
  module_load_include('inc', 'islandora', 'includes/datastream');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/metadata');

  // We should eventually remove the DC object and dc_array code as it only
  // exists to not break legacy implementations.
  try {
    if (isset($islandora_object['DC']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['DC'])) {
      $dc = $islandora_object['DC']->content;
      $dc_object = DublinCore::importFromXMLString($dc);
    }
  }
  catch(Exception$e) {
    drupal_set_message(t('Error retrieving object %s %t', array('%s' => $islandora_object->id, '%t' => $e->getMessage())), 'error', FALSE);
  }
  $variables['islandora_dublin_core'] = isset($dc_object) ? $dc_object : NULL;
  $variables['dc_array'] = isset($dc_object) ? $dc_object->asArray() : array();
  $variables['islandora_object_label'] = $islandora_object->label;
  $variables['theme_hook_suggestions'][] = 'islandora_ohms__' . str_replace(':', '_', $islandora_object->id);
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($islandora_object);
  $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
  $variables['description'] = islandora_retrieve_description_markup($islandora_object);
  $variables['searchUrl'] = url('islandora/ohms/search');

  islandora_ohms_build_viewer($variables);
  $playerName = islandora_ohms_player($islandora_object);
  $variables['playerName'] = $playerName;
  islandora_ohms_assets($playerName);
}

