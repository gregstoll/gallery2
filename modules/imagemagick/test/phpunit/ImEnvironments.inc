<?php

/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

/**
 * @version $Revision: 17804 $
 * @package ImageMagick
 * @subpackage PHPUnit
 * @author Ernesto Baschny <ernst@baschny.de>
 */

/*
 * This file contain information to emulate specific ImageMagick/GraphicsMagick
 * installations. They are used by the ImageMagickToolkitTestPlatform.
 */

/*
 * Those are the default operations for all IM-environments.
 * Any element can be overridden in each environments definition below.
 */
$_commonOperations = array(
	'thumbnail' => array(
		'convert' => array(
			'-size',
			'%width%x%height%',
			'-geometry',
			'%width%x%height%',
			'+profile',
			'*',
			'%src%',
			'%dst%',
		),
	),
	'scale' => array(
		'convert' => array(
			'-size',
			'%width%x%height%',
			'-geometry',
			'%width%x%height%',
			'%src%',
			'%dst%',
		),
	),
	'resize' => array(
		'convert' => array(
			'-size',
			'%width%x%height%',
			'-geometry',
			'%width%x%height%',
			'%src%',
			'%dst%',
		),
	),
	'crop' => array(
		'convert' => array('-crop', '%width%x%height%+%x%+%y%', '%src%', '%dst%'),
	),
	'rotate' => array(
		'convert' => array('-rotate', '%angle%', '%src%', '%dst%'),
	),
	'convert-to-image/jpeg' => array(
		'convert' => array('%src%', '%dst%'),
	),
	'composite' => array(
		'composite' => array('-geometry', '+%x%+%y%', '%image%', '%src%', '%dst%'),
	),
	'convert-to-image/png' => array(
		'convert' => array('%src%', '%dst%'),
	),
	'select-page' => array(
		'convert' => array('%src%', '%dst%'),
	),
	'compress' => array(
		'convert' => array('%src%', '%dst%'),
	),
);

/* --------------------------------------------------------------------------
 * ImageMagick 4.2.9
 * --------------------------------------------------------------------------
 */

$imEnvironment[] = array(
	'name'                 => 'ImageMagick',
	'version'              => '4.2.9',
	'binary'               => '',
	'mimeTypes'            => array(
		'image/gif',
		'image/jpeg',
		'image/pjpeg',
		'image/png',
		'image/tiff',
		'image/bmp',
		'application/pdf',
		'application/postscript',
	),
	'binaries'             => array(
		'animate',
		'combine',
		'convert',
		'display',
		'identify',
		'import',
		'mogrify',
		'montage',
		'xtp',
	),
	'removeMetaDataSwitch' => array(),
	'operations'           => array_merge(
		$_commonOperations,
		array(
			'thumbnail' => array(
				'convert' => array(
					'-size',
					'%width%x%height%',
					'-geometry',
					'%width%x%height%',
					'%src%',
					'%dst%',
				),
			),
			'scale' => array(
				'convert' => array(
					'-size',
					'%width%x%height%',
					'-geometry',
					'%width%x%height%',
					'%src%',
					'%dst%',
				),
			),
			'composite' => array(
				'combine' => array('-geometry', '+%x%+%y%', '%image%', '%src%', '%dst%'),
			),
		)
	),
	'identify'             => array(
		''                    => 'Version: @(#)ImageMagick 4.2.9 99/09/01 cristy@mystic.es.dupont.com
Copyright: Copyright 1999 E. I. du Pont de Nemours and Company
Usage: identify [ options ... ] file [ file ... ]
Where options include:
  -ping           efficiently determine image characteristics
  -size geometry  width and height of image
  -verbose        print detailed information about the image
		',
		'UNKNOWN'             => 'identify: no delegate for this image format ().
identify: Missing an image file name [No such file or directory].',
		'test-compressed.tif' => 'identify: test-compressed.tif: unknown field with tag 700 (0x2bc) ignored..
identify: test-compressed.tif: unknown field with tag 34665 (0x8769) ignored..
test-compressed.tif 50x50 DirectClass 8000b TIFF 1s',
		'test-multiscene.tif' => 'test-multiscene.tif[0] 100x75 DirectClass 44kb TIFF 1s
test-multiscene.tif[1] 100x75 DirectClass 44kb TIFF 1s',
		'test.bmp'            => 'test.bmp 50x50 PseudoClass 15c 504b BMP 1s',
		'SIZED.gif'           => '%filename% %width%x%height%+0+0 DirectClass 232b GIF 1s',
		'test.gif'            => 'test.gif 50x50+0+0 DirectClass 232b GIF 1s',
		'test.jpg'            => 'test.jpg 50x50 DirectClass 763b JPEG 1s',
		'test.jp2'            => 'identify: no delegate for this image format (JP2).
identify: Missing an image file name [No such file or directory].',
		'test.pdf'            => 'test.pdf=>/tmp/magicFnmgfF[0] 612x792 PseudoClass 256c 29kb PDF 1s
test.pdf=>/tmp/magicFnmgfF[1] 612x792 PseudoClass 256c 29kb PDF 1s',
		'test.pjpg'           => 'test.pjpg 50x50 DirectClass 970b JPEG 1s',
		'test.png'            => 'test.png 50x50 DirectClass 730b PNG 1s',
		'test.svg'            => 'identify: no delegate for this image format (SVG).
identify: Missing an image file name [No such file or directory].',
		'test.tif'            => 'test.tif 50x50 PseudoClass 256c 2729b TIFF 1s',
		'test.eps'            => 'test.eps[0] 43x18 PseudoClass 2c 3141b PS 1s
test.eps[1] 43x18 PseudoClass 2c 3141b PS 1s',
		'test.wmf'            => 'identify: no delegate for this image format (WMF)',
		'test.tga'            => 'identify: no delegate for this image format (TGA)',
		'testProfile.jpg'     => 'testProfile.jpg 40x30 DirectClass 29kb JPEG 1s',
		'-format'             => array(
			'cmyk.jpg' => 'Does4.xWork?',
		),
	),
);

