<?php

/**
 * @file
 * Generic Database schema code.
 */

require_once dirname(__FILE__) . '/query.inc';

/**
 * @defgroup schemaapi Schema API
 * @{
 * API to handle database schemas.
 *
 * A Drupal schema definition is an array structure representing one or
 * more tables and their related keys and indexes. A schema is defined by
 * hook_schema(), which usually lives in a modulename.install file.
 *
 * By implementing hook_schema() and specifying the tables your module
 * declares, you can easily create and drop these tables on all
 * supported database engines. You don't have to deal with the
 * different SQL dialects for table creation and alteration of the
 * supported database engines.
 *
 * hook_schema() should return an array with a key for each table that
 * the module defines.
 *
 * The following keys are defined:
 *   - 'description': A string in non-markup plain text describing this table
 *     and its purpose. References to other tables should be enclosed in
 *     curly-brackets. For example, the node_revisions table
 *     description field might contain "Stores per-revision title and
 *     body data for each {node}."
 *   - 'fields': An associative array ('fieldname' => specification)
 *     that describes the table's database columns. The specification
 *     is also an array. The following specification parameters are defined:
 *     - 'description': A string in non-markup plain text describing this field
 *       and its purpose. References to other tables should be enclosed in
 *       curly-brackets. For example, the node table vid field
 *       description might contain "Always holds the largest (most
 *       recent) {node_revision}.vid value for this nid."
 *     - 'type': The generic datatype: 'char', 'varchar', 'text', 'blob', 'int',
 *       'float', 'numeric', or 'serial'. Most types just map to the according
 *       database engine specific datatypes. Use 'serial' for auto incrementing
 *       fields. This will expand to 'INT auto_increment' on MySQL.
 *     - 'mysql_type', 'pgsql_type', 'sqlite_type', etc.: If you need to
 *       use a record type not included in the officially supported list
 *       of types above, you can specify a type for each database
 *       backend. In this case, you can leave out the type parameter,
 *       but be advised that your schema will fail to load on backends that
 *       do not have a type specified. A possible solution can be to
 *       use the "text" type as a fallback.
 *     - 'serialize': A boolean indicating whether the field will be stored as
 *       a serialized string.
 *     - 'size': The data size: 'tiny', 'small', 'medium', 'normal',
 *       'big'. This is a hint about the largest value the field will
 *       store and determines which of the database engine specific
 *       datatypes will be used (e.g. on MySQL, TINYINT vs. INT vs. BIGINT).
 *       'normal', the default, selects the base type (e.g. on MySQL,
 *       INT, VARCHAR, BLOB, etc.).
 *       Not all sizes are available for all data types. See
 *       DatabaseSchema::getFieldTypeMap() for possible combinations.
 *     - 'not null': If true, no NULL values will be allowed in this
 *       database column. Defaults to false.
 *     - 'default': The field's default value. The PHP type of the
 *       value matters: '', '0', and 0 are all different. If you
 *       specify '0' as the default value for a type 'int' field it
 *       will not work because '0' is a string containing the
 *       character "zero", not an integer.
 *     - 'length': The maximal length of a type 'char', 'varchar' or 'text'
 *       field. Ignored for other field types.
 *     - 'unsigned': A boolean indicating whether a type 'int', 'float'
 *       and 'numeric' only is signed or unsigned. Defaults to
 *       FALSE. Ignored for other field types.
 *     - 'precision', 'scale': For type 'numeric' fields, indicates
 *       the precision (total number of significant digits) and scale
 *       (decimal digits right of the decimal point). Both values are
 *       mandatory. Ignored for other field types.
 *     - 'binary': A boolean indicating that MySQL should force 'char',
 *       'varchar' or 'text' fields to use case-sensitive binary collation.
 *       This has no effect on other database types for which case sensitivity
 *       is already the default behavior.
 *     All parameters apart from 'type' are optional except that type
 *     'numeric' columns must specify 'precision' and 'scale', and type
 *     'varchar' must specify the 'length' parameter.
 *  - 'primary key': An array of one or more key column specifiers (see below)
 *    that form the primary key.
 *  - 'unique keys': An associative array of unique keys ('keyname' =>
 *    specification). Each specification is an array of one or more
 *    key column specifiers (see below) that form a unique key on the table.
 *  - 'foreign keys': An associative array of relations ('my_relation' =>
 *    specification). Each specification is an array containing the name of
 *    the referenced table ('table'), and an array of column mappings
 *    ('columns'). Column mappings are defined by key pairs ('source_column' =>
 *    'referenced_column'). This key is for documentation purposes only; foreign
 *    keys are not created in the database, nor are they enforced by Drupal.
 *  - 'indexes':  An associative array of indexes ('indexname' =>
 *    specification). Each specification is an array of one or more
 *    key column specifiers (see below) that form an index on the
 *    table.
 *
 * A key column specifier is either a string naming a column or an
 * array of two elements, column name and length, specifying a prefix
 * of the named column.
 *
 * As an example, here is a SUBSET of the schema definition for
 * Drupal's 'node' table. It show four fields (nid, vid, type, and
 * title), the primary key on field 'nid', a unique key named 'vid' on
 * field 'vid', and two indexes, one named 'nid' on field 'nid' and
 * one named 'node_title_type' on the field 'title' and the first four
 * bytes of the field 'type':
 *
 * @code
 * $schema['node'] = array(
 *   'description' => 'The base table for nodes.',
 *   'fields' => array(
 *     'nid'       => array('type' => 'serial', 'unsigned' => TRUE, 'not null' => TRUE),
 *     'vid'       => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE,'default' => 0),
 *     'type'      => array('type' => 'varchar','length' => 32,'not null' => TRUE, 'default' => ''),
 *     'language'  => array('type' => 'varchar','length' => 12,'not null' => TRUE,'default' => ''),
 *     'title'     => array('type' => 'varchar','length' => 255,'not null' => TRUE, 'default' => ''),
 *     'uid'       => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *     'status'    => array('type' => 'int', 'not null' => TRUE, 'default' => 1),
 *     'created'   => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *     'changed'   => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *     'comment'   => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *     'promote'   => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *     'moderate'  => array('type' => 'int', 'not null' => TRUE,'default' => 0),
 *     'sticky'    => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *     'tnid'      => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *     'translate' => array('type' => 'int', 'not null' => TRUE, 'default' => 0),
 *   ),
 *   'indexes' => array(
 *     'node_changed'        => array('changed'),
 *     'node_created'        => array('created'),
 *     'node_moderate'       => array('moderate'),
 *     'node_frontpage'      => array('promote', 'status', 'sticky', 'created'),
 *     'node_status_type'    => array('status', 'type', 'nid'),
 *     'node_title_type'     => array('title', array('type', 4)),
 *     'node_type'           => array(array('type', 4)),
 *     'uid'                 => array('uid'),
 *     'tnid'                => array('tnid'),
 *     'translate'           => array('translate'),
 *   ),
 *   'unique keys' => array(
 *     'vid' => array('vid'),
 *   ),
 *   // For documentation purposes only; foreign keys are not created in the
 *   // database.
 *   'foreign keys' => array(
 *     'node_revision' => array(
 *       'table' => 'node_revision',
 *       'columns' => array('vid' => 'vid'),
 *      ),
 *     'node_author' => array(
 *       'table' => 'users',
 *       'columns' => array('uid' => 'uid'),
 *      ),
 *    ),
 *   'primary key' => array('nid'),
 * );
 * @endcode
 *
 * @see drupal_install_schema()
 */

