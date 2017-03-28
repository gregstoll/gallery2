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

/*
 * Smarty plugin
 * -------------------------------------------------------------
 * Type:     modifier
 * Name:     markup
 * Purpose:  Format embedded markup in the given string according
 *           to settings specified in site admin (no markup, bbcode or
 *           raw html)
 * -------------------------------------------------------------
 */
function smarty_modifier_markup($text) {
    static $parsers = array();
    static $cacheKey = 'smarty_modifier_markup';

    $stripTags = false;
    $args = func_get_args();
    array_shift($args);
    foreach ($args as $arg) {
	if ($arg == 'strip') {
	    $stripTags = true;
	} else {
	    $markupType = $arg;
	}
    }
    if (!isset($markupType)) {
	if (!GalleryDataCache::containsKey($cacheKey)) {
	    list ($ret, $defaultMarkupType) =
		GalleryCoreApi::getPluginParameter('module', 'core', 'misc.markup');
	    if ($ret) {
		/* This code is used by the UI -- we can't return an error. Choose something safe */
		$defaultMarkupType = 'none';
	    }
	    GalleryDataCache::put($cacheKey, $defaultMarkupType);
	}

	$markupType = GalleryDataCache::get($cacheKey);
    }

    if (!isset($parsers[$markupType])) {
	switch($markupType) {
	case 'bbcode':
	    $parsers[$markupType] = new GalleryBbcodeMarkupParser();
	    break;

	case 'html':
	    $parsers[$markupType] = new GalleryHtmlMarkupParser();
	    break;

	case 'none':
	default:
	    $parsers[$markupType] = new GalleryNoMarkupParser();
	}
    }

    $text = $parsers[$markupType]->parse($text);
    return $stripTags ? strip_tags($text) : $text;
}

class GalleryNoMarkupParser {
    function parse($text) {
	return $text;
    }
}

class GalleryHtmlMarkupParser {
    function parse($text) {
	/* http://bugs.php.net/bug.php?id=22014 - TODO: remove empty check when min php is 4.3.2+ */
	return empty($text) ? $text : GalleryUtilities::htmlSafe(html_entity_decode($text));
    }
}

class GalleryBbcodeMarkupParser {
    var $_bbcode;

    function GalleryBbcodeMarkupParser() {
	if (!class_exists('StringParser_BBCode')) {
	    GalleryCoreApi::requireOnce('lib/bbcode/stringparser_bbcode.class.php');
	}

	$this->_bbcode = new StringParser_BBCode();
	$this->_bbcode->setGlobalCaseSensitive(false);

	/* Convert line breaks everywhere */
	$this->_bbcode->addParser(array('block', 'inline', 'link', 'listitem', 'list'),
				  array($this, 'convertLineBreaks'));

	/*
	 * Escape all characters everywhere
	 * We don't need to do this 'cause G2 doesn't allow raw entities into the database
	 * $this->_bbcode->addParser('htmlspecialchars',
	 *			     array('block', 'inline', 'link', 'listitem'));
	 */

	/* Convert line endings */
	$this->_bbcode->addParser(array('block', 'inline', 'link', 'listitem'), 'nl2br');

	/* Strip last line break in list items */
	$this->_bbcode->addParser(array('listitem'), array($this, 'stripLastLineBreak'));

	/* Strip contents in list elements */
	$this->_bbcode->addParser(array('list'), array($this, 'stripContents'));

	/* [b], [i] */
	$this->_bbcode->addCode('b', 'simple_replace', null,
				array('start_tag' => '<b>', 'end_tag' => '</b>'),
				'inline', array('listitem', 'block', 'inline', 'link'), array());

	$this->_bbcode->addCode('i', 'simple_replace', null,
				array('start_tag' => '<i>', 'end_tag' => '</i>'),
				'inline', array('listitem', 'block', 'inline', 'link'), array());

	/* [url]http://...[/url], [url=http://...]Text[/url] */
	$this->_bbcode->addCode('url', 'usecontent?', array($this, 'url'),
			 array('usecontent_param' => 'default'),
			 'link', array('listitem', 'block', 'inline'), array('link'));

	/* [color=...]Text[/color] */
	$this->_bbcode->addCode('color', 'callback_replace', array($this, 'color'),
			 array('usecontent_param' => 'default'),
			 'inline', array('listitem', 'block', 'inline', 'link'), array());

	/* [img]http://...[/img] */
	$this->_bbcode->addCode('img', 'usecontent', array($this, 'image'), array(),
			 'image', array('listitem', 'block', 'inline', 'link'), array());

	/* [list] [*]Element [/list] */
	$this->_bbcode->addCode('list', 'simple_replace', null,
				array('start_tag' => '<ul>', 'end_tag' => '</ul>'),
				'list', array('block', 'listitem'), array());
	$this->_bbcode->addCode('*', 'simple_replace', null,
				array('start_tag' => '<li>', 'end_tag' => "</li>\n"),
				'listitem', array('list'), array());
	$this->_bbcode->setCodeFlag('*', 'closetag', BBCODE_CLOSETAG_OPTIONAL);
    }

    function parse($text) {
	return $this->_bbcode->parse($text);
    }

    function url($action, $attributes, $content, $params, &$node_object) {
	if ($action == 'validate') {
	    /* The code is like [url]http://.../[/url] */
	    if (!isset($attributes['default'])) {
		return preg_match('#^(https?|ftp|mailto):|^/#', $content);
	    } else {
		/* The code is like [url=http://.../]Text[/url] */
		return preg_match('#^(https?|ftp|mailto):|^/#', $attributes['default']);
	    }
	} else {
	    /* Output of HTML. */
	    /* The code is like [url]http://.../[/url] */
	    if (!isset($attributes['default'])) {
		return '<a href="' . $content . '" rel="nofollow">' . $content . '</a>';
	    } else {
		/* The code is like [url=http://.../]Text[/url] */
		return '<a href="' . $attributes['default'] . '" rel="nofollow">'
		    . $content . '</a>';
	    }
	}
    }

    function image($action, $attrs, $content, $params, &$node_object) {
	if ($action == 'validate') {
	    return preg_match('#^(https?|ftp|mailto):|^/#', $content);
	} else {
	    /* Output of HTML. */
	    $size = (isset($attrs['width']) ? ' width="' . (int)$attrs['width'] . '"' : '')
		. (isset($attrs['height']) ? ' height="' . (int)$attrs['height'] . '"' : '');
	    /* Input should have entities already, so no htmlspecialchars here */
	    return sprintf('<img src="%s" alt=""%s/>', $content, $size);
	}
    }

    function color($action, $attrs, $content, $params, &$node_object) {
	if ($action == 'validate') {
	    return !empty($attrs['default']);
	} else {
	    /* Output of HTML. */
	    $color = empty($attrs) ? 'bummer' : $attrs['default'];
	    return sprintf('<font color="%s">%s</font>', $color, $content);
	}
    }

    function convertLineBreaks($text) {
	return preg_replace("/\015\012|\015|\012/", "\n", $text);
    }

    function stripContents($text) {
	return preg_replace("/[^\n]/", '', $text);
    }

    function stripLastLineBreak ($text) {
	return preg_replace("/\n( +)?$/", '$1', $text);
    }
}
?>
