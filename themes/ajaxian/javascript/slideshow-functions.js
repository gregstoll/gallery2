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

/* Global Variables */

/* play/pause state of slideshow */
var slideshowState;
var PAUSED = 0;
var PLAYING = 1;
/* After an image load, should the slideshow continue? */
var returnToPlaying;

/* Number of seconds before the images switch */
var SECONDS_BETWEEN_IMAGES = 4;
/* Tracks the time between images in the show */
var slideShowInterval;

/* Easy access to slideshow controls elements */
var controls;
/* Tracks the hiding/showing of the controls */
var showControlsInterval;
var hideControlsInterval;
/* vertical offset in pixels for controls */
var CONTROLS_HIDDEN_DEPTH = -35;

/* Initialisation */
addEvent(window, 'load', slideShow, false);

function slideShow() {
    if (!document.getElementById) {
	return;
    }
    var playpause = document.getElementById('controls-play');
    if (!playpause) {
	return;
    }

    /* Set up controls */
    addEvent(playpause, 'click', togglePlayPause, false);
    addEvent(document.getElementById('controls-left'), 'click', skipLeft, false);
    addEvent(document.getElementById('controls-right'), 'click', skipRight, false);

    /* Add animation to controls and thumbs */
    addEvent(document.getElementById('slideshow-controls'), 'mousemove', showControls, false);
    addEvent(document.getElementById('slideshow-controls'), 'mouseout', hideControlBar, false);

    /* Load in controls and thumbs */
    showControls();

    /* Set initial state */
    pause();
    returnToPlaying = false;
}

function advanceSlideShow() {
    if (slideshowState == PLAYING) {
	switchImages(currentImageIndex + 1);
    }
}

function play() {
    slideshowState = PLAYING;
    resetInterval();
    returnToPlaying = true;
    /* Swap Images */
    var theImage =
	document.getElementById('controls-play').getElementsByTagName('img')[0];
    theImage.src = theImage.src.replace(/play/, 'pause');
}

function pause() {
    slideshowState = PAUSED;
    clearInterval(slideShowInterval);
    returnToPlaying = false;
    /* Swap Images */
    var theImage =
	document.getElementById('controls-play').getElementsByTagName('img')[0];
    theImage.src = theImage.src.replace(/pause/, 'play');
}

function resetInterval() {
    clearInterval(slideShowInterval);	/* Just in case... */
    slideShowInterval =
	setInterval("advanceSlideShow()", SECONDS_BETWEEN_IMAGES * 1000);
}

function skipLeft() {
    switchImages(currentImageIndex - 1);
    resetInterval();
}

function skipRight() {
    switchImages(currentImageIndex + 1);
    resetInterval();
}

function togglePlayPause() {
    if (slideshowState == PAUSED) {
	play();
    } else {
	pause();
    }
}

function showControls() {
    controls = document.getElementById('control-buttons');
    clearInterval(showControlsInterval);
    clearInterval(hideControlsInterval);
    controls.state = 'showing';
    if (controls.state != 'shown') {
	showControlsInterval = setInterval('animateControlShow()', 15);
    }
}

function hideControlBar() {
    clearInterval(hideControlsInterval);
    /*
     * Pass an anonymous function to setTimeout that is only executed after
     * 1500 milliseconds has passed. Mousing over the controls' action area
     * within this time will reset the timeout.
     */
    hideControlsInterval = setTimeout(function() {
	if (controls.state == 'shown' || controls.state == 'showing') {
	  hideControlsInterval = setInterval('animateControlHide()', 50);
	}
    }, 1500);
}

function animateControlShow() {
    var pos = parseInt(getElementStyle(controls.id, 'top'));
    if (pos < 0) {
	controls.style.top = (pos + 3) + 'px';
    } else {
	clearInterval(showControlsInterval);
	controls.state = 'shown';
    }
}

function animateControlHide() {
    var pos = parseInt(getElementStyle(controls.id, 'top'));
    if (pos > CONTROLS_HIDDEN_DEPTH) {
	controls.style.top = (pos - 3) + 'px';
    } else {
	clearInterval(hideControlsInterval);
	controls.state = 'hidden';
    }
}

