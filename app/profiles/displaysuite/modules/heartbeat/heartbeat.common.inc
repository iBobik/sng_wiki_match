<?php
// $Id: heartbeat.common.inc,v 1.1.2.2.2.20.2.2 2010/08/14 13:17:57 stalski Exp $

/**
 * @file
 *   Commonly functions used in heartbeat
 */

/**
 * Include heartbeat .inc class files
 */
function heartbeat_include($file, $module = 'heartbeat', $_file_path = '') {

  static $loaded = array();
  $file = drupal_strtolower($file);
  $found = FALSE;
  
  if (!isset($loaded[$file])) {
  	
	  $path = drupal_get_path('module', $module);

	  if (!empty($_file_path) && file_exists($path . $_file_path)) {
	  	$file_path = drupal_strtolower($_file_path);
      $found = TRUE;
	  }
    elseif (file_exists($path . '/includes/'. $file . '.inc')) {
    	$file_path = $path . '/includes/'. $file . '.inc';
    	$found = TRUE;
    }
    elseif (file_exists($path . '/streams/'. $file . '.inc')) {
      $file_path = $path . '/streams/'. $file . '.inc';
      $found = TRUE;
    }
    elseif (file_exists($path . '/'. $file . '.inc')) {
      $file_path = $path . '/'. $file . '.inc';
      $found = TRUE;
    }
    
    if ($found) {
      require $file_path;
      $loaded[$file] = TRUE;
    }
    
  }
  else {
  	$found = TRUE;
  }
  
  return $found ? $loaded[$file] : FALSE;
}

/**
 * Function to get the heartbeat tags
 */
function heartbeat_get_available_tags($hid = 0) {

  $query = db_select('heartbeat_tags', 'ht')->fields('ht');
  $join = $query->join('heartbeat_mt', 'hmt', 'ht.htid = hmt.htid');
  $tags = array();
  if ($hid !== 0) {
    $query->condition('hid', $hid);
  }
  $result = $query->execute();
  foreach ($result as $row) {
    $tags[$row->htid] = $row->name;
  }

  return $tags;
}

/**
 * Decode heartbeat message variables
 */
function heartbeat_decode_message_variables($string, $object = FALSE) {

  if (!is_string($string)) {
    return array();
  }

  // Variable string need to be cleared from spaces to decode properly
  $array = explode("-|-", $string);
  $variables = array();
  if (!empty($array)) {
    foreach ($array as $varvalue) {
      $parts = explode("=|=", $varvalue);
      if (isset($parts[0]) && !empty($parts[0])) {
        if (eregi("\*\*\*", $parts[1])) {
          $parts[1] = explode("***", $parts[1]);
        }
        $variables[$parts[0]] = !is_array($parts[1]) ? (string)$parts[1] : $parts[1];
      }
    }
  }

  return $object ? (object) $variables : (array) $variables;
}

/**
 * Encode heartbeat message variables
 */
function heartbeat_encode_message_variables($array) {
  $string = '';
  foreach ($array as $key => $value) {
    if (is_array($value)) {
      $value = implode('***', $value);
    }
    $string .= $key .'=|='. $value .'-|-';
  }
  //$string = serialize((object)$array);
  return $string;
}

/**
 * Helper function to map a array to dropdown
 *   with a field and value for the options
 *
 * @param array $options
 * @param string target $field
 * @param sring target $value
 * @return array mapped for options dropdown
 */
function _heartbeat_map_assoc($options, $field, $value) {
  $mapped = array();
  foreach ($options as $heartbeat_activity) {
    $mapped[$heartbeat_activity->{$field}] = $heartbeat_activity->{$value};
  }
  return $mapped;
}

/**
 * Helper function to get the fields
 */
function _heartbeat_get_fields($table) {
  $fields = array();
  $schema = drupal_get_schema($table, TRUE);
  // Go through our schema
  foreach ($schema['fields'] as $field => $info) {
    $fields[$field] = !empty($info['alias']) ? $info['alias'] : $field;
  }
  return $fields;
}

/**
 * Helper function to check access on activity stream
 */
