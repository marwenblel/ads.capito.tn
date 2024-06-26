<?php

/**
 * @file
 * Database schema code for PostgreSQL database servers.
 */

/**
 * @ingroup schemaapi
 * @{
 */

class DatabaseSchema_pgsql extends DatabaseSchema {

  /**
   * PostgreSQL's temporary namespace name.
   *
   * @var string
   */
  protected $tempNamespaceName;

  /**
   * A cache of information about blob columns and sequences of tables.
   *
   * This is collected by DatabaseConnection_pgsql->queryTableInformation(),
   * by introspecting the database.
   *
   * @see DatabaseConnection_pgsql->queryTableInformation()
   * @var array
   */
  protected $tableInformation = array();

  /**
   * The maximum allowed length for index, primary key and constraint names.
   *
   * Value will usually be set to a 63 chars limit but PostgreSQL allows
   * to higher this value before compiling, so we need to check for that.
   *
   * @var int
   */
  protected $maxIdentifierLength;

  /**
   * Make sure to limit identifiers according to PostgreSQL compiled in length.
   *
   * PostgreSQL allows in standard configuration identifiers no longer than 63
   * chars for table/relation names, indexes, primary keys, and constraints. So
   * we map all identifiers that are too long to drupal_base64hash_tag, where
   * tag is one of:
   *   - idx for indexes
   *   - key for constraints
   *   - pkey for primary keys
   *   - seq for sequences
   *
   * @param string $table_identifier_part
   *   The first argument used to build the identifier string. This usually
   *   refers to a table/relation name.
   * @param string $column_identifier_part
   *   The second argument used to build the identifier string. This usually
   *   refers to one or more column names.
   * @param string $tag
   *   The identifier tag. It can be one of 'idx', 'key', 'pkey' or 'seq'.
   *
   * @return string
   *   The index/constraint/pkey identifier.
   */
  protected function ensureIdentifiersLength($table_identifier_part, $column_identifier_part, $tag) {
    $info = $this->getPrefixInfo($table_identifier_part);
    $table_identifier_part = $info['table'];

    // Filters out potentially empty $column_identifier_part to ensure
    // compatibility with old naming convention (see prefixNonTable()).
    $identifiers = array_filter(array($table_identifier_part, $column_identifier_part, $tag));
    $identifierName = implode('_', $identifiers);

    // Retrieve the max identifier length which is usually 63 characters
    // but can be altered before PostgreSQL is compiled so we need to check.
    if (empty($this->maxIdentifierLength)) {
      $this->maxIdentifierLength = $this->connection->query("SHOW max_identifier_length")->fetchField();
    }

    if (strlen($identifierName) > $this->maxIdentifierLength) {
      $saveIdentifier = 'drupal_' . $this->hashBase64($identifierName) . '_' . $tag;
    }
    else {
      $saveIdentifier = $identifierName;
    }
    return $saveIdentifier;
  }

