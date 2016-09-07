<?php

/**
 * @file
 * Theming functions for the OHMS Viewer.
 */

function islandora_ohms_modify_content($content) {
  $dom = new DOMDocument();
  $dom->loadHTML('<?xml encoding="utf-8" ?>' . $content);

  $header = $dom->getElementById('header');
  if ($header) {
    $header->setAttribute('id', 'ohms-header');
    $header->setAttribute('class', 'center');
  }
  $headerVid = $dom->getElementById('headervid');
  if ($headerVid) {
    $headerVid->setAttribute('id', 'ohms-headervid');
    $headerVid->setAttribute('class', 'center');
  }
  $headerImg = $dom->getElementById('headerimg');
  if ($headerImg) {
    $headerImg->setAttribute('id', 'ohms-headerimg');
  }

  $main = $dom->getElementById('main');
  $main->setAttribute('id', 'ohms-main');
  $main->setAttribute('class', 'center');

  return $dom->saveHTML();
}

/**
 * Generate the HTML for the viewer.
 *
 * @param array $variables
 *   Drupal theming variables.
 */
function islandora_ohms_build_viewer(array &$variables) {
  $path = drupal_get_path('module', 'islandora_ohms');
  require $path . '/lib/ohms-viewer/vendor/autoload.php';
  $islandora_object = $variables['islandora_object'];
  $controller = new \Ohms\ViewerController(
    NULL, $islandora_object['OBJ']->content, islandora_ohms_settings()
  );
  ob_start();
  $controller->route(NULL, NULL, 'Allen Allens', 'minimal');
  $output = ob_get_clean();

  $variables['islandora_content'] = islandora_ohms_modify_content($output);
}

/**
 * Return the name of the media player to use in the viewer.
 *
 * @param FedoraObject $object
 *   The object being viewed.
 *
 * @return string
 *   The name of the player.
 */
function islandora_ohms_player(FedoraObject $object) {
  $content = $object['OBJ']->content;
  $xml = simplexml_load_string($content);
  $clip_source = (string) $xml->record->mediafile->host;
  $player_name = strtolower($clip_source);
  $players = explode(',', variable_get('islandora_ohms_players', 'other,brightcove,kaltura,youtube'));
  if (in_array($player_name, $players)) {
    return $player_name;
  }
  return 'other';
}
/**
 * Load all the required javascript and css files for the player.
 *
 * @param string $player_name
 *   The name of the player to use to view the media.
 */
function islandora_ohms_assets($player_name) {
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');

  $path = drupal_get_path('module', 'islandora_ohms');
  drupal_add_css($path . '/css/ohms_viewer.css');

  drupal_add_css($path . "/lib/ohms-viewer/skin/jplayer.blue.monday.css");

  $css_path = $path . '/lib/ohms-viewer/css';
  drupal_add_css($css_path . "/jquery-ui.toggleSwitch.css");
  drupal_add_css($css_path . "/jquery-ui-1.8.16.custom.css");
  drupal_add_css($css_path . "/font-awesome.css");

  $js_path = $path . '/lib/ohms-viewer/js';
  drupal_add_css($js_path . "/fancybox_2_1_5/source/jquery.fancybox.css");
  drupal_add_css($js_path . "/fancybox_2_1_5/source/helpers/jquery.fancybox-buttons.css");
  drupal_add_css($js_path . "/fancybox_2_1_5/source/helpers/jquery.fancybox-thumbs.css");

  drupal_add_js('//code.jquery.com/jquery-migrate-1.4.1.min.js');
  drupal_add_js('//ajax.googleapis.com/ajax/libs/jqueryui/1.8.18/jquery-ui.min.js');
  drupal_add_js($js_path . '/jquery-ui.toggleSwitch.js');
  drupal_add_js($js_path . "/jquery.jplayer.min.js");
  drupal_add_js($js_path . "/jquery.easing.1.3.js");
  drupal_add_js($js_path . "/jquery.scrollTo-min.js");
  drupal_add_js($js_path . "/fancybox_2_1_5/source/jquery.fancybox.pack.js");
  drupal_add_js($js_path . "/fancybox_2_1_5/source/helpers/jquery.fancybox-buttons.js");
  drupal_add_js($js_path . "/fancybox_2_1_5/source/helpers/jquery.fancybox-media.js");
  drupal_add_js($js_path . "/fancybox_2_1_5/source/helpers/jquery.fancybox-thumbs.js");
  drupal_add_js($js_path . "/viewer.js");
  drupal_add_js($js_path . '/viewer_' . $player_name . '.js');
  drupal_add_js($js_path . "/viewer_init.js");
}

/**
 * Implements template_preprocess_hook().
 */
function islandora_ohms_preprocess_islandora_ohms_viewer(array & $variables) {
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
  catch (Exception$e) {
    drupal_set_message(t('Error retrieving object %s %t', array('%s' => $islandora_object->id, '%t' => $e->getMessage())), 'error', FALSE);
  }
  $variables['islandora_dublin_core'] = isset($dc_object) ? $dc_object : NULL;
  $variables['dc_array'] = isset($dc_object) ? $dc_object->asArray() : array();
  $variables['islandora_object_label'] = $islandora_object->label;
  $variables['theme_hook_suggestions'][] = 'islandora_ohms__' . str_replace(':', '_', $islandora_object->id);
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($islandora_object);
  $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
  $variables['description'] = islandora_retrieve_description_markup($islandora_object);

  islandora_ohms_build_viewer($variables);
  $player_name = islandora_ohms_player($islandora_object);
  $variables['playerName'] = $player_name;
  islandora_ohms_assets($player_name);
}

/**
 * Implements template_preprocess_hook().
 */
function islandora_ohms_preprocess_islandora_ohms_metadata(array & $variables) {
  $islandora_object = $variables['islandora_object'];
  module_load_include('inc', 'islandora', 'includes/datastream');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/metadata');

  $path = drupal_get_path('module', 'islandora_ohms');
  drupal_add_css($path . '/css/ohms_metadata.css');

  $variables['islandora_object_label'] = $islandora_object->label;
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($islandora_object);
}
