<?php

/**
 * @file
 * Drupal XML-RPC library.
 *
 * Based on the IXR - The Incutio XML-RPC Library - (c) Incutio Ltd 2002-2005
 * Version 1.7 (beta) - Simon Willison, 23rd May 2005
 * Site:   http://scripts.incutio.com/xmlrpc/
 * Manual: http://scripts.incutio.com/xmlrpc/manual.php
 * This version is made available under the GNU GPL License
 */

/**
 * Turns a data structure into objects with 'data' and 'type' attributes.
 *
 * @param $data
 *   The data structure.
 * @param $type
 *   Optional type to assign to $data.
 *
 * @return object
 *   An XML-RPC data object containing the input $data.
 */
function xmlrpc_value($data, $type = FALSE) {
  $xmlrpc_value = new stdClass();
  $xmlrpc_value->data = $data;
  if (!$type) {
    $type = xmlrpc_value_calculate_type($xmlrpc_value);
  }
  $xmlrpc_value->type = $type;
  if ($type == 'struct') {
    // Turn all the values in the array into new xmlrpc_values
    foreach ($xmlrpc_value->data as $key => $value) {
      $xmlrpc_value->data[$key] = xmlrpc_value($value);
    }
  }
  if ($type == 'array') {
    for ($i = 0, $j = count($xmlrpc_value->data); $i < $j; $i++) {
      $xmlrpc_value->data[$i] = xmlrpc_value($xmlrpc_value->data[$i]);
    }
  }
  return $xmlrpc_value;
}

/**
 * Maps a PHP type to an XML-RPC type.
 *
 * @param $xmlrpc_value
 *   Variable whose type should be mapped.
 *
 * @return string
 *   The corresponding XML-RPC type.
 *
 * @see http://www.xmlrpc.com/spec#scalars
 */
function xmlrpc_value_calculate_type($xmlrpc_value) {
  // http://www.php.net/gettype: Never use gettype() to test for a certain type
  // [...] Instead, use the is_* functions.
  if (is_bool($xmlrpc_value->data)) {
    return 'boolean';
  }
  if (is_double($xmlrpc_value->data)) {
    return 'double';
  }
  if (is_int($xmlrpc_value->data)) {
    return 'int';
  }
  if (is_array($xmlrpc_value->data)) {
    // empty or integer-indexed arrays are 'array', string-indexed arrays 'struct'
    return empty($xmlrpc_value->data) || range(0, count($xmlrpc_value->data) - 1) === array_keys($xmlrpc_value->data) ? 'array' : 'struct';
  }
  if (is_object($xmlrpc_value->data)) {
    if (isset($xmlrpc_value->data->is_date)) {
      return 'date';
    }
    if (isset($xmlrpc_value->data->is_base64)) {
      return 'base64';
    }
    $xmlrpc_value->data = get_object_vars($xmlrpc_value->data);
    return 'struct';
  }
  // default
  return 'string';
}

/**
 * Generates XML representing the given value.
 *
 * @param $xmlrpc_value
 *   A value to be represented in XML.
 *
 * @return
 *   XML representation of $xmlrpc_value.
 */
function xmlrpc_value_get_xml($xmlrpc_value) {
  switch ($xmlrpc_value->type) {
    case 'boolean':
      return '<boolean>' . (($xmlrpc_value->data) ? '1' : '0') . '</boolean>';

    case 'int':
      return '<int>' . $xmlrpc_value->data . '</int>';

    case 'double':
      return '<double>' . $xmlrpc_value->data . '</double>';

    case 'string':
      // Note: we don't escape apostrophes because of the many blogging clients
      // that don't support numerical entities (and XML in general) properly.
      return '<string>' . htmlspecialchars($xmlrpc_value->data) . '</string>';

    case 'array':
      $return = '<array><data>' . "\n";
      foreach ($xmlrpc_value->data as $item) {
        $return .= '  <value>' . xmlrpc_value_get_xml($item) . "</value>\n";
      }
      $return .= '</data></array>';
      return $return;

    case 'struct':
      $return = '<struct>' . "\n";
      foreach ($xmlrpc_value->data as $name => $value) {
        $return .= "  <member><name>" . check_plain($name) . "</name><value>";
        $return .= xmlrpc_value_get_xml($value) . "</value></member>\n";
      }
      $return .= '</struct>';
      return $return;

    case 'date':
      return xmlrpc_date_get_xml($xmlrpc_value->data);

    case 'base64':
      return xmlrpc_base64_get_xml($xmlrpc_value->data);
  }
  return FALSE;
}

