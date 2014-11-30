<?php
// $Id: ds.field_ui.inc,v 1.1.2.32 2011/02/02 18:41:13 swentel Exp $

/**
 * @file
 * Field UI functions for Display Suite.
 */

/**
 * Adds the Display Suite fields and layouts to the form.
 */
function ds_field_ui_fields_layouts(&$form, &$form_state) {

  // Get the entity_type, bundle and view mode.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  // Add layout fieldset.
  _ds_field_ui_table_layouts($entity_type, $bundle, $view_mode, $form);

  // Add the fields.
  _ds_field_ui_fields($entity_type, $bundle, $view_mode, $form);

  // Attach js.
  $form['#attached']['js'][] = drupal_get_path('module', 'ds') . '/js/ds.js';

  // Add process function to add the regions.
  $form['#process'][] = 'ds_field_ui_regions';

  // Alter the text of the custom display settings.
  $form['modes']['view_modes_custom']['#description'] = t('<a href="!url">Manage view modes</a>', array('!url' => url('admin/structure/ds/view_modes')));
  $form['display_suite']['modes'] = $form['modes'];
  $form['display_suite']['modes']['#weight'] = -10;
  unset($form['modes']);
}

/**
 * Add Regions to 'Manage display' screen.
 *
 * @param $form
 *   The form to add layout fieldset and extra display suite fields.
 * @param $form_state
 *   The current form state.
 */
function ds_field_ui_regions($form, $form_state) {

  // Get the entity_type, bundle and view mode.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  // Check layout.
  $layout = isset($form['#ds_layout']) ? $form['#ds_layout'] : FALSE;

  // Change UI to add Region column if we have a layout.
  if ($layout) {

    $table = &$form['fields'];

    $table['#header'] = array(
      t('Field'),
      t('Weight'),
      t('Parent'),
      t('Region'),
      t('Label'),
      array('data' => t('Format'), 'colspan' => 3),
    );

    $table['#regions'] = array();
    foreach ($layout->regions as $region_key => $region_title) {
      $region_options[$region_key] = $region_title;
      $table['#regions'][$region_key] = array(
        'title' => $region_title,
        'message' => t('No fields are displayed in this region'),
      );
    }
    $region_options['hidden'] = t('Disabled');
    $table['#regions']['hidden'] = array(
      'title' => t('Disabled'),
      'message' => t('No fields are hidden.')
    );

    $region = array(
      '#type' => 'select',
      '#options' => $region_options,
      '#default_value' => 'hidden',
      '#attributes' => array(
        'class' => array('ds-field-region'),
      )
    );

    // Update existing rows by changing rowHandler and adding regions.
    foreach (element_children($table) as $name) {
      $row = &$table[$name];
      $row['#js_settings'] = array('rowHandler' => 'ds');
      $row['#region_callback'] = 'ds_field_ui_row_region';

      // Remove hidden format.
      if (isset($row['format']['type']['#options']['hidden'])) {
        unset($row['format']['type']['#options']['hidden']);
      }

      // Add region between parent_wrapper & label.
      $second = array_splice($row, 7);
      $row['region'] = $region;
      $row['region']['#default_value'] = (isset($layout->settings['fields'][$name])) ? $layout->settings['fields'][$name] : 'hidden';
      $row = array_merge($row, $second);
    }
  }

  return $form;
}

/**
 * Returns the region to which a row in the Field UI screen belongs.
 *
 * @param $row
 *   The current row that is being rendered in the Field UI screen.
 */
function ds_field_ui_row_region($row) {
  return isset($row['region']['#value']) ? $row['region']['#value'] : 'hidden';
}

/**
 * Save the layout settings from the 'Manage display' screen.
 */
