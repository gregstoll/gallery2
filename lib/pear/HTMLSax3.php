<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to either version 3.0 of the PHP license |
// | or version 3.0 of the GNU Lesser General Public License.             |
// | The PHP license 3.0 is bundled with this package in the file LICENSE,|
// | and is available at  through the world-wide-web at                   |
// | http://www.php.net/license/3_0.txt. If you did not receive a copy of |
// | the PHP license and are unable to obtain it through the              |
// | world-wide-web, please send a note to license@php.net so we can mail |
// | you a copy immediately.                                              |
// | You should have received a copy of the GNU Lesser General Public     |
// | License along with this library.  If not, see                        |
// | <http://www.gnu.org/licenses/>.                                      |
// +----------------------------------------------------------------------+
// | Authors: Alexander Zhukov <alex@veresk.ru> Original port from Python |
// | Authors: Harry Fuecks <hfuecks@phppatterns.com> Port to PEAR + more  |
// | Authors: Many @ Sitepointforums Advanced PHP Forums                  |
// +----------------------------------------------------------------------+
//
// PEAR  Id: HTMLSax3.php,v 1.2 2007/10/29 21:41:34 hfuecks
//   G2 $Id: HTMLSax3.php 20960 2009-12-16 06:54:53Z mindless $
//
/**
* Main parser components
* @package XML_HTMLSax3
* @version  Id: HTMLSax3.php,v 1.2 2007/10/29 21:41:34 hfuecks
*/
/**
* Required classes
*/
//if (!defined('XML_HTMLSAX3')) {
//    define('XML_HTMLSAX3', 'XML/');
//}
//require_once(XML_HTMLSAX3 . 'HTMLSax3/States.php');
//require_once(XML_HTMLSAX3 . 'HTMLSax3/Decorators.php');

/**
* Base State Parser
* @package XML_HTMLSax3
* @access protected
* @abstract
*/
class XML_HTMLSax3_StateParser {
    /**
    * Instance of user front end class to be passed to callbacks
    * @var XML_HTMLSax3
    * @access private
    */
    var $htmlsax;
    /**
    * User defined object for handling elements
    * @var object
    * @access private
    */
    var $handler_object_element;
    /**
    * User defined open tag handler method
    * @var string
    * @access private
    */
    var $handler_method_opening;
    /**
    * User defined close tag handler method
    * @var string
    * @access private
    */
    var $handler_method_closing;
    /**
    * User defined object for handling data in elements
    * @var object
    * @access private
    */
    var $handler_object_data;
    /**
    * User defined data handler method
    * @var string
    * @access private
    */
    var $handler_method_data;
    /**
    * User defined object for handling processing instructions
    * @var object
    * @access private
    */
    var $handler_object_pi;
    /**
    * User defined processing instruction handler method
    * @var string
    * @access private
    */
    var $handler_method_pi;
    /**
    * User defined object for handling JSP/ASP tags
    * @var object
    * @access private
    */
    var $handler_object_jasp;
    /**
    * User defined JSP/ASP handler method
    * @var string
    * @access private
    */
    var $handler_method_jasp;
    /**
    * User defined object for handling XML escapes
    * @var object
    * @access private
    */
    var $handler_object_escape;
    /**
    * User defined XML escape handler method
    * @var string
    * @access private
    */
    var $handler_method_escape;
    /**
    * User defined handler object or NullHandler
    * @var object
    * @access private
    */
    var $handler_default;
    /**
    * Parser options determining parsing behavior
    * @var array
    * @access private
    */
    var $parser_options = array();
    /**
    * XML document being parsed
    * @var string
    * @access private
    */
    var $rawtext;
    /**
    * Position in XML document relative to start (0)
    * @var int
    * @access private
    */
    var $position;
    /**
    * Length of the XML document in characters
    * @var int
    * @access private
    */
    var $length;
    /**
    * Array of state objects
    * @var array
    * @access private
    */
    var $State = array();

    /**
    * Constructs XML_HTMLSax3_StateParser setting up states
    * @var XML_HTMLSax3 instance of user front end class
    * @access protected
    */
    function XML_HTMLSax3_StateParser (& $htmlsax) {
        $this->htmlsax = & $htmlsax;
        $this->State[XML_HTMLSAX3_STATE_START] = new XML_HTMLSax3_StartingState();

        $this->State[XML_HTMLSAX3_STATE_CLOSING_TAG] = new XML_HTMLSax3_ClosingTagState();
        $this->State[XML_HTMLSAX3_STATE_TAG] = new XML_HTMLSax3_TagState();
        $this->State[XML_HTMLSAX3_STATE_OPENING_TAG] = new XML_HTMLSax3_OpeningTagState();

        $this->State[XML_HTMLSAX3_STATE_PI] = new XML_HTMLSax3_PiState();
        $this->State[XML_HTMLSAX3_STATE_JASP] = new XML_HTMLSax3_JaspState();
        $this->State[XML_HTMLSAX3_STATE_ESCAPE] = new XML_HTMLSax3_EscapeState();
    }