/**
 * Base class for database schema definitions.
 */
abstract class DatabaseSchema implements QueryPlaceholderInterface {

  /**
   * The database connection.
   *
   * @var DatabaseConnection
   */
  protected $connection;

  /**
   * The placeholder counter.
   */
  protected $placeholder = 0;

  /**
   * Definition of prefixInfo array structure.
   *
   * Rather than redefining DatabaseSchema::getPrefixInfo() for each driver,
   * by defining the defaultSchema variable only MySQL has to re-write the
   * method.
   *
   * @see DatabaseSchema::getPrefixInfo()
   */
  protected $defaultSchema = 'public';

  /**
   * A unique identifier for this query object.
   */
  protected $uniqueIdentifier;

  public function __construct($connection) {
    $this->uniqueIdentifier = uniqid('', TRUE);
    $this->connection = $connection;
  }

  /**
   * Implements the magic __clone function.
   */
  public function __clone() {
    $this->uniqueIdentifier = uniqid('', TRUE);
  }

  /**
   * Implements QueryPlaceHolderInterface::uniqueIdentifier().
   */
  public function uniqueIdentifier() {
    return $this->uniqueIdentifier;
  }

  /**
   * Implements QueryPlaceHolderInterface::nextPlaceholder().
   */
  public function nextPlaceholder() {
    return $this->placeholder++;
  }

