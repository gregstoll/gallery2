<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */
ini_set('error_reporting', 2047);

if (!empty($_SERVER['SERVER_NAME'])) {
	echo "You must run this from the command line\n";

	exit(1);
}

require_once __DIR__ . '/XmlParser.inc';

$output = '';

foreach (array('mysql', 'postgres', 'oracle', 'db2', 'mssql', 'sqlite') as $db) {
	$output  .= '## ' . $db . "\n";
	$xmlFiles = glob('tmp/dbxml/*.xml');

	if (empty($xmlFiles)) {
		continue;
	}

	sort($xmlFiles);

	foreach ($xmlFiles as $xmlFile) {
		$p    = new XmlParser();
		$root = $p->parse($xmlFile);

		$generatorClass = "${db}Generator";
		$generator      = new $generatorClass();

		$base            = basename($xmlFile);
		$base            = preg_replace('/\.[^\.]*$/', '', $base);
		$output         .= '# ' . $base . "\n";
		$root[0]['base'] = $base;
		$output         .= $generator->createSql($root[0], 0, 0, null);
	}
}

$fd = fopen('schema.tpl', 'w');
fwrite($fd, $output);
fclose($fd);

class BaseGenerator {
	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'SCHEMA':
				$output .= "INSERT INTO DB_TABLE_PREFIXSchema (\n";
				$output .= " DB_COLUMN_PREFIXname,\n";
				$output .= " DB_COLUMN_PREFIXmajor,\n";
				$output .= " DB_COLUMN_PREFIXminor\n";
				$output .= ') VALUES(';
				$output .= "'" . $parent['child'][0]['content'] . "', " . $child[0]['content'] . ', ' .
				$child[1]['content'];
				$output .= ");\n\n";

				break;

			case 'COLUMN':
				// column-name, column-type, column-size, not-null?
				$output .= ' DB_COLUMN_PREFIX' . $child[0]['content'];
				$output .= ' ' . $this->columnDefinition($child);

				break;

			default:
				$output .= "1. UNIMPLEMLENTED: $node[name]";

				break;
		}

		return $output;
	}

	public function getIndexCrc($columns) {
		$buf = '';

		for ($i = 0; $i < count($columns); $i++) {
			$buf .= $columns[$i]['content'];
		}

		/*
		 * crc32 returns different results on 32-bit vs. 64-bit systems.  e.g. crc32('groupId')
		 * returns -310277968 for 32-bit systems and 3984689328 on 64-bit systems. We don't
		 * completely understand the issue, but adding 2^32 for negative crc32 values
		 * (32-bit overflows?!) seems to do the trick. And we eschew the 64-bit unsafe modulo
		 * operation by using substr instead of % 100000.
		 * Note: We also want strictly positive values since we use the value in SQL index key
		 * names.
		 */
		$crc = crc32($buf);

		if ($crc > 0) {
			return $crc % 100000;
		}

		return (int)substr(crc32($buf) + 2 ** 32, -5);
	}

	public function getNotNullElement($child) {
		for ($i = 0; $i < count($child); $i++) {
			if ($child[$i]['name'] == 'NOT-NULL') {
				return $child[$i];
			}
		}

		return null;
	}

	public function getDefaultElement($child) {
		for ($i = 0; $i < count($child); $i++) {
			if ($child[$i]['name'] == 'DEFAULT') {
				return $child[$i]['content'];
			}
		}

		return null;
	}

	public function setColumnDefinitionMap($map) {
		$this->_columnDefinitionMap = $map;
	}

	public function columnDefinition($child, $includeNotNull = true, $includeDefault = true) {
		$output = '';
		$key    = $child[1]['content'] . '-' .
		(!empty($child[2]['content']) ? $child[2]['content'] : '');

		if (isset($this->_columnDefinitionMap[$key])) {
			$output .= $this->_columnDefinitionMap[$key];
		} else {
			$output .= "2. UNIMPLEMLENTED: $key";
		}

		if ($includeDefault) {
			$defaultValue = $this->getDefaultElement($child);

			if (isset($defaultValue)) {
				$output .= " DEFAULT '$defaultValue'";
			}
		}

		if ($includeNotNull) {
			if ($this->getNotNullElement($child)) {
				$output .= ' NOT NULL';
			}
		}

		return $output;
	}

	public function generateSchemaUpdate($child) {
		$output  = "UPDATE DB_TABLE_PREFIXSchema\n";
		$output .= sprintf(
			"  SET DB_COLUMN_PREFIXmajor=%d, DB_COLUMN_PREFIXminor=%d\n",
			$child[2]['child'][0]['content'],
			$child[2]['child'][1]['content']
		);
		$output .= sprintf(
			"  WHERE DB_COLUMN_PREFIXname='%s' AND DB_COLUMN_PREFIXmajor=%d " .
			   "AND DB_COLUMN_PREFIXminor=%d;\n\n",
			$child[0]['content'],
			$child[1]['child'][0]['content'],
			(!empty($child[1]['child'][1]['content']) ? $child[1]['child'][1]['content'] : 0)
		);

		return $output;
	}

	public function isPrimaryKey($child) {
		return $this->isIndex($child) && !empty($child['attrs']['PRIMARY']);
	}

	public function isIndex($child) {
		return $child['name'] == 'INDEX';
	}
}

class MySqlGenerator extends BaseGenerator {
	public function __construct() {
		$this->setColumnDefinitionMap(
			array(
				'INTEGER-'       => 'int(11)',
				'INTEGER-MEDIUM' => 'int(11)',
				'INTEGER-LARGE'  => 'int(11)',
				'BIT-LARGE'      => 'int(11)',
				'BIT-MEDIUM'     => 'int(11)',
				'STRING-SMALL'   => 'varchar(32)',
				'STRING-MEDIUM'  => 'varchar(128)',
				'STRING-LARGE'   => 'varchar(255)',
				'TEXT-SMALL'     => 'text',
				'TEXT-'          => 'text',
				'TEXT-MEDIUM'    => 'text',
				'TEXT-LARGE'     => 'longtext',
				'BOOLEAN-'       => 'int(1)',
				'BOOLEAN-MEDIUM' => 'int(1)',
				'TIMESTAMP-'     => 'datetime',
			)
		);
	}

	public function columnDefinition($child, $includeNotNull = true, $includeDefault = true) {
		$output = parent::columnDefinition($child, $includeNotNull, false);

		// MySQL -> DEFAULT expression after NOT NULL
		if ($includeDefault) {
			$defaultValue = $this->getDefaultElement($child);

			if (isset($defaultValue)) {
				$output .= " DEFAULT '$defaultValue'";
			}
		}

		return $output;
	}

	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'TABLE':
				// table-name, schema, column+, (key | index)
				$output .= 'CREATE TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "(\n";

				for ($i = 2; $i < count($child); $i++) {
					$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);

