<?php

/**
 * @file
 * Admin form functions for the OHMS solution pack.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ohms_admin(array $form, array&$form_state) {
  $form['ohms_viewer_link_xml'] = array(
    '#type' => 'checkbox',
    '#title' => 'Link to the OHMS XML',
    '#description' => 'Link to the XML transcript produced by OHMS.',
    '#default_value' => variable_get('islandora_ohms_viewer_link_xml', FALSE),
  );
  
  $form['islandora_ohms_tmpdir'] = array(
    '#type' => 'textfield',
    '#title' => 'Temporary Directory',
    '#description' => 'OHMS temporary directory for XML files.',
    '#default_value' => variable_get('islandora_ohms_tmpdir', sys_get_temp_dir()),
  );
  
  $form['islandora_ohms_players'] = array(
    '#type' => 'textfield',
    '#title' => 'OHMS Players',
    '#description' => 'Comma-separated list of players that OHMS can use.',
    '#default_value' => variable_get('islandora_ohms_players', 'other,brightcove,kaltura,youtube'),
  );
  
  $form['islandora_ohms_timezone'] = array(
    '#type' => 'textfield',
    '#title' => 'Timezone',
    '#description' => 'Timezone to use for the player.',
    '#default_value' => variable_get('islandora_ohms_timezone', date_default_timezone(false)),
  );
  
  $form['islandora_ohms_collection'] = array(
    '#type' => 'textfield',
    '#title' => 'Collection Name',
    '#description' => 'Name of the collection, as it appears in the OHMS application and the XML it exports.',
    '#default_value' => variable_get('islandora_ohms_collection', ''),
  );

  $form['islandora_ohms_contactemail'] = array(
    '#type' => 'textfield',
    '#title' => 'Contact Email',
    '#description' => 'Email address of the primary contact for the collection.',
    '#default_value' => variable_get('islandora_ohms_contactemail', ''),
  );
  
  $form['islandora_ohms_contactlink'] = array(
    '#type' => 'textfield',
    '#title' => 'Contact Link',
    '#description' => 'URL for the contact for the collection.',
    '#default_value' => variable_get('islandora_ohms_contactlink', ''),
  );
  
  $form['islandora_ohms_copyright'] = array(
    '#type' => 'textfield',
    '#title' => 'Copyright holder',
    '#description' => 'Name of one or more copyright holders, in &lt;span> tags.',
    '#default_value' => variable_get('islandora_ohms_copyright', ''),
  );
  
//  $form[''] = array(
//    '#type' => 'textfield',
//    '#title' => '',
//    '#description' => '',
//    '#default_value' => variable_get('', ''),
//  );
  
  return system_settings_form($form);
}
