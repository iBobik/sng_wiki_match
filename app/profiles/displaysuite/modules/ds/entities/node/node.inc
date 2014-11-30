<?php
// $Id: node.inc,v 1.1.2.15 2011/02/02 16:09:24 swentel Exp $

/**
 * @file
 * Display Suite node entity file.
 */

/**
 * Implements hook_ds_fields().
 */
function node_ds_fields($entity_type, $bundle, $view_mode) {
  $fields = array();

  // Title.
  $fields['title'] = array(
    'title' => t('Title'),
    'type' => DS_FIELD_TYPE_FUNCTION,
    'status' => DS_FIELD_STATUS_STATIC,
    'function' => 'ds_render_title_field',
    'properties' => array(
      'formatters' => array(
        'node_title_nolink_h1' => t('H1 title'),
        'node_title_link_h1' => t('H1 title, linked to node'),
        'node_title_nolink_h2' => t('H2 title'),
        'node_title_link_h2' => t('H2 title, linked to node'),
        'node_title_nolink_h2_block' => t('H2 block title'),
        'node_title_link_h2_block' => t('H2 block title, linked to node'),
        'node_title_nolink_p' => t('Paragraph title'),
        'node_title_link_p' => t('Paragraph title, linked to node'),
        'node_title_nolink' => t('Plain text'),
        'node_title_link' => t('Plain text, linked to node'),
      ),
    )
  );

  // Links.
  $fields['links'] = array(
    'title' => t('Links'),
    'type' => DS_FIELD_TYPE_IGNORE,
    'status' => DS_FIELD_STATUS_STATIC,
  );

  // Comments.
  if (module_exists('comment') && ($view_mode == 'full' || $view_mode == 'default')) {
    $fields['comments'] = array(
      'title' => t('Comments'),
      'type' => DS_FIELD_TYPE_IGNORE,
      'status' => DS_FIELD_STATUS_STATIC,
    );
  }

  // Read more link.
  $fields['read_more'] = array(
    'title' => t('Read more'),
    'type' => DS_FIELD_TYPE_CODE,
    'status' => DS_FIELD_STATUS_DEFAULT,
    'properties' => array(
      'code' => array(
        'value' => '<?php echo l("Read more", "node/" . $entity->nid); ?>',
        'format' => 'ds_code',
      ),
    ),
  );

  // Author.
  $fields['author'] = array(
    'title' => t('Author'),
    'type' => DS_FIELD_TYPE_FUNCTION,
    'status' => DS_FIELD_STATUS_STATIC,
    'function' => 'ds_render_author_field',
    'properties' => array(
      'formatters' => array(
        'author' => t('Author'),
        'author_linked' => t('Author linked to profile')
      ),
    ),
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

  return array('node' => $fields);
}
