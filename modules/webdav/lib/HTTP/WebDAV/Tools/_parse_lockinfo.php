<?php // $Id: _parse_lockinfo.php 17189 2007-11-17 08:53:36Z bharat $
/*
   +----------------------------------------------------------------------+
   | Copyright (c) 2002-2007 Christian Stocker, Hartmut Holzgraefe        |
   | All rights reserved                                                  |
   |                                                                      |
   | Redistribution and use in source and binary forms, with or without   |
   | modification, are permitted provided that the following conditions   |
   | are met:                                                             |
   |                                                                      |
   | 1. Redistributions of source code must retain the above copyright    |
   |    notice, this list of conditions and the following disclaimer.     |
   | 2. Redistributions in binary form must reproduce the above copyright |
   |    notice, this list of conditions and the following disclaimer in   |
   |    the documentation and/or other materials provided with the        |
   |    distribution.                                                     |
   | 3. The names of the authors may not be used to endorse or promote    |
   |    products derived from this software without specific prior        |
   |    written permission.                                               |
   |                                                                      |
   | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS  |
   | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT    |
   | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS    |
   | FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE       |
   | COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  |
   | INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, |
   | BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     |
   | LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER     |
   | CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   |
   | LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    |
   | ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE      |
   | POSSIBILITY OF SUCH DAMAGE.                                          |
   +----------------------------------------------------------------------+
*/

/**
 * Helper class for parsing LOCK request bodies
 *
 * @package HTTP_WebDAV_Server
 * @author Hartmut Holzgraefe <hholzgra@php.net>
 * @version 0.99.1dev
 */
class _parse_lockinfo
{
    /**
     * Success state flag
     *
     * @var boolean
     * @access public
     */
    var $success = false;

    /**
     * Lock type, currently only write
     *
     * @var string
     * @access public
     */
    var $locktype = '';

    /**
     * Lock scope, shared or exclusive
     *
     * @var string
     * @access public
     */
    var $lockscope = '';

    /**
     * Lock owner information
     *
     * @var string
     * @access public
     */
    var $owner = '';

    /**
     * Flag that is set during lock owner read
     *
     * @var boolean
     * @access private
     */
    var $collect_owner = false;

    /**
     * Constructor
     *
     * @param resource input stream file descriptor
     * @access public
     */
    function _parse_lockinfo($handle)
    {
        // open input stream
        if (!$handle) {
            $this->success = false;
            return;
        }

        // success state flag
        $this->success = true;

        // remember if any input was parsed
        $had_input = false;

        // create namespace aware XML parser
        $parser = xml_parser_create_ns('UTF-8', ' ');

        // set tag & data handlers
        xml_set_element_handler($parser, array(&$this, '_startElement'),
            array(&$this, "_endElement"));

        xml_set_character_data_handler($parser, array(&$this, '_data'));

        // we want a case sensitive parser
        xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, false);

        // parse input
        while ($this->success && !feof($handle)) {
            $line = fgets($handle);
            if (is_string($line)) {
                $had_input = true;
                $this->success &= xml_parse($parser, $line, false);
            }
        }

        // finish parsing
        if ($had_input) {
            $this->success &= xml_parse($parser, '', true);
        }

        // check if required tags where found
        $this->success &= !empty($this->locktype);
        $this->success &= !empty($this->lockscope);

        // free parser resource
        xml_parser_free($parser);

        // close input stream
        fclose($handle);
    }

    /**
     * Start tag handler
     *
     * @access private
     * @param resource parser
     * @param string tag name
     * @param array tag attributes
     * @return void
     */
    function _startElement($parser, $name, $attrs)
    {
        // namespace handling
        if (strstr($name, ' ')) {
            list ($ns, $name) = explode(' ', $name);
            if (empty($ns)) {
                $this->success = false;
            }
        }

        // everything within the <owner> tag needs to be collected
        if ($this->collect_owner) {
            $ns_short = '';
            $ns_attr = '';
            if ($ns) {
                if ($ns == 'DAV:') {
                    $ns_short = 'D:';
                } else {
                    $ns_attr = ' xmlns="' . $ns . '"';
                }
            }
            $this->owner .= "<$ns_short$name$ns_attr>";
        } else if ($ns == 'DAV:') {
            // parse only the essential tags
            switch ($name) {
            case 'write':
                $this->locktype = $name;
                break;
            case 'exclusive':
            case 'shared':
                $this->lockscope = $name;
                break;
            case 'owner':
                $this->collect_owner = true;
                break;
            }
        }
    }

    /**
     * End tag handler
     *
     * @access private
     * @param resource parser
     * @param string tag name
     * @return void
     */
    function _endElement($parser, $name)
    {
        // namespace handling
        if (strstr($name, ' ')) {
            list ($ns, $name) = explode(' ', $name);
	} else {
	    $ns = '';
	}

        // <owner> finished?
        if ($ns == 'DAV:' && $name == 'owner') {
            $this->collect_owner = false;
        }

        // within <owner> we have to collect everything
        if ($this->collect_owner) {
            $ns_short = '';
            $ns_attr = '';
            if ($ns) {
                if ($ns == 'DAV:') {
                    $ns_short = 'D:';
                } else {
                    $ns_attr = ' xmlns="' . $ns . '"';
                }
            }
            $this->owner .= "</$ns_short$name$ns_attr>";
        }
    }

    /**
     * Character data handler
     *
     * @access private
     * @param resource parser
     * @param string data
     * @return void
     */
    function _data($parser, $data)
    {
        // only the <owner> tag has data content
        if ($this->collect_owner) {
            $this->owner .= $data;
        }
    }
}
?>
