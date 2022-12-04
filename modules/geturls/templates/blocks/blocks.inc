<?php
$blocks = array(
    'GetUrls' => array(
	'description' => $gallery->i18n('Formatted URLs'),
	'vars' => array(
	    'width' => array(
		'description' => $gallery->i18n('Width of the fields'),
		'type' => 'choice',
		'default' => '85',
		'choices' => array(
		    '10' => $gallery->i18n('10'),
		    '15' => $gallery->i18n('15'),
		    '20' => $gallery->i18n('20'),
		    '30' => $gallery->i18n('30'),
		    '40' => $gallery->i18n('40'),
		    '50' => $gallery->i18n('50'),
		    '60' => $gallery->i18n('60'),
		    '70' => $gallery->i18n('70'),
		    '85' => $gallery->i18n('85'),
		    '100' => $gallery->i18n('100'),
		    '120' => $gallery->i18n('120'),
		    '150' => $gallery->i18n('150')
		    )
		),
	    'showWarnings' => array(
	        'description' => $gallery->i18n('Show warnings'),
		'type' => 'boolean',
		'default' => 'true'
		)
	    )
	)
    );
?>