    /**
    * Moves the position back one character
    * @access protected
    * @return void
    */
    function unscanCharacter() {
        $this->position -= 1;
    }

    /**
    * Moves the position forward one character
    * @access protected
    * @return void
    */
    function ignoreCharacter() {
        $this->position += 1;
    }

    /**
    * Returns the next character from the XML document or void if at end
    * @access protected
    * @return mixed
    */
    function scanCharacter() {
        if ($this->position < $this->length) {
            return $this->rawtext{$this->position++};
        }
    }

    /**
    * Returns a string from the current position to the next occurance
    * of the supplied string
    * @param string string to search until
    * @access protected
    * @return string
    */
    function scanUntilString($string) {
        $start = $this->position;
        $this->position = strpos($this->rawtext, $string, $start);
        if ($this->position === FALSE) {
            $this->position = $this->length;
        }
        return substr($this->rawtext, $start, $this->position - $start);
    }

    /**
    * Returns a string from the current position until the first instance of
    * one of the characters in the supplied string argument
    * @param string string to search until
    * @access protected
    * @return string
    * @abstract
    */
    function scanUntilCharacters($string) {}

    /**
    * Moves the position forward past any whitespace characters
    * @access protected
    * @return void
    * @abstract
    */
    function ignoreWhitespace() {}

    /**
    * Begins the parsing operation, setting up any decorators, depending on
    * parse options invoking _parse() to execute parsing
    * @param string XML document to parse
    * @access protected
    * @return void
    */
    function parse($data) {
        if ($this->parser_options['XML_OPTION_TRIM_DATA_NODES']==1) {
            $decorator = new XML_HTMLSax3_Trim(
                $this->handler_object_data,
                $this->handler_method_data);
            $this->handler_object_data =& $decorator;
            $this->handler_method_data = 'trimData';
        }
        if ($this->parser_options['XML_OPTION_CASE_FOLDING']==1) {
            $open_decor = new XML_HTMLSax3_CaseFolding(
                $this->handler_object_element,
                $this->handler_method_opening,
                $this->handler_method_closing);
            $this->handler_object_element =& $open_decor;
            $this->handler_method_opening ='foldOpen';
            $this->handler_method_closing ='foldClose';
        }
        if ($this->parser_options['XML_OPTION_LINEFEED_BREAK']==1) {
            $decorator = new XML_HTMLSax3_Linefeed(
                $this->handler_object_data,
                $this->handler_method_data);
            $this->handler_object_data =& $decorator;
            $this->handler_method_data = 'breakData';
        }
        if ($this->parser_options['XML_OPTION_TAB_BREAK']==1) {
            $decorator = new XML_HTMLSax3_Tab(
                $this->handler_object_data,
                $this->handler_method_data);
            $this->handler_object_data =& $decorator;
            $this->handler_method_data = 'breakData';
        }
        if ($this->parser_options['XML_OPTION_ENTITIES_UNPARSED']==1) {
            $decorator = new XML_HTMLSax3_Entities_Unparsed(
                $this->handler_object_data,
                $this->handler_method_data);
            $this->handler_object_data =& $decorator;
            $this->handler_method_data = 'breakData';
        }
        if ($this->parser_options['XML_OPTION_ENTITIES_PARSED']==1) {
            $decorator = new XML_HTMLSax3_Entities_Parsed(
                $this->handler_object_data,
                $this->handler_method_data);
            $this->handler_object_data =& $decorator;
            $this->handler_method_data = 'breakData';
        }
        // Note switched on by default
        if ($this->parser_options['XML_OPTION_STRIP_ESCAPES']==1) {
            $decorator = new XML_HTMLSax3_Escape_Stripper(
                $this->handler_object_escape,
                $this->handler_method_escape);
            $this->handler_object_escape =& $decorator;
            $this->handler_method_escape = 'strip';
        }
        $this->rawtext = $data;
        $this->length = strlen($data);
        $this->position = 0;
        $this->_parse();
    }

    /**
    * Performs the parsing itself, delegating calls to a specific parser
    * state
    * @param constant state object to parse with
    * @access protected
    * @return void
    */
    function _parse($state = XML_HTMLSAX3_STATE_START) {
        do {
            $state = $this->State[$state]->parse($this);
        } while ($state != XML_HTMLSAX3_STATE_STOP &&
                    $this->position < $this->length);
    }
}