  /**
   * Fetch the list of blobs and sequences used on a table.
   *
   * We introspect the database to collect the information required by insert
   * and update queries.
   *
   * @param $table_name
   *   The non-prefixed name of the table.
   * @return
   *   An object with two member variables:
   *     - 'blob_fields' that lists all the blob fields in the table.
   *     - 'sequences' that lists the sequences used in that table.
   */
  public function queryTableInformation($table) {
    // Generate a key to reference this table's information on.
    $key = $this->connection->prefixTables('{' . $table . '}');

    // Take into account that temporary tables are stored in a different schema.
    // \DatabaseConnection::generateTemporaryTableName() sets 'db_temporary_'
    // prefix to all temporary tables.
    if (strpos($key, '.') === FALSE && strpos($table, 'db_temporary_') === FALSE) {
      $key = 'public.' . $key;
    }
    else {
      $key = $this->getTempNamespaceName() . '.' . $key;
    }

    if (!isset($this->tableInformation[$key])) {
      $table_information = (object) array(
        'blob_fields' => array(),
        'sequences' => array(),
      );

      // The bytea columns and sequences for a table can be found in
      // pg_attribute, which is significantly faster than querying the
      // information_schema. The data type of a field can be found by lookup
      // of the attribute ID, and the default value must be extracted from the
      // node tree for the attribute definition instead of the historical
      // human-readable column, adsrc.
      $sql = <<<'EOD'
SELECT pg_attribute.attname AS column_name, format_type(pg_attribute.atttypid, pg_attribute.atttypmod) AS data_type, pg_get_expr(pg_attrdef.adbin, pg_attribute.attrelid) AS column_default
FROM pg_attribute
LEFT JOIN pg_attrdef ON pg_attrdef.adrelid = pg_attribute.attrelid AND pg_attrdef.adnum = pg_attribute.attnum
WHERE pg_attribute.attnum > 0
AND NOT pg_attribute.attisdropped
AND pg_attribute.attrelid = :key::regclass
AND (format_type(pg_attribute.atttypid, pg_attribute.atttypmod) = 'bytea'
OR pg_get_expr(pg_attrdef.adbin, pg_attribute.attrelid) LIKE 'nextval%')
EOD;
      $result = $this->connection->query($sql, array(
        ':key' => $key,
      ));

      if (empty($result)) {
        return $table_information;
      }

      foreach ($result as $column) {
        if ($column->data_type == 'bytea') {
          $table_information->blob_fields[$column->column_name] = TRUE;
        }
        elseif (preg_match("/nextval\('([^']+)'/", $column->column_default, $matches)) {
          // We must know of any sequences in the table structure to help us
          // return the last insert id. If there is more than 1 sequences the
          // first one (index 0 of the sequences array) will be used.
          $table_information->sequences[] = $matches[1];
          $table_information->serial_fields[] = $column->column_name;
        }
      }
      $this->tableInformation[$key] = $table_information;
    }
    return $this->tableInformation[$key];
  }

  /**
   * Gets PostgreSQL's temporary namespace name.
   *
   * @return string
   *   PostgreSQL's temporary namespace anme.
   */
  protected function getTempNamespaceName() {
    if (!isset($this->tempNamespaceName)) {
      $this->tempNamespaceName = $this->connection->query('SELECT nspname FROM pg_namespace WHERE oid = pg_my_temp_schema()')->fetchField();
    }
    return $this->tempNamespaceName;
  }

  /**
   * Fetch the list of CHECK constraints used on a field.
   *
   * We introspect the database to collect the information required by field
   * alteration.
   *
   * @param $table
   *   The non-prefixed name of the table.
   * @param $field
   *   The name of the field.
   * @return
   *   An array of all the checks for the field.
   */
  public function queryFieldInformation($table, $field) {
    $prefixInfo = $this->getPrefixInfo($table, TRUE);

    // Split the key into schema and table for querying.
    $schema = $prefixInfo['schema'];
    $table_name = $prefixInfo['table'];

    $field_information = (object) array(
        'checks' => array(),
    );
    $checks = $this->connection->query("SELECT conname FROM pg_class cl INNER JOIN pg_constraint co ON co.conrelid = cl.oid INNER JOIN pg_attribute attr ON attr.attrelid = cl.oid AND attr.attnum = ANY (co.conkey) INNER JOIN pg_namespace ns ON cl.relnamespace = ns.oid WHERE co.contype = 'c' AND ns.nspname = :schema AND cl.relname = :table AND attr.attname = :column", array(
      ':schema' => $schema,
      ':table' => $table_name,
      ':column' => $field,
    ));
    $field_information = $checks->fetchCol();

    return $field_information;
  }

