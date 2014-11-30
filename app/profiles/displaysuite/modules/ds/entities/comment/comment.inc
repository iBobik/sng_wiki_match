<?php
// $Id: comment.inc,v 1.1.2.5 2011/02/02 18:40:18 swentel Exp $

/**
 * @file
 * Display Suite comment entity file.
 */

/**
 * Implements hook_ds_fields().
 */
function comment_ds_fields($entity_type, $bundle, $view_mode) {
  $fields = array();

  // Links.
  $fields['links'] = array(
    'title' => t('Links'),
    'type' => DS_FIELD_TYPE_IGNORE,
    'status' => DS_FIELD_STATUS_STATIC,
  );

  // Created time.
  $format_types = system_get_date_types();
  $date_formatters = array();
  foreach ($format_types as $formatter) {
    $date_formatters['ds_post_date_'. $formatter['type']] = t($formatter['title']);
  }
  $fields['post_date'] = array(
    'title' => t('Post date'),
    'type' => DS_FIELD_TYPE_FUNCTION,
    'status' => DS_FIELD_STATUS_STATIC,
    'function' => 'ds_render_date_field',
    'properties' => array(
      'formatters' => $date_formatters,
    ),
  );

  // Permalink.
  $fields['permalink'] = array(
    'title' => t('Permalink'),
    'type' => DS_FIELD_TYPE_PREPROCESS,
    'status' => DS_FIELD_STATUS_STATIC,
  );

  // Submitted.
  $fields['submitted'] = array(
    'title' => t('Submitted'),
    'type' => DS_FIELD_TYPE_PREPROCESS,
    'status' => DS_FIELD_STATUS_STATIC,
  );

  // Title.
  $fields['title'] = array(
    'title' => t('Title'),
    'type' => DS_FIELD_TYPE_PREPROCESS,
    'status' => DS_FIELD_STATUS_STATIC,
  );

  // Author.
  $fields['author'] = array(
    'title' => t('Author'),
    'type' => DS_FIELD_TYPE_PREPROCESS,
    'status' => DS_FIELD_STATUS_STATIC,
  );

  // User signature.
  if (variable_get('user_signatures', 0)) {
    $fields['signature'] = array(
      'title' => t('User signature'),
      'type' => DS_FIELD_TYPE_PREPROCESS,
      'status' => DS_FIELD_STATUS_STATIC,
    );
  }

  // User picture
  if (variable_get('user_pictures', 0)) {
    $key = 'picture';
    $type = DS_FIELD_TYPE_PREPROCESS;
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

  return array('comment' => $fields);
}