<?php
/**
 * @file
 * sng_wiki_match.custom_formatters.inc
 */

/**
 * Implements hook_custom_formatters_defaults().
 */
function sng_wiki_match_custom_formatters_defaults() {
  $export = array();

  $formatter = new stdClass();
  $formatter->disabled = FALSE; /* Edit this to true to make a default formatter disabled initially */
  $formatter->api_version = 2;
  $formatter->name = 'image_from_url';
  $formatter->label = 'Image from url';
  $formatter->description = '';
  $formatter->mode = 'token';
  $formatter->field_types = 'text';
  $formatter->code = '<img src="[node:field_thumbnail]" />';
  $formatter->fapi = '';
  $export['image_from_url'] = $formatter;

  return $export;
}