  /**
   * Generate SQL to create a new table from a Drupal schema definition.
   *
   * @param $name
   *   The name of the table to create.
   * @param $table
   *   A Schema API table definition array.
   * @return
   *   An array of SQL statements to create the table.
   */
  protected function createTableSql($name, $table) {
    $sql_fields = array();
    foreach ($table['fields'] as $field_name => $field) {
      $sql_fields[] = $this->createFieldSql($field_name, $this->processField($field));
    }

    $sql_keys = array();
    if (isset($table['primary key']) && is_array($table['primary key'])) {
      $sql_keys[] = 'CONSTRAINT ' . $this->ensureIdentifiersLength($name, '', 'pkey') . ' PRIMARY KEY (' . implode(', ', $table['primary key']) . ')';
    }
    if (isset($table['unique keys']) && is_array($table['unique keys'])) {
      foreach ($table['unique keys'] as $key_name => $key) {
        $sql_keys[] = 'CONSTRAINT ' . $this->ensureIdentifiersLength($name, $key_name, 'key') . ' UNIQUE (' . implode(', ', $key) . ')';
      }
    }

    $sql = "CREATE TABLE {" . $name . "} (\n\t";
    $sql .= implode(",\n\t", $sql_fields);
    if (count($sql_keys) > 0) {
      $sql .= ",\n\t";
    }
    $sql .= implode(",\n\t", $sql_keys);
    $sql .= "\n)";
    $statements[] = $sql;

    if (isset($table['indexes']) && is_array($table['indexes'])) {
      foreach ($table['indexes'] as $key_name => $key) {
        $statements[] = $this->_createIndexSql($name, $key_name, $key);
      }
    }

    // Add table comment.
    if (!empty($table['description'])) {
      $statements[] = 'COMMENT ON TABLE {' . $name . '} IS ' . $this->prepareComment($table['description']);
    }

    // Add column comments.
    foreach ($table['fields'] as $field_name => $field) {
      if (!empty($field['description'])) {
        $statements[] = 'COMMENT ON COLUMN {' . $name . '}.' . $field_name . ' IS ' . $this->prepareComment($field['description']);
      }
    }

    return $statements;
  }

  /**
   * Create an SQL string for a field to be used in table creation or
   * alteration.
   *
   * Before passing a field out of a schema definition into this
   * function it has to be processed by _db_process_field().
   *
   * @param $name
   *    Name of the field.
   * @param $spec
   *    The field specification, as per the schema data structure format.
   */
  protected function createFieldSql($name, $spec) {
    $sql = $name . ' ' . $spec['pgsql_type'];

    if (isset($spec['type']) && $spec['type'] == 'serial') {
      unset($spec['not null']);
    }

    if (in_array($spec['pgsql_type'], array('varchar', 'character', 'text')) && isset($spec['length'])) {
      $sql .= '(' . $spec['length'] . ')';
    }
    elseif (isset($spec['precision']) && isset($spec['scale'])) {
      $sql .= '(' . $spec['precision'] . ', ' . $spec['scale'] . ')';
    }

    if (!empty($spec['unsigned'])) {
      $sql .= " CHECK ($name >= 0)";
    }

    if (isset($spec['not null'])) {
      if ($spec['not null']) {
        $sql .= ' NOT NULL';
      }
      else {
        $sql .= ' NULL';
      }
    }
    if (isset($spec['default'])) {
      $default = is_string($spec['default']) ? "'" . $spec['default'] . "'" : $spec['default'];
      $sql .= " default $default";
    }

    return $sql;
  }

  /**
   * Set database-engine specific properties for a field.
   *
   * @param $field
   *   A field description array, as specified in the schema documentation.
   */
  protected function processField($field) {
    if (!isset($field['size'])) {
      $field['size'] = 'normal';
    }

    // Set the correct database-engine specific datatype.
    // In case one is already provided, force it to lowercase.
    if (isset($field['pgsql_type'])) {
      $field['pgsql_type'] = drupal_strtolower($field['pgsql_type']);
    }
    else {
      $map = $this->getFieldTypeMap();
      $field['pgsql_type'] = $map[$field['type'] . ':' . $field['size']];
    }

    if (!empty($field['unsigned'])) {
      // Unsigned datatypes are not supported in PostgreSQL 8.3. In MySQL,
      // they are used to ensure a positive number is inserted and it also
      // doubles the maximum integer size that can be stored in a field.
      // The PostgreSQL schema in Drupal creates a check constraint
      // to ensure that a value inserted is >= 0. To provide the extra
      // integer capacity, here, we bump up the column field size.
      if (!isset($map)) {
        $map = $this->getFieldTypeMap();
      }
      switch ($field['pgsql_type']) {
        case 'smallint':
          $field['pgsql_type'] = $map['int:medium'];
          break;
        case 'int' :
          $field['pgsql_type'] = $map['int:big'];
          break;
      }
    }
    if (isset($field['type']) && $field['type'] == 'serial') {
      unset($field['not null']);
    }
    return $field;
  }

