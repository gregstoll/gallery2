<?php // $Id: Server.php 17189 2007-11-17 08:53:36Z bharat $
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

require_once(dirname(__FILE__) . '/Tools/_parse_propfind.php');
require_once(dirname(__FILE__) . '/Tools/_parse_proppatch.php');
require_once(dirname(__FILE__) . '/Tools/_parse_lockinfo.php');

define('HTTP_WEBDAV_SERVER_DATATYPE_NAMESPACE',
    'urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882');

/**
 * Virtual base class for implementing WebDAV servers
 *
 * WebDAV server base class, needs to be extended to do useful work
 *
 * @package HTTP_WebDAV_Server
 * @author Hartmut Holzgraefe <hholzgra@php.net>
 * @version 0.99.1dev
 */
class HTTP_WebDAV_Server
{
    // {{{ Member Variables

    /**
     * URL path for this request
     *
     * @var string
     */
    var $path;

    /**
     * Base URL for this request
     *
     * See PHP parse_url structure
     *
     * @var array
     */
    var $baseUrl;

    /**
     * Realm string to be used in authentification popups
     *
     * @var string
     */
    var $http_auth_realm = 'PHP WebDAV';

    /**
     * String to be used in "X-Dav-Powered-By" header
     *
     * @var string
     */
    var $dav_powered_by = '';

    /**
     * Remember parsed If: (RFC2518 9.4) header conditions
     *
     * @var array
     */
    var $_if_header_uris = array();

    /**
     * HTTP response headers
     *
     * @var array
     */
    var $headers = array();

    /**
     * Encoding of property values passed in
     *
     * @var string
     */
    var $_prop_encoding = 'utf-8';

    // }}}

    // {{{ handleRequest

    /**
     * Handle WebDAV request
     *
     * Dispatch WebDAV request to the apropriate method wrapper
     *
     * @param void
     * @return void
     */
    function handleRequest()
    {
        // identify ourselves
        if (empty($this->dav_powered_by)) {
            $this->dav_powered_by = 'PHP class: ' . get_class($this);
        }
        $this->setResponseHeader('X-Dav-Powered-By: ' . $this->dav_powered_by);

        // set path
        if (empty($this->path)) {
            $this->path = $this->_urldecode($_SERVER['PATH_INFO']);
            $this->path = trim($this->path, '/');
        }

        if (ini_get('magic_quotes_gpc')) {
            $this->path = stripslashes($this->path);
        }

        // set base URL
        if (empty($this->baseUrl)) {
            $this->baseUrl = parse_url(
                "http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]");
            $this->baseUrl['path'] = substr($this->baseUrl['path'], 0,
                strlen($this->baseUrl['path']) - strlen($_SERVER['PATH_INFO']));
        }

        // check authentication
        if (!$this->check_auth_wrapper()) {

            // RFC2518 says we must use Digest instead of Basic but Microsoft
            // clients do not support Digest and we don't support NTLM or
            // Kerberos so we are stuck with Basic here
            $this->setResponseHeader('WWW-Authenticate: Basic realm="'
                . $this->http_auth_realm . '"');

            // Windows seems to require this being the last header sent
            // (changed according to PECL bug #3138)
            $this->setResponseStatus('401 Authentication Required');
            return;
        }

        // check
        if (!$this->_check_if_header_conditions()) {
            $this->setResponseStatus('412 Precondition Failed');
            return;
        }

        // detect requested method names
        $method = strtolower($_SERVER['REQUEST_METHOD']);
        $wrapper = $method . '_wrapper';

        // emulate HEAD using GET if no HEAD method found
        if ($wrapper == 'head_wrapper' &&
                !method_exists($this, 'head')) {
            $method = 'get';
        }

        if (method_exists($this, $method) &&
                method_exists($this, $wrapper) ||
                $method == 'options') {
            $this->$wrapper();
            return;
        }

        // method not found/implemented
        if ($method == 'lock') {
            $this->setResponseStatus('412 Precondition Failed');
            return;
        }

        // tell client what's allowed
        $this->setResponseStatus('405 Method Not Allowed');
        $this->setResponseHeader('Allow: ' . implode(', ', $this->_allow()));
    }

    // }}}

    // {{{ abstract WebDAV methods

    // {{{ PROPFIND

    /**
     * PROPFIND implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function propfind()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ PROPPATCH

    /**
     * PROPPATCH implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function proppatch()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ MKCOL

    /**
     * MKCOL implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function mkcol()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ GET

    /**
     * GET implementation
     *
     * Overload this method to retrieve resources from your server
     *
     * @abstract
     * @param array &$params array of input and output parameters
     * <br><b>input</b><ul>
     * <li> path -
     * </ul>
     * <br><b>output</b><ul>
     * <li> size -
     * </ul>
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function get()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ DELETE

    /**
     * DELETE implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function delete()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ PUT

    /**
     * PUT implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function put()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ COPY

    /**
     * COPY implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function copy()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ MOVE

    /**
     * MOVE implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function move()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ LOCK

    /**
     * LOCK implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function lock()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ UNLOCK

    /**
     * UNLOCK implementation
     *
     * @abstract
     * @param array &$params
     * @returns int HTTP-Statuscode
     */

    /* abstract
       function unlock()
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // }}}

    // {{{ other abstract methods

    // {{{ checkAuth

    /**
     * Check authentication
     *
     * Overload this method to retrieve and confirm authentication information
     *
     * @abstract
     * @param string type Authentication type, e.g. "basic" or "digest"
     * @param string username Transmitted username
     * @param string passwort Transmitted password
     * @returns bool Authentication status
     */

    /* abstract
       function checkAuth($type, $username, $password)
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // {{{ getLocks

    /**
     * Get lock entries for a resource
     *
     * Overload this method to return shared and exclusive locks active for
     * this resource
     *
     * @abstract
     * @param string resource path to check
     * @returns array of lock entries each consisting
     *                of 'type' ('shared'/'exclusive'), 'token' and 'timeout'
     */

    /* abstract
       function getLocks($path)
       {
           // dummy entry for PHPDoc
       }
    */

    // }}}

    // }}}

    // {{{ WebDAV HTTP method wrappers

    // {{{ options

    /**
     * OPTIONS method handler
     *
     * The OPTIONS method handler creates a valid OPTIONS reply including Dav:
     * and Allowed: heaers based on the implemented methods found in the actual
     * instance
     *
     * @param void
     * @return void
     */
    function options()
    {
        // get allowed methods
        $allow = $this->_allow();

        // dav header
        $dav = array(1); // assume we are always dav class 1 compliant
        if (in_array('LOCK', $allow) && in_array('UNLOCK', $allow)) {
            $dav[] = 2; // dav class 2 requires that locking is supported
        }

        // tell clients what we found
        $this->setResponseHeader('Allow: ' . implode(', ', $allow));
        $this->setResponseHeader('DAV: ' . implode(',', $dav));
        $this->setResponseHeader('Content-Length: 0');

        // Microsoft clients default to the Frontpage protocol unless we tell
        // them to use WebDAV
        $this->setResponseHeader('MS-Author-Via: DAV');

        $this->setResponseStatus('200 OK');
    }

    // }}}

    // {{{ propfind_request_helper