  /**
   * Get information about the table name and schema from the prefix.
   *
   * @param
   *   Name of table to look prefix up for. Defaults to 'default' because thats
   *   default key for prefix.
   * @param $add_prefix
   *   Boolean that indicates whether the given table name should be prefixed.
   *
   * @return
   *   A keyed array with information about the schema, table name and prefix.
   */
  protected function getPrefixInfo($table = 'default', $add_prefix = TRUE) {
    $info = array(
      'schema' => $this->defaultSchema,
      'prefix' => $this->connection->tablePrefix($table),
    );
    if ($add_prefix) {
      $table = $info['prefix'] . $table;
    }
    // If the prefix contains a period in it, then that means the prefix also
    // contains a schema reference in which case we will change the schema key
    // to the value before the period in the prefix. Everything after the dot
    // will be prefixed onto the front of the table.
    if (($pos = strpos($table, '.')) !== FALSE) {
      // Grab everything before the period.
      $info['schema'] = substr($table, 0, $pos);
      // Grab everything after the dot.
      $info['table'] = substr($table, ++$pos);
    }
    else {
      $info['table'] = $table;
    }
    return $info;
  }

  /**
   * Create names for indexes, primary keys and constraints.
   *
   * This prevents using {} around non-table names like indexes and keys.
   */
  function prefixNonTable($table) {
    $args = func_get_args();
    $info = $this->getPrefixInfo($table);
    $args[0] = $info['table'];
    return implode('_', $args);
  }

  /**
   * Build a condition to match a table name against a standard information_schema.
   *
   * The information_schema is a SQL standard that provides information about the
   * database server and the databases, schemas, tables, columns and users within
   * it. This makes information_schema a useful tool to use across the drupal
   * database drivers and is used by a few different functions. The function below
   * describes the conditions to be meet when querying information_schema.tables
   * for drupal tables or information associated with drupal tables. Even though
   * this is the standard method, not all databases follow standards and so this
   * method should be overwritten by a database driver if the database provider
   * uses alternate methods. Because information_schema.tables is used in a few
   * different functions, a database driver will only need to override this function
   * to make all the others work. For example see includes/databases/mysql/schema.inc.
   *
   * @param $table_name
   *   The name of the table in question.
   * @param $operator
   *   The operator to apply on the 'table' part of the condition.
   * @param $add_prefix
   *   Boolean to indicate whether the table name needs to be prefixed.
   *
   * @return QueryConditionInterface
   *   A DatabaseCondition object.
   */
  protected function buildTableNameCondition($table_name, $operator = '=', $add_prefix = TRUE) {
    $info = $this->connection->getConnectionOptions();

    // Retrieve the table name and schema
    $table_info = $this->getPrefixInfo($table_name, $add_prefix);

    $condition = new DatabaseCondition('AND');
    $condition->condition('table_catalog', $info['database']);
    $condition->condition('table_schema', $table_info['schema']);
    $condition->condition('table_name', $table_info['table'], $operator);
    return $condition;
  }

  /**
   * Check if a table exists.
   *
   * @param $table
   *   The name of the table in drupal (no prefixing).
   *
   * @return
   *   TRUE if the given table exists, otherwise FALSE.
   */
  public function tableExists($table) {
    $condition = $this->buildTableNameCondition($table);
    $condition->compile($this->connection, $this);
    // Normally, we would heartily discourage the use of string
    // concatenation for conditionals like this however, we
    // couldn't use db_select() here because it would prefix
    // information_schema.tables and the query would fail.
    // Don't use {} around information_schema.tables table.
    return (bool) $this->connection->query("SELECT 1 FROM information_schema.tables WHERE " . (string) $condition, $condition->arguments())->fetchField();
  }

  /**
   * Find all tables that are like the specified base table name.
   *
   * @param $table_expression
   *   An SQL expression, for example "simpletest%" (without the quotes).
   *   BEWARE: this is not prefixed, the caller should take care of that.
   *
   * @return
   *   Array, both the keys and the values are the matching tables.
   */
  public function findTables($table_expression) {
    $condition = $this->buildTableNameCondition($table_expression, 'LIKE', FALSE);

    $condition->compile($this->connection, $this);
    // Normally, we would heartily discourage the use of string
    // concatenation for conditionals like this however, we
    // couldn't use db_select() here because it would prefix
    // information_schema.tables and the query would fail.
    // Don't use {} around information_schema.tables table.
    return $this->connection->query("SELECT table_name AS table_name FROM information_schema.tables WHERE " . (string) $condition, $condition->arguments())->fetchAllKeyed(0, 0);
  }

