<?php
$blocks['RssBlock'] = array(
    'description' => $gallery->i18n('List of RSS Feeds'),
    'default' => 'true',
    'vars' => array(
	'number' => array(
	    'description' => $gallery->i18n('Number of feeds to display'),
	    'type' => 'choice',
	    'default' => '5',
	    'choices' => array(
		'1' => $gallery->i18n('1'),
		'5' => $gallery->i18n('5'),
		'10' => $gallery->i18n('10'),
		'999' => $gallery->i18n('all'),
	    )
	)
    )
);
?>
