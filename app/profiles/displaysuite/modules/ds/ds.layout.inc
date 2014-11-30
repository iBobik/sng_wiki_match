<?php
// $Id: ds.layout.inc,v 1.1.2.1 2011/01/29 12:41:47 swentel Exp $

/**
 * @file
 * Shows the overview screen with all links to entities.
 */

/**
 * Menu callback: Show the layout list.
 */
function ds_layout_list() {
  $build = array();

  // Info
  $info = '<p>' . t('This is a list of all entities available in your Drupal website. Clicking on a link will bring you to the "Manage display" screens if the Field UI module is enabled.') . '</p>';
  $build['info'] = array('#markup' => $info);

  // All entities.
  $links_output = '';
  $entities = entity_get_info();
  foreach ($entities as $entity_type => $entity) {
    if ($entity['fieldable']) {

      $links = array();
      foreach ($entity['bundles'] as $bundle_type => $bundle) {
        $path = isset($bundle['admin']['real path']) ? $bundle['admin']['real path'] : $bundle['admin']['path'];
        $links[] = array(
          'title' => $bundle['label'],
          'href' => $path . '/display',
        );
      }

      if (!empty($links)) {
        $variables['heading'] = $entity['label'];
        $variables['links'] = $links;
        $links_output .= '<div style="float: left; margin-right: 20px;">' . theme('links', $variables) . '</div>';
      }
    }
  }

  if (!empty($links_output)) {
    $build['list'] = array('#markup' => $links_output);
  }

  return $build;
}
