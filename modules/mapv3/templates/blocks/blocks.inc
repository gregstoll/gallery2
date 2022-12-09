<?php
$blocks = array(
	'mapFilter' => array(
		'description' => $gallery->i18n('Google Map v3 Filters (Map View only)'),
	),
	'Legend' => array(
		'description' => $gallery->i18n('Google Map v3 Marker Legend (Map View only)'),
	),
	'Thumb' => array(
		'description' => $gallery->i18n('Google Map v3 Thumbnail Bar (Map View only)'),
	),
	'ItemCoords' => array(
		'description' => $gallery->i18n('Item Map coordinates v3'),
		'vars'        => array(
			'coordStyle' => array(
				'description' => $gallery->i18n('Coordinate display style'),
				'type'        => 'choice',
				'default'     => '1',
				'choices'     => array(
					'1' => $gallery->i18n('Decimal Degrees'),
					'2' => $gallery->i18n('Degrees Decimal Minutes'),
					'3' => $gallery->i18n('Degrees Minutes Seconds'),
				),
			),
		),
	),
	'MiniMap' => array(
		'description' => $gallery->i18n('Mini Google Map'),
		'vars'        => array(
			'mapHeight' => array(
				'description' => $gallery->i18n('Map height (pixels)'),
				'type'        => 'text',
				'default'     => '150',
			),
			'mapWidth' => array(
				'description' => $gallery->i18n('Map width (% or px)'),
				'type'        => 'text',
				'default'     => '100%',
			),
			'mapType' => array(
				'description' => $gallery->i18n('Map type'),
				// array('text' => 'Map type', 'hint' => 'Style of Google map')
				'type'        => 'choice',
				'default'     => '3',
				'choices'     => array(
					'1' => $gallery->i18n('Map'),
					// array('text' => 'Map', 'hint' => 'Map style')
					'2' => $gallery->i18n('Satellite'),
					// array('text' => 'Satellite', 'hint' => 'Map style')
					'3' => $gallery->i18n('Hybrid'),
				),
			),
			// array('text' => 'Hybrid', 'hint' => 'Map style')
			'showControls' => array(
				'description' => $gallery->i18n('Enable pan and zoom controls'),
				'type'        => 'boolean',
				'default'     => 'false',
			),
			'useParentCoords' => array(
				'description' => $gallery->i18n('Use parent album coordinates if item has none'),
				'type'        => 'boolean',
				'default'     => 'false',
			),
			'albumMarker' => array(
				'description' => $gallery->i18n('Album page: Show album marker'),
				'type'        => 'boolean',
				'default'     => 'true',
			),
			'albumItems' => array(
				'description' => $gallery->i18n('Album page: Show item markers'),
				'type'        => 'choice',
				'default'     => '2',
				'choices'     => array(
					'1' => $gallery->i18n('None'),
					'2' => $gallery->i18n('Show items in current album level'),
					'3' => $gallery->i18n('Show all album subitems'),
				),
			),
		),
	),
	'quickEdit' => array(
		'description' => $gallery->i18n('Quick edit coordinates'),
	),
);