    /**
     * PROPFIND request helper - prepares data-structures from PROPFIND requests
     *
     * @param options
     * @return void
     */
    function propfind_request_helper(&$options)
    {
        $options = array();
        $options['path'] = $this->path;

        // get depth from header (default is 'infinity')
        $options['depth'] = 'infinity';
        if ($this->_hasNonEmptyDepthRequestHeader()) {
            $options['depth'] = $_SERVER['HTTP_DEPTH'];
        }

        // analyze request payload
        $parser = new _parse_propfind($this->openRequestBody());
        if (!$parser->success) {
            $this->setResponseStatus('400 Bad Request');
            return;
        }

        $options['props'] = $parser->props;

        return true;
    }

    // }}}

    // {{{ propfind_response_helper

    /**
     * PROPFIND response helper - format PROPFIND response
     *
     * @param options
     * @param files
     * @return void
     */
    function propfind_response_helper($options, $files)
    {
        $responses = array();

        // now loop over all returned files
        foreach ($files as $file) {
            $response = array();

            if (empty($file['href'])) {
                $response['href'] = $this->getHref($file['path']);
            } else {
                $response['href'] = $file['href'];
            }

            $response['propstat'] = array();

            // collect namespaces here
            $response['namespaces'] = array();
            if (!empty($options['namespaces'])) {
                $response['namespaces'] = $options['namespaces'];
            }

            // Microsoft needs this special namespace for date and time values
            $response['namespaces'][HTTP_WEBDAV_SERVER_DATATYPE_NAMESPACE] =
                'ns' . count($response['namespaces']);

            if (is_array($options['props'])) {

                // loop over all requested properties
                foreach ($options['props'] as $reqprop) {
                    $status = '200 OK';
                    $prop = $this->getProp($reqprop, $file, $options);

                    if (!empty($prop['status'])) {
                        $status = $prop['status'];
                    }

                    if (empty($response['propstat'][$status])) {
                        $response['propstat'][$status] = array();
                    }

                    $response['propstat'][$status][] = $prop;

                    // namespace handling
                    if (empty($prop['ns']) || // empty namespace
                            $prop['ns'] == 'DAV:' || // default namespace
                            !empty($response['namespaces'][$prop['ns']])) { // already known
                        continue;
                    }

                    // register namespace
                    $response['namespaces'][$prop['ns']] = 'ns' . count($response['namespaces']);
                }
            } else if (is_array($file['props'])) {

                // loop over all returned properties
                foreach ($file['props'] as $prop) {
                    $status = '200 OK';

                    if (!empty($prop['status'])) {
                        $status = $prop['status'];
                    }

                    if (empty($response['propstat'][$status])) {
                        $response['propstat'][$status] = array();
                    }

                    if ($options['props'] == 'propname') {

                        // only names of all existing properties were requested
                        // so remove values
                        unset($prop['value']);
                    }

                    $response['propstat'][$status][] = $prop;
                        unset($prop['value']);

                    // namespace handling
                    if (empty($prop['ns']) || // empty namespace
                            $prop['ns'] == 'DAV:' || // default namespace
                            !empty($response['namespaces'][$prop['ns']])) { // already known
                        continue;
                    }

                    // register namespace
                    $response['namespaces'][$prop['ns']] = 'ns' . count($response['namespaces']);
                }
            }

            $responses[] = $response;
        }

        $this->_multistatusResponseHelper($responses);
    }

    // }}}

    // {{{ propfind_wrapper

    /**
     * PROPFIND method wrapper
     *
     * @param void
     * @return void
     */
    function propfind_wrapper()
    {
        // prepare data-structure from PROPFIND request
        if (!$this->propfind_request_helper($options)) {
            return;
        }

        // call user handler
        if (!$this->propfind($options, $files)) {
            return;
        }

        // format PROPFIND response
        $this->propfind_response_helper($options, $files);
    }

    // }}}

    // {{{ proppatch_request_helper

    /**
     * PROPPATCH request helper - prepares data-structures from PROPPATCH requests
     *
     * @param options
     * @return void
     */
    function proppatch_request_helper(&$options)
    {
        $options = array();
        $options['path'] = $this->path;

        $propinfo = new _parse_proppatch($this->openRequestBody());

        if (!$propinfo->success) {
            $this->setResponseStatus('400 Bad Request');
            return;
        }

        $options['props'] = $propinfo->props;

        return true;
    }

    // }}}

    // {{{ proppatch_response_helper

    /**
     * PROPPATCH response helper - format PROPPATCH response
     *
     * @param options
     * @param responsedescr
     * @return void
     */
    function proppatch_response_helper($options, $responsedescription=null)
    {
        $response = array();

        if (empty($options['href'])) {
            $response['href'] = $this->getHref($options['path']);
        } else {
            $response['href'] = $options['href'];
        }

        $response['propstat'] = array();

        // collect namespaces here
        $response['namespaces'] = array();
        if (!empty($options['namespaces'])) {
            $response['namespaces'] = $options['namespaces'];
        }

        if (!empty($options['props']) && is_array($options['props'])) {
            foreach ($options['props'] as $prop) {
                $status = '200 OK';
                if (!empty($prop['status'])) {
                    $status = $prop['status'];
                }

                if (empty($response['propstat'][$status])) {
                    $response['propstat'][$status] = array();
                }

                $response['propstat'][$status][] = $prop;

                // namespace handling
                if (empty($prop['ns']) || // empty namespace
                        $prop['ns'] == 'DAV:' || // default namespace
                        !empty($response['namespaces'][$prop['ns']])) { // already known
                    continue;
                }

                // register namespace
                $response['namespaces'][$prop['ns']] = 'ns' . count($response['namespaces']);
            }
        }

        $response['responsedescription'] = $responsedescription;

        $this->_multistatusResponseHelper(array($response));
    }

    // }}}

    // {{{ proppatch_wrapper

    /**
     * PROPPATCH method wrapper
     *
     * @param void
     * @return void
     */
    function proppatch_wrapper()
    {
        // check resource is not locked
        if (!$this->check_locks_wrapper($this->path)) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        // perpare data-structure from PROPATCH request
        if (!$this->proppatch_request_helper($options)) {
            return;
        }

        // call user handler
        $responsedescription = $this->proppatch($options);

        // format PROPPATCH response
        $this->proppatch_response_helper($options, $responsedescription);
    }

    // }}}

    // {{{ mkcol_wrapper

    /**
     * MKCOL method wrapper
     *
     * @param void
     * @return void
     */
    function mkcol_wrapper()
    {
        $options = array();
        $options['path'] = $this->path;

        $status = $this->mkcol($options);

        $this->setResponseStatus($status);
    }

    // }}}

    // {{{ get_request_helper

    /**
     * GET request helper - prepares data-structures from GET requests
     *
     * @param options
     * @return void
     */
    function get_request_helper(&$options)
    {
        // TODO check for invalid stream

        $options = array();
        $options['path'] = $this->path;

        $this->_get_ranges($options);

        return true;
    }

    /**
     * Parse HTTP Range: header
     *
     * @param  array options array to store result in
     * @return void
     */
    function _get_ranges(&$options)
    {
        // process Range: header if present
        if (!empty($_SERVER['HTTP_RANGE'])) {

            // we only support standard 'bytes' range specifications for now
            if (ereg('bytes[[:space:]]*=[[:space:]]*(.+)', $_SERVER['HTTP_RANGE'], $matches)) {
                $options['ranges'] = array();

                // ranges are comma separated
                foreach (explode(',', $matches[1]) as $range) {
                    // ranges are either from-to pairs or just end positions
                    list($start, $end) = explode('-', $range);
                    $options['ranges'][] = ($start === '') ? array('last' => $end) : array('start' => $start, 'end' => $end);
                }
            }
        }
    }

    // }}}

    // {{{ get_response_helper