  /**
   * This maps a generic data type in combination with its data size
   * to the engine-specific data type.
   */
  function getFieldTypeMap() {
    // Put :normal last so it gets preserved by array_flip. This makes
    // it much easier for modules (such as schema.module) to map
    // database types back into schema types.
    // $map does not use drupal_static as its value never changes.
    static $map = array(
      'varchar:normal' => 'varchar',
      'char:normal' => 'character',

      'text:tiny' => 'text',
      'text:small' => 'text',
      'text:medium' => 'text',
      'text:big' => 'text',
      'text:normal' => 'text',

      'int:tiny' => 'smallint',
      'int:small' => 'smallint',
      'int:medium' => 'int',
      'int:big' => 'bigint',
      'int:normal' => 'int',

      'float:tiny' => 'real',
      'float:small' => 'real',
      'float:medium' => 'real',
      'float:big' => 'double precision',
      'float:normal' => 'real',

      'numeric:normal' => 'numeric',

      'blob:big' => 'bytea',
      'blob:normal' => 'bytea',

      'serial:tiny' => 'serial',
      'serial:small' => 'serial',
      'serial:medium' => 'serial',
      'serial:big' => 'bigserial',
      'serial:normal' => 'serial',
      );
    return $map;
  }

  protected function _createKeySql($fields) {
    $return = array();
    foreach ($fields as $field) {
      if (is_array($field)) {
        $return[] = 'substr(' . $field[0] . ', 1, ' . $field[1] . ')';
      }
      else {
        $return[] = '"' . $field . '"';
      }
    }
    return implode(', ', $return);
  }

  /**
   * {@inheritdoc}
   */
  public function tableExists($table) {
    // In PostgreSQL "unquoted names are always folded to lower case."
    // @see DatabaseSchema_pgsql::buildTableNameCondition().
    $prefixInfo = $this->getPrefixInfo(strtolower($table), TRUE);

    return (bool) $this->connection->query("SELECT 1 FROM pg_tables WHERE schemaname = :schema AND tablename = :table", array(':schema' => $prefixInfo['schema'], ':table' => $prefixInfo['table']))->fetchField();
  }

  /**
   * {@inheritdoc}
   */
  public function findTables($table_expression) {
    $individually_prefixed_tables = $this->connection->getUnprefixedTablesMap();
    $default_prefix = $this->connection->tablePrefix();
    $default_prefix_length = strlen($default_prefix);
    $tables = array();

    // Load all the tables up front in order to take into account per-table
    // prefixes. The actual matching is done at the bottom of the method.
    $results = $this->connection->query("SELECT tablename FROM pg_tables WHERE schemaname = :schema", array(':schema' => $this->defaultSchema));
    foreach ($results as $table) {
      // Take into account tables that have an individual prefix.
      if (isset($individually_prefixed_tables[$table->tablename])) {
        $prefix_length = strlen($this->connection->tablePrefix($individually_prefixed_tables[$table->tablename]));
      }
      elseif ($default_prefix && substr($table->tablename, 0, $default_prefix_length) !== $default_prefix) {
        // This table name does not start the default prefix, which means that
        // it is not managed by Drupal so it should be excluded from the result.
        continue;
      }
      else {
        $prefix_length = $default_prefix_length;
      }

      // Remove the prefix from the returned tables.
      $unprefixed_table_name = substr($table->tablename, $prefix_length);

      // The pattern can match a table which is the same as the prefix. That
      // will become an empty string when we remove the prefix, which will
      // probably surprise the caller, besides not being a prefixed table. So
      // remove it.
      if (!empty($unprefixed_table_name)) {
        $tables[$unprefixed_table_name] = $unprefixed_table_name;
      }
    }

    // Need to use strtolower on the table name as it was used previously by
    // DatabaseSchema_pgsql::buildTableNameCondition().
    // @see https://www.drupal.org/project/drupal/issues/3262341
    $table_expression = strtolower($table_expression);

    // Convert the table expression from its SQL LIKE syntax to a regular
    // expression and escape the delimiter that will be used for matching.
    $table_expression = str_replace(array('%', '_'), array('.*?', '.'), preg_quote($table_expression, '/'));
    $tables = preg_grep('/^' . $table_expression . '$/i', $tables);

    return $tables;
  }

