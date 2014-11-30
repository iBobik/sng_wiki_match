<?php
// $Id: user.inc,v 1.1.2.12 2011/02/02 18:40:18 swentel Exp $

/**
 * @file
 * Display Suite user entity file.
 */

/**
 * Implements hook_ds_fields().
 */
function user_ds_fields($entity_type, $bundle, $view_mode) {
  $fields = array();

  // Username
  $fields['name'] = array(
    'title' => 'Username',
    'type' => DS_FIELD_TYPE_FUNCTION,
    'status' => DS_FIELD_STATUS_STATIC,
    'function' => 'ds_render_title_field',
    'properties' => array(
      'formatters' => array(
        'user_name_nolink_h1' => t('H1 title'),
        'user_name_link_h1' => t('H1 title, linked to user'),
        'user_name_nolink_h2' => t('H2 title'),
        'user_name_link_h2' => t('H2 title, linked to user'),
        'user_name_nolink_h2_block' => t('H2 block title'),
        'user_name_link_h2_block' => t('H2 block title, linked to user'),
        'user_name_nolink_p' => t('Paragraph title'),
        'user_name_link_p' => t('Paragraph title, linked to user'),
        'user_name_nolink' => t('Plain text'),
        'user_name_link' => t('Plain text, linked to user'),
      ),
    )
  );

  // User signature
  if (variable_get('user_signatures', 0)) {
    $fields['user_signature'] = array(
      'title' => t('User signature'),
      'type' => DS_FIELD_TYPE_FUNCTION,
      'status' => DS_FIELD_STATUS_STATIC,
      'function' => 'ds_render_markup',
      'properties' => array(
        'key' => 'signature',
        'format' => 'signature_format',
      ),
    );
  }

  // User picture
  if (variable_get('user_pictures', 0)) {
    $key = 'user_picture';
    $type = DS_FIELD_TYPE_IGNORE;
    $picture_formatters = array();
    if (module_exists('image')) {
      $key = 'ds_user_picture';
      $type = DS_FIELD_TYPE_FUNCTION;
      $styles = image_styles();
      foreach ($styles as $formatter) {
        $picture_formatters['ds_picture_'. $formatter['name']] = ucfirst(str_replace('_', ' ', $formatter['name']));
      }
    }
    else {
      $picture_formatters['default'] = t('Default');
    }
    $fields[$key] = array(
      'title' => t('User picture'),
      'type' => $type,
      'status' => DS_FIELD_STATUS_STATIC,
      'properties' => array(
        'formatters' => $picture_formatters,
      ),
    );
    if ($type == DS_FIELD_TYPE_FUNCTION) {
      $fields[$key]['function'] = 'ds_render_user_picture';
    }
  }

  return array('user' => $fields);
}