  /**
   * Finds all tables that are like the specified base table name. This is a
   * backport of the change made to findTables in Drupal 8 to work with virtual,
   * un-prefixed table names. The original function is retained for Backwards
   * Compatibility.
   * @see https://www.drupal.org/node/2552435
   *
   * @param string $table_expression
   *   An SQL expression, for example "cache_%" (without the quotes).
   *
   * @return array
   *   Both the keys and the values are the matching tables.
   */
  public function findTablesD8($table_expression) {
    // Load all the tables up front in order to take into account per-table
    // prefixes. The actual matching is done at the bottom of the method.
    $condition = $this->buildTableNameCondition('%', 'LIKE');
    $condition->compile($this->connection, $this);

    $individually_prefixed_tables = $this->connection->getUnprefixedTablesMap();
    $default_prefix = $this->connection->tablePrefix();
    $default_prefix_length = strlen($default_prefix);
    $tables = array();
    // Normally, we would heartily discourage the use of string
    // concatenation for conditionals like this however, we
    // couldn't use db_select() here because it would prefix
    // information_schema.tables and the query would fail.
    // Don't use {} around information_schema.tables table.
    $results = $this->connection->query("SELECT table_name AS table_name FROM information_schema.tables WHERE " . (string) $condition, $condition->arguments());
    foreach ($results as $table) {
      // Take into account tables that have an individual prefix.
      if (isset($individually_prefixed_tables[$table->table_name])) {
        $prefix_length = strlen($this->connection->tablePrefix($individually_prefixed_tables[$table->table_name]));
      }
      elseif ($default_prefix && substr($table->table_name, 0, $default_prefix_length) !== $default_prefix) {
        // This table name does not start the default prefix, which means that
        // it is not managed by Drupal so it should be excluded from the result.
        continue;
      }
      else {
        $prefix_length = $default_prefix_length;
      }

      // Remove the prefix from the returned tables.
      $unprefixed_table_name = substr($table->table_name, $prefix_length);

      // The pattern can match a table which is the same as the prefix. That
      // will become an empty string when we remove the prefix, which will
      // probably surprise the caller, besides not being a prefixed table. So
      // remove it.
      if (!empty($unprefixed_table_name)) {
        $tables[$unprefixed_table_name] = $unprefixed_table_name;
      }
    }

    // Convert the table expression from its SQL LIKE syntax to a regular
    // expression and escape the delimiter that will be used for matching.
    $table_expression = str_replace(array('%', '_'), array('.*?', '.'), preg_quote($table_expression, '/'));
    $tables = preg_grep('/^' . $table_expression . '$/i', $tables);

    return $tables;
  }

  /**
   * Check if a column exists in the given table.
   *
   * @param $table
   *   The name of the table in drupal (no prefixing).
   * @param $name
   *   The name of the column.
   *
   * @return
   *   TRUE if the given column exists, otherwise FALSE.
   */
  public function fieldExists($table, $column) {
    $condition = $this->buildTableNameCondition($table);
    $condition->condition('column_name', $column);
    $condition->compile($this->connection, $this);
    // Normally, we would heartily discourage the use of string
    // concatenation for conditionals like this however, we
    // couldn't use db_select() here because it would prefix
    // information_schema.tables and the query would fail.
    // Don't use {} around information_schema.columns table.
    return (bool) $this->connection->query("SELECT 1 FROM information_schema.columns WHERE " . (string) $condition, $condition->arguments())->fetchField();
  }

  /**
   * Returns a mapping of Drupal schema field names to DB-native field types.
   *
   * Because different field types do not map 1:1 between databases, Drupal has
   * its own normalized field type names. This function returns a driver-specific
   * mapping table from Drupal names to the native names for each database.
   *
   * @return array
   *   An array of Schema API field types to driver-specific field types.
   */
  abstract public function getFieldTypeMap();

  /**
   * Rename a table.
   *
   * @param $table
   *   The table to be renamed.
   * @param $new_name
   *   The new name for the table.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table doesn't exist.
   * @throws DatabaseSchemaObjectExistsException
   *   If a table with the specified new name already exists.
   */
  abstract public function renameTable($table, $new_name);