  function renameTable($table, $new_name) {
    if (!$this->tableExists($table)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot rename @table to @table_new: table @table doesn't exist.", array('@table' => $table, '@table_new' => $new_name)));
    }
    if ($this->tableExists($new_name)) {
      throw new DatabaseSchemaObjectExistsException(t("Cannot rename @table to @table_new: table @table_new already exists.", array('@table' => $table, '@table_new' => $new_name)));
    }

    // Get the schema and tablename for the old table.
    $old_full_name = $this->connection->prefixTables('{' . $table . '}');
    list($old_schema, $old_table_name) = strpos($old_full_name, '.') ? explode('.', $old_full_name) : array('public', $old_full_name);

    // Index names and constraint names are global in PostgreSQL, so we need to
    // rename them when renaming the table.
    $indexes = $this->connection->query('SELECT indexname FROM pg_indexes WHERE schemaname = :schema AND tablename = :table', array(':schema' => $old_schema, ':table' => $old_table_name));
    foreach ($indexes as $index) {
      // Get the index type by suffix, e.g. idx/key/pkey
      $index_type = substr($index->indexname, strrpos($index->indexname, '_') + 1);

      // If the index is already rewritten by ensureIdentifiersLength() to not
      // exceed the 63 chars limit of PostgreSQL, we need to take care of that.
      // Example (drupal_Gk7Su_T1jcBHVuvSPeP22_I3Ni4GrVEgTYlIYnBJkro_idx).
      if (strpos($index->indexname, 'drupal_') !== FALSE) {
        preg_match('/^drupal_(.*)_' . preg_quote($index_type) . '/', $index->indexname, $matches);
        $index_name = $matches[1];
      }
      else {
        if ($index_type == 'pkey') {
          // Primary keys do not have a specific name in D7.
          $index_name = '';
        }
        else {
          // Make sure to remove the suffix from index names, because
          // ensureIdentifiersLength() will add the suffix again and thus
          // would result in a wrong index name.
          preg_match('/^' . preg_quote($old_full_name) . '_(.*)_' . preg_quote($index_type) . '/', $index->indexname, $matches);
          $index_name = $matches[1];
        }
      }

      $this->connection->query('ALTER INDEX ' . $index->indexname . ' RENAME TO ' . $this->ensureIdentifiersLength($new_name, $index_name, $index_type));
    }

    // Now rename the table.
    // Ensure the new table name does not include schema syntax.
    $prefixInfo = $this->getPrefixInfo($new_name);
    $this->connection->query('ALTER TABLE {' . $table . '} RENAME TO ' . $prefixInfo['table']);
  }

  public function dropTable($table) {
    if (!$this->tableExists($table)) {
      return FALSE;
    }

    $this->connection->query('DROP TABLE {' . $table . '}');
    return TRUE;
  }

  public function addField($table, $field, $spec, $new_keys = array()) {
    if (!$this->tableExists($table)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot add field @table.@field: table doesn't exist.", array('@field' => $field, '@table' => $table)));
    }
    if ($this->fieldExists($table, $field)) {
      throw new DatabaseSchemaObjectExistsException(t("Cannot add field @table.@field: field already exists.", array('@field' => $field, '@table' => $table)));
    }

    $fixnull = FALSE;
    if (!empty($spec['not null']) && !isset($spec['default'])) {
      $fixnull = TRUE;
      $spec['not null'] = FALSE;
    }
    $query = 'ALTER TABLE {' . $table . '} ADD COLUMN ';
    $query .= $this->createFieldSql($field, $this->processField($spec));
    $this->connection->query($query);
    if (isset($spec['initial'])) {
      $this->connection->update($table)
        ->fields(array($field => $spec['initial']))
        ->execute();
    }
    if ($fixnull) {
      $this->connection->query("ALTER TABLE {" . $table . "} ALTER $field SET NOT NULL");
    }
    if (isset($new_keys)) {
      $this->_createKeys($table, $new_keys);
    }
    // Add column comment.
    if (!empty($spec['description'])) {
      $this->connection->query('COMMENT ON COLUMN {' . $table . '}.' . $field . ' IS ' . $this->prepareComment($spec['description']));
    }
  }

