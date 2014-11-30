<?php
// $Id: ds.registry.inc,v 1.1.2.21 2011/02/03 17:08:44 swentel Exp $

/**
 * @file
 * Registry file for Display Suite.
 */

/**
 * Implements hook_menu().
 */
function _ds_menu() {
  $items = array();

  // Start page of Display Suite.
  $items['admin/structure/ds'] = array(
    'title' => 'Display suite',
    'description' => 'Manage configuration for Display Suite.',
    'page callback' => 'system_admin_menu_block_page',
    'access arguments' => array('access administration pages'),
    'file' => 'system.admin.inc',
    'file path' => drupal_get_path('module', 'system'),
  );

  // Custom styles.
  $items['admin/structure/ds/styles'] = array(
    'title' => 'Styles',
    'description' => 'Define classes which you can use as classes for regions.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_styles_form'),
    'file' => 'ds.styles.inc',
    'access arguments' => array('admin_display_suite'),
  );

  // Custom styles.
  $items['admin/structure/ds/layout'] = array(
    'title' => 'Layout',
    'description' => 'Displays an overview of all entities.',
    'page callback' => 'ds_layout_list',
    'file' => 'ds.layout.inc',
    'access arguments' => array('admin_display_suite'),
  );

  // Custom view modes.
  $items['admin/structure/ds/view_modes'] = array(
    'title' => 'View modes',
    'description' => 'Manage custom view modes for all content.',
    'page callback' => 'ds_custom_view_modes_list',
    'file' => 'ds.view_modes.inc',
    'access arguments' => array('admin_view_modes'),
  );
  $items['admin/structure/ds/view_modes/manage'] = array(
    'title' => 'Add a view mode',
    'description' => 'Manage a view mode',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_edit_view_mode_form'),
    'file' => 'ds.view_modes.inc',
    'access arguments' => array('admin_view_modes'),
    'type' => MENU_LOCAL_ACTION,
  );
  $items['admin/structure/ds/view_modes/delete'] = array(
    'title' => 'Delete a view mode',
    'description' => 'Delete a custom view mode.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_delete_view_mode_confirm'),
    'file' => 'ds.view_modes.inc',
    'access arguments' => array('admin_view_modes'),
    'type' => MENU_CALLBACK,
  );

  // Fields.
  $items['admin/structure/ds/fields'] = array(
    'title' => 'Fields',
    'description' => 'Manage fields for all content.',
    'page callback' => 'ds_custom_fields_list',
    'file' => 'ds.fields.inc',
    'access arguments' => array('admin_fields'),
  );
  $items['admin/structure/ds/fields/manage'] = array(
    'title' => 'Fields edit',
    'page callback' => 'ds_custom_manage',
    'file' => 'ds.fields.inc',
    'access arguments' => array('admin_fields'),
    'type' => MENU_CALLBACK,
  );
  $items['admin/structure/ds/fields/manage_custom'] = array(
    'title' => 'Add a field',
    'description' => 'Manage a field',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_edit_custom_field_form'),
    'file' => 'ds.fields.inc',
    'access arguments' => array('admin_view_modes'),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 0,
  );
  $items['admin/structure/ds/fields/delete'] = array(
    'title' => 'Delete a field',
    'description' => 'Delete a field.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_delete_field_confirm'),
    'file' => 'ds.fields.inc',
    'access arguments' => array('admin_fields'),
    'type' => MENU_CALLBACK,
  );
  // Block can be disabled.
  if (module_exists('block')) {
    $items['admin/structure/ds/fields/manage_block'] = array(
      'title' => 'Add a block field',
      'description' => 'Manage a block field',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('ds_edit_block_field_form'),
      'file' => 'ds.fields.inc',
      'access arguments' => array('admin_view_modes'),
      'type' => MENU_LOCAL_ACTION,
      'weight' => 1,
    );
  }

  return $items;
}

/**
 * Implements hook_theme().
 */
function _ds_theme() {
  $theme_functions = array();

  // Layouts
  $layouts = module_invoke_all('ds_layouts');
  foreach ($layouts as $key => $layout) {
    $theme_functions[$key] = array(
      'render element' => 'elements',
      'template' => 'layouts/' . $key . '/' . strtr($key, '_', '-'),
    );
  }

  // Load theming functions from entities we implement.
  $entities = ds_load_entity_implementations(TRUE);
  foreach ($entities as $entity_type) {
    $entity_type_theme_function = $entity_type . '_ds_theme_registry';
    if (function_exists($entity_type_theme_function)) {
      $theme_functions += $entity_type_theme_function();
    }
  }

  return $theme_functions;
}

/**
 * Implements hook_theme_registry_alter().
 */
function _ds_theme_registry_alter(&$theme_registry) {

  // Inject ds_preprocess_entity in all base theming functions. User profile is
  // different than the rest, using user_profile though :(
  $entity_info = entity_get_info();
  foreach ($entity_info as $entity => $info) {
    if ($entity_info[$entity]['fieldable']) {
      if ($entity == 'user') $entity = 'user_profile';
      $theme_registry[$entity]['preprocess functions'][] = 'ds_preprocess_entity';
    }
  }
}

/**
 * Implements hook_entity_info_alter().
 */