function _heartbeat_stream_has_access($stream_name) {

  $access = FALSE;
  $stream = heartbeat_stream_load($stream_name);

  if (user_access('view heartbeat messages')) {

    // Allow for all
    if (!isset($stream['access'])) {
      $access = TRUE;
    }
    // Keep booleans as they were defined
    elseif (is_bool($stream['access']) || is_numeric($stream['access'])) {
      $access = $stream['access'];
    }
    // Use callback function to detect the message accessibility
    elseif (is_string($stream['access']) && function_exists($stream['access'])) {
      $access = call_user_func($stream['access']);
    }
  }

  return $access ? $stream : $access;

}

/**
 * Helper function to check if a user has access to edit/delete buttons
 */
function _heartbeat_activity_stream_delete_access() {
  if (variable_get('heartbeat_activity_stream_delete', 1)) {
    return user_access('delete heartbeat activity logs');
  }
  return FALSE;
}

/**
 * Helper function to check if a user has access to delete a message
 */
function _heartbeat_message_delete_access($uaid) {
  if (_heartbeat_activity_stream_delete_access()) {
    return TRUE;
  }

  if (variable_get('heartbeat_activity_stream_actor_delete', 1)) {
    global $user;
    $uid = db_result(db_query("SELECT uid FROM {heartbeat_activity} WHERE uaid = %d", $uaid));
    return $uid == $user->uid;
  }
  return FALSE;
}

/**
 * Helper function to get the options for streams
 *
 * @return array of streams.
 */
function _heartbeat_stream_options() {
  $return = array();
  $types = variable_get('heartbeat_streams', array());
  foreach ($types as $type) {
    $return[$type['class']] = $type['name'];
  }
  return $return;
}

/**
 * Function to get the allowed tags for heartbeat strings
 */
function heartbeat_allowed_tags() {
  $tags = str_replace(' ', '', variable_get('heartbeat_allowed_tags', ''));
  return explode(',', $tags);
}

/**
 * Helper function to get the options for perm types
 * @param boolean $profile indicator for personal or profile labels
 * @return array of perm types
 */
function _heartbeat_perms_options($profile = FALSE) {
  if ($profile) {
    return array(
      HEARTBEAT_NONE => ('Never'),
      HEARTBEAT_PRIVATE => t('Only me'),
      HEARTBEAT_PUBLIC_TO_CONNECTED => t('Only my friends'),
      HEARTBEAT_PUBLIC_TO_ALL => t('Everyone'),
    );
  }
  else  {
    return array(
      HEARTBEAT_PRIVATE => t('Only the user himself is allowed to see this message'),
      HEARTBEAT_PUBLIC_TO_CONNECTED => t('Only user and relations are allowed to see this message'),
      HEARTBEAT_PUBLIC_TO_ALL => t('Everyone can see this message'),
    );
  }
}

/**
 * Helper function to get the stream default properties
 */
function _heartbeat_stream_defaults($values) {
  $default = array(
    'name' => '',
    'class' => '',
    'path' => '',
    'module' => '',
    'access' => 1,
    'status' => 'default',
    'messages_denied' => array(),
    'display_filters' => 0,
    'display_block_filters' => 0,
    'filters' => array(),
    'filters_cumul' => 0,
    'settings' => array(
      'block_items_max' => 10,
      'block_show_pager' => 2,
      'page_items_max' => 15,
      'page_show_pager' => 1,
      'page_pager_ajax' => 1,
      'num_load_max' => 30,
    ),
    'poll_messages' => 0,
  );

  // if same string keys exist, the used configuration will
  // override the default one
  $update = array_merge($default, $values);

  return $update;
}

/**
 * Helper function for a more link on streams (older messages)
 */
function heartbeat_stream_prev_link($stream_name, $offset_time = 0) {
  $path = 'heartbeat/'. $stream_name . ($offset_time == 0 ? '' : '/' . $offset_time);
  return '<div class="more previous-archive">' . l(t('Go to previous messages'), $path) . '</div>';
}

/**
 *
 * @return unknown_type
 */
function _theme_time_ago($time) {

  return format_interval(($_SERVER['REQUEST_TIME'] - $time), 1) .' '. t('ago') ;
}

/**
 * Function to get the corresponding tuaid
 * @param Integer $uaid
 * @return Integer $tuaid
 */
function heartbeat_get_tuaid($uaid) {
  $result = db_fetch_object(db_query("SELECT tuaid FROM {heartbeat_translations} WHERE uaid=%d", $uaid));
  return $result ? $result->tuaid : $uaid;
}
