<?php
/**
 * @file
 * sng_wiki_match.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function sng_wiki_match_taxonomy_default_vocabularies() {
  return array(
    'author' => array(
      'name' => 'Author',
      'machine_name' => 'author',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
