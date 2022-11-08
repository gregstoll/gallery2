<?php

// This is a sample valid index used for testing.
$sampleIndex = array(
	'modules' => array(
		'testModule' => array(
			'header' => array(
				'id'                => 'testModule',
				'version'           => '1.0.0',
				'requiredCoreApi'   => GalleryCoreApi::getApiVersion(),
				'requiredPluginApi' => GalleryModule::getApiVersion(),
				'stringsRevision'   => '22222',
				'buildTimestamp'    => '44444',
			),
			'descriptions' => array(
				'en_US' => array(
					'name'        => 'Test Module',
					'description' => 'This is a test module.',
					'groupLabel'  => 'Test Group',
				),
				'en_GB' => array(
					'name'        => 'en_GB-modules_testModule-Test Module',
					'description' => 'en_GB-modules_testModule-This is a test module.',
					'groupLabel'  => 'en_GB-modules_testModule-Test Group',
				),
				'pt_BR' => array(
					'name'        => 'pt_BR-modules_testModule-Test Module',
					'description' => 'pt_BR-modules_testModule-This is a test module.',
					'groupLabel'  => 'pt_BR-modules_testModule-Test Group',
				),
				'pt_PT' => array(
					'name'        => 'pt_PT-modules_testModule-Test Module',
					'description' => 'pt_PT-modules_testModule-This is a test module.',
					'groupLabel'  => 'pt_PT-modules_testModule-Test Group',
				),
			),
			'languages' => array(
				'22222' => array(
					'en_US' => '33333',
					'en_GB' => '33333',
					'pt_PT' => '33333',
					'pt_BR' => '33333',
				),
			),
		),
	),
	'themes' => array(
		'testTheme' => array(
			'header' => array(
				'id'                => 'testTheme',
				'version'           => '1.0.0',
				'requiredCoreApi'   => GalleryCoreApi::getApiVersion(),
				'requiredPluginApi' => GalleryTheme::getApiVersion(),
				'stringsRevision'   => '22222',
				'buildTimestamp'    => '11111',
			),
			'descriptions' => array(
				'en_US' => array(
					'name'        => 'Test Theme',
					'description' => 'This is a test theme.',
					'groupLabel'  => 'Test Group',
				),
				'en_GB' => array(
					'name'        => 'en_GB-themes_testTheme-Test Theme',
					'description' => 'en_GB-themes_testTheme-This is a test theme.',
					'groupLabel'  => 'en_GB-themes_testTheme-Test Group',
				),
				'pt_BR' => array(
					'name'        => 'pt_BR-themes_testTheme-Test Theme',
					'description' => 'pt_BR-themes_testTheme-This is a test theme.',
					'groupLabel'  => 'pt_BR-themes_testTheme-Test Group',
				),
				'pt_PT' => array(
					'name'        => 'pt_PT-themes_testTheme-Test Theme',
					'description' => 'pt_PT-themes_testTheme-This is a test theme.',
					'groupLabel'  => 'pt_PT-themes_testTheme-Test Group',
				),
			),
			'languages' => array(
				'22222' => array(
					'en_US' => '33333',
					'en_GB' => '22222',
					'pt_PT' => '33333',
					'pt_BR' => '33333',
				),
			),
		),
	),
);

