<?php
/**
 * @file
 * sng_wiki_match.feeds_tamper_default.inc
 */

/**
 * Implements hook_feeds_tamper_default().
 */
function sng_wiki_match_feeds_tamper_default() {
  $export = array();

  $feeds_tamper = new stdClass();
  $feeds_tamper->disabled = FALSE; /* Edit this to true to make a default feeds_tamper disabled initially */
  $feeds_tamper->api_version = 2;
  $feeds_tamper->id = 'artworks_importer-blank_source_1-rewrite';
  $feeds_tamper->importer = 'artworks_importer';
  $feeds_tamper->source = 'Blank source 1';
  $feeds_tamper->plugin_id = 'rewrite';
  $feeds_tamper->settings = array(
    'text' => '[url]',
  );
  $feeds_tamper->weight = 0;
  $feeds_tamper->description = 'Rewrite';
  $export['artworks_importer-blank_source_1-rewrite'] = $feeds_tamper;

  $feeds_tamper = new stdClass();
  $feeds_tamper->disabled = FALSE; /* Edit this to true to make a default feeds_tamper disabled initially */
  $feeds_tamper->api_version = 2;
  $feeds_tamper->id = 'artworks_importer-blank_source_2-rewrite';
  $feeds_tamper->importer = 'artworks_importer';
  $feeds_tamper->source = 'Blank source 2';
  $feeds_tamper->plugin_id = 'rewrite';
  $feeds_tamper->settings = array(
    'text' => '[guid]',
  );
  $feeds_tamper->weight = 0;
  $feeds_tamper->description = 'Rewrite';
  $export['artworks_importer-blank_source_2-rewrite'] = $feeds_tamper;

  $feeds_tamper = new stdClass();
  $feeds_tamper->disabled = FALSE; /* Edit this to true to make a default feeds_tamper disabled initially */
  $feeds_tamper->api_version = 2;
  $feeds_tamper->id = 'artworks_importer-blank_source_3-php';
  $feeds_tamper->importer = 'artworks_importer';
  $feeds_tamper->source = 'Blank source 3';
  $feeds_tamper->plugin_id = 'php';
  $feeds_tamper->settings = array(
    'php' => 'return var_export($item, TRUE);',
  );
  $feeds_tamper->weight = 0;
  $feeds_tamper->description = 'Execute php code';
  $export['artworks_importer-blank_source_3-php'] = $feeds_tamper;

  return $export;
}