/**
* Parser for PHP Versions below 4.3.0. Uses a slower parsing mechanism than
* the equivalent PHP 4.3.0+  subclass of StateParser
* @package XML_HTMLSax3
* @access protected
* @see XML_HTMLSax3_StateParser_Gtet430
*/
class XML_HTMLSax3_StateParser_Lt430 extends XML_HTMLSax3_StateParser {
    /**
    * Constructs XML_HTMLSax3_StateParser_Lt430 defining available
    * parser options
    * @var XML_HTMLSax3 instance of user front end class
    * @access protected
    */
    function XML_HTMLSax3_StateParser_Lt430(& $htmlsax) {
        parent::XML_HTMLSax3_StateParser($htmlsax);
        $this->parser_options['XML_OPTION_TRIM_DATA_NODES'] = 0;
        $this->parser_options['XML_OPTION_CASE_FOLDING'] = 0;
        $this->parser_options['XML_OPTION_LINEFEED_BREAK'] = 0;
        $this->parser_options['XML_OPTION_TAB_BREAK'] = 0;
        $this->parser_options['XML_OPTION_ENTITIES_PARSED'] = 0;
        $this->parser_options['XML_OPTION_ENTITIES_UNPARSED'] = 0;
        $this->parser_options['XML_OPTION_STRIP_ESCAPES'] = 0;
    }

    /**
    * Returns a string from the current position until the first instance of
    * one of the characters in the supplied string argument
    * @param string string to search until
    * @access protected
    * @return string
    */
    function scanUntilCharacters($string) {
        $startpos = $this->position;
        while ($this->position < $this->length && strpos($string, $this->rawtext{$this->position}) === FALSE) {
            $this->position++;
        }
        return substr($this->rawtext, $startpos, $this->position - $startpos);
    }

    /**
    * Moves the position forward past any whitespace characters
    * @access protected
    * @return void
    */
    function ignoreWhitespace() {
        while ($this->position < $this->length && 
            strpos(" \n\r\t", $this->rawtext{$this->position}) !== FALSE) {
            $this->position++;
        }
    }

    /**
    * Begins the parsing operation, setting up the unparsed XML entities
    * decorator if necessary then delegating further work to parent
    * @param string XML document to parse
    * @access protected
    * @return void
    */
    function parse($data) {
        parent::parse($data);
    }
}

/**
* Parser for PHP Versions equal to or greater than 4.3.0. Uses a faster
* parsing mechanism than the equivalent PHP < 4.3.0 subclass of StateParser
* @package XML_HTMLSax3
* @access protected
* @see XML_HTMLSax3_StateParser_Lt430
*/
class XML_HTMLSax3_StateParser_Gtet430 extends XML_HTMLSax3_StateParser {
    /**
    * Constructs XML_HTMLSax3_StateParser_Gtet430 defining available
    * parser options
    * @var XML_HTMLSax3 instance of user front end class
    * @access protected
    */
    function XML_HTMLSax3_StateParser_Gtet430(& $htmlsax) {
        parent::XML_HTMLSax3_StateParser($htmlsax);
        $this->parser_options['XML_OPTION_TRIM_DATA_NODES'] = 0;
        $this->parser_options['XML_OPTION_CASE_FOLDING'] = 0;
        $this->parser_options['XML_OPTION_LINEFEED_BREAK'] = 0;
        $this->parser_options['XML_OPTION_TAB_BREAK'] = 0;
        $this->parser_options['XML_OPTION_ENTITIES_PARSED'] = 0;
        $this->parser_options['XML_OPTION_ENTITIES_UNPARSED'] = 0;
        $this->parser_options['XML_OPTION_STRIP_ESCAPES'] = 0;
    }
    /**
    * Returns a string from the current position until the first instance of
    * one of the characters in the supplied string argument.
    * @param string string to search until
    * @access protected
    * @return string
    */
    function scanUntilCharacters($string) {
        $startpos = $this->position;
        $length = strcspn($this->rawtext, $string, $startpos);
        $this->position += $length;
        return substr($this->rawtext, $startpos, $length);
    }

    /**
    * Moves the position forward past any whitespace characters
    * @access protected
    * @return void
    */
    function ignoreWhitespace() {
        $this->position += strspn($this->rawtext, " \n\r\t", $this->position);
    }

    /**
    * Begins the parsing operation, setting up the parsed and unparsed
    * XML entity decorators if necessary then delegating further work
    * to parent
    * @param string XML document to parse
    * @access protected
    * @return void
    */
    function parse($data) {
        parent::parse($data);
    }
}

/**
* Default NullHandler for methods which were not set by user
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_NullHandler {
    /**
    * Generic handler method which does nothing
    * @access protected
    * @return void
    */
    function DoNothing() {
    }
}

/**
* User interface class. All user calls should only be made to this class
* @package XML_HTMLSax3
* @access public
*/
class XML_HTMLSax3 {
    /**
    * Instance of concrete subclass of XML_HTMLSax3_StateParser
    * @var XML_HTMLSax3_StateParser
    * @access private
    */
    var $state_parser;