					if ($i < count($child) - 1) {
						$output .= ',';
					}
					$output .= "\n";
				}
				$output .= ") DB_TABLE_TYPE\n";
				// Character set, enclosed in comments that are ignored by MySQL < 4.1.0
				$output .= "/*!40100 DEFAULT CHARACTER SET utf8 */;\n\n";

				// Schema info
				$output .= $this->createSql($child[1], 0, 0, $node);

				break;

			case 'ALTER':
				// column+
				for ($i = 0; $i < count($child); $i++) {
					$output .= '  MODIFY COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'];
					$output .= ' ' . $this->columnDefinition($child[$i]['child']);

					if ($i < count($child) - 1) {
						$output .= ",\n";
					}
				}

				break;

			case 'CHANGE':
				// table-name, schema-from, schema-to, (add, alter, remove)+
				if (count($child) > 3) {
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "\n";

					for ($i = 3; $i < count($child); $i++) {
						if ($i > 3) {
							$output .= ",\n";
						}
						$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);
					}
					$output .= ";\n\n";
				}
				$output .= $this->generateSchemaUpdate($child);

				break;

			case 'ADD':
				// (column, key, index)+
				for ($i = 0; $i < count($child); $i++) {
					$c = $child[$i];

					switch ($c['name']) {
						case 'COLUMN':
							// column-name
							$output .= '  ADD COLUMN DB_COLUMN_PREFIX' . $c['child'][0]['content'];
							$output .= ' ' . $this->columnDefinition($c['child']);

							break;

						case 'KEY':
							$output .= '  ADD' . $this->createSql($c, 0, 0, null);

							break;

						case 'INDEX':
							// column-name
							$output .= '  ADD INDEX ';
							$nameKey = strtoupper('name_' . $this->getDbType());
							$columns = $c['child'];

							if (isset($c['attrs'][$nameKey])) {
								$output .= $c['attrs'][$nameKey];
							} else {
								$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
								'_' . $this->getIndexCrc($columns);
							}
							$output .= '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';

							break;

						default:
							$output .= "3. UNIMPLEMLENTED: ADD $c[name]\n";
					}

					if ($i < count($child) - 1) {
						$output .= ",\n";
					}
				}

				break;

			case 'REMOVE':
				if (!isset($parent['name'])) {
					$output .= 'DROP TABLE DB_TABLE_PREFIX' . $node['child'][0]['content'] . ";\n\n";

					if ($node['child'][0]['content'] != 'Schema') {
						$output .= "DELETE FROM DB_TABLE_PREFIXSchema WHERE DB_COLUMN_PREFIXname='" .
						$node['child'][0]['content'] . "';\n\n";
					}
				} elseif ($parent['name'] == 'CHANGE') {
					// (column-name, key, index)+
					$i = 0;

					foreach ($child as $c) {
						if ($i++ > 0) {
							$output .= ",\n";
						}

						switch ($c['name']) {
							case 'COLUMN-NAME':
								$output .= '  DROP COLUMN DB_COLUMN_PREFIX' . $c['content'];

								break;

							case 'KEY':
								if (!empty($child[0]['attrs']['PRIMARY'])) {
									$output .= '  DROP PRIMARY KEY';
								} else {
									/*
									 * For MySQL, our UNIQUE index names are the name of the first
									 * column that is part of the index (MySQL sets the name that way
									 * for unnamed indices (they only need to be unique in each table)
									 */
									$output .= '  DROP INDEX DB_COLUMN_PREFIX' . $c['child'][0]['content'];
								}

								break;

							case 'INDEX':
								// column-name
								$output .= '  DROP INDEX ';
								$nameKey = strtoupper('name_' . $this->getDbType());

								if (isset($child[0]['attrs'][$nameKey])) {
									$output .= $child[0]['attrs'][$nameKey];
								} else {
									$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
									'_' . $this->getIndexCrc($c['child']);
								}

								break;

							default:
								$output .= "4. UNIMPLEMENTED: REMOVE $c[name]\n";
						}
					}
				}

				break;

			case 'KEY':
				// column-name+
				if (!empty($node['attrs']['PRIMARY'])) {
					$output .= ' PRIMARY KEY(';
				} else {
					/*
					 * In MySQL, it would be UNIQUE [INDEX] so INDEX is optional, since UNIQUE is
					 * often called a KEY and we use <key> in our XML for UNIQUE, we just use UNIQUE
					 * without INDEX here. Don't add an index name, see our REMOVE code.
					 */
					$output .= ' UNIQUE (';
				}

				for ($i = 0; $i < count($child); $i++) {
					$output .= 'DB_COLUMN_PREFIX' . $child[$i]['content'];

					if ($i < count($child) - 1) {
						$output .= ', ';
					}
				}
				$output .= ')';

				break;

			case 'INDEX':
				// column-name+
				$crc     = $this->getIndexCrc($child);
				$output .= ' INDEX DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '_' . $crc . '(';

				for ($i = 0; $i < count($child); $i++) {
					$output .= 'DB_COLUMN_PREFIX' . $child[$i]['content'];

					if ($i < count($child) - 1) {
						$output .= ', ';
					}
				}
				$output .= ')';

				break;

			default:
				$output .= parent::createSql($node, $index, $lastPeerIndex, $parent);
		}

		return $output;
	}

	public function getDbType() {
		return 'mysql';
	}
}

