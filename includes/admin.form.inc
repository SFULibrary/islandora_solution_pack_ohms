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
  $form = array(
    'link_xml_source' => array(
      '#type' => 'checkbox',
      '#title' => 'Link to XML',
      '#description' => 'Let visitors download the XML source files by clicking a link.',
      '#default_value' => variable_get('islandora_ohms_link_xml', FALSE),

    ),
  );
  return system_settings_form($form);
}