  public function dropField($table, $field) {
    if (!$this->fieldExists($table, $field)) {
      return FALSE;
    }

    $this->connection->query('ALTER TABLE {' . $table . '} DROP COLUMN "' . $field . '"');
    return TRUE;
  }

  public function fieldSetDefault($table, $field, $default) {
    if (!$this->fieldExists($table, $field)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot set default value of field @table.@field: field doesn't exist.", array('@table' => $table, '@field' => $field)));
    }

    if (!isset($default)) {
      $default = 'NULL';
    }
    else {
      $default = is_string($default) ? "'$default'" : $default;
    }

    $this->connection->query('ALTER TABLE {' . $table . '} ALTER COLUMN "' . $field . '" SET DEFAULT ' . $default);
  }

  public function fieldSetNoDefault($table, $field) {
    if (!$this->fieldExists($table, $field)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot remove default value of field @table.@field: field doesn't exist.", array('@table' => $table, '@field' => $field)));
    }

    $this->connection->query('ALTER TABLE {' . $table . '} ALTER COLUMN "' . $field . '" DROP DEFAULT');
  }

  /**
   * {@inheritdoc}
   */
  public function fieldExists($table, $column) {
    // In PostgreSQL "unquoted names are always folded to lower case."
    // @see DatabaseSchema_pgsql::buildTableNameCondition().
    $prefixInfo = $this->getPrefixInfo(strtolower($table));

    return (bool) $this->connection->query("SELECT 1 FROM pg_attribute WHERE attrelid = :key::regclass AND attname = :column AND NOT attisdropped AND attnum > 0", array(':key' => $prefixInfo['schema'] . '.' . $prefixInfo['table'], ':column' => $column))->fetchField();
  }

  public function indexExists($table, $name) {
    // Details https://www.postgresql.org/docs/10/view-pg-indexes.html
    $index_name = $this->ensureIdentifiersLength($table, $name, 'idx');
    return (bool) $this->connection->query("SELECT 1 FROM pg_indexes WHERE indexname = '$index_name'")->fetchField();
  }

  /**
   * Helper function: check if a constraint (PK, FK, UK) exists.
   *
   * @param $table
   *   The name of the table.
   * @param $name
   *   The name of the constraint (typically 'pkey' or '[constraint]_key').
   */
  protected function constraintExists($table, $name) {
    // ensureIdentifiersLength() expects three parameters, thus we split our
    // constraint name in a proper name and a suffix.
    if ($name == 'pkey') {
      $suffix = $name;
      $name = '';
    }
    else {
      $pos = strrpos($name, '_');
      $suffix = substr($name, $pos + 1);
      $name = substr($name, 0, $pos);
    }
    $constraint_name = $this->ensureIdentifiersLength($table, $name, $suffix);
    return (bool) $this->connection->query("SELECT 1 FROM pg_constraint WHERE conname = '$constraint_name'")->fetchField();
  }

  public function addPrimaryKey($table, $fields) {
    if (!$this->tableExists($table)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot add primary key to table @table: table doesn't exist.", array('@table' => $table)));
    }
    if ($this->constraintExists($table, 'pkey')) {
      throw new DatabaseSchemaObjectExistsException(t("Cannot add primary key to table @table: primary key already exists.", array('@table' => $table)));
    }

    $this->connection->query('ALTER TABLE {' . $table . '} ADD CONSTRAINT ' . $this->ensureIdentifiersLength($table, '', 'pkey') . ' PRIMARY KEY (' . implode(',', $fields) . ')');
  }

  public function dropPrimaryKey($table) {
    if (!$this->constraintExists($table, 'pkey')) {
      return FALSE;
    }

    $this->connection->query('ALTER TABLE {' . $table . '} DROP CONSTRAINT ' . $this->ensureIdentifiersLength($table, '', 'pkey'));
    return TRUE;
  }

