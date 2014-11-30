<?php
// $Id: ds.api.php,v 1.1.2.11 2011/01/28 17:00:35 swentel Exp $

/**
 * @file
 * Hooks provided by Display Suite module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Define custom fields.
 *
 * @param $entity_type
 *   The name of the entity which we are requesting fields for, e.g. 'node'.
 * @param $bundle
 *   The name of the bundle in the entity, e.g. 'article'.
 * @param $view_mode
 *   The name of the view mode, e.g. 'full'.
 *
 *
 * @return $fields
 *   A collection of fields which keys are the entity type name and values
 *   a collection fields.
 *
 * @see ds_get_fields()
 */
function hook_ds_fields($entity_type, $bundle, $view_mode) {
  $fields = array();

  $fields['title'] = array(

    // title: title of the field
    'title' => t('Title'),

    // type: type of field
    // - DS_FIELD_TYPE_THEME    : calls a theming function.
    // - DS_FIELD_TYPE_FUNCTION : calls a custom function.
    // - DS_FIELD_TYPE_CODE     : calls ds_render_code_field().
    // - DS_FIELD_TYPE_BLOCK    : calls ds_render_block_field().
    // - DS_FIELD_TYPE_IGNORE   : calls nothing, use this if you simple want
    //                            to drag and drop. The field itself will have
    //                            a theme function.
    'type' => DS_FIELD_TYPE_FUNCTION,

    // file: an optional file in which the function resides.
    // Only for DS_FIELD_TYPE_FUNCTION.
    'file' => 'optional_filename',

    // status: status of the field.
    // - DS_FIELD_STATUS_STATIC  : static field
    // - DS_FIELD_STATUS_DEFAULT : default field
    'status' => DS_FIELD_STATUS_STATIC,

    // function: only for DS_FIELD_TYPE_FUNCTION.
    'function' => 'theme_ds_title_field',

    // properties: can have different keys.
    'properties' => array(

      // formatters: optional if a a function is used.
      'formatters' => array(
        'node_title_nolink_h1' => t('H1 title'),
        'node_title_link_h1' => t('H1 title, linked to node'),
      ),

      // code: optional, only for code field.
      'code' => 'my code here',

      // use_token: optional, only for code field.
      'code' => TRUE, // or FALSE,

      // block: the module and delta of the block, only for block fields.
      'block' => 'user-menu',

      // block_render: block render type, only for block fields.
      // - DS_BLOCK_CONTENT       : render through block template file.
      // - DS_BLOCK_TITLE_CONTENT : render only title and content.
      // - DS_BLOCK_CONTENT       : render only content.
      'block_render' => DS_BLOCK_CONTENT,
    )
  );

  return array('node' => $fields);

}

/**
 * Alter fields defined by Display Suite
 *
 * @param $fields
 *   An array with fields which can altered just before they are cached.
 */
function hook_ds_fields_alter(&$fields) {
  if (isset($fields['title'])) {
    $fields['title']['title'] = t('My title');
  }
}

/**
 * Define layouts from code.
 *
 * @return $layouts
 *   A collection of layouts.
 */
function hook_ds_layouts() {
  $path = drupal_get_path('module', 'foo');

  $layouts = array(
    'foo_1col' => array(
      'label' => t('Foo one column'),
      'path' => $path . '/layouts/foo_1col',
      'regions' => array(
        'foo_content' => t('Content'),
      ),
      'css' => TRUE,
    ),
  );

  return $layouts;
}

/**
 * Themes can also define extra layouts.
 *
 * Create a ds_layouts folder and then a folder name that will
 * be used as key for the layout. The folder should at least have 2 files:
 *
 * - key.inc
 * - key.tpl.php
 *
 * The css file is optional.
 * - key.css
 *
 * e.g.
 * bartik/ds_layouts/bartik_ds/bartik_ds.inc
 *                            /bartik-ds.tpl.php
 *                            /bartik_ds.css
 *
 * bartik_ds.inc must look like this:
 *

  // Fuction name is ds_LAYOUT_KEY
  function ds_bartik_ds() {
    return array(
      'label' => t('Bartik DS'),
      'regions' => array(
        // The key of this region name is also the variable used in
        // the template to print the content of that region.
        'bartik' => t('Bartik DS'),
      ),
      // Add this if there is a default css file.
      'css' => TRUE,
    );
  }

 */

/**
 * Theme an entity coming from the views entity plugin.
 *
 * @param $entity
 *   The complete entity.
 * @param $view_mode
 *   The name of the view mode.
 */
function ds_views_row_ENTITY_NAME($entity, $view_mode) {
  $nid = $vars['row']->{$vars['field_alias']};
  $node = node_load($nid);
  return drupal_render(node_view($node, $view_mode));
}

/**
 * Theme an entity through an advanced function coming from the views entity plugin.
 *
 * @param $vars
 *   An array of variables from the views preprocess functions.
 * @param $view_mode
 *   The name of the view mode.
 */
function ds_views_row_adv_VIEWS_NAME(&$vars, $view_mode) {
  // You can do whatever you want to here.
  $vars['object'] = 'This is what I want for christmas.';
}

/**
 * @} End of "addtogroup hooks".
 */