    /**
    * Constructs XML_HTMLSax3 selecting concrete StateParser subclass
    * depending on PHP version being used as well as setting the default
    * NullHandler for all callbacks<br />
    * <b>Example:</b>
    * <pre>
    * $myHandler = & new MyHandler();
    * $parser = new XML_HTMLSax3();
    * $parser->set_object($myHandler);
    * $parser->set_option('XML_OPTION_CASE_FOLDING');
    * $parser->set_element_handler('myOpenHandler','myCloseHandler');
    * $parser->set_data_handler('myDataHandler');
    * $parser->parser($xml);
    * </pre>
    * @access public
    */
    function XML_HTMLSax3() {
        if (version_compare(phpversion(), '4.3', 'ge')) {
            $this->state_parser = new XML_HTMLSax3_StateParser_Gtet430($this);
        } else {
            $this->state_parser = new XML_HTMLSax3_StateParser_Lt430($this);
        }
        $nullhandler = new XML_HTMLSax3_NullHandler();
        $this->set_object($nullhandler);
        $this->set_element_handler('DoNothing', 'DoNothing');
        $this->set_data_handler('DoNothing');
        $this->set_pi_handler('DoNothing');
        $this->set_jasp_handler('DoNothing');
        $this->set_escape_handler('DoNothing');
    }

    /**
    * Sets the user defined handler object. Returns a PEAR Error
    * if supplied argument is not an object.
    * @param object handler object containing SAX callback methods
    * @access public
    * @return mixed
    */
    function set_object(&$object) {
        if ( is_object($object) ) {
            $this->state_parser->handler_default =& $object;
            return true;
        } else {
            //require_once('PEAR.php');
            //PEAR::raiseError('XML_HTMLSax3::set_object requires '.
            //    'an object instance');
        }
    }

    /**
    * Sets a parser option. By default all options are switched off.
    * Returns a PEAR Error if option is invalid<br />
    * <b>Available options:</b>
    * <ul>
    * <li>XML_OPTION_TRIM_DATA_NODES: trim whitespace off the beginning
    * and end of data passed to the data handler</li>
    * <li>XML_OPTION_LINEFEED_BREAK: linefeeds result in additional data
    * handler calls</li>
    * <li>XML_OPTION_TAB_BREAK: tabs result in additional data handler
    * calls</li>
    * <li>XML_OPTION_ENTITIES_UNPARSED: XML entities are returned as
    * seperate data handler calls in unparsed form</li>
    * <li>XML_OPTION_ENTITIES_PARSED: (PHP 4.3.0+ only) XML entities are
    * returned as seperate data handler calls and are parsed with 
    * PHP's html_entity_decode() function</li>
    * <li>XML_OPTION_STRIP_ESCAPES: strips out the -- -- comment markers
    * or CDATA markup inside an XML escape, if found.</li>
    * </ul>
    * To get HTMLSax to behave in the same way as the native PHP SAX parser,
    * using it's default state, you need to switch on XML_OPTION_LINEFEED_BREAK,
    * XML_OPTION_ENTITIES_PARSED and XML_OPTION_CASE_FOLDING
    * @param string name of parser option
    * @param int (optional) 1 to switch on, 0 for off
    * @access public
    * @return boolean
    */
    function set_option($name, $value=1) {
        if ( array_key_exists($name,$this->state_parser->parser_options) ) {
            $this->state_parser->parser_options[$name] = $value;
            return true;
        } else {
            //require_once('PEAR.php');
            //PEAR::raiseError('XML_HTMLSax3::set_option('.$name.') illegal');
        }
    }

    /**
    * Sets the data handler method which deals with the contents of XML
    * elements.<br />
    * The handler method must accept two arguments, the first being an
    * instance of XML_HTMLSax3 and the second being the contents of an
    * XML element e.g.
    * <pre>
    * function myDataHander(& $parser,$data){}
    * </pre>
    * @param string name of method
    * @access public
    * @return void
    * @see set_object
    */
    function set_data_handler($data_method) {
        $this->state_parser->handler_object_data =& $this->state_parser->handler_default;
        $this->state_parser->handler_method_data = $data_method;
    }

    /**
    * Sets the open and close tag handlers
    * <br />The open handler method must accept three arguments; the parser,
    * the tag name and an array of attributes e.g.
    * <pre>
    * function myOpenHander(& $parser,$tagname,$attrs=array()){}
    * </pre>
    * The close handler method must accept two arguments; the parser and
    * the tag name e.g.
    * <pre>
    * function myCloseHander(& $parser,$tagname){}
    * </pre>
    * @param string name of open method
    * @param string name of close method
    * @access public
    * @return void
    * @see set_object
    */
    function set_element_handler($opening_method, $closing_method) {
        $this->state_parser->handler_object_element =& $this->state_parser->handler_default;
        $this->state_parser->handler_method_opening = $opening_method;
        $this->state_parser->handler_method_closing = $closing_method;
    }

