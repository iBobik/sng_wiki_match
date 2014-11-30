<?php
/**
 * @file
 * sng_wiki_match.feeds_importer_default.inc
 */

/**
 * Implements hook_feeds_importer_default().
 */
function sng_wiki_match_feeds_importer_default() {
  $export = array();

  $feeds_importer = new stdClass();
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'artworks_importer';
  $feeds_importer->config = array(
    'name' => 'Artworks importer',
    'description' => '',
    'fetcher' => array(
      'plugin_key' => 'FeedsOAIHTTPFetcher',
      'config' => array(
        'auto_detect_feeds' => FALSE,
        'use_pubsubhubbub' => FALSE,
        'last_fetched_timestamp' => '',
        'earliest_timestamp' => '',
        'use_dates' => FALSE,
        'to' => array(),
        'from' => array(),
      ),
    ),
    'parser' => array(
      'plugin_key' => 'FeedsOAIParser',
      'config' => array(),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsNodeProcessor',
      'config' => array(
        'expire' => '-1',
        'author' => 0,
        'authorize' => 1,
        'mappings' => array(
          0 => array(
            'source' => 'creator',
            'target' => 'field_author',
            'term_search' => '0',
            'autocreate' => 1,
          ),
          1 => array(
            'source' => 'title',
            'target' => 'title',
            'unique' => FALSE,
          ),
          2 => array(
            'source' => 'Blank source 2',
            'target' => 'guid',
            'unique' => 1,
          ),
          3 => array(
            'source' => 'Blank source 1',
            'target' => 'field_thumbnail',
            'unique' => FALSE,
          ),
          4 => array(
            'source' => 'Blank source 3',
            'target' => 'field_item',
            'unique' => FALSE,
          ),
          5 => array(
            'source' => 'format',
            'target' => 'field_technique',
            'unique' => FALSE,
          ),
        ),
        'update_existing' => '2',
        'input_format' => 'plain_text',
        'skip_hash_check' => 0,
        'bundle' => 'artwork',
      ),
    ),
    'content_type' => '',
    'update' => 0,
    'import_period' => '0',
    'expire_period' => 3600,
    'import_on_create' => 1,
    'process_in_background' => 0,
  );
  $export['artworks_importer'] = $feeds_importer;

  return $export;
}