    /**
     * GET response helper - format GET response
     *
     * @param options
     * @param status
     * @return void
     */
    function get_response_helper($options, $status)
    {
        if (empty($status)) {
            $status = '404 Not Found';
        }

        // set headers before we start printing
        $this->setResponseStatus($status);

        if ($status !== true) {
            return;
        }

        if (empty($options['mimetype'])) {
            $options['mimetype'] = 'application/octet-stream';
        }
        $this->setResponseHeader("Content-Type: $options[mimetype]");

        if (!empty($options['mtime'])) {
            $this->setResponseHeader('Last-Modified:'
                . gmdate('D, d M Y H:i:s', $options['mtime']) . 'GMT');
        }

        if ($options['stream']) {
            // GET handler returned a stream

            if (!empty($options['ranges']) &&
                    (fseek($options['stream'], 0, SEEK_SET) === 0)) {
                // partial request and stream is seekable

                if (count($options['ranges']) === 1) {
                    $range = $options['ranges'][0];

                    if (!empty($range['start'])) {
                        fseek($options['stream'], $range['start'], SEEK_SET);
                        if (feof($options['stream'])) {
                            $this->setResponseStatus(
                                '416 Requested Range Not Satisfiable');
                            return;
                        }

                        if (!empty($range['end'])) {
                            $size = $range['end'] - $range['start'] + 1;
                            $this->setResponseStatus('206 Partial');
                            $this->setResponseHeader("Content-Length: $size");
                            $this->setResponseHeader(
                                "Content-Range: $range[start]-$range[end]/"
                                . (!empty($options['size']) ? $options['size'] : '*'));
                            while ($size && !feof($options['stream'])) {
                                $buffer = fread($options['stream'], 4096);
                                $size -= strlen($buffer);
                                echo $buffer;
                            }
                        } else {
                            $this->setResponseStatus('206 Partial');
                            if (!empty($options['size'])) {
                                $this->setResponseHeader("Content-Length: "
                                    . ($options['size'] - $range['start']));
                                $this->setResponseHeader(
                                    "Content-Range: $range[start]-$range[end]/"
                                    . (!empty($options['size']) ? $options['size'] : '*'));
                            }
                            fpassthru($options['stream']);
                        }
                    } else {
                        $this->setResponseHeader("Content-Length: $range[last]");
                        fseek($options['stream'], -$range['last'], SEEK_END);
                        fpassthru($options['stream']);
                    }
                } else {
                    $this->_multipart_byterange_header(); // init multipart
                    foreach ($options['ranges'] as $range) {

                        // TODO what if size unknown? 500?
                        if (!empty($range['start'])) {
                            $from = $range['start'];
                            $to = !empty($range['end']) ? $range['end'] : $options['size'] - 1;
                        } else {
                            $from = $options['size'] - $range['last'] - 1;
                            $to = $options['size'] - 1;
                        }
                        $total = !empty($options['size']) ? $options['size'] : '*';
                        $size = $to - $from + 1;
                        $this->_multipart_byterange_header($options['mimetype'],
                            $from, $to, $total);

                        fseek($options['stream'], $start, SEEK_SET);
                        while ($size && !feof($options['stream'])) {
                            $buffer = fread($options['stream'], 4096);
                            $size -= strlen($buffer);
                            echo $buffer;
                        }
                    }

                    // end multipart
                    $this->_multipart_byterange_header();
                }
            } else {
                // normal request or stream isn't seekable, return full content
                if (!empty($options['size'])) {
                    $this->setResponseHeader("Content-Length: $options[size]");
                }

                fpassthru($options['stream']);
            }
        } else if (!empty($options['data']))  {
            if (is_array($options['data'])) {
                // reply to partial request
            } else {
                $this->setResponseHeader("Content-Length: "
                    . strlen($options['data']));
                echo $options['data'];
            }
        }
    }

    /**
     * Generate separator headers for multipart response
     *
     * First and last call happen without parameters to generate the initial
     * header and closing sequence, all calls inbetween require content
     * mimetype, start and end byte position and optionaly the total byte
     * length of the requested resource
     *
     * @param string mimetype
     * @param int start byte position
     * @param int end byte position
     * @param int total resource byte size
     */
    function _multipart_byterange_header($mimetype = false, $from = false,
        $to = false, $total = false)
    {
        if ($mimetype === false) {
            if (empty($this->multipart_separator)) {
                // init
                // a little naive, this sequence *might* be part of the content
                // but it's really not likely and rather expensive to check
                $this->multipart_separator = 'SEPARATOR_' . md5(microtime());

                // generate HTTP header
                $this->setResponseHeader(
                    'Content-Type: multipart/byteranges; boundary='
                    . $this->multipart_separator);
                return;
            }

            // end
            // generate closing multipart sequence
            echo "\n--{$this->multipart_separator}--";
            return;
        }

        // generate separator and header for next part
        echo "\n--{$this->multipart_separator}\n";
        echo "Content-Type: $mimetype\n";
        echo "Content-Range: $from-$to/"
            . ($total === false ? "*" : $total) . "\n\n";
    }

    // }}}

    // {{{ get_wrapper

    /**
     * GET method wrapper
     *
     * @param void
     * @return void
     */
    function get_wrapper()
    {
        // perpare data-structure from GET request
        if (!$this->get_request_helper($options)) {
            return;
        }

        // call user handler
        $status = $this->get($options);

        // format GET response
        $this->get_response_helper($options, $status);
    }

    // }}}

    // {{{ head_response_helper

    /**
     * HEAD response helper - format HEAD response
     *
     * @param options
     * @param status
     * @return void
     */
    function head_response_helper($options, $status)
    {
        if (empty($status)) {
            $status = '404 Not Found';
        }

        // set headers before we start printing
        $this->setResponseStatus($status);

        if ($status !== true) {
            return;
        }

        if (empty($options['mimetype'])) {
            $options['mimetype'] = 'application/octet-stream';
        }
        $this->setResponseHeader("Content-Type: $options[mimetype]");

        if (!empty($options['mtime'])) {
            $this->setResponseHeader('Last-Modified:'
                . gmdate('D, d M Y H:i:s', $options['mtime']) . 'GMT');
        }

        if (!empty($options['stream'])) {
            // GET handler returned a stream

            if (!empty($options['ranges'])
                    && (fseek($options['stream'], 0, SEEK_SET) === 0)) {
                // partial request and stream is seekable

                if (count($options['ranges']) === 1) {
                    $range = $options['ranges'][0];

                    if (!empty($range['start'])) {
                        fseek($options['stream'], $range['start'], SEEK_SET);
                        if (feof($options['stream'])) {
                            $this->setResponseStatus(
                                '416 Requested Range Not Satisfiable');
                            return;
                        }

                        if (!empty($range['end'])) {
                            $size = $range['end'] - $range['start'] + 1;
                            $this->setResponseStatus('206 Partial');
                            $this->setResponseHeader("Content-Length: $size");
                            $this->setResponseHeader(
                                "Content-Range: $range[start]-$range[end]/"
                                . (!empty($options['size']) ? $options['size'] : '*'));
                        } else {
                            $this->setResponseStatus('206 Partial');
                            if (!empty($options['size'])) {
                                $this->setResponseHeader("Content-Length: "
                                    . ($options['size'] - $range['start']));
                                $this->setResponseHeader(
                                    "Content-Range: $start-$end/"
                                    . (!empty($options['size']) ? $options['size'] : '*'));
                            }
                        }
                    } else {
                        $this->setResponseHeader(
                            "Content-Length: $range[last]");
                        fseek($options['stream'], -$range['last'], SEEK_END);
                    }
                } else {
                    $this->_multipart_byterange_header(); // init multipart
                    foreach ($options['ranges'] as $range) {

                        // TODO what if size unknown? 500?
                        if (!empty($range['start'])) {
                            $from = $range['start'];
                            $to = !empty($range['end']) ? $range['end'] :
                                $options['size'] - 1;
                        } else {
                            $from = $options['size'] - $range['last'] - 1;
                            $to = $options['size'] - 1;
                        }
                        $total = !empty($options['size']) ? $options['size'] :
                            '*';
                        $size = $to - $from + 1;
                        $this->_multipart_byterange_header($options['mimetype'],
                            $from, $to, $total);

                        fseek($options['stream'], $start, SEEK_SET);
                    }
                    $this->_multipart_byterange_header(); // end multipart
                }
            } else {
                // normal request or stream isn't seekable, return full content
                if (!empty($options['size'])) {
                    $this->setResponseHeader("Content-Length: $options[size]");
                }
            }
        } else if (!empty($options['data']))  {
            if (is_array($options['data'])) {
                // reply to partial request
            } else {
                $this->setResponseHeader("Content-Length: "
                    . strlen($options['data']));
            }
        }
    }