/**
 * Constructs an object representing an XML-RPC message.
 *
 * @param $message
 *   A string containing an XML message.
 *
 * @return object
 *   An XML-RPC object containing the message.
 *
 * @see http://www.xmlrpc.com/spec
 */
function xmlrpc_message($message) {
  $xmlrpc_message = new stdClass();
  // The stack used to keep track of the current array/struct
  $xmlrpc_message->array_structs = array();
  // The stack used to keep track of if things are structs or array
  $xmlrpc_message->array_structs_types = array();
  // A stack as well
  $xmlrpc_message->current_struct_name = array();
  $xmlrpc_message->message = $message;
  return $xmlrpc_message;
}

/**
 * Parses an XML-RPC message.
 *
 * If parsing fails, the faultCode and faultString will be added to the message
 * object.
 *
 * @param $xmlrpc_message
 *   An object generated by xmlrpc_message().
 *
 * @return
 *   TRUE if parsing succeeded; FALSE otherwise.
 */
function xmlrpc_message_parse($xmlrpc_message) {
  $xmlrpc_message->_parser = xml_parser_create();
  // Set XML parser to take the case of tags into account.
  xml_parser_set_option($xmlrpc_message->_parser, XML_OPTION_CASE_FOLDING, FALSE);
  // Set XML parser callback functions
  xml_set_element_handler($xmlrpc_message->_parser, 'xmlrpc_message_tag_open', 'xmlrpc_message_tag_close');
  xml_set_character_data_handler($xmlrpc_message->_parser, 'xmlrpc_message_cdata');
  xmlrpc_message_set($xmlrpc_message);

  // Strip XML declaration.
  $header = preg_replace('/<\?xml.*?\?'.'>/s', '', substr($xmlrpc_message->message, 0, 100), 1);
  $xml = trim(substr_replace($xmlrpc_message->message, $header, 0, 100));
  if ($xml == '') {
    return FALSE;
  }
  // Strip DTD.
  $header = preg_replace('/^<!DOCTYPE[^>]*+>/i', '', substr($xml, 0, 200), 1);
  $xml = trim(substr_replace($xml, $header, 0, 200));
  if ($xml == '') {
    return FALSE;
  }
  // Confirm the XML now starts with a valid root tag. A root tag can end in [> \t\r\n]
  $root_tag = substr($xml, 0, strcspn(substr($xml, 0, 20), "> \t\r\n"));
  // Reject a second DTD.
  if (strtoupper($root_tag) == '<!DOCTYPE') {
    return FALSE;
  }
  if (!in_array($root_tag, array('<methodCall', '<methodResponse', '<fault'))) {
    return FALSE;
  }
  // Skip parsing if there is an unreasonably large number of tags.
  try {
    $dom = new DOMDocument();
    @$dom->loadXML($xml);
    if ($dom->getElementsByTagName('*')->length > variable_get('xmlrpc_message_maximum_tag_count', 30000)) {
      return FALSE;
    }
  }
  catch (Exception $e) {
    return FALSE;
  }

  if (!xml_parse($xmlrpc_message->_parser, $xml)) {
    return FALSE;
  }
  xml_parser_free($xmlrpc_message->_parser);

  // Grab the error messages, if any.
  $xmlrpc_message = xmlrpc_message_get();
  if (!isset($xmlrpc_message->messagetype)) {
    return FALSE;
  }
  elseif ($xmlrpc_message->messagetype == 'fault') {
    $xmlrpc_message->fault_code = $xmlrpc_message->params[0]['faultCode'];
    $xmlrpc_message->fault_string = $xmlrpc_message->params[0]['faultString'];
  }
  return TRUE;
}

/**
 * Stores a copy of the most recent XML-RPC message object temporarily.
 *
 * @param $value
 *   An XML-RPC message to store, or NULL to keep the last message.
 *
 * @return object
 *   The most recently stored message.
 *
 * @see xmlrpc_message_get()
 */
function xmlrpc_message_set($value = NULL) {
  static $xmlrpc_message;
  if ($value) {
    $xmlrpc_message = $value;
  }
  return $xmlrpc_message;
}