function ds_field_ui_layouts_save($form, &$form_state) {

  // Get some default values.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = isset($form['#view_mode']) ? $form['#view_mode'] : 'form';

  // Determine layout variables.
  $layout = $form_state['values']['display_suite']['layout'];
  $hide_empty_regions = $form_state['values']['display_suite']['hide_empty_regions'];
  $old_layout = $form_state['values']['display_suite']['old_layout'];
  $new_layout = ($layout != $old_layout) || empty($old_layout);

  // Save layout and add regions if necessary.
  $record = new stdClass;
  $record->entity_type = $entity_type;
  $record->bundle = $bundle;
  $record->view_mode = $view_mode;
  $record->layout = $layout;
  $record->settings = array('hide_empty_regions' => $hide_empty_regions);

  // Remove old layout if necessary.
  if ($new_layout && !empty($old_layout)) {
    db_delete('ds_layouts')
      ->condition('entity_type', $entity_type)
      ->condition('bundle', $bundle)
      ->condition('view_mode', $view_mode)
      ->execute();
  }

  if ($new_layout && !empty($layout)) {
    // Save new layout.
    $record->settings = serialize($record->settings);
    drupal_write_record('ds_layouts', $record);
  }
  // Update existing layout.
  elseif (!empty($layout)) {

    $fields = _ds_sort_fields($form_state['values']['fields'], 'weight');

    // Ignore the Field group module.
    foreach ($fields as $key => $field) {
      if ($key == '_add_new_group') {
        continue;
      }
      if (!isset($record->settings['regions'][$field['region']])) {
        $record->settings['regions'][$field['region']] = array();
      }
      $record->settings['regions'][$field['region']][] = $key;
      $record->settings['fields'][$key] = $field['region'];
    }
    $record->settings['classes'] = array();
    foreach (array_keys($form['fields']['#regions']) as $region) {
      // Ignore hidden region.
      if ($region == 'hidden') {
        continue;
      }
      if (isset($form_state['values']['display_suite'][$region])) {
        $record->settings['classes'][$region] = $form_state['values']['display_suite'][$region];
      }
    }
    $record->settings = serialize($record->settings);
    drupal_write_record('ds_layouts', $record, array('entity_type', 'bundle', 'view_mode', 'layout'));
  }
}

/**
 * Move the view modes so Field UI can handle them.
 */
function ds_field_ui_layouts_validate($form, &$form_state) {
  if (isset($form_state['values']['display_suite']['modes']['view_modes_custom'])) {
    $form_state['values']['view_modes_custom'] = $form_state['values']['display_suite']['modes']['view_modes_custom'];
  }
}

/**
 * Save the field settings from the 'Manage display' screen.
 */
function ds_field_ui_fields_save($form, &$form_state) {

  // Setup some variables.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = isset($form['#view_mode']) ? $form['#view_mode'] : 'form';

  // Delete previous field configuration configuration.
  db_delete('ds_settings')
    ->condition('entity_type', $entity_type)
    ->condition('bundle', $bundle)
    ->condition('view_mode', $view_mode)
    ->execute();

  // Create a record to insert into the ds_settings table.
  $record = new stdClass;
  $record->entity_type = $entity_type;
  $record->bundle = $bundle;
  $record->view_mode = $view_mode;

  // Save settings for each field.
  $fields = $form['#ds_fields'];
  foreach ($fields as $key => $field) {

    // Field values.
    $field_values = $form_state['values']['fields'][$field];

    // Build settings.
    $settings = array();
    $settings['weight'] = $field_values['weight'];
    $settings['label'] = $field_values['label'];
    $settings['format'] = $field_values['format']['type'];

    // Save the record.
    $record->field = $field;
    $record->settings = serialize($settings);
    drupal_write_record('ds_settings', $record);
  }

  // Clear the ds_fields_configuration cache.
  cache_clear_all('ds_fields_cached', 'cache');
  cache_clear_all('ds_fields_configuration', 'cache');

  // Clear entity cache.
  cache_clear_all('entity_info', 'cache', TRUE);
}