/* --------------------------------------------------------------------------
 * ImageMagick 5.4.2
 * --------------------------------------------------------------------------
 */
$imEnvironment[] = array(
	'name'                 => 'ImageMagick',
	'version'              => '5.4.2',
	'binary'               => '',
	'mimeTypes'            => array(
		'image/gif',
		'image/jpeg',
		'image/pjpeg',
		'image/png',
		'image/tiff',
		'image/svg+xml',
		'image/bmp',
		'application/pdf',
		'application/postscript',
		'application/photoshop',
	),
	'binaries'             => array(
		'animate',
		'composite',
		'conjure',
		'convert',
		'display',
		'identify',
		'import',
		'mogrify',
		'montage',
		'xtp',
	),
	'removeMetaDataSwitch' => array('+profile', '*'),
	'operations'           => $_commonOperations,
	'identify'             => array(
		''                    => 'Version: @(#)ImageMagick 5.4.2 01/01/02 Q:16 http://www.imagemagick.org
Copyright: Copyright (C) 2002 ImageMagick Studio LLC
Usage: identify [ -option value ... ] file [ file ... ]
Where options include:
  -cache threshold   megabytes of memory available to the pixel cache
  -debug             display copious debugging information
  -density geometry  vertical and horizontal density of the image
  -depth value       depth of the image
  -format "string"   output formatted image characteristics
  -help              print program options
  -size geometry     width and height of image
  -verbose           print detailed information about the image
		',
		'UNKNOWN'             => 'identify: Unable to open file (NOTHING) [No such file or directory].
identify: Missing an image file name.',
		'SIZED.gif'           => '%filename% GIF %width%x%height%+0+0 PseudoClass 8c 8-bit 232b 0.0u 0:01',
		'test-compressed.tif' => 'identify: unknown field ' .
		'with tag 34665 (0x8769) ignored. (test-compressed.tif) [No such file or directory].
test-compressed.tif TIFF 50x50 DirectClass 8-bit 8000b 0.0u 0:01',
		'test-multiscene.tif' => 'test-multiscene.tif[0] TIFF 100x75 DirectClass 8-bit 45814b 0.0u 0:01
test-multiscene.tif[1] TIFF 100x75 DirectClass 8-bit 45814b 0.0u 0:01',
		'test.bmp'            => 'test.bmp BMP 50x50 PseudoClass 15c 8-bit 504b 0.0u 0:01',
		'test.gif'            => 'test.gif GIF 50x50+0+0 PseudoClass 8c 8-bit 232b 0.0u 0:01',
		'test.jpg'            => 'test.jpg JPEG 50x50 DirectClass 8-bit 763b 0.0u 0:01',
		'test.jp2'            => 'identify: no delegate for this image format (test.jp2) [No such file or directory].
identify: Missing an image file name.',
		'test.pdf'            => 'test.pdf[0] PDF 612x792 DirectClass 8-bit 2840kb 0.1u 0:01
test.pdf[1] PDF 612x792 DirectClass 8-bit 2840kb 0.0u 0:01',
		'test.pjpg'           => 'test.pjpg JPEG 50x50 DirectClass 8-bit 970b 0.0u 0:01',
		'test.png'            => 'test.png PNG 50x50 DirectClass 8-bit 730b 0.0u 0:01',
		'test.svg'            => 'test.svg SVG 595x841+0+0 DirectClass 16-bit 7.8kb 0.08u 0:01',
		'test.tif'            => 'test.tif TIFF 50x50 PseudoClass 256c 8-bit 2729b 0.0u 0:01',
		'test.eps'            => 'test.eps PS 43x18 DirectClass 8-bit 2388b 0.0u 0:01',
		'test.psd'            => 'test.psd PSD 178x80+0+0 DirectClass 8-bit 127.1kb 0.000u 0:01',
		'test.wmf'            => 'identify: no delegate for this image format (test.wmf) [No such file or directory].
identify: Missing an image file name.',
		'test.tga'            => 'identify: no delegate for this image format (test.tga) [No such file or directory].
identify: Missing an image file name.',
		'testProfile.jpg'     => 'testProfile.jpg JPEG 40x30 DirectClass 8-bit 30585b 0.0u 0:01',
		'-format'             => array(
			'cmyk.jpg' => "DirectClassCMYK\n",
			'rgb_file' => "DirectClassCMYK\n",
		), // Fails to convert to RGB
	),
);