  /**
   * Drop a table.
   *
   * @param $table
   *   The table to be dropped.
   *
   * @return
   *   TRUE if the table was successfully dropped, FALSE if there was no table
   *   by that name to begin with.
   */
  abstract public function dropTable($table);

  /**
   * Add a new field to a table.
   *
   * @param $table
   *   Name of the table to be altered.
   * @param $field
   *   Name of the field to be added.
   * @param $spec
   *   The field specification array, as taken from a schema definition.
   *   The specification may also contain the key 'initial', the newly
   *   created field will be set to the value of the key in all rows.
   *   This is most useful for creating NOT NULL columns with no default
   *   value in existing tables.
   * @param $keys_new
   *   (optional) Keys and indexes specification to be created on the
   *   table along with adding the field. The format is the same as a
   *   table specification but without the 'fields' element. If you are
   *   adding a type 'serial' field, you MUST specify at least one key
   *   or index including it in this array. See db_change_field() for more
   *   explanation why.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table doesn't exist.
   * @throws DatabaseSchemaObjectExistsException
   *   If the specified table already has a field by that name.
   */
  abstract public function addField($table, $field, $spec, $keys_new = array());

  /**
   * Drop a field.
   *
   * @param $table
   *   The table to be altered.
   * @param $field
   *   The field to be dropped.
   *
   * @return
   *   TRUE if the field was successfully dropped, FALSE if there was no field
   *   by that name to begin with.
   */
  abstract public function dropField($table, $field);

  /**
   * Set the default value for a field.
   *
   * @param $table
   *   The table to be altered.
   * @param $field
   *   The field to be altered.
   * @param $default
   *   Default value to be set. NULL for 'default NULL'.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table or field doesn't exist.
   */
  abstract public function fieldSetDefault($table, $field, $default);

  /**
   * Set a field to have no default value.
   *
   * @param $table
   *   The table to be altered.
   * @param $field
   *   The field to be altered.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table or field doesn't exist.
   */
  abstract public function fieldSetNoDefault($table, $field);

  /**
   * Checks if an index exists in the given table.
   *
   * @param $table
   *   The name of the table in drupal (no prefixing).
   * @param $name
   *   The name of the index in drupal (no prefixing).
   *
   * @return
   *   TRUE if the given index exists, otherwise FALSE.
   */
  abstract public function indexExists($table, $name);

  /**
   * Add a primary key.
   *
   * @param $table
   *   The table to be altered.
   * @param $fields
   *   Fields for the primary key.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table doesn't exist.
   * @throws DatabaseSchemaObjectExistsException
   *   If the specified table already has a primary key.
   */
  abstract public function addPrimaryKey($table, $fields);

  /**
   * Drop the primary key.
   *
   * @param $table
   *   The table to be altered.
   *
   * @return
   *   TRUE if the primary key was successfully dropped, FALSE if there was no
   *   primary key on this table to begin with.
   */
  abstract public function dropPrimaryKey($table);

  /**
   * Add a unique key.
   *
   * @param $table
   *   The table to be altered.
   * @param $name
   *   The name of the key.
   * @param $fields
   *   An array of field names.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table doesn't exist.
   * @throws DatabaseSchemaObjectExistsException
   *   If the specified table already has a key by that name.
   */
  abstract public function addUniqueKey($table, $name, $fields);

  /**
   * Drop a unique key.
   *
   * @param $table
   *   The table to be altered.
   * @param $name
   *   The name of the key.
   *
   * @return
   *   TRUE if the key was successfully dropped, FALSE if there was no key by
   *   that name to begin with.
   */
  abstract public function dropUniqueKey($table, $name);

  /**
   * Add an index.
   *
   * @param $table
   *   The table to be altered.
   * @param $name
   *   The name of the index.
   * @param $fields
   *   An array of field names.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table doesn't exist.
   * @throws DatabaseSchemaObjectExistsException
   *   If the specified table already has an index by that name.
   */
  abstract public function addIndex($table, $name, $fields);

  /**
   * Drop an index.
   *
   * @param $table
   *   The table to be altered.
   * @param $name
   *   The name of the index.
   *
   * @return
   *   TRUE if the index was successfully dropped, FALSE if there was no index
   *   by that name to begin with.
   */
  abstract public function dropIndex($table, $name);