/**
 * Add the layouts fieldset on the Field UI screen.
 *
 * @param $entity_type
 *   The name of the entity type.
 * @param $bundle
 *   The name of the bundle
 * @param $view_mode
 *   The name of the view_mode
 * @param $form
 *   A collection of form properties.
 */
function _ds_field_ui_table_layouts($entity_type, $bundle, $view_mode, &$form) {

  $layout_options = array();
  $ds_layouts = module_invoke_all('ds_layouts');
  $layout_options = array('' => t(' - None - '));
  foreach ($ds_layouts as $key => $layout) {
    $layout_options[$key] = $layout['label'];
  }

  $form['display_suite'] = array(
    '#type' => 'vertical_tabs',
    '#theme_wrappers' => array('vertical_tabs'),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  $form['#attached']['js'][] = 'misc/form.js';
  $form['#attached']['js'][] = 'misc/collapse.js';

  // Add layouts form.
  $form['display_suite']['ds_layouts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Layout for !bundle in !view_mode', array('!bundle' => str_replace('_', ' ', $bundle), '!view_mode' => str_replace('_', ' ', $view_mode))),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#parents' => array('display_suite'),
  );

  // @todo convert to db_select();
  $layout = db_query('SELECT layout, settings FROM {ds_layouts} WHERE entity_type = :entity_type AND bundle = :bundle AND view_mode = :view_mode', array(':entity_type' => $entity_type, ':bundle' => $bundle, ':view_mode' => $view_mode))->fetchObject();

  if (!empty($layout) && isset($layout->layout) && isset($ds_layouts[$layout->layout])) {
    $chosen_layout = $ds_layouts[$layout->layout];
    $layout_string = $layout->layout;
    $selected = t('You have selected the %layout_label layout. The default template can be found in %path', array('%layout_label' => $chosen_layout['label'], '%path' => $chosen_layout['path']));
    $suggestions = 'Possible template suggestions are: <br />';
    $suggestions .= '- ' . $layout_string .'--'. $entity_type .'.tpl.php<br />';
    $suggestions .= '- ' . $layout_string .'--'. $entity_type .'-'. $bundle .'.tpl.php<br />';
    $suggestions .= '- ' . $layout_string .'--'. $entity_type .'-'. $bundle .'-' . $view_mode .'.tpl.php<br />';
    $layout->settings = unserialize($layout->settings);
    $layout->regions = $chosen_layout['regions'];
    $form['#ds_layout'] = $layout;
  }

  $form['display_suite']['ds_layouts']['layout'] = array(
    '#type' => 'select',
    '#title' => t('Select a layout'),
    '#options' => $layout_options,
    '#default_value' => isset($layout->layout) ? $layout->layout : '',
    '#weight' => -1,
  );

  $form['display_suite']['ds_layouts']['hide_empty_regions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide empty regions'),
    '#default_value' => isset($layout->settings['hide_empty_regions']) ? $layout->settings['hide_empty_regions'] : FALSE,
    '#weight' => 0,
  );

  if (!empty($layout) && isset($layout->regions)) {
    $form['display_suite']['ds_layouts']['suggestions'] = array(
      '#markup' => '<p>' . $selected . '</p><p>' . t('!suggestions', array('!suggestions' => strtr($suggestions, '_', '-'))) . '</p>',
    );

    // Add extra classes for the regions to have more control while theming.
    $form['display_suite']['ds_classes'] = array(
      '#type' => 'fieldset',
      '#title' => t('Extra classes for regions'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#parents' => array('display_suite'),
    );

    $styles = _ds_styles();
    if (!empty($styles)) {
      foreach (array_keys($layout->regions) as $region) {
        $form['display_suite']['ds_classes'][$region] = array(
          '#type' => 'select',
          '#multiple' => TRUE,
          '#options' => $styles,
          '#title' => t('Class for @region', array('@region' => $region)),
          '#default_value' => isset($layout->settings['classes'], $layout->settings['classes'][$region]) ? $layout->settings['classes'][$region] : '',
        );
      }
    }
    $form['display_suite']['ds_classes']['info'] = array('#markup' => l(t('Manage region styles'), 'admin/structure/ds/styles'));
  }

  $form['display_suite']['ds_layouts']['old_layout'] = array(
    '#type' => 'value',
    '#value' => isset($layout->layout) ? $layout->layout : 0,
  );

  // Add validate and submit handlers.
  $form['#validate'][] = 'ds_field_ui_layouts_validate';
  $form['#submit'][] = 'ds_field_ui_layouts_save';
}