    // }}}

    // {{{ head_wrapper

    /**
     * HEAD method wrapper
     *
     * @param void
     * @return void
     */
    function head_wrapper()
    {
        $options = array();
        $options['path'] = $this->path;

        // call user handler
        if (method_exists($this, 'head')) {
            $status = $this->head($options);
        } else {

            // can emulate HEAD using GET
            ob_start();
            $status = $this->get($options);
            ob_end_clean();
        }

        // format HEAD response
        $this->head_response_helper($options, $status);
    }

    // }}}

    // {{{ put_request_helper

    /**
     * PUT request helper - prepares data-structures from PUT requests
     *
     * @param options
     * @return void
     */
    function put_request_helper(&$options)
    {
        $options = array();
        $options['path'] = $this->path;

        /* Content-Length may be zero */
        if (!isset($_SERVER['CONTENT_LENGTH'])) {
            return;
        }
        $options['content_length'] = $_SERVER['CONTENT_LENGTH'];

        // default content type if none given
        $options['content_type'] = 'application/unknown';

        // get the content-type
        if (!empty($_SERVER['CONTENT_TYPE'])) {

            // for now we do not support any sort of multipart requests
            if (!strncmp($_SERVER['CONTENT_TYPE'], 'multipart/', 10)) {
                $this->setResponseStatus('501 Not Implemented');
                echo 'The service does not support mulipart PUT requests';
                return;
            }

            $options['content_type'] = $_SERVER['CONTENT_TYPE'];
        }

        // RFC2616 2.6: The recipient of the entity MUST NOT ignore any
        // Content-* (e.g. Content-Range) headers that it does not understand
        // or implement and MUST return a 501 (Not Implemented) response in
        // such cases.
        foreach ($_SERVER as $key => $value) {
            if (strncmp($key, 'HTTP_CONTENT', 11)) {
                continue;
            }

            switch ($key) {
            case 'HTTP_CONTENT_ENCODING': // RFC2616 14.11

                // TODO support this if ext/zlib filters are available
                $this->setResponseStatus('501 Not Implemented');
                echo "The service does not support '$value' content encoding";
                return;

            case 'HTTP_CONTENT_LANGUAGE': // RFC2616 14.12

                // we assume it is not critical if this one is ignored in the
                // actual PUT implementation
                $options['content_language'] = $value;
                break;

            case 'HTTP_CONTENT_LENGTH':

                // defined on IIS and has the same value as CONTENT_LENGTH
                break;

            case 'HTTP_CONTENT_LOCATION': // RFC2616 14.14

                // meaning of the Content-Location header in PUT or POST
                // requests is undefined; servers are free to ignore it in
                // those cases
                break;

            case 'HTTP_CONTENT_RANGE': // RFC2616 14.16

                // single byte range requests are supported
                // the header format is also specified in RFC2616 14.16
                // TODO we have to ensure that implementations support this or send 501 instead
                if (!preg_match('@bytes\s+(\d+)-(\d+)/((\d+)|\*)@', $value, $matches)) {
                    $this->setResponseStatus('400 Bad Request');
                    echo 'The service does only support single byte ranges';
                    return;
                }

                $range = array('start' => $matches[1], 'end' => $matches[2]);
                if (is_numeric($matches[3])) {
                    $range['total_length'] = $matches[3];
                }
                $option['ranges'][] = $range;

                // TODO make sure the implementation supports partial PUT
                // this has to be done in advance to avoid data being overwritten
                // on implementations that do not support this...
                break;

            case 'HTTP_CONTENT_MD5': // RFC2616 14.15

                // TODO maybe we can just pretend here?
                $this->setResponseStatus('501 Not Implemented');
                echo 'The service does not support content MD5 checksum verification';
                return;

            case 'HTTP_CONTENT_TYPE':

                // defined on IIS and has the same value as CONTENT_TYPE
                break;

            default:

                // any other unknown Content-* headers
                $this->setResponseStatus('501 Not Implemented');
                echo "The service does not support '$key'";
                return;
            }
        }

        $options['stream'] = $this->openRequestBody();

        return true;
    }

    // }}}

    // {{{ put_response_helper

    /**
     * PUT response helper - format PUT response
     *
     * @param options
     * @param status
     * @return void
     */
    function put_response_helper($options, $status)
    {
        if (empty($status)) {
            $status = '403 Forbidden';
        } else if (is_resource($status)
                && get_resource_type($status) == 'stream') {
            $stream = $status;
            $status = isset($options['new']) && $options['new'] === false ? '204 No Content' : '201 Created';

            if (!empty($options['ranges'])) {

                // TODO multipart support is missing (see also above)
                if (0 == fseek($stream, $range[0]['start'], SEEK_SET)) {
                    $length = $range[0]['end'] - $range[0]['start'] + 1;
                    if (!fwrite($stream, fread($options['stream'], $length))) {
                        $status = '403 Forbidden';
                    }
                } else {
                    $status = '403 Forbidden';
                }
            } else {
                while (!feof($options['stream'])) {
                    $buf = fread($options['stream'], 4096);
                    if (fwrite($stream, $buf) != 4096) {
                        break;
                    }
                }
            }

            fclose($stream);
        }

        $this->setResponseStatus($status);
    }

    // }}}

    // {{{ put_wrapper

    /**
     * PUT method wrapper
     *
     * @param void
     * @return void
     */
    function put_wrapper()
    {
        // check resource is not locked
        if (!$this->check_locks_wrapper($this->path)) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        // perpare data-structure from PUT request
        if (!$this->put_request_helper($options)) {
            return;
        }

        // call user handler
        $status = $this->put($options);

        // format PUT response
        $this->put_response_helper($options, $status);
    }

    // }}}

    // {{{ delete_wrapper

    /**
     * DELETE method wrapper
     *
     * @param void
     * @return void
     */
    function delete_wrapper()
    {
        // RFC2518 9.2 last paragraph
        if (isset($_SERVER['HTTP_DEPTH'])
                && $_SERVER['HTTP_DEPTH'] !== 'infinity') {
            $this->setResponseStatus('400 Bad Request');
            return;
        }

        // check resource is not locked
        if (!$this->check_locks_wrapper($this->path)) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        $options = array();
        $options['path'] = $this->path;

        // call user handler
        $status = $this->delete($options);
        if ($status === true) {
            $status = '204 No Content';
        }

        $this->setResponseStatus($status);
    }

