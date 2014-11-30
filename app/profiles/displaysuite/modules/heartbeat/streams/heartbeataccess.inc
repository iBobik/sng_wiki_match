<?php
// $Id: heartbeataccess.inc,v 1.1.2.1 2010/08/14 13:17:57 stalski Exp $

/**
 * @file
 *   HeartbeatAccess object is the object that takes stream
 *   configuration to create a stream of activity objects.
 *   It is the controlling organ at the pre-query, query and
 *   post-query phases.
 */

/**
 * Abstract class heartbeataccess
 *   This base class has final template methods which are
 *   used by the derived concretes. The HeartbeatAccess is a
 *   state object that is given to the HeartbeatMessageBuilder to
 *   set the access to the current request.
 */
abstract class HeartbeatAccess {

  protected $_whoisuser_types = array(
    self::TYPE_ACTOR => 'Acting user',
    self::TYPE_USER_PROFILE => 'Requested user (from url)'
  );
  protected $_whoisuser_type = self::TYPE_ACTOR;
  protected $_uid = 0;
  protected $_errors = array();
  protected $_has_errors = FALSE;
  protected $_offset_sql = 0;
  protected $_page = FALSE;
  protected $query = NULL;
  
  public $stream = NULL;

  const TYPE_ACTOR = 0;
  const TYPE_USER_PROFILE = 1;

  /**
   * Constructor
   *
   * @param $stram HeartbeatStream object with the neccesairy parameters
   * @param $page Boolean to indicate if this is a page view
   */
  public function __construct(HeartbeatStream $stream, $page = FALSE, $account = NULL) {

    global $user;

    $this->_page = $page;
    $this->stream = $stream;
    $this->stream->setItemsMax($this->_page);

    // Assign the who-is-user type for block displays.
    if (!$page) {
      $this->_whoisuser_type = variable_get('heartbeat_show_user_profile_messages_' . drupal_strtolower(get_class($this)), self::TYPE_ACTOR);
    }

    // If an account is given, then the stream is build for that user.
    if (isset($account)) {
      $this->_whoisuser_type = self::TYPE_USER_PROFILE;
      $this->_uid = $account->uid;
    }
    else {
      $this->_uid = $user->uid;
    }

    // Load an account in case profile and account not set yet.
    if ($this->_whoisuser_type == self::TYPE_USER_PROFILE && !isset($account)) {
      if (arg(0) == 'user' && is_numeric(arg(1))) {
        $account = user_load(arg(1));
        $this->_uid = $account->uid;
      }
    }
    elseif (!isset($account)) {
      $account = $user;
    }

    // Set info properties.
    $this->stream->uid = $this->_uid;
    $this->stream->user_relations = $account->heartbeat_relations;
  }

  /**
   * setError
   *
   * @param $text String of found error
   * @return void
   */
  protected function setError($text) {
    $this->_errors[] = $text;
    $this->_has_errors = TRUE;
  }

  /**
   * hasAccess
   *
   * @param $text
   * @return boolean to indicate the access to the stream
   */
  protected function hasAccess() {
    return TRUE;
  }

  /**
   * hasErrors
   *
   * @return boolean has errors
   */
  public function hasErrors() {
    return $this->_has_errors;
  }

  /**
   *
   * @param $text
   * @return unknown_type
   */
  public function getErrors() {
    return $this->_errors;
  }

  /**
   * Getter function for heartbeat page/blocks
   */
  public function isPage() {
    return $this->_page;
  }

  /**
   * Get HeartbeatStream object with all configurations
   * @return HeartbeatStream object
   */
  public function getStream() {
    return $this->stream;
  }

  /**
   *
   * @param $text
   * @return Array of messages
   */
  public function fetchMessages() {

    $heartbeat = $this->prepareStream();

    // Call to a hook method to adapt sql and thus result
    $heartbeat = $this->dressUpMessages($heartbeat);

    return $this->hasAccess() ? $this->finishMessages($heartbeat) : array();
  }