/**
 * Add the fields to the Field UI form.
 *
 * @param $entity_type
 *   The name of the entity type.
 * @param $bundle
 *   The name of the bundle
 * @param $view_mode
 *   The name of the view_mode
 * @param $form
 *   A collection of form properties.
 */
function _ds_field_ui_fields($entity_type, $bundle, $view_mode, &$form) {

  // Get the fields.
  $fields = ds_get_fields($entity_type, $bundle, $view_mode, TRUE, FALSE);
  $field_values = ds_get_fields_configuration($entity_type, $bundle, $view_mode);

  $table = &$form['fields'];
  $form['#ds_fields'] = array();

  $field_label_options = array(
    'above' => t('Above'),
    'inline' => t('Inline'),
    'hidden' => t('<Hidden>'),
  );

  foreach ($fields as $key => $field) {
    $form['#ds_fields'][] = $key;

    $hidden = array('hidden' => t('<Hidden>'));
    $formatters = isset($field['properties']['formatters']) ? $hidden + $field['properties']['formatters'] : $hidden + array('default' => t('Default'));

    $table[$key] = array(
      //'#parents' => array($key),
      '#row_type' => 'field',
      '#js_settings' => array('field'),
      '#region_callback' => 'field_ui_display_overview_row_region',
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      'human_name' => array(
        '#markup' => check_plain($field['title']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => isset($field_values[$key]['weight']) ? $field_values[$key]['weight'] : 0,
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#empty_value' => '',
          '#options' => array(),
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $key, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $key,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'label' => array(
        '#type' => 'select',
        '#options' => $field_label_options,
        '#default_value' => isset($field_values[$key]['label']) ? $field_values[$key]['label'] : 'hidden',
      ),
      'format' => array(
        'type' => array(
          '#type' => 'select',
          '#options' => $formatters,
          '#default_value' => isset($field_values[$key]['format']) ? $field_values[$key]['format'] : 'hidden',
          '#attributes' => array('class' => array('field-formatter-type')),
        ),
      ),
      'settings_summary' => array(),
      'settings_edit' => array(),
    );
  }

  // Add fields submit handler.
  $form['#submit'][] = 'ds_field_ui_fields_save';
}

/**
 * Return styles.
 */
function _ds_styles() {
  static $run = FALSE;
  static $styles = array();

  if (!$run) {
    $region_styles = trim(variable_get('ds_styles_regions', ''));
    if (!empty($region_styles)) {
      // @todo multiple select has become smarter in D7 no ? so
      // none can probably be ditched ..
      $styles[''] = t('None');
      $region_styles = explode("\n", $region_styles);
      foreach ($region_styles as $key => $value) {
        $classes = explode("|", $value);
        $key = trim($classes[0]);
        $name = isset($classes[1]) ? trim($classes[1]) : $key;
        $styles[$key] = $name;
      }
    }
    $run = TRUE;
  }

  return $styles;
}


/**
 * Utility function to sort a multidimensional array by a value in a sub-array.
 *
 * @param $a
 *   The array to sort.
 * @param $subkey
 *   The subkey to sort by.
 */
function _ds_sort_fields($a, $subkey) {
  foreach ($a as $k => $v) {
    $b[$k] = $v[$subkey];
  }
  asort($b);
  foreach ($b as $key => $val) {
    $c[$key] = $a[$key];
  }
  return $c;
}