    /**
    * Sets the processing instruction handler method e.g. for PHP open
    * and close tags<br />
    * The handler method must accept three arguments; the parser, the
    * PI target and data inside the PI
    * <pre>
    * function myPIHander(& $parser,$target, $data){}
    * </pre>
    * @param string name of method
    * @access public
    * @return void
    * @see set_object
    */
    function set_pi_handler($pi_method) {
        $this->state_parser->handler_object_pi =& $this->state_parser->handler_default;
        $this->state_parser->handler_method_pi = $pi_method;
    }

    /**
    * Sets the XML escape handler method e.g. for comments and doctype
    * declarations<br />
    * The handler method must accept two arguments; the parser and the
    * contents of the escaped section
    * <pre>
    * function myEscapeHander(& $parser, $data){}
    * </pre>
    * @param string name of method
    * @access public
    * @return void
    * @see set_object
    */
    function set_escape_handler($escape_method) {
        $this->state_parser->handler_object_escape =& $this->state_parser->handler_default;
        $this->state_parser->handler_method_escape = $escape_method;
    }

    /**
    * Sets the JSP/ASP markup handler<br />
    * The handler method must accept two arguments; the parser and
    * body of the JASP tag
    * <pre>
    * function myJaspHander(& $parser, $data){}
    * </pre>
    * @param string name of method
    * @access public
    * @return void
    * @see set_object
    */
    function set_jasp_handler ($jasp_method) {
        $this->state_parser->handler_object_jasp =& $this->state_parser->handler_default;
        $this->state_parser->handler_method_jasp = $jasp_method;
    }

    /**
    * Returns the current string position of the "cursor" inside the XML
    * document
    * <br />Intended for use from within a user defined handler called
    * via the $parser reference e.g.
    * <pre>
    * function myDataHandler(& $parser,$data) {
    *     echo( 'Current position: '.$parser->get_current_position() );
    * }
    * </pre>
    * @access public
    * @return int
    * @see get_length
    */
    function get_current_position() {
        return $this->state_parser->position;
    }

    /**
    * Returns the string length of the XML document being parsed
    * @access public
    * @return int
    */
    function get_length() {
        return $this->state_parser->length;
    }