  function addUniqueKey($table, $name, $fields) {
    if (!$this->tableExists($table)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot add unique key @name to table @table: table doesn't exist.", array('@table' => $table, '@name' => $name)));
    }
    if ($this->constraintExists($table, $name . '_key')) {
      throw new DatabaseSchemaObjectExistsException(t("Cannot add unique key @name to table @table: unique key already exists.", array('@table' => $table, '@name' => $name)));
    }

    $this->connection->query('ALTER TABLE {' . $table . '} ADD CONSTRAINT "' . $this->ensureIdentifiersLength($table, $name, 'key') . '" UNIQUE (' . implode(',', $fields) . ')');
  }

  public function dropUniqueKey($table, $name) {
    if (!$this->constraintExists($table, $name . '_key')) {
      return FALSE;
    }

    $this->connection->query('ALTER TABLE {' . $table . '} DROP CONSTRAINT "' . $this->ensureIdentifiersLength($table, $name, 'key') . '"');
    return TRUE;
  }

  public function addIndex($table, $name, $fields) {
    if (!$this->tableExists($table)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot add index @name to table @table: table doesn't exist.", array('@table' => $table, '@name' => $name)));
    }
    if ($this->indexExists($table, $name)) {
      throw new DatabaseSchemaObjectExistsException(t("Cannot add index @name to table @table: index already exists.", array('@table' => $table, '@name' => $name)));
    }

    $this->connection->query($this->_createIndexSql($table, $name, $fields));
  }

  public function dropIndex($table, $name) {
    if (!$this->indexExists($table, $name)) {
      return FALSE;
    }

    $this->connection->query('DROP INDEX ' . $this->ensureIdentifiersLength($table, $name, 'idx'));
    return TRUE;
  }

  public function changeField($table, $field, $field_new, $spec, $new_keys = array()) {
    if (!$this->fieldExists($table, $field)) {
      throw new DatabaseSchemaObjectDoesNotExistException(t("Cannot change the definition of field @table.@name: field doesn't exist.", array('@table' => $table, '@name' => $field)));
    }
    if (($field != $field_new) && $this->fieldExists($table, $field_new)) {
      throw new DatabaseSchemaObjectExistsException(t("Cannot rename field @table.@name to @name_new: target field already exists.", array('@table' => $table, '@name' => $field, '@name_new' => $field_new)));
    }

    $spec = $this->processField($spec);

    // We need to typecast the new column to best be able to transfer the data
    // Schema_pgsql::getFieldTypeMap() will return possibilities that are not
    // 'cast-able' such as 'serial' - so they need to be casted int instead.
    if (in_array($spec['pgsql_type'], array('serial', 'bigserial', 'numeric'))) {
      $typecast = 'int';
    }
    else {
      $typecast = $spec['pgsql_type'];
    }

    if (in_array($spec['pgsql_type'], array('varchar', 'character', 'text')) && isset($spec['length'])) {
      $typecast .= '(' . $spec['length'] . ')';
    }
    elseif (isset($spec['precision']) && isset($spec['scale'])) {
      $typecast .= '(' . $spec['precision'] . ', ' . $spec['scale'] . ')';
    }

    // Remove old check constraints.
    $field_info = $this->queryFieldInformation($table, $field);

    foreach ($field_info as $check) {
      $this->connection->query('ALTER TABLE {' . $table . '} DROP CONSTRAINT "' . $check . '"');
    }

    // Remove old default.
    $this->fieldSetNoDefault($table, $field);

    $this->connection->query('ALTER TABLE {' . $table . '} ALTER "' . $field . '" TYPE ' . $typecast . ' USING "' . $field . '"::' . $typecast);

    if (isset($spec['not null'])) {
      if ($spec['not null']) {
        $nullaction = 'SET NOT NULL';
      }
      else {
        $nullaction = 'DROP NOT NULL';
      }
      $this->connection->query('ALTER TABLE {' . $table . '} ALTER "' . $field . '" ' . $nullaction);
    }

    if (in_array($spec['pgsql_type'], array('serial', 'bigserial'))) {
      // Type "serial" is known to PostgreSQL, but *only* during table creation,
      // not when altering. Because of that, the sequence needs to be created
      // and initialized by hand.
      $seq = "{" . $table . "}_" . $field_new . "_seq";
      $this->connection->query("CREATE SEQUENCE " . $seq);
      // Set sequence to maximal field value to not conflict with existing
      // entries.
      $this->connection->query("SELECT setval('" . $seq . "', MAX(\"" . $field . '")) FROM {' . $table . "}");
      $this->connection->query('ALTER TABLE {' . $table . '} ALTER "' . $field . '" SET DEFAULT nextval(\'' . $seq . '\')');
    }

    // Rename the column if necessary.
    if ($field != $field_new) {
      $this->connection->query('ALTER TABLE {' . $table . '} RENAME "' . $field . '" TO "' . $field_new . '"');
    }

    // Add unsigned check if necessary.
    if (!empty($spec['unsigned'])) {
      $this->connection->query('ALTER TABLE {' . $table . '} ADD CHECK ("' . $field_new . '" >= 0)');
    }

    // Add default if necessary.
    if (isset($spec['default'])) {
      $this->fieldSetDefault($table, $field_new, $spec['default']);
    }

    // Change description if necessary.
    if (!empty($spec['description'])) {
      $this->connection->query('COMMENT ON COLUMN {' . $table . '}."' . $field_new . '" IS ' . $this->prepareComment($spec['description']));
    }

    if (isset($new_keys)) {
      $this->_createKeys($table, $new_keys);
    }
  }