  /**
   * prepareStream
   *
   * @param $text
   * @return HeartbeatParser object
   */
  protected function prepareStream() {

    global $user, $language;
    $this->stream->offset_sql = $this->_offset_sql;
    // Prevent already assigned language to be overriden
    if (empty($this->stream->language)) {
      $this->stream->language = $language->language;
    }

    $this->query = db_select('heartbeat_activity', 'ha');
    $this->query->join('heartbeat_messages', 'hm', 'hm.message_id = ha.message_id');
    $this->query->fields('ha');
    $this->query->fields('hm');
    $this->query->addField('ha', 'message', 'message');
    $this->query->addField('ha', 'message_concat', 'message_concat');
    $this->query->addField('hm', 'message', 'message_orig');
    $this->query->addField('hm', 'message_concat', 'message_concat_orig');
    $this->query->addField('hm', 'variables', 'variables_orig');
    
    $this->query->condition('ha.language', $this->stream->language, '>');
    $this->query->orderBy('ha.timestamp');
    $this->query->orderBy('ha.uaid');

    // Calls with an offset uaid will fetch newer messages
    if ($this->stream->latest_uaid > 0) {
      $this->query->condition('ha.uaid', $this->stream->latest_uaid, '>');
    }
    else {
      $this->query->condition('ha.timestamp', $this->stream->offset_sql, '<');

      // Limit the query for messages newer than oldest_date,
      // add param delete too old messages if set
      if ($this->stream->oldest_date) {
        $this->query->condition('ha.timestamp', $_SERVER['REQUEST_TIME'] - $this->stream->oldest_date, '>');
      }
    }
    // Messages that have been denied through stream configuration
    // or overriden by user profile can be taken out in the query.
    // Note that for private and connected restrictions, it can only
    // be done later after the query.
    $denied_messages = array();
    if (!empty($this->stream->messages_denied)) {
      $denied_messages += array_keys($this->stream->messages_denied);
    }
    if (!empty($user->heartbeat_activity_settings)) {
      foreach ($user->heartbeat_activity_settings as $id => $message_type) {
        if ($message_type['access'] == HEARTBEAT_PUBLIC_TO_ALL) {
          continue;
        }
        if ($message_type['access'] == HEARTBEAT_NONE) {
          $denied_messages[] = $id;
        }
      }
    }
    $denied_messages = array_unique($denied_messages);
    if (!empty($denied_messages)) {
      $this->query->where(" NOT IN (':messages') ", array(':messages' => implode("','", $denied_messages)));
    }

    $heartbeat = $this->createHeartbeatParser();

    return $heartbeat;
  }

  /**
   * resultSql
   *
   * @param $sql String query to extend the current query
   * @param $values Array of values to substitute in the query extension
   * @return array results
   */
  protected function resultSql($sql = '', $values = array()) {

    $this->query->range(0, $this->stream->num_load_max);
    $result = $this->query->execute()->fetchAll();
    $messages = array();

    foreach ($result as $row) {

      $template = new HeartbeatMessageTemplate($row->hid, $row->message_id, $row->message_orig, $row->message_concat_orig, $row->concat_args);
      $template->perms = $row->perms;
      $template->custom = $row->custom;
      $template->description = $row->description;
      $template->set_variables($row->variables_orig);
      $template->set_attachments($row->attachments);
      $template->set_roles(isset($template->concat_args['roles']) ? $template->concat_args['roles'] : array());

      $heartbeatactivity = new HeartbeatActivity($row, $template);
      $heartbeatactivity->count = 1;

      $messages[$row->uaid] = $heartbeatactivity;
    }

    $stream = heartbeat_stream_load($this->getAccess());

    $this->stream->display_filters = !empty($stream['display_' . ($this->_page ? '' : 'block_') . 'filters']);
    if ($this->stream->display_filters) {
      $this->stream->filters = $stream['filters'];
    }

    return $messages;
  }

  /**
   * createHeartbeatParser
   *
   * @return HeartbeatParser object
   */
  final protected function createHeartbeatParser() {
    $heartbeat = HeartbeatParser::instantiate($this->getAccess());
    return $heartbeat;
  }

  /**
   * setOffsetSql
   *
   * @param $offset integer to set the offset time
   * @return void
   */
  final public function setOffsetSql($offset = 0) {
    if ($offset == 0) {
      $offset = $_SERVER['REQUEST_TIME'];
    }
    $this->_offset_sql = $offset;
  }

  /**
   *
   * @param $text
   * @return HeartbeatParser object
   */
  protected function dressUpMessages(HeartbeatParser $heartbeat) {
    $heartbeat->raw_messages = $this->resultSql();
  }

  /**
   * finishMessages
   *
   * @param HeartbeatParser $heartbeat Parser object
   * @return HeartbeatParser object
   */
  protected function finishMessages(HeartbeatParser $heartbeat) {
    return $heartbeat;
  }

  /**
   * addViewQuery
   *
   * @param $view Object View
   * @return void
   */
  public function addViewQuery(&$view) {
    // Nothing for base class
  }

  /**
   * getAccess
   *
   * @return string Stream
   */
  final public function getAccess() {
    return drupal_strtolower(get_class($this));
  }
}
