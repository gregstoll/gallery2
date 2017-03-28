<?php
$blocks = array(
    'KeywordLinks' => array(
	'description' => $gallery->i18n('Keyword album links'),
	'vars' => array(
	    'forItem' => array(
		'description' =>
		    $gallery->i18n('Keywords for current item (uncheck to show all keywords)'),
		'type' => 'boolean',
		'default' => 'true',
		'overrides' => array('onlyPublic', 'showCloud', 'sizeLimit')),
	    'onlyPublic' => array(
		'description' => $gallery->i18n('Keywords from public items only'),
		'type' => 'boolean',
		'default' => 'true'),
	    'showCloud' => array(
		'description' => $gallery->i18n('Show keyword cloud instead of select list'),
		'type' => 'boolean',
		'default' => 'false'),
	    'sizeLimit' => array(
		'description' => $gallery->i18n('Maximum number of keywords (empty for no limit)'),
		'type' => 'text',
		'default' => ''),
	    )));
?>