class PostgresGenerator extends BaseGenerator {
	public function __construct() {
		$this->setColumnDefinitionMap(
			array(
				'INTEGER-'       => 'INTEGER',
				'INTEGER-MEDIUM' => 'INTEGER',
				'INTEGER-LARGE'  => 'INTEGER',
				'BIT-LARGE'      => 'BIT(32)',
				'BIT-MEDIUM'     => 'BIT(32)',
				'STRING-SMALL'   => 'VARCHAR(32)',
				'STRING-MEDIUM'  => 'VARCHAR(128)',
				'STRING-LARGE'   => 'VARCHAR(255)',
				'TEXT-SMALL'     => 'text',
				'TEXT-'          => 'text',
				'TEXT-MEDIUM'    => 'text',
				'TEXT-LARGE'     => 'text',
				'BOOLEAN-'       => 'SMALLINT',
				'BOOLEAN-MEDIUM' => 'SMALLINT',
				'TIMESTAMP-'     => 'datetime',
			)
		);
	}

	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'CHANGE':
				// table-name, schema-from, schema-to, (add, alter, remove)+
				for ($i = 3; $i < count($child); $i++) {
					$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);
				}
				$output .= $this->generateSchemaUpdate($child);

				break;

			case 'REMOVE':
				if (!isset($parent['name'])) {
					$output .= 'DROP TABLE DB_TABLE_PREFIX' . $node['child'][0]['content'] . ";\n\n";

					if ($node['child'][0]['content'] != 'Schema') {
						$output .= "DELETE FROM DB_TABLE_PREFIXSchema WHERE DB_COLUMN_PREFIXname='" .
						$node['child'][0]['content'] . "';\n\n";
					}
				} elseif ($parent['name'] == 'CHANGE') {
					// (column-name, key, index)+
					for ($i = 0; $i < count($child); $i++) {
						$c = $child[$i];

						switch ($c['name']) {
							case 'COLUMN-NAME':
								// column-name
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= ' DROP COLUMN DB_COLUMN_PREFIX' . $c['content'];
								$output .= ";\n\n";

								break;

							case 'KEY':
								if (empty($c['attrs']['PRIMARY'])) {
									$crc     = $this->getIndexCrc($c['child']);
									$output .= 'DROP INDEX DB_TABLE_PREFIX' .
									$parent['child'][0]['content'] . '_' . $crc . ";\n\n";
								} else {
									$output .= 'ALTER TABLE DB_TABLE_PREFIX' .
									$parent['child'][0]['content'] . ' DROP CONSTRAINT DB_TABLE_PREFIX'
									. $parent['child'][0]['content'] . "_pkey;\n\n";
								}

								break;

							case 'INDEX':
								// column-name
								$output .= 'DROP INDEX ';
								$nameKey = strtoupper('name_' . $this->getDbType());

								if (isset($c['attrs'][$nameKey])) {
									$output .= $c['attrs'][$nameKey];
								} else {
									$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
									'_' . $this->getIndexCrc($c['child']);
								}
								$output .= ";\n\n";

								break;

							default:
								$output .= "5. UNIMPLEMENTED: REMOVE $c[name]\n";
						}
					}
				}

				break;

			case 'ADD':
				// (column, key, index)+
				foreach ($child as $c) {
					switch ($c['name']) {
						case 'COLUMN':
							/* Add a new column, optionally with a default value and a not null constraint
							 * In PG7, we can not set the default value in the add column statement
							 * (PG8 doesn't have this limitation though). Therefore do it in 3 steps:
							 * 1. Add the column without any options.
							 * 2. Set the default value (only affects future rows) and add the default
							 *    value for existing rows.
							 * 3. Add the not-null constraint
							 */
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= ' ADD COLUMN DB_COLUMN_PREFIX' . $c['child'][0]['content'];
							$output .= ' ' . $this->columnDefinition($c['child'], false, false);
							$output .= ";\n\n";

							$defaultValue = $this->getDefaultElement($c['child']);

							if (isset($defaultValue)) {
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= ' ALTER COLUMN DB_COLUMN_PREFIX' . $c['child'][0]['content'];
								$output .= " SET DEFAULT '$defaultValue';\n\n";

								$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= ' SET DB_COLUMN_PREFIX' . $c['child'][0]['content'];
								$output .= " = '$defaultValue';\n\n";
							}

							if ($this->getNotNullElement($c['child'])) {
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
								' ALTER DB_COLUMN_PREFIX' . $c['child'][0]['content'] .
								" SET NOT NULL;\n\n";
							}

							break;

						case 'KEY':
							// column-name+
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
							' ADD ';

							if (!empty($c['attrs']['PRIMARY'])) {
								$output .= 'PRIMARY KEY(';
							} else {
								$output .= 'UNIQUE KEY(';
							}

							for ($i = 0; $i < count($c['child']); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $c['child'][$i]['content'];

								if ($i < count($c['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						case 'INDEX':
							// column-name
							$output .= 'CREATE INDEX ';
							$nameKey = strtoupper('name_' . $this->getDbType());
							$columns = $c['child'];

							if (isset($c['attrs'][$nameKey])) {
								$output .= $c['attrs'][$nameKey];
							} else {
								$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
								'_' . $this->getIndexCrc($columns);
							}
							$output .= ' ON ' . 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						default:
							$output .= "6. UNIMPLEMLENTED: ADD $c[name]\n";
					}
				}

				break;

			case 'TABLE':
				// table-name, schema, column+, (key | index)
				$output .= 'CREATE TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "(\n";

				for ($i = 2; $i < count($child); $i++) {
					if ($child[$i]['name'] != 'COLUMN') {
						$output .= "\n";

						break;
					}

					if ($i > 2) {
						$output .= ",\n";
					}
					$output        .= $this->createSql($child[$i], $i, count($child) - 1, $node);
					$firstNonColumn = $i + 1;
				}
				$output .= ");\n\n";

				for ($i = $firstNonColumn; $i < count($child); $i++) {
					if ($child[$i]['name'] == 'INDEX') {
						$crc     = $this->getIndexCrc($child[$i]['child']);
						$output .= 'CREATE INDEX DB_TABLE_PREFIX' . $child[0]['content'] . '_' . $crc .
						' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

						for ($j = 0; $j < count($child[$i]['child']); $j++) {
							$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

							if ($j < count($child[$i]['child']) - 1) {
								$output .= ', ';
							}
						}
						$output .= ");\n\n";
					} else /* key */ {
						if (!empty($child[$i]['attrs']['PRIMARY'])) {
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $child[0]['content'] .
							' ADD PRIMARY KEY (';
							$columns = $child[$i]['child'];

							for ($j = 0; $j < count($columns); $j++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$j]['content'];

								if ($j < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ");\n\n";
						} else {
							$crc     = $this->getIndexCrc($child[$i]['child']);
							$output .= 'CREATE UNIQUE INDEX DB_TABLE_PREFIX' . $child[0]['content'] .
							'_' . $crc . ' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

							for ($j = 0; $j < count($child[$i]['child']); $j++) {
								$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

								if ($j < count($child[$i]['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ");\n\n";
						}
					}
				}

				// Schema info
				$output .= $this->createSql($child[1], 0, 0, $node);

				break;

			case 'ALTER':
				// column+
				for ($i = 0; $i < count($child); $i++) {
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp';
					$output .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ";\n\n";
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' = CAST(DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ' AS ' .
					$this->columnDefinition($child[$i]['child'], false) . ");\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' RENAME DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' to DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";

					if ($this->getNotNullElement($child[$i]['child'])) {
						$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
						' ALTER DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
						" SET NOT NULL;\n\n";
					}
				}

				break;

			default:
				$output .= parent::createSql($node, $index, $lastPeerIndex, $parent);
		}

		return $output;
	}

	public function getDbType() {
		return 'postgres';
	}
}

class OracleGenerator extends BaseGenerator {
	public function __construct() {
		$this->setColumnDefinitionMap(
			array(
				'INTEGER-'       => 'INTEGER',
				'INTEGER-MEDIUM' => 'INTEGER',
				'INTEGER-LARGE'  => 'INTEGER',
				'BIT-LARGE'      => 'INTEGER',
				'BIT-MEDIUM'     => 'INTEGER',
				'STRING-SMALL'   => 'VARCHAR2(32)',
				'STRING-MEDIUM'  => 'VARCHAR2(128)',
				'STRING-LARGE'   => 'VARCHAR2(255)',
				'TEXT-SMALL'     => 'VARCHAR2(4000)',
				'TEXT-'          => 'CLOB',
				'TEXT-MEDIUM'    => 'CLOB',
				'TEXT-LARGE'     => 'CLOB',
				'BOOLEAN-'       => 'NUMBER(1)',
				'BOOLEAN-MEDIUM' => 'NUMBER(1)',
				'TIMESTAMP-'     => 'datetime',
			)
		);
	}

	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'CHANGE':
				// table-name, schema-from, schema-to, (add, alter, remove)+
				for ($i = 3; $i < count($child); $i++) {
					$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);
				}
				$output .= $this->generateSchemaUpdate($child);

				break;

			case 'REMOVE':
				if (!isset($parent['name'])) {
					$output .= 'DROP TABLE DB_TABLE_PREFIX' . $node['child'][0]['content'] . ";\n\n";

					if ($node['child'][0]['content'] != 'Schema') {
						$output .= "DELETE FROM DB_TABLE_PREFIXSchema WHERE DB_COLUMN_PREFIXname='" .
						$node['child'][0]['content'] . "';\n\n";
					}
				} elseif ($parent['name'] == 'CHANGE') {
					// (column-name, key, index)+
					foreach ($child as $c) {
						switch ($c['name']) {
							case 'COLUMN-NAME':
								// column-name
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= "\n" . '  DROP (DB_COLUMN_PREFIX' . $c['content'] . ')';

								break;

							case 'KEY':
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];

								if (isset($child[0]['attrs']['PRIMARY'])) {
									$output .= "\n  DROP PRIMARY KEY";
								} else {
									$keyColumns = array();

									foreach ($c['child'] as $keyColumn) {
										$keyColumns[] = 'DB_COLUMN_PREFIX' . $keyColumn['content'];
									}
									$output .= "\n" . '  DROP UNIQUE (' . implode(', ', $keyColumns) . ')';
								}

								break;

							case 'INDEX':
								// column-name
								$output .= '  DROP INDEX ';
								$nameKey = strtoupper('name_' . $this->getDbType());

								if (isset($child[0]['attrs'][$nameKey])) {
									$output .= $child[0]['attrs'][$nameKey];
								} else {
									$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
									'_' . $this->getIndexCrc($c['child']);
								}

								break;

							default:
								$output .= "7. UNIMPLEMENTED: REMOVE $c[name]\n";
						}
						$output .= ";\n\n";
					}
				}

				break;

			case 'ADD':
				// (column, key, index)+
				for ($k = 0; $k < count($child); $k++) {
					$c = $child[$k];

					switch ($c['name']) {
						case 'COLUMN':
							// column-name
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= "\n" . '  ADD (DB_COLUMN_PREFIX' . $c['child'][0]['content'];
							$output .= ' ' . $this->columnDefinition($c['child']) . ')';

							break;

						case 'KEY':
							// column-name+
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= "\n  ADD ";

							if (!empty($c['attrs']['PRIMARY'])) {
								$output .= 'PRIMARY KEY(';
							} else {
								$output .= 'UNIQUE KEY(';
							}

							for ($i = 0; $i < count($c['child']); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $c['child'][$i]['content'];

								if ($i < count($c['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';

							break;

						case 'INDEX':
							// column-name
							$output .= 'CREATE INDEX ';
							$nameKey = strtoupper('name_' . $this->getDbType());
							$columns = $c['child'];

							if (isset($c['attrs'][$nameKey])) {
								$output .= $c['attrs'][$nameKey];
							} else {
								$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
								'_' . $this->getIndexCrc($columns);
							}
							$output .= ' ON DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';

							break;

						default:
							$output .= "8. UNIMPLEMLENTED: ADD $c[name]\n";
					}
					$output .= ";\n\n";
				}

				break;

			case 'TABLE':
				// table-name, schema, column+, (key | index)
				$output .= 'CREATE TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "(\n";

				for ($i = 2; $i < count($child); $i++) {
					if ($child[$i]['name'] != 'COLUMN') {
						$output .= "\n";

						break;
					}

					if ($i > 2) {
						$output .= ",\n";
					}
					$output        .= $this->createSql($child[$i], $i, count($child) - 1, $node);
					$firstNonColumn = $i + 1;
				}
				$output .= ");\n\n";

				$keyColumns = array();

				for ($i = $firstNonColumn; $i < count($child); $i++) {
					if ($child[$i]['name'] == 'INDEX') {
						$crc     = $this->getIndexCrc($child[$i]['child']);
						$output .= 'CREATE INDEX DB_TABLE_PREFIX' . $child[0]['content'] . '_' . $crc .
						"\n  " . ' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

						for ($j = 0; $j < count($child[$i]['child']); $j++) {
							$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

							if ($j < count($child[$i]['child']) - 1) {
								$output .= ', ';
							}
						}
						$output .= ");\n\n";
					} else {
						$keys[] = $child[$i];
					}
				}

				if (!empty($keys)) {
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "\n";

					foreach ($keys as $key) {
						if (!empty($key['attrs']['PRIMARY'])) {
							$output .= ' ADD PRIMARY KEY (';
						} else {
							$output .= ' ADD UNIQUE (';
						}

						for ($i = 0; $i < count($key['child']); $i++) {
							$output .= 'DB_COLUMN_PREFIX' . $key['child'][$i]['content'];

							if ($i < count($key['child']) - 1) {
								$output .= ', ';
							}
						}
						$output .= ")\n";
					}
					$output .= ";\n\n";
				}

				// Schema info
				$output .= $this->createSql($child[1], 0, 0, $node);

				break;

			case 'COLUMN':
				// column-name, column-type, column-size, not-null?
				$output .= ' DB_COLUMN_PREFIX' . $child[0]['content'];
				$output .= ' ' . $this->columnDefinition($child, false);

				if (($notNull = $this->getNotNullElement($child))
					&& (empty($notNull['attrs']['EMPTY']) || $notNull['attrs']['EMPTY'] != 'allowed')
				) {
					$output .= ' NOT NULL';
				}

				break;

			case 'ALTER':
				// column+
				for ($i = 0; $i < count($child); $i++) {
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD (DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp';
					$output .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ");\n\n";
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' = DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP (DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ");\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' RENAME COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' TO DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";

					if (($notNull = $this->getNotNullElement($child[$i]['child']))
						&& (empty($notNull['attrs']['EMPTY'])
						|| $notNull['attrs']['EMPTY'] != 'allowed')
					) {
						$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
						' MODIFY (DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
						" NOT NULL);\n\n";
					}
				}

				break;

			default:
				$output .= parent::createSql($node, $index, $lastPeerIndex, $parent);
		}

		return $output;
	}

	public function getDbType() {
		return 'oracle';
	}
}

/**
 *  Notes regarding DB2 limitations on Table and Index names:
 *
 *  DB2 currently limits the length of table names to 30 characters, and index names to 18
 *  characters.  We don't have to worry about the 30 character table name problem because we force
 *  table names to be shorter than this in GalleryStorage (and it's very important that the table
 *  names we choose here match up with the ones that GalleryStorage expects).  However we have
 *  (and need) no such provision for indexes because this is the only place where we define index
 *  names.
 *
 *  The installer "database setup" step prefixes all tables and indexes with "gtst#" (5 chars).
 *  The installer default is "g2_" (3 chars).  So if we allow room for a 5 char prefix, that
 *  leaves us 13 characters for an 18-character index name.  Our index CRC values are another 5
 *  characters.  That leaves us 8 characters to use for a descriptive index name.  I don't know if
 *  DB2 index names are required to be unique in the database or just to the table so to avoid any
 *  risks we can't just use a prefix or suffix of the table name because it may overlap with
 *  another similar table name.
 *
 *  So for indexes we'll use the following format:
 *    DB_TABLE_PREFIX + substr(table name, 0, 5) + substr(md5(table name), -2) + '_' + index crc
 *
 *  That works out to:
 *    <= 5 chars      + 5                        + 2                           +  1  + 5 = <= 18
 */
class Db2Generator extends BaseGenerator {
	public function __construct() {
		// The column size is limited to 32kbyte
		$this->setColumnDefinitionMap(
			array(
				'INTEGER-'       => 'INTEGER',
				'INTEGER-MEDIUM' => 'INTEGER',
				'INTEGER-LARGE'  => 'INTEGER',
				'BIT-LARGE'      => 'VARCHAR(32) FOR BIT DATA',
				'BIT-MEDIUM'     => 'VARCHAR(32) FOR BIT DATA',
				'STRING-SMALL'   => 'VARCHAR(32)',
				'STRING-MEDIUM'  => 'VARCHAR(128)',
				'STRING-LARGE'   => 'VARCHAR(255)',
				'TEXT-SMALL'     => 'VARCHAR(10000)',
				'TEXT-'          => 'VARCHAR(15000)',
				'TEXT-MEDIUM'    => 'VARCHAR(15000)',
				'TEXT-LARGE'     => 'CLOB(2G) NOT LOGGED',
				'BOOLEAN-'       => 'SMALLINT',
				'BOOLEAN-MEDIUM' => 'SMALLINT',
				'TIMESTAMP-'     => 'datestamp',
			)
		);
	}

	public function columnDefinition($child, $includeNotNull = true, $includeDefault = true) {
		$output = parent::columnDefinition($child, $includeNotNull, false);

		// DB2 -> Make sure DEFAULT expression doesn't have quotes for numeric
		if ($includeDefault) {
			$defaultValue = $this->getDefaultElement($child);

			if (isset($defaultValue)) {
				if ($child[1]['content'] != 'INTEGER' && $child[1]['content'] != 'BOOLEAN') {
					$defaultValue = "'$defaultValue'";
				}
				$output .= " DEFAULT $defaultValue";
			}
		}

		return $output;
	}

	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'CHANGE':
				// table-name, schema-from, schema-to, (add, alter, remove)+
				for ($i = 3; $i < count($child); $i++) {
					$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);
				}
				$output .= $this->generateSchemaUpdate($child);

				break;

			case 'REMOVE':
				if (!isset($parent['name'])) {
					$output .= 'DROP TABLE DB_TABLE_PREFIX' . $node['child'][0]['content'] . ";\n\n";

					if ($node['child'][0]['content'] != 'Schema') {
						$output .= "DELETE FROM DB_TABLE_PREFIXSchema WHERE DB_COLUMN_PREFIXname='" .
						$node['child'][0]['content'] . "';\n\n";
					}
				} elseif ($parent['name'] == 'CHANGE') {
					// (column-name, key, index)+
					for ($i = 0; $i < count($child); $i++) {
						$c = $child[$i];

						switch ($c['name']) {
							case 'COLUMN-NAME':
								// column-name
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= ' DROP COLUMN DB_COLUMN_PREFIX' . $c['content'] . ";\n\n";
								$output .= "CALL ADMIN_CMD ('REORG TABLE DB_TABLE_PREFIX";
								$output .= $parent['child'][0]['content'] . "');\n\n";

								break;

							case 'KEY':
								if (empty($c['attrs']['PRIMARY'])) {
									$crc     = $this->getIndexCrc($c['child']);
									$output .= 'DROP INDEX DB_TABLE_PREFIX' .
									$parent['child'][0]['content'] . '_' . $crc . ";\n\n";
								} else {
									$output .= 'ALTER TABLE DB_TABLE_PREFIX' .
									$parent['child'][0]['content'] . " DROP PRIMARY KEY;\n\n";
								}

								break;

							case 'INDEX':
								// column-name
								$output .= 'DROP INDEX ';
								$nameKey = strtoupper('name_' . $this->getDbType());

								if (isset($c['attrs'][$nameKey])) {
									$output .= $c['attrs'][$nameKey];
								} else {
									$output .= 'DB_TABLE_PREFIX' .
									substr($parent['child'][0]['content'], 0, 5) .
									substr(md5($parent['child'][0]['content']), -2) .
									'_' . $this->getIndexCrc($c['child']);
								}
								$output .= ";\n\n";

								break;

							default:
								$output .= "5. UNIMPLEMENTED: REMOVE $c[name]\n";
						}
					}
				}

				break;

			case 'ADD':
				// (column, key, index)+
				foreach ($child as $c) {
					switch ($c['name']) {
						case 'COLUMN':
							// column-name
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= ' ADD COLUMN DB_COLUMN_PREFIX' . $c['child'][0]['content'];
							$output .= ' ' . $this->columnDefinition($c['child']);
							$output .= ";\n\n";

							break;

						case 'KEY':
							// column-name+
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
							' ADD ';

							if (!empty($c['attrs']['PRIMARY'])) {
								$output .= 'PRIMARY KEY(';
							} else {
								$output .= 'UNIQUE KEY(';
							}

							for ($i = 0; $i < count($c['child']); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $c['child'][$i]['content'];

								if ($i < count($c['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						case 'INDEX':
							// column-name
							$output .= 'CREATE INDEX ';
							$nameKey = strtoupper('name_' . $this->getDbType());
							$columns = $c['child'];

							if (isset($c['attrs'][$nameKey])) {
								$output .= $c['attrs'][$nameKey];
							} else {
								$output .= 'DB_TABLE_PREFIX' .
								substr($parent['child'][0]['content'], 0, 5) .
								substr(md5($parent['child'][0]['content']), -2) .
								'_' . $this->getIndexCrc($c['child']);
							}
							$output .= ' ON ' . 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						default:
							$output .= "6. UNIMPLEMLENTED: ADD $c[name]\n";
					}
				}

				break;

			case 'TABLE':
				// table-name, schema, column+, (key | index)
				$output .= 'CREATE TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "(\n";

				for ($i = 2; $i < count($child); $i++) {
					if ($child[$i]['name'] != 'COLUMN') {
						$output .= "\n";

						break;
					}

					if ($i > 2) {
						$output .= ",\n";
					}
					$output        .= $this->createSql($child[$i], $i, count($child) - 1, $node);
					$firstNonColumn = $i + 1;
				}
				$output .= ");\n\n";

				for ($i = $firstNonColumn; $i < count($child); $i++) {
					if ($child[$i]['name'] == 'INDEX') {
						$crc     = $this->getIndexCrc($child[$i]['child']);
						$output .= 'CREATE INDEX DB_TABLE_PREFIX' .
						substr($child[0]['content'], 0, 5) .
						substr(md5($child[0]['content']), -2) . '_' . $crc .
						"\n  " . ' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

						for ($j = 0; $j < count($child[$i]['child']); $j++) {
							$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

							if ($j < count($child[$i]['child']) - 1) {
								$output .= ', ';
							}
						}
						$output .= ");\n\n";
					} else /* key */ {
						if (!empty($child[$i]['attrs']['PRIMARY'])) {
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $child[0]['content'] .
							' ADD PRIMARY KEY (';
							$columns = $child[$i]['child'];

							for ($j = 0; $j < count($columns); $j++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$j]['content'];

								if ($j < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ");\n\n";
						} else {
							$crc     = $this->getIndexCrc($child[$i]['child']);
							$output .= 'CREATE UNIQUE INDEX DB_TABLE_PREFIX' .
							substr($child[0]['content'], 0, 5) .
							substr(md5($child[0]['content']), -2) . '_' . $crc .
							"  \n" . ' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

							for ($j = 0; $j < count($child[$i]['child']); $j++) {
								$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

								if ($j < count($child[$i]['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ");\n\n";
						}
					}
				}

				// Schema info
				$output .= $this->createSql($child[1], 0, 0, $node);

				break;

			case 'ALTER':
				// column+
				for ($i = 0; $i < count($child); $i++) {
					// DB2's "ALTER TABLE ALTER COLUMN" is somewhat limited. Use a workaround.
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp';
					$output .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ";\n\n";
					// Omit the CAST when the target type is CLOB to avoid invalid SQL state.
					$targetType = $this->columnDefinition($child[$i]['child'], false);
					$copyFrom   = 'DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'];

					if (strpos($targetType, 'CLOB') === false) {
						$copyFrom = 'CAST(' . $copyFrom . ' AS ' . $targetType . ')';
					}
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' = ' . $copyFrom . ";\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";
					/*
					 * DROP COLUMN puts the table into a state that requires REORG TABLE before
					 * it can be accessed again.
					 */
					$output .= "CALL ADMIN_CMD ('REORG TABLE DB_TABLE_PREFIX" .
					$parent['child'][0]['content'] . "');\n\n";
					// DB2 can't rename columns
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'];
					$output .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ";\n\n";
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
					' = DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . "Temp;\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
					"Temp;\n\n";
					$output .= "CALL ADMIN_CMD ('REORG TABLE DB_TABLE_PREFIX" .
					$parent['child'][0]['content'] . "');\n\n";

					if ($this->getNotNullElement($child[$i]['child'])) {
						$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
						' ALTER DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
						" SET NOT NULL;\n\n";
					}
				}

				break;

			default:
				$output .= parent::createSql($node, $index, $lastPeerIndex, $parent);
		}

		return $output;
	}

	public function getDbType() {
		return 'db2';
	}
}

class MSSqlGenerator extends BaseGenerator {
	public function __construct() {
		$this->setColumnDefinitionMap(
			array(
				'INTEGER-'       => 'INT',
				'INTEGER-MEDIUM' => 'INT',
				'INTEGER-LARGE'  => 'INT',
				'BIT-LARGE'      => 'INT',
				'BIT-MEDIUM'     => 'INT',
				'STRING-SMALL'   => 'NVARCHAR(32)',
				'STRING-MEDIUM'  => 'NVARCHAR(128)',
				'STRING-LARGE'   => 'NVARCHAR(255)',
				'TEXT-SMALL'     => 'NVARCHAR(MAX)',
				'TEXT-'          => 'NVARCHAR(MAX)',
				'TEXT-MEDIUM'    => 'NVARCHAR(MAX)',
				'TEXT-LARGE'     => 'NVARCHAR(MAX)',
				'BOOLEAN-'       => 'BIT',
				'BOOLEAN-MEDIUM' => 'BIT',
				'TIMESTAMP-'     => 'datetime',
			)
		);
	}

	public function columnDefinition($child, $includeNotNull = true, $includeDefault = true) {
		$output = parent::columnDefinition($child, $includeNotNull, $includeDefault);

		if ($includeNotNull && !$this->getNotNullElement($child)) {
			$output .= ' NULL';
		}

		return $output;
	}

	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'CHANGE':
				// table-name, schema-from, schema-to, (add, alter, remove)+
				for ($i = 3; $i < count($child); $i++) {
					$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);
				}
				$output .= $this->generateSchemaUpdate($child);

				break;

			case 'REMOVE':
				if (!isset($parent['name'])) {
					$output .= 'DROP TABLE DB_TABLE_PREFIX' . $node['child'][0]['content'] . ";\n\n";

					if ($node['child'][0]['content'] != 'Schema') {
						$output .= "DELETE FROM DB_TABLE_PREFIXSchema WHERE DB_COLUMN_PREFIXname='" .
						$node['child'][0]['content'] . "';\n\n";
					}
				} elseif ($parent['name'] == 'CHANGE') {
					// (column-name, key, index)+
					for ($i = 0; $i < count($child); $i++) {
						$c = $child[$i];

						switch ($c['name']) {
							case 'COLUMN-NAME':
								// column-name
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= ' DROP COLUMN DB_COLUMN_PREFIX' . $c['content'];
								$output .= ";\n\n";

								break;

							case 'KEY':
								if (empty($c['attrs']['PRIMARY'])) {
									$crc     = $this->getIndexCrc($c['child']);
									$output .= 'DROP INDEX DB_TABLE_PREFIX' .
									$parent['child'][0]['content'] . '_' . $crc . ";\n\n";
								} else {
									$output .= 'ALTER TABLE DB_TABLE_PREFIX' .
									$parent['child'][0]['content'] . ' DROP CONSTRAINT DB_TABLE_PREFIX'
									. $parent['child'][0]['content'] . "_pkey;\n\n";
								}

								break;

							case 'INDEX':
								// column-name
								$output .= 'DROP INDEX ';
								$nameKey = strtoupper('name_' . $this->getDbType());

								if (isset($c['attrs'][$nameKey])) {
									$output .= $c['attrs'][$nameKey];
								} else {
									$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
									'.DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
									'_' . $this->getIndexCrc($c['child']);
								}
								$output .= ";\n\n";

								break;

							default:
								$output .= "5. UNIMPLEMENTED: REMOVE $c[name]\n";
						}
					}
				}

				break;

			case 'ADD':
				// (column, key, index)+
				foreach ($child as $c) {
					switch ($c['name']) {
						case 'COLUMN':
							// column-name
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= ' ADD DB_COLUMN_PREFIX' . $c['child'][0]['content'];
							$output .= ' ' . $this->columnDefinition($c['child']);
							$output .= ";\n\n";

							break;

						case 'KEY':
							// column-name+
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
							' ADD ';

							if (!empty($c['attrs']['PRIMARY'])) {
								$output .= 'PRIMARY KEY(';
							} else {
								$output .= 'UNIQUE KEY(';
							}

							for ($i = 0; $i < count($c['child']); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $c['child'][$i]['content'];

								if ($i < count($c['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						case 'INDEX':
							// column-name
							$output .= 'CREATE INDEX ';
							$nameKey = strtoupper('name_' . $this->getDbType());
							$columns = $c['child'];

							if (isset($c['attrs'][$nameKey])) {
								$output .= $c['attrs'][$nameKey];
							} else {
								$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
								'_' . $this->getIndexCrc($columns);
							}
							$output .= ' ON ' . 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						default:
							$output .= "6. UNIMPLEMLENTED: ADD $c[name]\n";
					}
				}

				break;

			case 'TABLE':
				// table-name, schema, column+, (key | index)
				$output .= 'CREATE TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "(\n";

				for ($i = 2; $i < count($child); $i++) {
					if ($child[$i]['name'] != 'COLUMN') {
						$output .= "\n";

						break;
					}

					if ($i > 2) {
						$output .= ",\n";
					}
					$output        .= $this->createSql($child[$i], $i, count($child) - 1, $node);
					$firstNonColumn = $i + 1;
				}
				$output .= ");\n\n";

				for ($i = $firstNonColumn; $i < count($child); $i++) {
					if ($child[$i]['name'] == 'INDEX') {
						$crc     = $this->getIndexCrc($child[$i]['child']);
						$output .= 'CREATE INDEX DB_TABLE_PREFIX' . $child[0]['content'] . '_' . $crc .
						' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

						for ($j = 0; $j < count($child[$i]['child']); $j++) {
							$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

							if ($j < count($child[$i]['child']) - 1) {
								$output .= ', ';
							}
						}
						$output .= ");\n\n";
					} else /* key */ {
						if (!empty($child[$i]['attrs']['PRIMARY'])) {
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $child[0]['content'] .
							' ADD PRIMARY KEY (';
							$columns = $child[$i]['child'];

							for ($j = 0; $j < count($columns); $j++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$j]['content'];

								if ($j < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ");\n\n";
						} else {
							$crc     = $this->getIndexCrc($child[$i]['child']);
							$output .= 'CREATE UNIQUE INDEX DB_TABLE_PREFIX' . $child[0]['content'] .
							'_' . $crc . ' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

							for ($j = 0; $j < count($child[$i]['child']); $j++) {
								$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

								if ($j < count($child[$i]['child']) - 1) {
									$output .= ', ';
								}
							}
							$output .= ");\n\n";
						}
					}
				}

				// Schema info
				$output .= $this->createSql($child[1], 0, 0, $node);

				break;

			case 'ALTER':
				// column+
				for ($i = 0; $i < count($child); $i++) {
					// MSSQL can't add defaults when altering columns. Use a workaround.
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp';
					$output .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ";\n\n";
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' = CAST(DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ' AS ' .
					$this->columnDefinition($child[$i]['child'], false) . ");\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";
					// MSSQL can't rename columns
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'];
					$output .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ";\n\n";
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
					' = DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . "Temp;\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
					"Temp;\n\n";

					if ($this->getNotNullElement($child[$i]['child'])) {
						$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
						' ALTER COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
						' ' . $this->columnDefinition($child[$i]['child'], true, false) . ";\n\n";
					}
				}

				break;

			default:
				$output .= parent::createSql($node, $index, $lastPeerIndex, $parent);
		}

		return $output;
	}

	public function getDbType() {
		return 'mssql';
	}
}

class SQLiteGenerator extends BaseGenerator {
	public function __construct() {
		$this->setColumnDefinitionMap(
			array(
				'INTEGER-'       => 'INTEGER',
				'INTEGER-MEDIUM' => 'INTEGER',
				'INTEGER-LARGE'  => 'INTEGER',
				'BIT-LARGE'      => 'TEXT',
				'BIT-MEDIUM'     => 'TEXT',
				'STRING-SMALL'   => 'TEXT',
				'STRING-MEDIUM'  => 'TEXT',
				'STRING-LARGE'   => 'TEXT',
				'TEXT-SMALL'     => 'TEXT',
				'TEXT-'          => 'TEXT',
				'TEXT-MEDIUM'    => 'TEXT',
				'TEXT-LARGE'     => 'TEXT',
				'BOOLEAN-'       => 'INTEGER',
				'BOOLEAN-MEDIUM' => 'INTEGER',
				'TIMESTAMP-'     => 'INTEGER',
			)
		);
	}

	public function columnDefinition($child, $includeNotNull = true, $includeDefault = true) {
		$output = parent::columnDefinition($child, $includeNotNull, false);

		if ($includeDefault) {
			$defaultValue = $this->getDefaultElement($child);

			if (isset($defaultValue)) {
				$output .= " DEFAULT '$defaultValue'";
			}
		}

		return $output;
	}

	public function createSql($node, $index, $lastPeerIndex, $parent) {
		$output = '';

		$child = $node['child'] = isset($node['child']) ? $node['child'] : array();

		switch ($node['name']) {
			case 'TABLE':
				// table-name, schema, column+, (key | index)
				$output .= 'CREATE TABLE DB_TABLE_PREFIX' . $child[0]['content'] . "(\n";

				for ($i = 2; $i < count($child); $i++) {
					if ($child[$i]['name'] != 'COLUMN') {
						$output .= "\n";

						break;
					}

					if ($i > 2) {
						$output .= ",\n";
					}
					$output        .= $this->createSql($child[$i], $i, count($child) - 1, $node);
					$firstNonColumn = $i + 1;
				}
				$output .= ");\n\n";

				for ($i = $firstNonColumn; $i < count($child); $i++) {
					if ($child[$i]['name'] == 'INDEX') {
						// column-name
						$output .= 'CREATE INDEX ';
						$nameKey = strtoupper('name_' . $this->getDbType());
						$columns = $child[$i]['child'];

						if (isset($child[$i]['attrs'][$nameKey])) {
							$output .= $child[$i]['attrs'][$nameKey];
						} else {
							$output .= 'DB_TABLE_PREFIX' . $child[0]['content']
							. '_' . $this->getIndexCrc($columns);
						}
						$output .= ' ON ' . 'DB_TABLE_PREFIX' . $child[0]['content'] . '(';

						for ($j = 0; $j < count($columns); $j++) {
							$output .= 'DB_COLUMN_PREFIX' . $columns[$j]['content'];

							if ($j < count($columns) - 1) {
								$output .= ', ';
							}
						}
						$output .= ')';
						$output .= ";\n\n";

						break;
					}
					$output .= 'CREATE UNIQUE INDEX DB_TABLE_PREFIX' . $child[0]['content'];

					if (!empty($child[$i]['attrs']['PRIMARY'])) {
						$output .= '_pkey';
					} else {
						$output .= '_' . $this->getIndexCrc($child[$i]['child']);
					}
					$output .= ' ON DB_TABLE_PREFIX' . $child[0]['content'] . '(';

					for ($j = 0; $j < count($child[$i]['child']); $j++) {
						$output .= 'DB_COLUMN_PREFIX' . $child[$i]['child'][$j]['content'];

						if ($j < count($child[$i]['child']) - 1) {
							$output .= ', ';
						}
					}
					$output .= ");\n\n";
				}

				// Schema info
				$output .= $this->createSql($child[1], 0, 0, $node);

				break;

			case 'CHANGE':
				// table-name, schema-from, schema-to, (add, alter, remove)+
				for ($i = 3; $i < count($child); $i++) {
					$output .= $this->createSql($child[$i], $i, count($child) - 1, $node);
				}
				$output .= $this->generateSchemaUpdate($child);

				break;

			case 'ADD':
				// (column, key, index)+
				foreach ($child as $c) {
					switch ($c['name']) {
						case 'COLUMN':
							// column-name
							$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= ' ADD DB_COLUMN_PREFIX' . $c['child'][0]['content'];
							$output .= ' ' . $this->columnDefinition($c['child']);
							$output .= ";\n";
							$output .= 'VACUUM DB_TABLE_PREFIX' . $parent['child'][0]['content'];
							$output .= ";\n\n";

							break;

						case 'KEY':
							$output .= 'CREATE UNIQUE INDEX ';
							$columns = $c['child'];
							$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '_';

							if (!empty($c['attrs']['PRIMARY'])) {
								$output .= 'pkey';
							} else {
								$output .= $this->getIndexCrc($columns);
							}
							$output .= ' ON ' . 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						case 'INDEX':
							// column-name
							$output .= 'CREATE INDEX ';
							$nameKey = strtoupper('name_' . $this->getDbType());
							$columns = $c['child'];

							if (isset($c['attrs'][$nameKey])) {
								$output .= $c['attrs'][$nameKey];
							} else {
								$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content']
								. '_' . $this->getIndexCrc($columns);
							}
							$output .= ' ON ' . 'DB_TABLE_PREFIX' . $parent['child'][0]['content'] . '(';

							for ($i = 0; $i < count($columns); $i++) {
								$output .= 'DB_COLUMN_PREFIX' . $columns[$i]['content'];

								if ($i < count($columns) - 1) {
									$output .= ', ';
								}
							}
							$output .= ')';
							$output .= ";\n\n";

							break;

						default:
							$output .= "6. UNIMPLEMLENTED: ADD $c[name]\n";
					}
				}

				break;

			case 'REMOVE':
				if (!isset($parent['name'])) {
					$output .= 'DROP TABLE DB_TABLE_PREFIX' . $node['child'][0]['content'] . ";\n\n";

					if ($node['child'][0]['content'] != 'Schema') {
						$output .= "DELETE FROM DB_TABLE_PREFIXSchema WHERE DB_COLUMN_PREFIXname='"
						. $node['child'][0]['content'] . "';\n\n";
					}
				} elseif ($parent['name'] == 'CHANGE') {
					// (column-name, key, index)+
					for ($i = 0; $i < count($child); $i++) {
						$c = $child[$i];

						switch ($c['name']) {
							case 'COLUMN-NAME':
								/**
								 * @todo Find a better way to handle DROP COLUMN. The below code doesn't
								 * work in SQLite 3 and our adodb driver intercepts and handles it instead.
								 */
								$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'];
								$output .= ' DROP COLUMN DB_COLUMN_PREFIX' . $c['content'];
								$output .= ";\n\n";

								break;

							case 'KEY':
								if (empty($c['attrs']['PRIMARY'])) {
									$crc     = $this->getIndexCrc($c['child']);
									$output .= 'DROP INDEX ' . $parent['child'][0]['content'] . '_' . $crc
									  . ";\n\n";
								} else {
									$output .= 'ALTER TABLE DB_TABLE_PREFIX'
									. $parent['child'][0]['content']
									. ' DROP CONSTRAINT DB_TABLE_PREFIX'
									. $parent['child'][0]['content'] . "_pkey;\n\n";
								}

								break;

							case 'INDEX':
								// column-name
								$output .= 'DROP INDEX ';
								$nameKey = strtoupper('name_' . $this->getDbType());

								if (isset($c['attrs'][$nameKey])) {
									$output .= $c['attrs'][$nameKey];
								} else {
									$output .= 'DB_TABLE_PREFIX' . $parent['child'][0]['content']
									. '_' . $this->getIndexCrc($c['child']);
								}
								$output .= ";\n\n";

								break;

							default:
								$output .= "5. UNIMPLEMENTED: REMOVE $c[name]\n";
						}
					}
				}

				break;

			case 'ALTER':
				 // column+
				for ($i = 0; $i < count($child); $i++) {
					/*
					 * SQLite only supports ADD COLUMN and the workaround DROP COLUMN.
					 * As a workaround for ALTER COLUMN:
					 * Create Temporary Table
					 * Copy current column to temporary
					 * Delete current column
					 * Recreate Column with new settings
					 * Copy content back to current column
					 * Delete Temporary Table
					 */
					$output      .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp';
					$output      .= ' ' . $this->columnDefinition($child[$i]['child'], false) . ";\n\n";
					$output      .= 'VACUUM DB_TABLE_PREFIX' . $parent['child'][0]['content'] . ";\n\n";
					$output      .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . 'Temp' .
					' = DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";
					$output      .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' DROP COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . ";\n\n";
					$output      .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					' ADD COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'];
					$output      .= ' ' . $this->columnDefinition($child[$i]['child'], false, false);
					$defaultValue = $this->getDefaultElement($child[$i]['child']);

					if (($notNull = $this->getNotNullElement($child[$i]['child']))
						&& (empty($notNull['attrs']['EMPTY'])
						|| $notNull['attrs']['EMPTY'] != 'allowed')
					) {
						$output .= " NOT NULL DEFAULT '" . (isset($defaultValue) ? $defaultValue : '') . "'";
					} elseif ($defaultValue !== null) {
						$output .= " DEFAULT '" . $defaultValue . "'";
					}
					$output .= ";\n\n";
					$output .= 'VACUUM DB_TABLE_PREFIX' . $parent['child'][0]['content'] . ";\n\n";
					$output .= 'UPDATE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					   ' SET DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] .
					   ' = DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . "Temp;\n\n";
					$output .= 'ALTER TABLE DB_TABLE_PREFIX' . $parent['child'][0]['content'] .
					   ' DROP COLUMN DB_COLUMN_PREFIX' . $child[$i]['child'][0]['content'] . "Temp;\n\n";
				}

				break;

			default:
				$output .= parent::createSql($node, $index, $lastPeerIndex, $parent);
		}

		return $output;
	}

	public function getDbType() {
		return 'sqlite';
	}
}