/**
 * Returns the most recently stored XML-RPC message object.
 *
 * @return object
 *   The most recently stored message.
 *
 * @see xmlrpc_message_set()
 */
function xmlrpc_message_get() {
  return xmlrpc_message_set();
}

/**
 * Handles opening tags for XML parsing in xmlrpc_message_parse().
 */
function xmlrpc_message_tag_open($parser, $tag, $attr) {
  $xmlrpc_message = xmlrpc_message_get();
  $xmlrpc_message->current_tag_contents = '';
  $xmlrpc_message->last_open = $tag;
  switch ($tag) {
    case 'methodCall':
    case 'methodResponse':
    case 'fault':
      $xmlrpc_message->messagetype = $tag;
      break;

    // Deal with stacks of arrays and structs
    case 'data':
      $xmlrpc_message->array_structs_types[] = 'array';
      $xmlrpc_message->array_structs[] = array();
      break;

    case 'struct':
      $xmlrpc_message->array_structs_types[] = 'struct';
      $xmlrpc_message->array_structs[] = array();
      break;
  }
  xmlrpc_message_set($xmlrpc_message);
}

/**
 * Handles character data for XML parsing in xmlrpc_message_parse().
 */
function xmlrpc_message_cdata($parser, $cdata) {
  $xmlrpc_message = xmlrpc_message_get();
  $xmlrpc_message->current_tag_contents .= $cdata;
  xmlrpc_message_set($xmlrpc_message);
}

/**
 * Handles closing tags for XML parsing in xmlrpc_message_parse().
 */
function xmlrpc_message_tag_close($parser, $tag) {
  $xmlrpc_message = xmlrpc_message_get();
  $value_flag = FALSE;
  switch ($tag) {
    case 'int':
    case 'i4':
      $value = (int)trim($xmlrpc_message->current_tag_contents);
      $value_flag = TRUE;
      break;

    case 'double':
      $value = (double)trim($xmlrpc_message->current_tag_contents);
      $value_flag = TRUE;
      break;

    case 'string':
      $value = $xmlrpc_message->current_tag_contents;
      $value_flag = TRUE;
      break;

    case 'dateTime.iso8601':
      $value = xmlrpc_date(trim($xmlrpc_message->current_tag_contents));
      // $value = $iso->getTimestamp();
      $value_flag = TRUE;
      break;

    case 'value':
      // If no type is indicated, the type is string
      // We take special care for empty values
      if (trim($xmlrpc_message->current_tag_contents) != '' || (isset($xmlrpc_message->last_open) && ($xmlrpc_message->last_open == 'value'))) {
        $value = (string) $xmlrpc_message->current_tag_contents;
        $value_flag = TRUE;
      }
      unset($xmlrpc_message->last_open);
      break;

    case 'boolean':
      $value = (boolean)trim($xmlrpc_message->current_tag_contents);
      $value_flag = TRUE;
      break;

    case 'base64':
      $value = base64_decode(trim($xmlrpc_message->current_tag_contents));
      $value_flag = TRUE;
      break;

    // Deal with stacks of arrays and structs
    case 'data':
    case 'struct':
      $value = array_pop($xmlrpc_message->array_structs);
      array_pop($xmlrpc_message->array_structs_types);
      $value_flag = TRUE;
      break;

    case 'member':
      array_pop($xmlrpc_message->current_struct_name);
      break;

    case 'name':
      $xmlrpc_message->current_struct_name[] = trim($xmlrpc_message->current_tag_contents);
      break;

    case 'methodName':
      $xmlrpc_message->methodname = trim($xmlrpc_message->current_tag_contents);
      break;
  }
  if ($value_flag) {
    if (count($xmlrpc_message->array_structs) > 0) {
      // Add value to struct or array
      if ($xmlrpc_message->array_structs_types[count($xmlrpc_message->array_structs_types) - 1] == 'struct') {
        // Add to struct
        $xmlrpc_message->array_structs[count($xmlrpc_message->array_structs) - 1][$xmlrpc_message->current_struct_name[count($xmlrpc_message->current_struct_name) - 1]] = $value;
      }
      else {
        // Add to array
        $xmlrpc_message->array_structs[count($xmlrpc_message->array_structs) - 1][] = $value;
      }
    }
    else {
      // Just add as a parameter
      $xmlrpc_message->params[] = $value;
    }
  }
  if (!in_array($tag, array("data", "struct", "member"))) {
    $xmlrpc_message->current_tag_contents = '';
  }
  xmlrpc_message_set($xmlrpc_message);
}