    /**
    * Start parsing some XML
    * @param string XML document
    * @access public
    * @return void
    */
    function parse($data) {
        $this->state_parser->parse($data);
    }
}
?>
<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to either version 3.0 of the PHP license |
// | or version 3.0 of the GNU Lesser General Public License.             |
// | The PHP license 3.0 is bundled with this package in the file LICENSE,|
// | and is available at  through the world-wide-web at                   |
// | http://www.php.net/license/3_0.txt. If you did not receive a copy of |
// | the PHP license and are unable to obtain it through the              |
// | world-wide-web, please send a note to license@php.net so we can mail |
// | you a copy immediately.                                              |
// | You should have received a copy of the GNU Lesser General Public     |
// | License along with this library.  If not, see                        |
// | <http://www.gnu.org/licenses/>.                                      |
// +----------------------------------------------------------------------+
// | Authors: Alexander Zhukov <alex@veresk.ru> Original port from Python |
// | Authors: Harry Fuecks <hfuecks@phppatterns.com> Port to PEAR + more  |
// | Authors: Many @ Sitepointforums Advanced PHP Forums                  |
// +----------------------------------------------------------------------+
//
// PEAR  Id: States.php,v 1.3 2007/10/29 21:41:35 hfuecks
//
/**
* Parsing states.
* @package XML_HTMLSax3
* @version  Id: States.php,v 1.3 2007/10/29 21:41:35 hfuecks
*/
/**
* Define parser states
*/
define('XML_HTMLSAX3_STATE_STOP', 0);
define('XML_HTMLSAX3_STATE_START', 1);
define('XML_HTMLSAX3_STATE_TAG', 2);
define('XML_HTMLSAX3_STATE_OPENING_TAG', 3);
define('XML_HTMLSAX3_STATE_CLOSING_TAG', 4);
define('XML_HTMLSAX3_STATE_ESCAPE', 6);
define('XML_HTMLSAX3_STATE_JASP', 7);
define('XML_HTMLSAX3_STATE_PI', 8);
/**
* StartingState searches for the start of any XML tag
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_StartingState  {
    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant XML_HTMLSAX3_STATE_TAG
    * @access protected
    */
    function parse(&$context) {
        $data = $context->scanUntilString('<');
        if ($data != '') {
            $context->handler_object_data->
                {$context->handler_method_data}($context->htmlsax, $data);
        }
        $context->IgnoreCharacter();
        return XML_HTMLSAX3_STATE_TAG;
    }
}
/**
* Decides which state to move one from after StartingState
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_TagState {
    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant the next state to move into
    * @access protected
    */
    function parse(&$context) {
        switch($context->ScanCharacter()) {
        case '/':
            return XML_HTMLSAX3_STATE_CLOSING_TAG;
            break;
        case '?':
            return XML_HTMLSAX3_STATE_PI;
            break;
        case '%':
            return XML_HTMLSAX3_STATE_JASP;
            break;
        case '!':
            return XML_HTMLSAX3_STATE_ESCAPE;
            break;
        default:
            $context->unscanCharacter();
            return XML_HTMLSAX3_STATE_OPENING_TAG;
        }
    }
}
/**
* Dealing with closing XML tags
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_ClosingTagState {
    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant XML_HTMLSAX3_STATE_START
    * @access protected
    */
    function parse(&$context) {
        $tag = $context->scanUntilCharacters('/>');
        if ($tag != '') {
            $char = $context->scanCharacter();
            if ($char == '/') {
                $char = $context->scanCharacter();
                if ($char != '>') {
                    $context->unscanCharacter();
                }
            }
            $context->handler_object_element->
                {$context->handler_method_closing}($context->htmlsax, $tag, FALSE);
        }
        return XML_HTMLSAX3_STATE_START;
    }
}
/**
* Dealing with opening XML tags
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_OpeningTagState {
    /**
    * Handles attributes
    * @param string attribute name
    * @param string attribute value
    * @return void
    * @access protected
    * @see XML_HTMLSax3_AttributeStartState
    */
    function parseAttributes(&$context) {
        $Attributes = array();
    
        $context->ignoreWhitespace();
        $attributename = $context->scanUntilCharacters("=/> \n\r\t");
        while ($attributename != '') {
            $attributevalue = NULL;
            $context->ignoreWhitespace();
            $char = $context->scanCharacter();
            if ($char == '=') {
                $context->ignoreWhitespace();
                $char = $context->ScanCharacter();
                if ($char == '"') {
                    $attributevalue= $context->scanUntilString('"');
                    $context->IgnoreCharacter();
                } else if ($char == "'") {
                    $attributevalue = $context->scanUntilString("'");
                    $context->IgnoreCharacter();
                } else {
                    $context->unscanCharacter();
                    $attributevalue =
                        $context->scanUntilCharacters("> \n\r\t");
                }
            } else if ($char !== NULL) {
                $attributevalue = NULL;
                $context->unscanCharacter();
            }
            $Attributes[$attributename] = $attributevalue;
            
            $context->ignoreWhitespace();
            $attributename = $context->scanUntilCharacters("=/> \n\r\t");
        }
        return $Attributes;
    }

    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant XML_HTMLSAX3_STATE_START
    * @access protected
    */
    function parse(&$context) {
        $tag = $context->scanUntilCharacters("/> \n\r\t");
        if ($tag != '') {
            $this->attrs = array();
            $Attributes = $this->parseAttributes($context);
            $char = $context->scanCharacter();
            if ($char == '/') {
                $char = $context->scanCharacter();
                if ($char != '>') {
                    $context->unscanCharacter();
                }
                $context->handler_object_element->
                    {$context->handler_method_opening}($context->htmlsax, $tag, 
                    $Attributes, TRUE);
                $context->handler_object_element->
                    {$context->handler_method_closing}($context->htmlsax, $tag, 
                    TRUE);
            } else {
                $context->handler_object_element->
                    {$context->handler_method_opening}($context->htmlsax, $tag, 
                    $Attributes, FALSE);
            }
        }
        return XML_HTMLSAX3_STATE_START;
    }
}

