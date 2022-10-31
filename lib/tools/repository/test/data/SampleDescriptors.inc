<?php

/*
 * These are sample descriptors that should be generated from our mock plugins. If the plugin is
 * changed, the changes must be reflected here as well. It is also used in the index generation test
 * as a sample descriptor. The sampleModule2Descriptor represents a newer version of sampleModule.
 */
$sampleModuleDescriptor = array(
	'header' => array(
		'id'                => 'testModule',
		'version'           => '1.0.0',
		'requiredCoreApi'   => GalleryCoreApi::getApiVersion(),
		'requiredPluginApi' => GalleryModule::getApiVersion(),
		'stringsRevision'   => '22222',
		'buildTimestamp'    => '11111',
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
	'directories' => array(
		'locale/' => array(
			'packages' => array('base'),
		),
		'locale/en_US/' => array(
			'packages' => array('lang-en_US'),
		),
		'locale/en_US/LC_MESSAGES/' => array(
			'packages' => array('lang-en_US'),
		),
		'locale/en_GB/' => array(
			'packages' => array('lang-en_GB'),
		),
		'locale/en_GB/LC_MESSAGES/' => array(
			'packages' => array('lang-en_GB'),
		),
		'locale/pt_BR/' => array(
			'packages' => array('lang-pt_BR'),
		),
		'locale/pt_BR/LC_MESSAGES/' => array(
			'packages' => array('lang-pt_BR'),
		),
		'locale/pt_PT/' => array(
			'packages' => array('lang-pt_PT'),
		),
		'locale/pt_PT/LC_MESSAGES/' => array(
			'packages' => array('lang-pt_PT'),
		),
		'images/' => array(
			'packages' => array('base'),
		),
		'po/' => array(
			'packages' => array('base'),
		),
	),
	'files' => array(
		'locale/en_US/LC_MESSAGES/modules_mockmodule.mo' => array(
			'bytes'    => 10,
			'hash'     => 'd3601c44692bb446fae92652f1c82c7c',
			'packages' => array('lang-en_US'),
		),
		'locale/en_GB/LC_MESSAGES/modules_mockmodule.mo' => array(
			'bytes'    => 10,
			'hash'     => '8a4903cab930c121abc7deb6e94d44bb',
			'packages' => array('lang-en_GB'),
		),
		'locale/pt_BR/LC_MESSAGES/modules_mockmodule.mo' => array(
			'bytes'    => 10,
			'hash'     => '59f22b5c4ff44bd73e5022694c8c808d',
			'packages' => array('lang-pt_BR'),
		),
		'locale/pt_PT/LC_MESSAGES/modules_mockmodule.mo' => array(
			'bytes'    => 10,
			'hash'     => '33e9fbf8b2c3467d9f734a81eb60b791',
			'packages' => array('lang-pt_PT'),
		),
		'images/image1.gif' => array(
			'bytes'    => 10,
			'hash'     => '5ffdb396d025097b3750d5604a42ec2d',
			'packages' => array('base'),
		),
		'images/image2.gif' => array(
			'bytes'    => 10,
			'hash'     => '3f9bd610f088dfcbdcc68a196b77d5d0',
			'packages' => array('base'),
		),
		'po/strings.raw' => array(
			'bytes'    => 10,
			'hash'     => 'a8fd41c1a6380ef825be57f9a47a64ec',
			'packages' => array('base'),
		),
		'po/GNUmakefile' => array(
			'bytes'    => 10,
			'hash'     => '491e08859dd235a9b3388ac9562bf585',
			'packages' => array('base'),
		),
		'po/en_US.po' => array(
			'bytes'    => 10,
			'hash'     => 'ff25f36483fbdec82e6e47774bbd6604',
			'packages' => array('lang-en_US'),
		),
		'po/en_GB.po' => array(
			'bytes'    => 10,
			'hash'     => 'ee5133693b26cc252cc746d0c1e5cefe',
			'packages' => array('lang-en_GB'),
		),
		'po/pt_PT.po' => array(
			'bytes'    => 10,
			'hash'     => 'd87df5696429d4f839e95e29ca27935f',
			'packages' => array('lang-pt_PT'),
		),
		'po/pt_BR.po' => array(
			'bytes'    => 10,
			'hash'     => '874e7416c838bf5e111934194268cc76',
			'packages' => array('lang-pt_BR'),
		),
		'MANIFEST' => array(
			'bytes'    => 10,
			'hash'     => '9e6ad3fb8fd40ad331df68561eeae4d0',
			'packages' => array('base'),
		),
		'module.inc' => array(
			'bytes'    => 10,
			'hash'     => 'f8fae297e8cb811310113ab7e2c24bfa',
			'packages' => array('base'),
		),
		'callbacks.inc' => array(
			'bytes'    => 10,
			'hash'     => '1a3e2f89a8d8a19e716d50ecdd3a2c2b',
			'packages' => array('base'),
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
);

/*
 * Set a newer plugin build and po file versions. These are
 * the po versions that should be in the index.
 */
$sampleModule2Descriptor                             = $sampleModuleDescriptor;
$sampleModule2Descriptor['header']['buildTimestamp'] = '44444';
$sampleModule2Descriptor['languages']['22222']       = array(
	'en_US' => '33333',
	'en_GB' => '33333',
	'pt_PT' => '33333',
	'pt_BR' => '33333',
);

$sampleThemeDescriptor = array(
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
	'directories' => array(
		'locale/' => array(
			'packages' => array('base'),
		),
		'locale/en_US/' => array(
			'packages' => array('lang-en_US'),
		),
		'locale/en_US/LC_MESSAGES/' => array(
			'packages' => array('lang-en_US'),
		),
		'locale/en_GB/' => array(
			'packages' => array('lang-en_GB'),
		),
		'locale/en_GB/LC_MESSAGES/' => array(
			'packages' => array('lang-en_GB'),
		),
		'locale/pt_BR/' => array(
			'packages' => array('lang-pt_BR'),
		),
		'locale/pt_BR/LC_MESSAGES/' => array(
			'packages' => array('lang-pt_BR'),
		),
		'locale/pt_PT/' => array(
			'packages' => array('lang-pt_PT'),
		),
		'locale/pt_PT/LC_MESSAGES/' => array(
			'packages' => array('lang-pt_PT'),
		),
		'images/' => array(
			'packages' => array('base'),
		),
		'po/' => array(
			'packages' => array('base'),
		),
	),
	'files' => array(
		'locale/en_US/LC_MESSAGES/themes_mocktheme.mo' => array(
			'bytes'    => 10,
			'hash'     => 'd3601c44692bb446fae92652f1c82c7c',
			'packages' => array('lang-en_US'),
		),
		'locale/en_GB/LC_MESSAGES/themes_mocktheme.mo' => array(
			'bytes'    => 10,
			'hash'     => '8a4903cab930c121abc7deb6e94d44bb',
			'packages' => array('lang-en_GB'),
		),
		'locale/pt_BR/LC_MESSAGES/themes_mocktheme.mo' => array(
			'bytes'    => 10,
			'hash'     => '59f22b5c4ff44bd73e5022694c8c808d',
			'packages' => array('lang-pt_BR'),
		),
		'locale/pt_PT/LC_MESSAGES/themes_mocktheme.mo' => array(
			'bytes'    => 10,
			'hash'     => '33e9fbf8b2c3467d9f734a81eb60b791',
			'packages' => array('lang-pt_PT'),
		),
		'images/image1.gif' => array(
			'bytes'    => 10,
			'hash'     => '5ffdb396d025097b3750d5604a42ec2d',
			'packages' => array('base'),
		),
		'images/image2.gif' => array(
			'bytes'    => 10,
			'hash'     => '3f9bd610f088dfcbdcc68a196b77d5d0',
			'packages' => array('base'),
		),
		'po/strings.raw' => array(
			'bytes'    => 10,
			'hash'     => 'a8fd41c1a6380ef825be57f9a47a64ec',
			'packages' => array('base'),
		),
		'po/GNUmakefile' => array(
			'bytes'    => 10,
			'hash'     => '491e08859dd235a9b3388ac9562bf585',
			'packages' => array('base'),
		),
		'po/en_US.po' => array(
			'bytes'    => 10,
			'hash'     => 'ff25f36483fbdec82e6e47774bbd6604',
			'packages' => array('lang-en_US'),
		),
		'po/en_GB.po' => array(
			'bytes'    => 10,
			'hash'     => 'ee5133693b26cc252cc746d0c1e5cefe',
			'packages' => array('lang-en_GB'),
		),
		'po/pt_PT.po' => array(
			'bytes'    => 10,
			'hash'     => 'd87df5696429d4f839e95e29ca27935f',
			'packages' => array('lang-pt_PT'),
		),
		'po/pt_BR.po' => array(
			'bytes'    => 10,
			'hash'     => '874e7416c838bf5e111934194268cc76',
			'packages' => array('lang-pt_BR'),
		),
		'MANIFEST' => array(
			'bytes'    => 10,
			'hash'     => '9e6ad3fb8fd40ad331df68561eeae4d0',
			'packages' => array('base'),
		),
		'theme.inc' => array(
			'bytes'    => 10,
			'hash'     => 'f8fae297e8cb811310113ab7e2c24bfa',
			'packages' => array('base'),
		),
		'callbacks.inc' => array(
			'bytes'    => 10,
			'hash'     => '1a3e2f89a8d8a19e716d50ecdd3a2c2b',
			'packages' => array('base'),
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
);