/**
 * Constructs an object representing an XML-RPC request.
 *
 * @param $method
 *   The name of the method to be called.
 * @param $args
 *   An array of parameters to send with the method.
 *
 * @return object
 *   An XML-RPC object representing the request.
 */
function xmlrpc_request($method, $args) {
  $xmlrpc_request = new stdClass();
  $xmlrpc_request->method = $method;
  $xmlrpc_request->args = $args;
  $xmlrpc_request->xml = <<<EOD
<?xml version="1.0"?>
<methodCall>
<methodName>{$xmlrpc_request->method}</methodName>
<params>

EOD;
  foreach ($xmlrpc_request->args as $arg) {
    $xmlrpc_request->xml .= '<param><value>';
    $v = xmlrpc_value($arg);
    $xmlrpc_request->xml .= xmlrpc_value_get_xml($v);
    $xmlrpc_request->xml .= "</value></param>\n";
  }
  $xmlrpc_request->xml .= '</params></methodCall>';
  return $xmlrpc_request;
}

/**
 * Generates, temporarily saves, and returns an XML-RPC error object.
 *
 * @param $code
 *   The error code.
 * @param $message
 *   The error message.
 * @param $reset
 *   TRUE to empty the temporary error storage. Ignored if $code is supplied.
 *
 * @return object
 *   An XML-RPC error object representing $code and $message, or the most
 *   recently stored error object if omitted.
 */
function xmlrpc_error($code = NULL, $message = NULL, $reset = FALSE) {
  static $xmlrpc_error;
  if (isset($code)) {
    $xmlrpc_error = new stdClass();
    $xmlrpc_error->is_error = TRUE;
    $xmlrpc_error->code = $code;
    $xmlrpc_error->message = $message;
  }
  elseif ($reset) {
    $xmlrpc_error = NULL;
  }
  return $xmlrpc_error;
}

/**
 * Converts an XML-RPC error object into XML.
 *
 * @param $xmlrpc_error
 *   The XML-RPC error object.
 *
 * @return string
 *   An XML representation of the error as an XML methodResponse.
 */
function xmlrpc_error_get_xml($xmlrpc_error) {
  return <<<EOD
<methodResponse>
  <fault>
  <value>
    <struct>
    <member>
      <name>faultCode</name>
      <value><int>{$xmlrpc_error->code}</int></value>
    </member>
    <member>
      <name>faultString</name>
      <value><string>{$xmlrpc_error->message}</string></value>
    </member>
    </struct>
  </value>
  </fault>
</methodResponse>

EOD;
}

/**
 * Converts a PHP or ISO date/time to an XML-RPC object.
 *
 * @param $time
 *   A PHP timestamp or an ISO date-time string.
 *
 * @return object
 *   An XML-RPC time/date object.
 */
function xmlrpc_date($time) {
  $xmlrpc_date = new stdClass();
  $xmlrpc_date->is_date = TRUE;
  // $time can be a PHP timestamp or an ISO one
  if (is_numeric($time)) {
    $xmlrpc_date->year = gmdate('Y', $time);
    $xmlrpc_date->month = gmdate('m', $time);
    $xmlrpc_date->day = gmdate('d', $time);
    $xmlrpc_date->hour = gmdate('H', $time);
    $xmlrpc_date->minute = gmdate('i', $time);
    $xmlrpc_date->second = gmdate('s', $time);
    $xmlrpc_date->iso8601 = gmdate('Ymd\TH:i:s', $time);
  }
  else {
    $xmlrpc_date->iso8601 = $time;
    $time = str_replace(array('-', ':'), '', $time);
    $xmlrpc_date->year = substr($time, 0, 4);
    $xmlrpc_date->month = substr($time, 4, 2);
    $xmlrpc_date->day = substr($time, 6, 2);
    $xmlrpc_date->hour = substr($time, 9, 2);
    $xmlrpc_date->minute = substr($time, 11, 2);
    $xmlrpc_date->second = substr($time, 13, 2);
  }
  return $xmlrpc_date;
}

/**
 * Converts an XML-RPC date-time object into XML.
 *
 * @param $xmlrpc_date
 *   The XML-RPC date-time object.
 *
 * @return string
 *   An XML representation of the date/time as XML.
 */