/**
* Deals with XML escapes handling comments and CDATA correctly
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_EscapeState {
    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant XML_HTMLSAX3_STATE_START
    * @access protected
    */
    function parse(&$context) {
        $char = $context->ScanCharacter();
        if ($char == '-') {
            $char = $context->ScanCharacter();
            if ($char == '-') {
                $context->unscanCharacter();
                $context->unscanCharacter();
                $text = $context->scanUntilString('-->');
                $text .= $context->scanCharacter();
                $text .= $context->scanCharacter();
            } else {
                $context->unscanCharacter();
                $text = $context->scanUntilString('>');
            }
        } else if ( $char == '[') {
            $context->unscanCharacter();
            $text = $context->scanUntilString(']>');
            $text.= $context->scanCharacter();
        } else {
            $context->unscanCharacter();
            $text = $context->scanUntilString('>');
        }

        $context->IgnoreCharacter();
        if ($text != '') {
            $context->handler_object_escape->
            {$context->handler_method_escape}($context->htmlsax, $text);
        }
        return XML_HTMLSAX3_STATE_START;
    }
}
/**
* Deals with JASP/ASP markup
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_JaspState {
    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant XML_HTMLSAX3_STATE_START
    * @access protected
    */
    function parse(&$context) {
        $text = $context->scanUntilString('%>');
        if ($text != '') {
            $context->handler_object_jasp->
                {$context->handler_method_jasp}($context->htmlsax, $text);
        }
        $context->IgnoreCharacter();
        $context->IgnoreCharacter();
        return XML_HTMLSAX3_STATE_START;
    }
}
/**
* Deals with XML processing instructions
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_PiState {
    /**
    * @param XML_HTMLSax3_StateParser subclass
    * @return constant XML_HTMLSAX3_STATE_START
    * @access protected
    */
    function parse(&$context) {
        $target = $context->scanUntilCharacters(" \n\r\t");
        $data = $context->scanUntilString('?>');
        if ($data != '') {
            $context->handler_object_pi->
            {$context->handler_method_pi}($context->htmlsax, $target, $data);
        }
        $context->IgnoreCharacter();
        $context->IgnoreCharacter();
        return XML_HTMLSAX3_STATE_START;
    }
}
?>
<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to either version 3.0 of the PHP license |
// | or version 3.0 of the GNU Lesser General Public License.             |
// | The PHP license 3.0 is bundled with this package in the file LICENSE,|
// | and is available at  through the world-wide-web at                   |
// | http://www.php.net/license/3_0.txt. If you did not receive a copy of |
// | the PHP license and are unable to obtain it through the              |
// | world-wide-web, please send a note to license@php.net so we can mail |
// | you a copy immediately.                                              |
// | You should have received a copy of the GNU Lesser General Public     |
// | License along with this library.  If not, see                        |
// | <http://www.gnu.org/licenses/>.                                      |
// +----------------------------------------------------------------------+
// | Authors: Alexander Zhukov <alex@veresk.ru> Original port from Python |
// | Authors: Harry Fuecks <hfuecks@phppatterns.com> Port to PEAR + more  |
// | Authors: Many @ Sitepointforums Advanced PHP Forums                  |
// +----------------------------------------------------------------------+
//
// PEAR  Id: Decorators.php,v 1.2 2007/10/29 21:41:35 hfuecks
//
/**
* Decorators for dealing with parser options
* @package XML_HTMLSax3
* @version  Id: Decorators.php,v 1.2 2007/10/29 21:41:35 hfuecks
* @see XML_HTMLSax3::set_option
*/
/**
* Trims the contents of element data from whitespace at start and end
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_Trim {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original handler method
    * @var string
    * @access private
    */
    var $orig_method;
    /**
    * Constructs XML_HTMLSax3_Trim
    * @param object handler object being decorated
    * @param string original handler method
    * @access protected
    */
    function XML_HTMLSax3_Trim(&$orig_obj, $orig_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_method = $orig_method;
    }
    /**
    * Trims the data
    * @param XML_HTMLSax3
    * @param string element data
    * @access protected
    */
    function trimData(&$parser, $data) {
        $data = trim($data);
        if ($data != '') {
            $this->orig_obj->{$this->orig_method}($parser, $data);
        }
    }
}
/**
* Coverts tag names to upper case
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_CaseFolding {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original open handler method
    * @var string
    * @access private
    */
    var $orig_open_method;
    /**
    * Original close handler method
    * @var string
    * @access private
    */
    var $orig_close_method;
    /**
    * Constructs XML_HTMLSax3_CaseFolding
    * @param object handler object being decorated
    * @param string original open handler method
    * @param string original close handler method
    * @access protected
    */
    function XML_HTMLSax3_CaseFolding(&$orig_obj, $orig_open_method, $orig_close_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_open_method = $orig_open_method;
        $this->orig_close_method = $orig_close_method;
    }
    /**
    * Folds up open tag callbacks
    * @param XML_HTMLSax3
    * @param string tag name
    * @param array tag attributes
    * @access protected
    */
    function foldOpen(&$parser, $tag, $attrs=array(), $empty = FALSE) {
        $this->orig_obj->{$this->orig_open_method}($parser, strtoupper($tag), $attrs, $empty);
    }
    /**
    * Folds up close tag callbacks
    * @param XML_HTMLSax3
    * @param string tag name
    * @access protected
    */
    function foldClose(&$parser, $tag, $empty = FALSE) {
        $this->orig_obj->{$this->orig_close_method}($parser, strtoupper($tag), $empty);
    }
}
/**
* Breaks up data by linefeed characters, resulting in additional
* calls to the data handler
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_Linefeed {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original handler method
    * @var string
    * @access private
    */
    var $orig_method;
    /**
    * Constructs XML_HTMLSax3_LineFeed
    * @param object handler object being decorated
    * @param string original handler method
    * @access protected
    */
    function XML_HTMLSax3_LineFeed(&$orig_obj, $orig_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_method = $orig_method;
    }
    /**
    * Breaks the data up by linefeeds
    * @param XML_HTMLSax3
    * @param string element data
    * @access protected
    */
    function breakData(&$parser, $data) {
        $data = explode("\n",$data);
        foreach ( $data as $chunk ) {
            $this->orig_obj->{$this->orig_method}($parser, $chunk);
        }
    }
}
/**
* Breaks up data by tab characters, resulting in additional
* calls to the data handler
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_Tab {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original handler method
    * @var string
    * @access private
    */
    var $orig_method;
    /**
    * Constructs XML_HTMLSax3_Tab
    * @param object handler object being decorated
    * @param string original handler method
    * @access protected
    */
    function XML_HTMLSax3_Tab(&$orig_obj, $orig_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_method = $orig_method;
    }
    /**
    * Breaks the data up by linefeeds
    * @param XML_HTMLSax3
    * @param string element data
    * @access protected
    */
    function breakData(&$parser, $data) {
        $data = explode("\t",$data);
        foreach ( $data as $chunk ) {
            $this->orig_obj->{$this->orig_method}($this, $chunk);
        }
    }
}
/**
* Breaks up data by XML entities and parses them with html_entity_decode(),
* resulting in additional calls to the data handler<br />
* Requires PHP 4.3.0+
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_Entities_Parsed {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original handler method
    * @var string
    * @access private
    */
    var $orig_method;
    /**
    * Constructs XML_HTMLSax3_Entities_Parsed
    * @param object handler object being decorated
    * @param string original handler method
    * @access protected
    */
    function XML_HTMLSax3_Entities_Parsed(&$orig_obj, $orig_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_method = $orig_method;
    }
    /**
    * Breaks the data up by XML entities
    * @param XML_HTMLSax3
    * @param string element data
    * @access protected
    */
    function breakData(&$parser, $data) {
        $data = preg_split('/(&.+?;)/',$data,-1,PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_NO_EMPTY);
        foreach ( $data as $chunk ) {
            $chunk = html_entity_decode($chunk,ENT_NOQUOTES);
            $this->orig_obj->{$this->orig_method}($this, $chunk);
        }
    }
}
/**
* Compatibility with older PHP versions
*/
if (version_compare(phpversion(), '4.3', '<') && !function_exists('html_entity_decode') ) {
    function html_entity_decode($str, $style=ENT_NOQUOTES) {
        return strtr($str,
            array_flip(get_html_translation_table(HTML_ENTITIES,$style)));
    }
}
/**
* Breaks up data by XML entities but leaves them unparsed,
* resulting in additional calls to the data handler<br />
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_Entities_Unparsed {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original handler method
    * @var string
    * @access private
    */
    var $orig_method;
    /**
    * Constructs XML_HTMLSax3_Entities_Unparsed
    * @param object handler object being decorated
    * @param string original handler method
    * @access protected
    */
    function XML_HTMLSax3_Entities_Unparsed(&$orig_obj, $orig_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_method = $orig_method;
    }
    /**
    * Breaks the data up by XML entities
    * @param XML_HTMLSax3
    * @param string element data
    * @access protected
    */
    function breakData(&$parser, $data) {
        $data = preg_split('/(&.+?;)/',$data,-1,PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_NO_EMPTY);
        foreach ( $data as $chunk ) {
            $this->orig_obj->{$this->orig_method}($this, $chunk);
        }
    }
}