    // }}}

    // {{{ copymove_request_helper

    /**
     * COPY/MOVE request helper - prepares data-structures from COPY/MOVE
     * requests
     *
     * @param options
     * @return void
     */
    function copymove_request_helper(&$options)
    {
        $options = array();
        $options['path'] = $this->path;

        $options['depth'] = 'infinity';
        if ($this->_hasNonEmptyDepthRequestHeader()) {
            $options['depth'] = $_SERVER['HTTP_DEPTH'];
        }

        // RFC2518 9.6, 8.8.4 and 8.9.3
        $options['overwrite'] = true;
        if (!empty($_SERVER['HTTP_OVERWRITE'])) {
            $options['overwrite'] = $_SERVER['HTTP_OVERWRITE'] == 'T';
        }

        $url = parse_url($_SERVER['HTTP_DESTINATION']);

        // does the destination resource belong on this server?
        if ($url['host'] == $this->baseUrl['host']
                && (empty($url['port']) ? 80 : $url['port']) == (empty($this->baseUrl['port']) ? 80 : $this->baseUrl['port'])
                && !strncmp($url['path'], $this->baseUrl['path'], strlen($this->baseUrl['path']))) {
            if (!empty($this->baseurl['query'])) {
                foreach (explode('&', $this->baseUrl['query']) as $queryComponent) {
                    if (!in_array($queryComponent, explode('&', $url['query']))) {
                        $options['dest_url'] = $_SERVER['HTTP_DESTINATION'];

                        return true;
                    }
                }
            }

            $options['dest'] =
                substr($url['path'], strlen($this->baseUrl['path']));

            $options['dest'] = $this->_urldecode($options['dest']);
            $options['dest'] = trim($options['dest'], '/');

            // check source and destination are not the same - data could be lost
            // if overwrite is true - RFC2518 8.8.5
            if ($options['dest'] == $this->path) {
                $this->setResponseStatus('403 Forbidden');
                return;
            }

            return true;
        }

        $options['dest_url'] = $_SERVER['HTTP_DESTINATION'];

        return true;
    }

    // }}}

    // {{{ copy_wrapper

    /**
     * COPY method wrapper
     *
     * @param void
     * @return void
     */
    function copy_wrapper()
    {
        // no need to check source is not locked

        // perpare data-structure from COPY request
        if (!$this->copymove_request_helper($options)) {
            return;
        }

        // check destination is not locked
        if (!empty($options['dest']) &&
                !$this->check_locks_wrapper($options['dest'])) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        // call user handler
        $status = $this->copy($options);
        if ($status === true) {
            $status = $options['new'] === false ? '204 No Content' :
                '201 Created';
        }

        $this->setResponseStatus($status);
    }

    // }}}

    // {{{ move_wrapper

    /**
     * MOVE method wrapper
     *
     * @param void
     * @return void
     */
    function move_wrapper()
    {
        // check resource is not locked
        if (!$this->check_locks_wrapper($this->path)) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        // perpare data-structure from MOVE request
        if (!$this->copymove_request_helper($options)) {
            return;
        }

        // check destination is not locked
        if (!empty($options['dest']) &&
                !$this->check_locks_wrapper($options['dest'])) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        // call user handler
        $status = $this->move($options);
        if ($status === true) {
            $status = $options['new'] === false ? '204 No Content' :
                '201 Created';
        }

        $this->setResponseStatus($status);
    }

    // }}}

    // {{{ lock_request_helper

    /**
     * LOCK request helper - prepares data-structures from LOCK requests
     *
     * @param options
     * @return void
     */
    function lock_request_helper(&$options)
    {
        $options = array();
        $options['path'] = $this->path;

        // a LOCK request with an If header but without a body is used to
        // refresh a lock.  Content-Lenght may be unset or zero.
        if (empty($_SERVER['CONTENT_LENGTH']) && !empty($_SERVER['HTTP_IF'])) {

            // FIXME: Refresh multiple locks?
            $options['update'] = substr($_SERVER['HTTP_IF'], 2, -2);

            return true;
        }

        $options['depth'] = 'infinity';
        if ($this->_hasNonEmptyDepthRequestHeader()) {
            $options['depth'] = $_SERVER['HTTP_DEPTH'];
        }

        if (!empty($_SERVER['HTTP_TIMEOUT'])) {
            $options['timeout'] = explode(',', $_SERVER['HTTP_TIMEOUT']);
        }

        // extract lock request information from request XML payload
        $lockinfo = new _parse_lockinfo($this->openRequestBody());
        if (!$lockinfo->success) {
            $this->setResponseStatus('400 Bad Request');
            return;
        }

        // new lock
        $options['scope'] = $lockinfo->lockscope;
        $options['type']  = $lockinfo->locktype;
        $options['owner'] = $lockinfo->owner;

        $options['token'] = $this->_new_locktoken();

        return true;
    }

    // }}}

    // {{{ lock_response_helper

    /**
     * LOCK response helper - format LOCK response
     *
     * @param options
     * @param status
     * @return void
     */
    function lock_response_helper($options, $status)
    {
        if (!empty($options['locks']) && is_array($options['locks'])) {
            $this->setResponseStatus('409 Conflict');

            $responses = array();
            foreach ($options['locks'] as $lock) {
                $response = array();

                if (empty($lock['href'])) {
                    $response['href'] = $this->getHref($lock['path']);
                } else {
                    $response['href'] = $lock['href'];
                }

                $response['status'] = '423 Locked';

                $responses[] = $response;
            }

            $this->_multistatusResponseHelper($responses);

            return;
        }

        if ($status === true) {
            $status = '200 OK';
        } else if ($status === false) {
            $status = '423 Locked';
        }

        // set headers before we start printing
        $this->setResponseStatus($status);

        if ($status{0} == 2) { // 2xx states are ok
            $this->setResponseHeader('Content-Type: text/xml; charset="utf-8"');

            // RFC2518 8.10.1: In order to indicate the lock token associated
            // with a newly created lock, a Lock-Token response header MUST be
            // included in the response for every successful LOCK request for a
            // new lock.  Note that the Lock-Token header would not be returned
            // in the response for a successful refresh LOCK request because a
            // new lock was not created.
            if (empty($options['update']) || !empty($options['token'])) {
                $this->setResponseHeader("Lock-Token: <$options[token]>");
            }

            $lock = array();
            foreach (array('scope', 'type', 'depth', 'owner') as $key) {
                $lock[$key] = $options[$key];
            }

            if (!empty($options['expires'])) {
                $lock['expires'] = $options['expires'];
            } else {
                $lock['timeout'] = $options['timeout'];
            }

            if (!empty($options['update'])) {
                $lock['token'] = $options['update'];
            } else {
                $lock['token'] = $options['token'];
            }

            echo "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
            echo "<D:prop xmlns:D=\"DAV:\">\n";
            echo "  <D:lockdiscovery>\n";
            echo '    ' . $this->_activelocksResponseHelper(array($lock))
                . "\n";
            echo "  </D:lockdiscovery>\n";
            echo "</D:prop>\n";
        }
    }

    // }}}

    // {{{ lock_wrapper

