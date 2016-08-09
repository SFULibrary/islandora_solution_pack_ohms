<?php

/**
 * @file
 * Admin functions for the OHMS viewer.
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
function islandora_ohms_viewer_admin(array $form, array &$form_state) {
  $form['ohms_viewer_link_xml'] = array(
    '#type' => 'checkbox',
    '#title' => 'Link to the OHMS XML',
    '#description' => 'Link to the XML transcript produced by OHMS.',
    '#default_value' => variable_get('islandora_ohms_viewer_link_xml', FALSE),
  );

  return system_settings_form($form);
}
