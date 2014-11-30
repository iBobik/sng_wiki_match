<?php
/**
 * @file
 * sng_wiki_match.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function sng_wiki_match_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'wiki_lookup';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'xml';
  $view->human_name = 'Wiki lookup';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Nájdi môjho tvorcu!';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['xml_file'] = 'http://en.wikipedia.org/w/api.php?action=opensearch&search=%1&format=xml';
  $handler->display->display_options['query']['options']['row_xpath'] = '/default:SearchSuggestion/default:Section/default:Item';
  $handler->display->display_options['query']['options']['show_errors'] = 1;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: default:Text: Text */
  $handler->display->display_options['fields']['text']['id'] = 'text';
  $handler->display->display_options['fields']['text']['table'] = 'xml';
  $handler->display->display_options['fields']['text']['field'] = 'text';
  $handler->display->display_options['fields']['text']['label'] = '';
  $handler->display->display_options['fields']['text']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['text']['element_wrapper_type'] = 'h2';
  $handler->display->display_options['fields']['text']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['text']['xpath_selector'] = 'default:Text';
  $handler->display->display_options['fields']['text']['multiple'] = 0;
  /* Field: default:Image/@source: Text */
  $handler->display->display_options['fields']['text_1']['id'] = 'text_1';
  $handler->display->display_options['fields']['text_1']['table'] = 'xml';
  $handler->display->display_options['fields']['text_1']['field'] = 'text';
  $handler->display->display_options['fields']['text_1']['label'] = '';
  $handler->display->display_options['fields']['text_1']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['text_1']['alter']['text'] = '<img src="[text_1]" />';
  $handler->display->display_options['fields']['text_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['text_1']['xpath_selector'] = 'default:Image/@source';
  $handler->display->display_options['fields']['text_1']['multiple'] = 0;
  /* Field: default:Description: Text */
  $handler->display->display_options['fields']['text_2']['id'] = 'text_2';
  $handler->display->display_options['fields']['text_2']['table'] = 'xml';
  $handler->display->display_options['fields']['text_2']['field'] = 'text';
  $handler->display->display_options['fields']['text_2']['label'] = '';
  $handler->display->display_options['fields']['text_2']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['text_2']['element_wrapper_type'] = 'p';
  $handler->display->display_options['fields']['text_2']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['text_2']['xpath_selector'] = 'default:Description';
  $handler->display->display_options['fields']['text_2']['multiple'] = 0;
  /* Field: default:Url: Text */
  $handler->display->display_options['fields']['text_3']['id'] = 'text_3';
  $handler->display->display_options['fields']['text_3']['table'] = 'xml';
  $handler->display->display_options['fields']['text_3']['field'] = 'text';
  $handler->display->display_options['fields']['text_3']['label'] = '';
  $handler->display->display_options['fields']['text_3']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['text_3']['alter']['text'] = 'Zobraziť článok';
  $handler->display->display_options['fields']['text_3']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['text_3']['alter']['path'] = '[text_3]';
  $handler->display->display_options['fields']['text_3']['alter']['target'] = '_blank';
  $handler->display->display_options['fields']['text_3']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['text_3']['xpath_selector'] = 'default:Url';
  $handler->display->display_options['fields']['text_3']['multiple'] = 0;
  /* Field: Global: PHP */
  $handler->display->display_options['fields']['php']['id'] = 'php';
  $handler->display->display_options['fields']['php']['table'] = 'views';
  $handler->display->display_options['fields']['php']['field'] = 'php';
  $handler->display->display_options['fields']['php']['label'] = '';
  $handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['php']['use_php_setup'] = 0;
  $handler->display->display_options['fields']['php']['php_value'] = 'return $view->args[1];';
  $handler->display->display_options['fields']['php']['php_output'] = '<?php
$form = drupal_get_form(\'custom_add_author_form\', $value, $row->text);
echo drupal_render($form);
?>';
  $handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
  $handler->display->display_options['fields']['php']['php_click_sortable'] = '';
  /* Contextual filter: XML: Pass through */
  $handler->display->display_options['arguments']['passthru']['id'] = 'passthru';
  $handler->display->display_options['arguments']['passthru']['table'] = 'xml';
  $handler->display->display_options['arguments']['passthru']['field'] = 'passthru';
  $handler->display->display_options['arguments']['passthru']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['passthru']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['passthru']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['passthru']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['passthru']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['passthru']['validate']['type'] = 'php';
  $handler->display->display_options['arguments']['passthru']['validate_options']['code'] = '$chunks = explode(\', \', $argument);
if (count($chunks) == 2)
  $argument = $chunks[1] . \' \' . $chunks[0];
$handler->argument = urlencode($argument);
return TRUE;';

  /* Display: EVA Field */
  $handler = $view->new_display('entity_view', 'EVA Field', 'entity_view_1');
  $handler->display->display_options['entity_type'] = 'taxonomy_term';
  $handler->display->display_options['bundles'] = array(
    0 => 'author',
  );
  $handler->display->display_options['argument_mode'] = 'token';
  $handler->display->display_options['default_argument'] = '[term:name]/[term:tid]';
  $handler->display->display_options['show_title'] = 1;
  $export['wiki_lookup'] = $view;

  return $export;
}