    /**
     * LOCK method wrapper
     *
     * @param void
     * @return void
     */
    function lock_wrapper()
    {
        // perpare data-structure from LOCK request
        if (!$this->lock_request_helper($options)) {
            return;
        }

        // check resource is not locked
        if (!empty($options['update'])
                && !$this->check_locks_wrapper(
                    $this->path, $options['scope'] == 'shared')) {
            $this->setResponseStatus('423 Locked');
            return;
        }

        $options['locks'] = $this->getDescendentsLocks($this->path);
        if (empty($options['locks'])) {

            // call user handler
            $status = $this->lock($options);
        }

        // format LOCK response
        $this->lock_response_helper($options, $status);
    }

    // }}}

    // {{{ unlock_request_helper

    /**
     * UNLOCK request helper - prepares data-structures from UNLOCK requests
     *
     * @param options
     * @return void
     */
    function unlock_request_helper(&$options)
    {
        $options = array();
        $options['path'] = $this->path;

        if (empty($_SERVER['HTTP_LOCK_TOKEN'])) {
            return;
        }

        // strip surrounding <>
        $options['token'] = substr(trim($_SERVER['HTTP_LOCK_TOKEN']), 1, -1);

        return true;
    }

    // }}}

    // {{{ unlock_wrapper

    /**
     * UNLOCK method wrapper
     *
     * @param void
     * @return void
     */
    function unlock_wrapper()
    {
        // perpare data-structure from DELETE request
        if (!$this->unlock_request_helper($options)) {
            return;
        }

        // call user handler
        $status = $this->unlock($options);

        // RFC2518 8.11.1
        if ($status === true) {
            $status = '204 No Content';
        }

        $this->setResponseStatus($status);
    }

    // }}}

    function _multistatusResponseHelper($responses)
    {
        // now we generate the response header...
        $this->setResponseStatus('207 Multi-Status', false);
        $this->setResponseHeader('Content-Type: text/xml; charset="utf-8"');

        // ...& payload
        echo "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
        echo "<D:multistatus xmlns:D=\"DAV:\">\n";

        foreach ($responses as $response) {

            // ignore empty or incomplete entries
            if (!is_array($response) || empty($response)) {
                continue;
            }

            $namespaces = '';
            if (!empty($response['namespaces'])) {
                foreach ($response['namespaces'] as $name => $prefix) {
                    $namespaces .= " xmlns:$prefix=\"$name\"";
                }
            }
            echo "  <D:response$namespaces>\n";
            echo "    <D:href>$response[href]</D:href>\n";

            // report all found properties and their values (if any)
            // nothing to do if no properties were returend for a file
            if (!empty($response['propstat']) &&
                    is_array($response['propstat'])) {

                foreach ($response['propstat'] as $status => $props) {
                    echo "    <D:propstat>\n";
                    echo "      <D:prop>\n";

                    foreach ($props as $prop) {
                        if (!is_array($prop) || empty($prop['name'])) {
                            continue;
                        }

                        // empty properties (cannot use empty for check as '0'
                        // is a legal value here)
                        if (empty($prop['value']) && (!isset($prop['value'])
                                || $prop['value'] !== 0)) {
                            if ($prop['ns'] == 'DAV:') {
                                echo "        <D:$prop[name]/>\n";
                                continue;
                            }

                            if (!empty($prop['ns'])) {
                                echo '        <' . $response['namespaces'][$prop['ns']] . ":$prop[name]/>\n";
                                continue;
                            }

                            echo "        <$prop[name] xmlns=\"\"/>";
                            continue;
                        }

                        // some WebDAV properties need special treatment
                        if ($prop['ns'] == 'DAV:') {

                            switch ($prop['name']) {
                            case 'creationdate':
                                echo '        <D:creationdate ' . $response['namespaces'][HTTP_WEBDAV_SERVER_DATATYPE_NAMESPACE] . ':dt="dateTime.tz">' . gmdate('Y-m-d\TH:i:s\Z', $prop['value']) . "</D:creationdate>\n";
                                break;

                            case 'getlastmodified':
                                echo '        <D:getlastmodified ' . $response['namespaces'][HTTP_WEBDAV_SERVER_DATATYPE_NAMESPACE] . ':dt="dateTime.rfc1123">' . gmdate('D, d M Y H:i:s', $prop['value']) . " UTC</D:getlastmodified>\n";
                                break;

                            case 'resourcetype':
                                echo "        <D:resourcetype><D:$prop[value]/></D:resourcetype>\n";
                                break;

                            case 'supportedlock':

                                if (is_array($prop['value'])) {
                                    $prop['value'] =
                                        $this->_lockentriesResponseHelper(
                                            $prop['value']);
                                }
                                echo "        <D:supportedlock>\n";
                                echo "          $prop[value]\n";
                                echo "        </D:supportedlock>\n";
                                break;

                            case 'lockdiscovery':

                                if (is_array($prop['value'])) {
                                    $prop['value'] =
                                        $this->_activelocksResponseHelper(
                                            $prop['value']);
                                }
                                echo "        <D:lockdiscovery>\n";
                                echo "          $prop[value]\n";
                                echo "        </D:lockdiscovery>\n";
                                break;

                            default:
                                echo "        <D:$prop[name]>" . $this->_prop_encode(htmlspecialchars($prop['value'])) . "</D:$prop[name]>\n";
                            }

                            continue;
                        }

                        if (!empty($prop['ns'])) {
                            echo '        <' . $response['namespaces'][$prop['ns']] . ":$prop[name]>" . $this->_prop_encode(htmlspecialchars($prop['value'])) . '</' . $response['namespaces'][$prop['ns']] . ":$prop[name]>\n";

                            continue;
                        }

                        echo "        <$prop[name] xmlns=\"\">" . $this->_prop_encode(htmlspecialchars($prop['value'])) . "</$prop[name]>\n";
                    }

                    echo "      </D:prop>\n";
                    echo "      <D:status>HTTP/1.1 $status</D:status>\n";
                    echo "    </D:propstat>\n";
                }
            }

            if (!empty($response['status'])) {
                echo "    <D:status>HTTP/1.1 $response[status]</D:status>\n";
            }

            if (!empty($response['responsedescription'])) {
                echo '    <D:responsedescription>' . $this->_prop_encode(htmlspecialchars($response['responsedescription'])) . "</D:responsedescription>\n";
            }

            echo "  </D:response>\n";
        }

        echo "</D:multistatus>\n";
    }

    function _activelocksResponseHelper($locks)
    {
        if (!is_array($locks) || empty($locks)) {
            return '';
        }

        foreach ($locks as $key => $lock) {
            if (!is_array($lock) || empty($lock)) {
                continue;
            }

            // check for 'timeout' or 'expires'
            $timeout = 'Infinite';
            if (!empty($lock['expires'])) {
                $timeout = 'Second-' . ($lock['expires'] - time());
            } else if (!empty($lock['timeout'])) {

                // more than a million is considered an absolute timestamp
                // less is more likely a relative value
                $timeout = "Second-$lock[timeout]";
                if ($lock['timeout'] > 1000000) {
                    $timeout = 'Second-' . ($lock['timeout'] - time());
                }
            }

            // genreate response block
            $locks[$key] = "<D:activelock>
            <D:lockscope><D:$lock[scope]/></D:lockscope>
            <D:locktype><D:$lock[type]/></D:locktype>
            <D:depth>" . ($lock['depth'] == 'infinity' ? 'Infinity' : $lock['depth']) . "</D:depth>
            <D:owner>$lock[owner]</D:owner>
            <D:timeout>$timeout</D:timeout>
            <D:locktoken><D:href>$lock[token]</D:href></D:locktoken>
          </D:activelock>";
        }

        return implode('', $locks);
    }

