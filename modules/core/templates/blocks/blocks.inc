<?php
$blocks = array(
    'LanguageSelector' => array(
	'description' => $gallery->i18n('Language selector')),

    'ItemLinks' => array(
	'description' => $gallery->i18n('Item actions'),
	'vars' => array(
	    'useDropdown' => array(
		'default' => 'true',
		'description' => $gallery->i18n('Use dropdown menu'),
		'type' => 'boolean'))),

    'LoginBlock' => array(
	'description' => $gallery->i18n('Login block')),

    'PeerList' => array(
	'description' => $gallery->i18n('Links to album/photo peers')));
?>