function _ds_entity_info_alter(&$entity_info) {

  // Add custom view modes to entities.
  $custom_view_modes = ds_custom_view_modes();
  foreach ($custom_view_modes as $view_mode_key => $view_mode_value) {
    $view_mode = array(
      'label' => check_plain($view_mode_value->label),
      'custom settings' => FALSE,
    );
    foreach ($view_mode_value->entities as $entity_type) {
      if (isset($entity_info[$entity_type])) {
        $entity_info[$entity_type]['view modes'][$view_mode_key] = $view_mode;
      }
    }
  }

  // Add layout if applicable.
  $ds_layouts = module_invoke_all('ds_layouts');
  $results = db_query('SELECT entity_type, bundle, view_mode, layout, settings FROM {ds_layouts}')->fetchAll();
  foreach ($results as $row) {
    $layout = $ds_layouts[$row->layout];
    $layout['layout'] = $row->layout;
    $layout['settings'] = unserialize($row->settings);
    $entity_info[$row->entity_type]['bundles'][$row->bundle]['layouts'][$row->view_mode] = $layout;
  }
}

/**
 * Implements hook_ds_layouts().
 */
function _ds_ds_layouts() {
  $path = drupal_get_path('module', 'ds');
  $layouts = array(
    'ds_1col' => array(
      'label' => t('One column'),
      'path' => $path . '/layouts/ds_1col',
      'regions' => array(
        'ds_content' => t('Content'),
      ),
    ),
    'ds_2col' => array(
      'label' => t('Two column'),
      'path' => $path . '/layouts/ds_2col',
      'regions' => array(
        'left' => t('Left'),
        'right' => t('Right')
      ),
      'css' => TRUE,
    ),
    'ds_2col_stacked' => array(
      'label' => t('Two column stacked'),
      'path' => $path . '/layouts/ds_2col_stacked',
      'regions' => array(
        'header' => t('Header'),
        'left' => t('Left'),
        'right' => t('Right'),
        'footer' => t('Footer'),
      ),
      'css' => TRUE,
    ),
    'ds_2col_stacked_fluid' => array(
      'label' => t('Two column stacked - fluid'),
      'path' => $path . '/layouts/ds_2col_stacked_fluid',
      'regions' => array(
        'header' => t('Header'),
        'left' => t('Left'),
        'right' => t('Right'),
        'footer' => t('Footer'),
      ),
      'css' => TRUE,
    ),
    'ds_3col' => array(
      'label' => t('Three column - 25/50/25'),
      'path' => $path . '/layouts/ds_3col',
      'regions' => array(
        'left' => t('Left'),
        'middle' => t('Middle'),
        'right' => t('Right'),
      ),
      'css' => TRUE,
    ),
    'ds_3col_equal_width' => array(
      'label' => t('Three column - equal width'),
      'path' => $path . '/layouts/ds_3col_equal_width',
      'regions' => array(
        'left' => t('Left'),
        'middle' => t('Middle'),
        'right' => t('Right'),
      ),
      'css' => TRUE,
    ),
    'ds_3col_stacked' => array(
      'label' => t('Three column stacked - 25/50/25'),
      'path' => $path . '/layouts/ds_3col_stacked',
      'regions' => array(
        'header' => t('Header'),
        'left' => t('Left'),
        'middle' => t('Middle'),
        'right' => t('Right'),
        'footer' => t('Footer'),
      ),
      'css' => TRUE,
    ),
    'ds_3col_stacked_equal_width' => array(
      'label' => t('Three column stacked - equal width'),
      'path' => $path . '/layouts/ds_3col_stacked_equal_width',
      'regions' => array(
        'header' => t('Header'),
        'left' => t('Left'),
        'middle' => t('Middle'),
        'right' => t('Right'),
        'footer' => t('Footer'),
      ),
      'css' => TRUE,
    ),
    'ds_3col_stacked_html5' => array(
      'label' => t('Three column stacked - 25/50/25 (HTML5)'),
      'path' => $path . '/layouts/ds_3col_stacked_html5',
      'regions' => array(
        'header' => t('Header'),
        'left' => t('Left'),
        'middle' => t('Middle'),
        'right' => t('Right'),
        'footer' => t('Footer'),
      ),
      'css' => TRUE,
    ),
    'ds_4col' => array(
      'label' => t('Four column - equal width'),
      'path' => $path . '/layouts/ds_4col',
      'regions' => array(
        'first' => t('First'),
        'second' => t('Second'),
        'third' => t('Third'),
        'fourth' => t('Fourth'),
      ),
       'css' => TRUE,
    ),
  );

  // Get layouts defined in the default theme and base theme (if applicable).
  $theme = variable_get('theme_default');
  _ds_layouts_scan_theme($theme, $layouts);
  $all_themes = list_themes();
  $theme_info = $all_themes[$theme];
  if (isset($theme_info->base_theme) && !empty($theme_info->base_theme)) {
    _ds_layouts_scan_theme($theme_info->base_theme, $layouts);
  }

  return $layouts;
}

function _ds_layouts_scan_theme($theme, &$layouts) {
  $theme_layouts = file_scan_directory(drupal_get_path('theme', $theme) . '/ds_layouts', '/inc/');
  foreach ($theme_layouts as $file => $values) {
    include_once(DRUPAL_ROOT . '/' . $file);
    $function = 'ds_' . $values->name;
    $layouts[$values->name] = $function();
    $layouts[$values->name]['path'] = str_replace('/' . $values->filename, '', $file);
  }
}