/* --------------------------------------------------------------------------
 * ImageMagick 6.0.7
 * --------------------------------------------------------------------------
 */
$imEnvironment[] = array(
	'name'                 => 'ImageMagick',
	'version'              => '6.0.7',
	'binary'               => '',
	'mimeTypes'            => array(
		'image/gif',
		'image/jpeg',
		'image/pjpeg',
		'image/jp2',
		'image/jpg2',
		'image/jpx',
		'image/png',
		'image/tiff',
		'image/svg+xml',
		'image/bmp',
		'application/pdf',
		'application/postscript',
		'application/photoshop',
		'image/wmf',
		'image/tga',
		'image/jpeg-cmyk',
		'image/tiff-cmyk',
		'application/photoshop-cmyk',
	),
	'binaries'             => array(
		'animate',
		'compare',
		'composite',
		'conjure',
		'convert',
		'display',
		'identify',
		'import',
		'mogrify',
		'montage',
	),
	'removeMetaDataSwitch' => array('+profile', '*'),
	'operations'           => array_merge(
		$_commonOperations,
		array(
			'convert-to-image/jpeg' => array(
				'convert' => array('-colorspace', 'RGB', '%src%', '%dst%'),
			),
		)
	),
	'identify'             => array(
		''                    => 'Version: ImageMagick 6.0.7 09/11/04 Q16 http://www.imagemagick.org
Copyright: Copyright (C) 1999-2004 ImageMagick Studio LLC
Usage: identify [options ...] file [ [options ...] file ... ]
Where options include:
  -authenticate value  decrypt image with this password
  -channel type        Red, Green, Blue, Opacity, Index, Cyan, Yellow,
                       Magenta, Black, or All
  -debug events        display copious debugging information
  -define format:option
                       define one or more image format options
  -density geometry    horizontal and vertical density of the image
  -depth value         image depth
  -extract geometry    extract area from image
  -format "string"     output formatted image characteristics
  -fuzz distance       colors within this distance are considered equal
  -help                print program options
  -interlace type      None, Line, Plane, or Partition
  -limit type value    Area, Disk, Map, or Memory resource limit
  -list type           Color, Configure, Delegate, Format, Magic, Module,
                       Resource, or Type
  -log format          format of debugging information
  -sampling-factor geometry
                       horizontal and vertical sampling factor
  -size geometry       width and height of image
  -strip               strip image of all profiles and comments
  -verbose             print detailed information about the image
  -version             print version information
  -virtual-pixel method
                       Constant, Edge, Mirror, or Tile
		',
		'UNKNOWN'             => 'identify: unable to open image `UNKNOWN\': No such file or directory.
identify: missing an image filename `UNKNOWN\'.',
		'test-compressed.tif' => 'identify: unknown field with tag 34665 (0x8769) ignored..
test-compressed.tif TIFF 50x50 DirectClass 8kb 0.110u 0:01',
		'test-multiscene.tif' => 'test-multiscene.tif[0] TIFF 100x75 DirectClass 45kb 0.000u 0:01
test-multiscene.tif[1] TIFF 100x75 DirectClass 45kb 0.000u 0:01',
		'test.bmp'            => 'test.bmp BMP 50x50 PseudoClass 15c 504 0.000u 0:01',
		'SIZED.gif'           => '%filename% GIF %width%x%height% %width%x%height%+0+0 PseudoClass 8c 232 0.000u 0:01',
		'test.gif'            => 'test.gif GIF 50x50 50x50+0+0 PseudoClass 8c 232 0.000u 0:01',
		'test.jpg'            => 'test.jpg JPEG 88x40 PseudoClass 256c 986 0.010u 0:01',
		'test.jp2'            => 'test.jp2 JP2 50x50 DirectClass 1kb 0.020u 0:01',
		'test.pdf'            => 'test.pdf[0] PDF 612x792 612x792+0+0 PseudoClass 256c 947kb 0.060u 0:01
test.pdf[1] PDF 612x792 612x792+0+0 PseudoClass 256c 947kb 0.030u 0:01',
		'test.pjpg'           => 'test.pjpg JPEG 50x50 DirectClass 970 0.010u 0:01',
		'test.png'            => 'test.png PNG 50x50 DirectClass 730 0.000u 0:01',
		'test.svg'            => 'test.svg SVG 595x842 DirectClass 8kb 0.020u 0:01',
		'test.tif'            => 'test.tif TIFF 50x50 PseudoClass 256c 3kb 0.000u 0:01',
		'test.eps'            => 'test.eps PS 43x18 43x18+0+0 PseudoClass 2c 170 0.000u 0:01',
		'test.psd'            => 'test.psd PSD 178x80+0+0 DirectClass 8-bit 127.1kb 0.000u 0:01',
		'test.wmf'            => 'test.wmf WMF 3x3 DirectClass 408 0.020u 0:01',
		'test.tga'            => 'test.tga TGA 3x3 DirectClass 54 0.020u 0:01',
		'testProfile.jpg'     => 'testProfile.jpg JPEG 40x30 DirectClass 30kb 0.010u 0:01',
		'-format'             => array(
			'cmyk.jpg' => 'DirectClassCMYK',
			'rgb_file' => 'DirectClassRGB',
		),
	),
);

/* --------------------------------------------------------------------------
 * ImageMagick 6.1.3
 * --------------------------------------------------------------------------
 */
$imEnvironment[] = array(
	'name'                 => 'ImageMagick',
	'version'              => '6.1.3',
	'binary'               => '',
	'mimeTypes'            => array(
		'image/gif',
		'image/jpeg',
		'image/pjpeg',
		'image/jp2',
		'image/jpg2',
		'image/jpx',
		'image/png',
		'image/tiff',
		'image/svg+xml',
		'image/bmp',
		'application/pdf',
		'application/postscript',
		'application/photoshop',
		'image/wmf',
		'image/tga',
		'image/jpeg-cmyk',
		'image/tiff-cmyk',
		'application/photoshop-cmyk',
	),
	'binaries'             => array(
		'animate',
		'compare',
		'composite',
		'conjure',
		'convert',
		'display',
		'identify',
		'import',
		'mogrify',
		'montage',
	),
	'removeMetaDataSwitch' => array('-strip'),
	'operations'           => array_merge(
		$_commonOperations,
		array(
			'thumbnail' => array(
				'convert' => array(
					'-size',
					'%width%x%height%',
					'-geometry',
					'%width%x%height%',
					'-strip',
					'%src%',
					'%dst%',
				),
			),
			'convert-to-image/jpeg' => array(
				'convert' => array('-colorspace', 'RGB', '%src%', '%dst%'),
			),
		)
	),
	'identify'             => array(
		''                    => 'Version: ImageMagick 6.1.3 11/01/04 Q16 http://www.imagemagick.org
Copyright: Copyright (C) 1999-2004 ImageMagick Studio LLC
Usage: identify [options ...] file [ [options ...] file ... ]
Where options include:
  -authenticate value  decrypt image with this password
  -channel type        Red, Green, Blue, Opacity, Index, Cyan, Yellow,
                       Magenta, Black, or All
  -debug events        display copious debugging information
  -define format:option
                       define one or more image format options
  -density geometry    horizontal and vertical density of the image
  -depth value         image depth
  -extract geometry    extract area from image
  -format "string"     output formatted image characteristics
  -fuzz distance       colors within this distance are considered equal
  -help                print program options
  -interlace type      None, Line, Plane, or Partition
  -limit type value    Area, Disk, Map, or Memory resource limit
  -list type           Color, Configure, Delegate, Format, Magic, Module,
                       Resource, or Type
  -log format          format of debugging information
  -sampling-factor geometry
                       horizontal and vertical sampling factor
  -size geometry       width and height of image
  -strip               strip image of all profiles and comments
  -units type          PixelsPerInch, PixelsPerCentimeter, or Undefined
  -verbose             print detailed information about the image
  -version             print version information
  -virtual-pixel method
                       Constant, Edge, Mirror, or Tile
		',
		'UNKNOWN'             => 'identify: unable to open image `UNKNOWN\': No such file or directory.
identify: missing an image filename `UNKNOWN\'.',
		'test-compressed.tif' => 'identify: unknown field with tag 34665 (0x8769) ignored..
test-compressed.tif TIFF 50x50 DirectClass 8kb 0.110u 0:01',
		'test-multiscene.tif' => 'test-multiscene.tif[0] TIFF 100x75 DirectClass 45kb 0.000u 0:01
test-multiscene.tif[1] TIFF 100x75 DirectClass 45kb 0.000u 0:01',
		'test.bmp'            => 'test.bmp BMP 50x50 PseudoClass 15c 504 0.000u 0:01',
		'SIZED.gif'           => '%filename% GIF %width%x%height% %width%x%height%+0+0 PseudoClass 8c 232 0.000u 0:01',
		'test.gif'            => 'test.gif GIF 50x50 50x50+0+0 PseudoClass 8c 232 0.000u 0:01',
		'test.jpg'            => 'test.jpg JPEG 88x40 PseudoClass 256c 986 0.010u 0:01',
		'test.jp2'            => 'test.jp2 JP2 50x50 DirectClass 1kb 0.020u 0:01',
		'test.pdf'            => 'test.pdf[0] PDF 612x792 612x792+0+0 PseudoClass 256c 947kb 0.060u 0:01
test.pdf[1] PDF 612x792 612x792+0+0 PseudoClass 256c 947kb 0.030u 0:01',
		'test.pjpg'           => 'test.pjpg JPEG 50x50 DirectClass 970 0.010u 0:01',
		'test.png'            => 'test.png PNG 50x50 DirectClass 730 0.000u 0:01',
		'test.svg'            => 'test.svg SVG 595x842 DirectClass 8kb 0.020u 0:01',
		'test.tif'            => 'test.tif TIFF 50x50 PseudoClass 256c 3kb 0.000u 0:01',
		'test.eps'            => 'test.eps PS 43x18 43x18+0+0 PseudoClass 2c 170 0.000u 0:01',
		'test.psd'            => 'test.psd PSD 178x80+0+0 DirectClass 8-bit 127.1kb 0.000u 0:01',
		'test.wmf'            => 'test.wmf WMF 3x3 DirectClass 408 0.020u 0:01',
		'test.tga'            => 'test.tga TGA 3x3 DirectClass 54 0.020u 0:01',
		'testProfile.jpg'     => 'testProfile.jpg JPEG 40x30 DirectClass 30kb 0.010u 0:01',
		'-format'             => array(
			'cmyk.jpg' => 'DirectClassCMYK',
			'rgb_file' => 'DirectClassRGB',
		),
	),
);

/* --------------------------------------------------------------------------
 * ImageMagick 6.2.7 (coalesce / deconstruct changes)
 * --------------------------------------------------------------------------
 */
$im627                 = $imEnvironment[count($imEnvironment) - 1];
$im627['version']      = '6.2.7';
$im627['identify'][''] = 'Version: ImageMagick 6.2.7 11/01/05 Q16 http://www.imagemagick.org
Copyright: Copyright (C) 1999-2004 ImageMagick Studio LLC';
$imEnvironment[]       = $im627;

/* --------------------------------------------------------------------------
 * GraphicsMagick 1.0.5
 * --------------------------------------------------------------------------
 */
$imEnvironment[] = array(
	'name'                 => 'GraphicsMagick',
	'version'              => '1.0.5',
	'binary'               => 'gm',
	'mimeTypes'            => array(
		'image/gif',
		'image/jpeg',
		'image/pjpeg',
		'image/png',
		'image/tiff',
		'image/svg+xml',
		'image/bmp',
		'application/pdf',
		'application/postscript',
	),
	'binaries'             => array(
		'animate',
		'composite',
		'conjure',
		'convert',
		'display',
		'identify',
		'import',
		'mogrify',
		'montage',
	),
	'removeMetaDataSwitch' => array('+profile', '*'),
	'operations'           => $_commonOperations,
	'identify'             => array(
		''                    => 'identify: Improper arguments supplied, please see manual.
Version: GraphicsMagick 1.0.5 November 26, 2004 Q8 http://www.GraphicsMagick.org/
Copyright: Copyright (C) 2002, 2003, 2004 GraphicsMagick Group
Usage: identify [options ...] file [ [options ...] file ... ]
Where options include:
  -authenticate value  decrypt image with this password
  -debug events        display copious debugging information
  -density geometry    horizontal and vertical density of the image
  -depth value         image depth
  -format "string"   output formatted image characteristics
  -help                print program options
  -interlace type      None, Line, Plane, or Partition
  -limit type value    Disk, Map, or Memory resource limit
  -log format          format of debugging information
  -size geometry       width and height of image
  -sampling-factor geometry
                       horizontal and vertical sampling factor
  -verbose             print detailed information about the image
  -version             print version information
  -virtual-pixel method
                       Constant, Edge, Mirror, or Tile
		',
		'UNKNOWN'             => 'identify: Unable to open file (NOTHING) [No such file or directory].',
		'test-compressed.tif' => 'identify: unknown field with tag 34665 (0x8769) ignored. (test-compressed.tif).
test-compressed.tif TIFF 50x50+0+0 DirectClass 8-bit 7.8k 0.000u 0:01',
		'test-multiscene.tif' => 'test-multiscene.tif[0] TIFF 100x75+0+0 DirectClass 8-bit 44.7k 0.010u 0:01
test-multiscene.tif[1] TIFF 100x75+0+0 DirectClass 8-bit 44.7k 0.000u 0:01',
		'test.bmp'            => 'test.bmp BMP 50x50+0+0 PseudoClass 15c 8-bit 504.0 0.000u 0:01',
		'SIZED.gif'           => '%filename% GIF %width%x%height%+0+0 PseudoClass 8c 8-bit 232b 0.000u 0:01',
		'test.gif'            => 'test.gif GIF 50x50+0+0 PseudoClass 8c 8-bit 232.0 0.000u 0:01',
		'test.jpg'            => 'test.jpg JPEG 50x50+0+0 DirectClass 8-bit 763.0 0.000u 0:01',
		'test.jp2'            => 'identify: No decode delegate for this image format (test.jp2).',
		'test.pdf'            => 'test.pdf[0] PDF 612x792+0+0 PseudoClass 256c 8-bit 946.8k 0.050u 0:01
test.pdf[1] PDF 612x792+0+0 PseudoClass 256c 8-bit 946.8k 0.030u 0:01',
		'test.pjpg'           => 'test.pjpg JPEG 50x50+0+0 DirectClass 8-bit 970.0 0.000u 0:01',
		'test.png'            => 'test.png PNG 50x50+0+0 DirectClass 8-bit 730.0 0.010u 0:01',
		'test.svg'            => 'test.svg SVG 595x841+0+0 DirectClass 16-bit 7.8kb 0.008u 0:01',
		'test.tif'            => 'test.tif TIFF 50x50+0+0 PseudoClass 256c 8-bit 2.7k 0.000u 0:01',
		'test.eps'            => 'test.eps PS 43x18+0+0 PseudoClass 2c 8-bit 170.0 0.000u 0:01',
		'test.wmf'            => 'identify: No decode delegate for this image format (test.wmf).',
		'test.tga'            => 'identify: No decode delegate for this image format (test.tga).',
		'testProfile.jpg'     => 'testProfile.jpg JPEG 40x30+0+0 DirectClass 8-bit 29.9k 0.000u 0:01',
		'-format'             => array(
			'cmyk.jpg' => 'DoesThisWork?',
		),
	),
);