  /**
   * Change a field definition.
   *
   * IMPORTANT NOTE: To maintain database portability, you have to explicitly
   * recreate all indices and primary keys that are using the changed field.
   *
   * That means that you have to drop all affected keys and indexes with
   * db_drop_{primary_key,unique_key,index}() before calling db_change_field().
   * To recreate the keys and indices, pass the key definitions as the
   * optional $keys_new argument directly to db_change_field().
   *
   * For example, suppose you have:
   * @code
   * $schema['foo'] = array(
   *   'fields' => array(
   *     'bar' => array('type' => 'int', 'not null' => TRUE)
   *   ),
   *   'primary key' => array('bar')
   * );
   * @endcode
   * and you want to change foo.bar to be type serial, leaving it as the
   * primary key. The correct sequence is:
   * @code
   * db_drop_primary_key('foo');
   * db_change_field('foo', 'bar', 'bar',
   *   array('type' => 'serial', 'not null' => TRUE),
   *   array('primary key' => array('bar')));
   * @endcode
   *
   * The reasons for this are due to the different database engines:
   *
   * On PostgreSQL, changing a field definition involves adding a new field
   * and dropping an old one which* causes any indices, primary keys and
   * sequences (from serial-type fields) that use the changed field to be dropped.
   *
   * On MySQL, all type 'serial' fields must be part of at least one key
   * or index as soon as they are created. You cannot use
   * db_add_{primary_key,unique_key,index}() for this purpose because
   * the ALTER TABLE command will fail to add the column without a key
   * or index specification. The solution is to use the optional
   * $keys_new argument to create the key or index at the same time as
   * field.
   *
   * You could use db_add_{primary_key,unique_key,index}() in all cases
   * unless you are converting a field to be type serial. You can use
   * the $keys_new argument in all cases.
   *
   * @param $table
   *   Name of the table.
   * @param $field
   *   Name of the field to change.
   * @param $field_new
   *   New name for the field (set to the same as $field if you don't want to change the name).
   * @param $spec
   *   The field specification for the new field.
   * @param $keys_new
   *   (optional) Keys and indexes specification to be created on the
   *   table along with changing the field. The format is the same as a
   *   table specification but without the 'fields' element.
   *
   * @throws DatabaseSchemaObjectDoesNotExistException
   *   If the specified table or source field doesn't exist.
   * @throws DatabaseSchemaObjectExistsException
   *   If the specified destination field already exists.
   */
  abstract public function changeField($table, $field, $field_new, $spec, $keys_new = array());

  /**
   * Create a new table from a Drupal table definition.
   *
   * @param $name
   *   The name of the table to create.
   * @param $table
   *   A Schema API table definition array.
   *
   * @throws DatabaseSchemaObjectExistsException
   *   If the specified table already exists.
   */
  public function createTable($name, $table) {
    if ($this->tableExists($name)) {
      throw new DatabaseSchemaObjectExistsException(t('Table @name already exists.', array('@name' => $name)));
    }
    $statements = $this->createTableSql($name, $table);
    foreach ($statements as $statement) {
      $this->connection->query($statement);
    }
  }

  /**
   * Return an array of field names from an array of key/index column specifiers.
   *
   * This is usually an identity function but if a key/index uses a column prefix
   * specification, this function extracts just the name.
   *
   * @param $fields
   *   An array of key/index column specifiers.
   *
   * @return
   *   An array of field names.
   */
  public function fieldNames($fields) {
    $return = array();
    foreach ($fields as $field) {
      if (is_array($field)) {
        $return[] = $field[0];
      }
      else {
        $return[] = $field;
      }
    }
    return $return;
  }

  /**
   * Prepare a table or column comment for database query.
   *
   * @param $comment
   *   The comment string to prepare.
   * @param $length
   *   Optional upper limit on the returned string length.
   *
   * @return
   *   The prepared comment.
   */
  public function prepareComment($comment, $length = NULL) {
    return $this->connection->quote($comment);
  }
}

/**
 * Exception thrown if an object being created already exists.
 *
 * For example, this exception should be thrown whenever there is an attempt to
 * create a new database table, field, or index that already exists in the
 * database schema.
 */
class DatabaseSchemaObjectExistsException extends Exception {}

/**
 * Exception thrown if an object being modified doesn't exist yet.
 *
 * For example, this exception should be thrown whenever there is an attempt to
 * modify a database table, field, or index that does not currently exist in
 * the database schema.
 */
class DatabaseSchemaObjectDoesNotExistException extends Exception {}

/**
 * @} End of "defgroup schemaapi".
 */