    function _lockentriesResponseHelper($locks)
    {
        if (!is_array($locks) || empty($locks)) {
            return '';
        }

        foreach ($locks as $key => $lock) {
            if (!is_array($lock) || empty($lock)) {
                continue;
            }

            $locks[$key] = "<D:lockentry>
            <D:lockscope><D:$lock[scope]/></D:lockscope>
            <D:locktype><D:$lock[type]/></D:locktype>
          </D:lockentry>";
        }

        return implode('', $locks);
    }

    function getHref($path)
    {
        return $this->baseUrl['path'] . '/' . $path;
    }

    function getProp($reqprop, $file, $options)
    {
        // check if property exists in response
        foreach ($file['props'] as $prop) {
            if ($reqprop['name'] == $prop['name']
                    && $reqprop['ns'] == $prop['ns']) {
                return $prop;
            }
        }

        if ($reqprop['name'] == 'lockdiscovery'
                && $reqprop['ns'] == 'DAV:'
                && method_exists($this, 'getLocks')) {
            return $this->mkprop('DAV:', 'lockdiscovery',
                $this->getLocks($file['path']));
        }

        // incase the requested property had a value, like calendar-data
        unset($reqprop['value']);
        $reqprop['status'] = '404 Not Found';

        return $reqprop;
    }

    function getDescendentsLocks($path)
    {
        $options = array();
        $options['path'] = $path;
        $options['depth'] = 'infinity';
        $options['props'] = array();
        $options['props'][] = $this->mkprop('DAV:', 'lockdiscovery', null);

        // call user handler
        if (!$this->propfind($options, $files)) {
            return;
        }

        return $files;
    }

    // {{{ _allow()

    /**
     * List implemented methods
     *
     * @param void
     * @return array something
     */
    function _allow()
    {
        // OPTIONS is always there
        $allow = array('OPTIONS');

        // all other methods need both a method_wrapper() and a method()
        // implementation
        // the base class defines only wrappers
        foreach(get_class_methods($this) as $method) {

            // strncmp breaks with negative len -
            // http://bugs.php.net/bug.php?id=36944
            //if (!strncmp('_wrapper', $method, -8)) {
            if (!strcmp(substr($method, -8), '_wrapper')) {
                $method = strtolower(substr($method, 0, -8));
                if (method_exists($this, $method) &&
                        ($method != 'lock' && $method != 'unlock' ||
                        method_exists($this, 'getLocks'))) {
                    $allow[] = $method;
                }
            }
        }

        // we can emulate a missing HEAD implemetation using GET
        if (in_array('GET', $allow)) {
            $allow[] = 'HEAD';
        }

        return $allow;
    }

    // }}}

    // {{{ mkprop

    /**
     * Helper for property element creation
     *
     * @param string XML namespace (optional)
     * @param string property name
     * @param string property value
     * @return array property array
     */
    function mkprop()
    {
        $args = func_get_args();

        $prop = array();
        $prop['ns'] = 'DAV:';
        if (count($args) > 2) {
            $prop['ns'] = array_shift($args);
        }

        $prop['name'] = array_shift($args);
        $prop['value'] = array_shift($args);
        $prop['status'] = array_shift($args);

        return $prop;
    }

    // }}}

    // {{{ check_auth_wrapper

    /**
     * Check authentication if implemented
     *
     * @param void
     * @return boolean true if authentication succeded or not necessary
     */
    function check_auth_wrapper()
    {
        if (method_exists($this, 'checkAuth')) {

            // PEAR style method name
            return $this->checkAuth(@$_SERVER['AUTH_TYPE'],
                @$_SERVER['PHP_AUTH_USER'],
                @$_SERVER['PHP_AUTH_PW']);
        }

        if (method_exists($this, 'check_auth')) {

            // old (pre 1.0) method name
            return $this->check_auth(@$_SERVER['AUTH_TYPE'],
                @$_SERVER['PHP_AUTH_USER'],
                @$_SERVER['PHP_AUTH_PW']);
        }

        // no method found -> no authentication required
        return true;
    }

    // }}}

    // {{{ UUID stuff

    /**
     * Generate Unique Universal IDentifier for lock token
     *
     * @param void
     * @return string a new UUID
     */
    function _new_uuid()
    {
        // use uuid extension from PECL if available
        if (function_exists('uuid_create')) {
            return uuid_create();
        }

        // fallback
        $uuid = md5(microtime() . getmypid()); // this should be random enough for now

        // set variant and version fields for 'true' random uuid
        $uuid{12} = '4';
        $n = 8 + (ord($uuid{16}) & 3);
        $hex = '0123456789abcdef';
        $uuid{16} = $hex{$n};

        // return formated uuid
        return substr($uuid,  0, 8)
            . '-' . substr($uuid,  8, 4)
            . '-' . substr($uuid, 12, 4)
            . '-' . substr($uuid, 16, 4)
            . '-' . substr($uuid, 20);
    }

    /**
     * Create a new opaque lock token as defined in RFC2518
     *
     * @param void
     * @return string new RFC2518 opaque lock token
     */
    function _new_locktoken()
    {
        return 'opaquelocktoken:' . $this->_new_uuid();
    }

    // }}}

    // {{{ WebDAV If: header parsing

    /**
     *
     *
     * @param string header string to parse
     * @param int current parsing position
     * @return array next token (type and value)
     */
    function _if_header_lexer($string, &$pos)
    {
        // skip whitespace
        while (ctype_space($string{$pos})) {
            ++$pos;
        }

        // already at end of string?
        if (strlen($string) <= $pos) {
            return;
        }

        // get next character
        $c = $string{$pos++};

        // now it depends on what we found
        switch ($c) {

            // URLs are enclosed in <...>
            case '<':
                $pos2 = strpos($string, '>', $pos);
                $uri = substr($string, $pos, $pos2 - $pos);
                $pos = $pos2 + 1;
                return array('URI', $uri);

            // ETags are enclosed in [...]
            case '[':
                $type = 'ETAG_STRONG';
                if ($string{$pos} == 'W') {
                    $type = 'ETAG_WEAK';
                    $pos += 2;
                }

                $pos2 = strpos($string, ']', $pos);
                $etag = substr($string, $pos + 1, $pos2 - $pos - 2);
                $pos = $pos2 + 1;
                return array($type, $etag);

            // 'N' indicates negation
            case 'N':
                $pos += 2;
                return array('NOT', 'Not');

            // anything else is passed verbatim char by char
            default:
                return array('CHAR', $c);
        }
    }

    /**
     * Parse If: header
     *
     * @param string header string
     * @return array URLs and their conditions
     */
    function _if_header_parser($str)
    {
        $pos = 0;
        $len = strlen($str);

        $uris = array();

        // parser loop
        while ($pos < $len) {

            // get next token
            $token = $this->_if_header_lexer($str, $pos);

            // check for URL
            $uri = '';
            if ($token[0] == 'URI') {
                $uri = $token[1]; // remember URL
                $token = $this->_if_header_lexer($str, $pos); // get next token
            }

            // sanity check
            if ($token[0] != 'CHAR' || $token[1] != '(') {
                return;
            }

            $list = array();
            $level = 1;
            while ($level) {
                $token = $this->_if_header_lexer($str, $pos);

                $not = '';
                if ($token[0] == 'NOT') {
                    $not = '!';
                    $token = $this->_if_header_lexer($str, $pos);
                }

                switch ($token[0]) {
                    case 'CHAR':
                        switch ($token[1]) {
                            case '(':
                                $level++;
                                break;

                            case ')':
                                $level--;
                                break;

                            default:
                                return;
                        }
                        break;

                    case 'URI':
                        $list[] = $not . "<$token[1]>";
                        break;

                    case 'ETAG_WEAK':
                        $list[] = $not . "[W/'$token[1]']";
                        break;

                    case 'ETAG_STRONG':
                        $list[] = $not . "['$token[1]']";
                        break;

                    default:
                        return;
                }
            }

            // RFC2518 9.4.1: The No-tag-list production describes a series of
            // state tokens and ETags.  If multiple No-tag-list productions are
            // used then one only needs to match the state of the resource for
            // the method to be allowed to continue.
            //
            // FIXME: Since only one list of conditions must be satisfied, it
            // is a mistake to merge all lists of conditions.  Instead, a URL
            // should reference an array of arrays of conditions, the inner
            // array representing a conjunction and the outer array
            // representing a disjunction, or $uris shouldn't be associative,
            // but be an array of productions array('href' => $href,
            // 'conditions' => array($condition, ...))
            if (!empty($uris[$uri])) {
                $uris[$uri] = array_merge($uris[$uri], $list);
                continue;
            }
            $uris[$uri] = $list;
        }

        return $uris;
    }

