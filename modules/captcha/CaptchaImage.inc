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

/**
 * Display the current captcha image for this session
 * @package Captcha
 * @subpackage UserInterface
 * @author Stefan Ioachim <stefanioachim@gmail.com>
 * @author Bharat Mediratta <bharat@menalto.com>
 * @version $Revision: 17580 $
 */
class CaptchaImageView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
	return true;
    }

    /**
     * @see GalleryView::isAllowedInMaintenance
     */
    function isAllowedInMaintenance() {
	return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
	global $gallery;
	$session =& $gallery->getSession();
	$code = $session->get('captcha.key');
	if (empty($code)) {
	    GalleryCoreApi::requireOnce('modules/captcha/classes/CaptchaHelper.class');
	    CaptchaHelper::generateCaptchaSessionKey();
	    $code = $session->get('captcha.key');
	    if (!$session->getId()) {
		$session->start();
	    }
	}

	header('Content-type: image/jpeg');
	header('Pragma: No-cache');

	/* Load the default background, black&white static */
	$image = @ImageCreateFromJPEG(dirname(__FILE__) . '/data/code_bg.jpg');
	$width = imagesx($image);
	$height = imagesy($image);

	/* Draw some colored ellipses at random to perturb the background */
	for ($i = 0; $i < 40; $i++) {
	    /* Pick a random 50-75% lightness color for this ellipse */
	    $color = ImageColorAllocate($image, rand(128, 192), rand(128, 192), rand(128, 192));
	    ImageEllipse($image, rand(-15, $width - 5), rand(-15, $height - 5),
			 rand(20, $width), rand(20, $height), $color);
	}

	/* With characters 16 pixels apart, figure out the edge padding to center the whole lot */
	$textX = ($width - 16 * strlen($code)) / 2;
	$textY = ($height - 12 * strlen($code)) / 2 - 3;

	/* Draw the code into the image, diagonally with some random displacement */
	for ($i = 0; $i < strlen($code); $i++) {
	    /* Pick a random 25-50% lightness color for this letter */
	    $color = ImageColorAllocate($image, rand(60, 120), rand(60, 120), rand(60, 120));
	    ImageString($image, 5, $textX + $i*16 + rand(-2, 2), $textY + $i*12 + rand(-5, 5),
			substr($code, $i, 1), $color);
	}

	/* Output the image and reclaim the memory it used */
	/* Use low quality jpeg compression to make the image less OCR-able */
	ImageJPEG($image, '', 50);
	ImageDestroy($image);

	return null;
    }
}
?>