/**
* Strips the HTML comment markers or CDATA sections from an escape.
* If XML_OPTIONS_FULL_ESCAPES is on, this decorator is not used.<br />
* @package XML_HTMLSax3
* @access protected
*/
class XML_HTMLSax3_Escape_Stripper {
    /**
    * Original handler object
    * @var object
    * @access private
    */
    var $orig_obj;
    /**
    * Original handler method
    * @var string
    * @access private
    */
    var $orig_method;
    /**
    * Constructs XML_HTMLSax3_Entities_Unparsed
    * @param object handler object being decorated
    * @param string original handler method
    * @access protected
    */
    function XML_HTMLSax3_Escape_Stripper(&$orig_obj, $orig_method) {
        $this->orig_obj =& $orig_obj;
        $this->orig_method = $orig_method;
    }
    /**
    * Breaks the data up by XML entities
    * @param XML_HTMLSax3
    * @param string element data
    * @access protected
    */
    function strip(&$parser, $data) {
        // Check for HTML comments first
        if ( substr($data,0,2) == '--' ) {
            $patterns = array(
                '/^\-\-/',          // Opening comment: --
                '/\-\-$/',          // Closing comment: --
            );
            $data = preg_replace($patterns,'',$data);

        // Check for XML CDATA sections (note: don't do both!)
        } else if ( substr($data,0,1) == '[' ) {
            $patterns = array(
                '/^\[.*CDATA.*\[/s', // Opening CDATA
                '/\].*\]$/s',       // Closing CDATA
                );
            $data = preg_replace($patterns,'',$data);
        }

        $this->orig_obj->{$this->orig_method}($this, $data);
    }
}
?>
