<?php
/**
 * Smarty plugin
 * @package Smarty
 * @subpackage plugins
 */


/**
 * Smarty implode modifier plugin
 *
 * Type:     modifier<br>
 * Name:     implode<br>
 * Purpose:  Implode (join) string 
 * @author   Greg Stoll <greg at gregstoll dot com>
 * @param array
 * @param string
 * @return string
 */
function smarty_modifier_implode($array, $separator)
{
    //return "implode('" . $separator . "', " . $array . ")";
    return implode($separator, $array);
}

/* vim: set expandtab: */

?>
