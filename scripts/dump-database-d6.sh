#!/usr/bin/env php
<?php

/**
 * Dump a Drupal 6 database into a Drupal 7 PHP script to test the upgrade
 * process.
 *
 * Run this script at the root of an existing Drupal 6 installation.
 *
 * The output of this script is a PHP script that can be ran inside Drupal 7
 * and recreates the Drupal 6 database as dumped. Transient data from cache
 * session and watchdog tables are not recorded.
 */

// Define default settings.
$cmd = 'index.php';
$_SERVER['HTTP_HOST']       = 'default';
$_SERVER['PHP_SELF']        = '/index.php';
$_SERVER['REMOTE_ADDR']     = '127.0.0.1';
$_SERVER['SERVER_SOFTWARE'] = NULL;
$_SERVER['REQUEST_METHOD']  = 'GET';
$_SERVER['QUERY_STRING']    = '';
$_SERVER['PHP_SELF']        = $_SERVER['REQUEST_URI'] = '/';
$_SERVER['HTTP_USER_AGENT'] = 'console';

// Bootstrap Drupal.
include_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

// Include the utility drupal_var_export() function.
include_once dirname(__FILE__) . '/../includes/utility.inc';

// Output the PHP header.
$output = <<<ENDOFHEADER
<?php

/**
 * @file
 * Filled installation of Drupal 6.17, for test purposes.
 *
 * This file was generated by the dump-database-d6.sh tool, from an
 * installation of Drupal 6, filled with data using the generate-d6-content.sh
 * tool. It has the following modules installed:

ENDOFHEADER;

foreach (module_list() as $module) {
  $output .= " *  - $module\n";
}
$output .= " */\n\n";

// Get the current schema, order it by table name.
$schema = drupal_get_schema();
ksort($schema);

// Export all the tables in the schema.
foreach ($schema as $table => $data) {
  // Remove descriptions to save time and code.
  unset($data['description']);
  foreach ($data['fields'] as &$field) {
    unset($field['description']);
  }

  // Dump the table structure.
  $output .= "db_create_table('" . $table . "', " . drupal_var_export($data) . ");\n";

  // Don't output values for those tables.
  if (substr($table, 0, 5) == 'cache' || $table == 'sessions' || $table == 'watchdog') {
    $output .= "\n";
    continue;
  }

  // Prepare the export of values.
  $result = db_query('SELECT * FROM {'. $table .'}');
  $insert = '';
  while ($record = db_fetch_array($result)) {
    // users.uid is a serial and inserting 0 into a serial can break MySQL.
    // So record uid + 1 instead of uid for every uid and once all records
    // are in place, fix them up.
    if ($table == 'users') {
      $record['uid']++;
    }
    $insert .= '->values('. drupal_var_export($record) .")\n";
  }

  // Dump the values if there are some.
  if ($insert) {
    $output .= "db_insert('". $table . "')->fields(". drupal_var_export(array_keys($data['fields'])) .")\n";
    $output .= $insert;
    $output .= "->execute();\n";
  }

  // Add the statement fixing the serial in the user table.
  if ($table == 'users') {
    $output .= "db_query('UPDATE {users} SET uid = uid - 1');\n";
  }

  $output .= "\n";
}

print $output;
