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
 *
 * XmlParser class adapted from a version by Monte Ohrt (monte at NOT-SP-AM dot ohrt dot com)
 * published at http://php.net/xml
 */
class XmlParser {
	public $_xmlObject;
	public $_output = array();

	public function __construct() {
		$this->_xmlObject = xml_parser_create();
		xml_set_object($this->_xmlObject, $this);
		xml_set_character_data_handler($this->_xmlObject, 'dataHandler');
		xml_set_element_handler($this->_xmlObject, 'startHandler', 'endHandler');
	}

	public function parse($path) {
		$data = file_get_contents($path);

		if ($data === false) {
			die('Cannot open XML data file: ' . $path);
		}

		if (!xml_parse($this->_xmlObject, $data, 1)) {
			die(
				sprintf(
					'XML error: %s at line %d',
					xml_error_string(xml_get_error_code($this->_xmlObject)),
					xml_get_current_line_number($this->_xmlObject)
				)
			);
			xml_parser_free($this->_xmlObject);
		}

		return $this->_output;
	}

	public function startHandler($parser, $name, $attribs) {
		$content = array(
			'name' => $name,
		);

		if (!empty($attribs)) {
			$content['attrs'] = $attribs;
		}
		array_push($this->_output, $content);
	}

	public function dataHandler($parser, $data) {
		if (isset($data)) {
			$outputIndex                            = count($this->_output) - 1;
			$this->_output[$outputIndex]['content'] = $data;
		}
	}

	public function endHandler($parser, $name) {
		if (count($this->_output) > 1) {
			$data                                   = array_pop($this->_output);
			$outputIndex                            = count($this->_output) - 1;
			$this->_output[$outputIndex]['child'][] = $data;
		}
	}

	public function cleanUp() {
		xml_parser_free($this->_xmlObject);
	}
}