    /**
     * Check if conditions from If: headers are met
     *
     * The If: header is an extension to HTTP/1.1 defined in RFC2518 9.4
     *
     * @param void
     * @return boolean
     */
    function _check_if_header_conditions()
    {
        if (empty($_SERVER['HTTP_IF'])) {
            return true;
        }

        $this->_if_header_uris =
            $this->_if_header_parser($_SERVER['HTTP_IF']);

        // any match is ok
        foreach ($this->_if_header_uris as $uri => $conditions) {

            // RFC2518 9.4.1: If a method, due to the presence of a Depth or
            // Destination header, is applied to multiple resources then the
            // No-tag-list production MUST be applied to each resource the
            // method is applied to.
            if (empty($uri)) {
                $uri = $this->getHref($this->path);
            }

            // all must match
            foreach ($conditions as $condition) {

                // lock tokens may be free form (RFC2518 6.3)
                // but if opaquelocktokens are used (RFC2518 6.4)
                // we have to check the format (litmus tests this)
                if (!strncmp($condition, '<opaquelocktoken:', strlen('<opaquelocktoken'))) {
                    if (!ereg('^<opaquelocktoken:[[:xdigit:]]{8}-[[:xdigit:]]{4}-[[:xdigit:]]{4}-[[:xdigit:]]{4}-[[:xdigit:]]{12}>$', $condition)) {
                        return;
                    }
                }

                if (!$this->_check_uri_condition($uri, $condition)) {
                    continue 2;
                }
            }

            return true;
        }
    }

    /**
     * Check a single URL condition parsed from an if-header
     *
     * @abstract
     * @param string URL to check
     * @param string condition to check for this URL
     * @return boolean condition check result
     */
    function _check_uri_condition($uri, $condition)
    {
        // not really implemented here,
        // implementations must override
        return true;
    }

    /**
     * For each lock on the requested resource, check that the lock token is in
     * the If header.  If requesting a shared lock, check only exclusive locks
     * on the requested resource.
     *
     * @param array of locks
     * @param string path of resource to check
     * @param boolean exclusive lock?
     * @return boolean true if the request is allowed
     */
    function check_locks_helper($locks, $path, $exclusive_only=false)
    {
        $conditions = array();
        if (!empty($_SERVER['HTTP_IF'])) {
            $conditions = $this->_if_header_parser($_SERVER['HTTP_IF']);
        }

        foreach ($locks as $lock) {
            if ($exclusive_only && ($lock['scope'] == 'shared')) {
                continue;
            }

            // for both Tagged-list and No-tag-list productions
            foreach (array($this->getHref($path), '') as $href) {
                if (!empty($conditions[$href])
                        && in_array("<$lock[token]>", $conditions[$href])) {
                    continue 2;
                }
            }

            return false;
        }

        return true;
    }

    /**
     * @param string path of resource to check
     * @param boolean exclusive lock?
     */
    function check_locks_wrapper($path, $exclusive_only=false)
    {
        if (!method_exists($this, 'getLocks')) {
            return true;
        }

        return $this->check_locks_helper(
            $this->getLocks($path), $path, $exclusive_only);
    }

    // }}}

    /**
     * Open request body input stream
     *
     * Because it's not possible to write to php://input (unlike the potential
     * to set request variables) and not possible until PHP 5.1 to register
     * alternative stream wrappers with php:// URLs, this function enables
     * sub-classes to override the request body.  Gallery uses this for unit
     * testing.  This function also collects all instances of opening the
     * request body in one place.
     *
     * @return resource a file descriptor
     */
    function openRequestBody()
    {
        return fopen('php://input', 'rb');
    }

    /**
     * Set HTTP response header
     *
     * This function enables sub-classes to override header-setting.  Gallery
     * uses this to avoid replacing headers elsewhere in the application, and
     * for testability.
     *
     * @param string status code and message
     * @return void
     */
    function setResponseHeader($header, $replace=true)
    {
        $key = 'status';
        if (strncasecmp($header, 'HTTP/', 5) !== 0) {
            $key = strtolower(substr($header, 0, strpos($header, ':')));
        }

        if ($replace || empty($this->headers[$key])) {
            header($header);
            $this->headers[$key] = $header;
        }
    }

    /**
     * Set HTTP response status and mirror it in a private header
     *
     * @param string status code and message
     * @return void
     */
    function setResponseStatus($status, $replace=true)
    {
        // simplified success case
        if ($status === true) {
            $status = '200 OK';
        }

        // didn't set a more specific status code
        if (empty($status)) {
            $status = '500 Internal Server Error';
        }

        // generate HTTP status response
        $this->setResponseHeader("HTTP/1.1 $status", $replace);
        $this->setResponseHeader("X-WebDAV-Status: $status", $replace);
    }

    /**
     * Private minimalistic version of PHP urlencode
     *
     * Only blanks and XML special chars must be encoded here.  Full urlencode
     * encoding confuses some clients.
     *
     * @param string URL to encode
     * @return string encoded URL
     */
    function _urlencode($url)
    {
        return strtr($url, array(' ' => '%20',
                '&' => '%26',
                '<' => '%3C',
                '>' => '%3E'));
    }

    /**
     * Private version of PHP urldecode
     *
     * Not really needed but added for completenes.
     *
     * @param string URL to decode
     * @return string decoded URL
     */
    function _urldecode($path)
    {
        return urldecode($path);
    }

    /**
     * UTF-8 encode property values if not already done so
     *
     * @param string text to encode
     * @return string UTF-8 encoded text
     */
    function _prop_encode($text)
    {
        switch (strtolower($this->_prop_encoding)) {
        case 'utf-8':
            return $text;
        case 'iso-8859-1':
        case 'iso-8859-15':
        case 'latin-1':
        default:
            return utf8_encode($text);
        }
    }

    /**
     * Make sure path ends in a slash
     *
     * @param string directory path
     * @return string directory path with trailing slash
     */
    function _slashify($path)
    {
        if (substr($path, -1) != '/') {
            $path .= '/';
        }

        return $path;
    }

    /**
     * Verify that the Depth request header is set and has a non-empty value.
     * Doesn't verify for allowed values though.
     * @return boolean true if set and not empty
     */
    function _hasNonEmptyDepthRequestHeader() {
        return isset($_SERVER['HTTP_DEPTH']) && strlen(trim((string)$_SERVER['HTTP_DEPTH']));
    }
}

// Local variables:
// tab-width: 4
// c-basic-offset: 4
// End:
?>