function xmlrpc_date_get_xml($xmlrpc_date) {
  return '<dateTime.iso8601>' . $xmlrpc_date->year . $xmlrpc_date->month . $xmlrpc_date->day . 'T' . $xmlrpc_date->hour . ':' . $xmlrpc_date->minute . ':' . $xmlrpc_date->second . '</dateTime.iso8601>';
}

/**
 * Returns an XML-RPC base 64 object.
 *
 * @param $data
 *   Base 64 data to store in returned object.
 *
 * @return object
 *   An XML-RPC base 64 object.
 */
function xmlrpc_base64($data) {
  $xmlrpc_base64 = new stdClass();
  $xmlrpc_base64->is_base64 = TRUE;
  $xmlrpc_base64->data = $data;
  return $xmlrpc_base64;
}

/**
 * Converts an XML-RPC base 64 object into XML.
 *
 * @param $xmlrpc_base64
 *   The XML-RPC base 64 object.
 *
 * @return string
 *   An XML representation of the base 64 data as XML.
 */
function xmlrpc_base64_get_xml($xmlrpc_base64) {
  return '<base64>' . base64_encode($xmlrpc_base64->data) . '</base64>';
}

/**
 * Performs one or more XML-RPC requests.
 *
 * @param $url
 *   An absolute URL of the XML-RPC endpoint, e.g.,
 *   http://example.com/xmlrpc.php
 * @param $args
 *   An associative array whose keys are the methods to call and whose values
 *   are the arguments to pass to the respective method. If multiple methods
 *   are specified, a system.multicall is performed.
 * @param $options
 *   (optional) An array of options to pass along to drupal_http_request().
 *
 * @return
 *   A single response (single request) or an array of responses (multicall
 *   request). Each response is the return value of the method, just as if it
 *   has been a local function call, on success, or FALSE on failure. If FALSE
 *   is returned, see xmlrpc_errno() and xmlrpc_error_msg() to get more
 *   information.
 */
function _xmlrpc($url, $args, $options = array()) {
  xmlrpc_clear_error();
  if (count($args) > 1) {
    $multicall_args = array();
    foreach ($args as $method => $call) {
      $multicall_args[] = array('methodName' => $method, 'params' => $call);
    }
    $method = 'system.multicall';
    $args = array($multicall_args);
  }
  else {
    $method = key($args);
    $args = $args[$method];
  }
  $xmlrpc_request = xmlrpc_request($method, $args);
  // Required options which will replace any that are passed in.
  $options['method'] = 'POST';
  $options['headers']['Content-Type'] = 'text/xml';
  $options['data'] = $xmlrpc_request->xml;
  $result = drupal_http_request($url, $options);
  if ($result->code != 200) {
    xmlrpc_error($result->code, $result->error);
    return FALSE;
  }
  $message = xmlrpc_message($result->data);
  // Now parse what we've got back
  if (!xmlrpc_message_parse($message)) {
    // XML error
    xmlrpc_error(-32700, t('Parse error. Not well formed'));
    return FALSE;
  }
  // Is the message a fault?
  if ($message->messagetype == 'fault') {
    xmlrpc_error($message->fault_code, $message->fault_string);
    return FALSE;
  }
  // We now know that the message is well-formed and a non-fault result.
  if ($method == 'system.multicall') {
    // Return per-method results or error objects.
    $return = array();
    foreach ($message->params[0] as $result) {
      if (array_keys($result) == array(0)) {
        $return[] = $result[0];
      }
      else {
        $return[] = xmlrpc_error($result['faultCode'], $result['faultString']);
      }
    }
  }
  else {
    $return = $message->params[0];
  }
  return $return;
}

/**
 * Returns the last XML-RPC client error number.
 */
function xmlrpc_errno() {
  $error = xmlrpc_error();
  return ($error != NULL ? $error->code : NULL);
}

/**
 * Returns the last XML-RPC client error message.
 */
function xmlrpc_error_msg() {
  $error = xmlrpc_error();
  return ($error != NULL ? $error->message : NULL);
}

/**
 * Clears any previously-saved errors.
 *
 * @see xmlrpc_error()
 */
function xmlrpc_clear_error() {
  xmlrpc_error(NULL, NULL, TRUE);
}