  protected function _createIndexSql($table, $name, $fields) {
    $query = 'CREATE INDEX "' . $this->ensureIdentifiersLength($table, $name, 'idx') . '" ON {' . $table . '} (';
    $query .= $this->_createKeySql($fields) . ')';
    return $query;
  }

  protected function _createKeys($table, $new_keys) {
    if (isset($new_keys['primary key'])) {
      $this->addPrimaryKey($table, $new_keys['primary key']);
    }
    if (isset($new_keys['unique keys'])) {
      foreach ($new_keys['unique keys'] as $name => $fields) {
        $this->addUniqueKey($table, $name, $fields);
      }
    }
    if (isset($new_keys['indexes'])) {
      foreach ($new_keys['indexes'] as $name => $fields) {
        $this->addIndex($table, $name, $fields);
      }
    }
  }

  /**
   * Retrieve a table or column comment.
   */
  public function getComment($table, $column = NULL) {
    $info = $this->getPrefixInfo($table);
    // Don't use {} around pg_class, pg_attribute tables.
    if (isset($column)) {
      return $this->connection->query('SELECT col_description(oid, attnum) FROM pg_class, pg_attribute WHERE attrelid = oid AND relname = ? AND attname = ?', array($info['table'], $column))->fetchField();
    }
    else {
      return $this->connection->query('SELECT obj_description(oid, ?) FROM pg_class WHERE relname = ?', array('pg_class', $info['table']))->fetchField();
    }
  }

  /**
   * Calculates a base-64 encoded, PostgreSQL-safe sha-256 hash per PostgreSQL
   * documentation: 4.1. Lexical Structure.
   *
   * @param $data
   *   String to be hashed.
   *
   * @return string
   *   A base-64 encoded sha-256 hash, with + and / replaced with _ and any =
   *   padding characters removed.
   */
  protected function hashBase64($data) {
    // Ensure lowercase as D7's pgsql driver does not quote identifiers
    // consistently, and they are therefore folded to lowercase by PostgreSQL.
    $hash = strtolower(base64_encode(hash('sha256', $data, TRUE)));
    // Modify the hash so it's safe to use in PostgreSQL identifiers.
    return strtr($hash, array('+' => '_', '/' => '_', '=' => ''));
  }

  /**
   * Build a condition to match a table name against a standard information_schema.
   *
   * In PostgreSQL "unquoted names are always folded to lower case." The pgsql
   * driver does not quote table names, so they are therefore always lowercase.
   *
   * @see https://www.postgresql.org/docs/14/sql-syntax-lexical.html
   */
  protected function buildTableNameCondition($table_name, $operator = '=', $add_prefix = TRUE) {
    return parent::buildTableNameCondition(strtolower($table_name), $operator, $add_prefix);
  }

}
