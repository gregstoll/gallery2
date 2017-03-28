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

var thumbnails;			/* array of thumbnails */
var currentImageIndex;		/* index into thumbnails array */
var mainPhoto;			/* The central image */
var photoScalingInterval;	/* scaling animation tracker */
var showLoadingTimeout;		/* Cancel the loading message display */
var showImageDataFadeTimeout	/* Fade in image data block */

var MAIN_PHOTO_WIDTH = 460;	/* Width each image is resized to when first loaded */
var PHOTO_MINIMISED = 300;	/* Width each image is resized down to when clicked */
var SCALE_DOWN_FACTOR = 12;	/* Pixels that an image scales down by at each step */
var SCALE_UP_FACTOR = 18;	/* Pixels that an image scales *up* by at each step */

/* @REVISIT: For preloading... */
fullsizePhotos = new Array();
fullsizeCount = 0;

/* Initialisation */
addEvent(window, 'load', init, false);

function init() {
    if (!document.getElementById) {
	/* Required functionality for even the most basic DHTML. */
	return;
    }
    /* Set up thumbnails */
    var thumbholder = document.getElementById('gsThumbMatrix');
    if (!thumbholder) {
	setupBackToAlbumLink(); /* Not an album page */
	return;
    }
    thumbnails = thumbholder.getElementsByTagName('img');
    for (var i = 0; i < thumbnails.length; i++) {
	thumbnails[i].thumbIndex = i;
	addEvent(thumbnails[i], 'click', activateThumbnail, false);
	thumbnails[i].onclick = cancelEventSafari;
    }

    /*
     * If the user has just come from a photo page, they may have a cookie set that holds the index
     * of the image that was active in the show when they left. Now they can rejoin at that point.
     */
    if (readCookie('slideshowOffset') != null &&
	    Number(readCookie('slideshowOffset')) < thumbnails.length) {
	currentImageIndex = Number(readCookie('slideshowOffset'));
	eraseCookie('slideshowOffset');
	addMessage('Restarting slideshow at index ' + currentImageIndex);
    } else {
	currentImageIndex = 0;
    }

    if (slideshowImages.length > 0) {
      setupMainImage();
      switchImages(currentImageIndex, true);
    }

    setupSubAlbums();
}

/* Called each time a new image is switched to */
function setupMainImage() {
    mainPhoto = document.getElementById('main-image');

    if (slideshowImageWidths[currentImageIndex] < 0
	    || MAIN_PHOTO_WIDTH <= slideshowImageWidths[currentImageIndex]) {
	mainPhoto.style.width = MAIN_PHOTO_WIDTH + 'px';
    } else {
	/*
	 * The image is below our ideal slideshow width, so we don't want to stretch it
	 * horizontally.  Instead we center the image that we have a little better.
	 */
	mainPhoto.style.width = slideshowImageWidths[currentImageIndex] + 'px';
	mainPhoto.style.left = parseInt(getElementStyle(mainPhoto.id, 'left')) +
	    ((MAIN_PHOTO_WIDTH - slideshowImageWidths[currentImageIndex])/2) + 'px';
    }
    mainPhoto.style.height = 'auto'; /* @REVISIT - Safari doesn't play nice with this */

    mainPhoto.origWidth = parseInt(getElementStyle(mainPhoto.id, 'width'));
    mainPhoto.origLeft = parseInt(getElementStyle(mainPhoto.id, 'left'));
    mainPhoto.galleryimg = 'no'; /* Disables IE's image toolbar, which was causing flicker */

    addEvent(mainPhoto, 'click', showPhotoDetails, false);
}

function activateThumbnail(e) {
    /* Find the source element through varying levels of browser support */
    var targetElement = window.event ? window.event.srcElement : e ? e.target : null;
    if (!targetElement) {
	return;
    }
    knackerEvent(e);
    switchImages(targetElement.thumbIndex);
    targetElement.blur();	/* Remove focus halo */
}

function switchImages(thumbIndex, firstLoad) {
    removeImageDataBlock();
    if (currentImageIndex == thumbIndex && !firstLoad) {
	addMessage("That image is already selected.");
	return;
    }
    /* Wrap around */
    if (thumbIndex < 0) {
	thumbIndex = thumbnails.length - 1;
    } else if (thumbIndex == thumbnails.length) {
	thumbIndex = 0;
    }
    if (slideshowImageWidths[thumbIndex] < 0) {
	/* This item is not a photo.. redirect to view it */
	if (!firstLoad) {
	    location = slideshowImages[thumbIndex];
	} else {
	    hideLoadingMessage();
	}
	return;
    }
    highlightCurrentThumb(thumbIndex);
    updateItemTitle(thumbIndex);

    hideLoadingMessage();

    var newImageSrc = slideshowImages[thumbIndex];
    var newImage = document.createElement('img');

    mainPhoto.id = null; /* Isn't enough in some browsers */
    mainPhoto.removeAttribute('id');

    newImage.id = 'main-image';
    addEvent(newImage, 'load', bigPhotoLoad, false);
    addEvent(newImage, 'error', bigPhotoFail, false);

    if (slideshowState == PLAYING) {
	pause();
	returnToPlaying = true;
    }

    /* Avoid showing loading message if its going to be fast */
    showLoadingTimeout = setTimeout("showLoadingMessage()", 600);

    addMessage("Swapping " + currentImageIndex + ", and " + thumbIndex + "...");
    currentImageIndex = thumbIndex;

    newImage.src = newImageSrc;
    mainPhoto.parentNode.removeChild(mainPhoto);
    document.getElementById('sliding-frame').getElementsByTagName('p')[0].appendChild(newImage);
    setupMainImage();

    if (mainPhoto.state == 'downsized') {
	insertimagedatablock();
    }
}

function bigPhotoLoad() {
    mainPhoto.style.height = 'auto';
    addMessage('Photo loaded');
    hideLoadingMessage();

    /*
     * The little switcheroo below prevents a problem with IE incorrectly calculating the 'height'
     * property for an image that has had its width changed when set to 'auto' (they should scale
     * together). Should be imperceptible in other browsers.
     */
    mainPhoto.style.height = '345px';
    mainPhoto.style.height = 'auto';
    if (returnToPlaying) {
	play();
    }
}

function bigPhotoFail() {
    addMessage('FAIL: Photo failed to load...');
    addMessage(mainPhoto.src);
    hideLoadingMessage();
    if (slideshowState == PLAYING) {
	advanceSlideShow();
    }
}

function showLoadingMessage() {
    var loadingMsg = document.createElement('div');
    loadingMsg.id = 'loading';
    loadingMsg.appendChild(document.createTextNode(LOADING_IMAGE));
    document.getElementById('main-image-container').appendChild(loadingMsg);
}

function hideLoadingMessage() {
    clearTimeout(showLoadingTimeout);
    var loading = document.getElementById('loading');
    if (loading && getElementStyle(loading.id, '-moz-opacity') > 0.0) {
	loading.style.MozOpacity = parseFloat(getElementStyle(loading.id, '-moz-opacity')) - 0.05;
	showLoadingTimeout = setTimeout("hideLoadingMessage()", 50);
    } else if (loading) {
	loading.parentNode.removeChild(loading);
    }
}

function highlightCurrentThumb(thumbIndex) {
    /* First, un-highlight the current image... */
    thumbnails[currentImageIndex].className =
	thumbnails[currentImageIndex].className.replace(/\bcurrentImage\b/, '');
    thumbnails[thumbIndex].className += ' currentImage';
}

function updateItemTitle(thumbIndex) {
    var itemTitle = document.getElementById('item-title').firstChild;
    itemTitle.data = thumbnails[thumbIndex].getAttribute('alt');
}

/*
 * Photo -scaling functions
 */
function showPhotoDetails(e) {
    addMessage('scaling image, dimensions: ' + mainPhoto.origWidth + 'x' + mainPhoto.origHeight);
    clearInterval(photoScalingInterval);
    if (slideshowState == PLAYING) {
	pause();
	returnToPlaying = true;
    }

    if (mainPhoto.state != 'downsized') {
	photoScalingInterval = setInterval("downSizePhoto()", 30);
	mainPhoto.state = 'downsized';
    } else {
	removeImageDataBlock();
	photoScalingInterval = setInterval("upSizePhoto()", 20);
	mainPhoto.state = 'upsized';
    }
}

function downSizePhoto() {
    if (parseInt(mainPhoto.style.width) > PHOTO_MINIMISED ||
	    parseInt(getElementStyle(mainPhoto.id, 'left')) > 40) {
	if (parseInt(mainPhoto.style.width) > PHOTO_MINIMISED) {
	    mainPhoto.style.width = parseInt(mainPhoto.style.width) - SCALE_DOWN_FACTOR + 'px';
	}

	/* REVISIT Works in everything but Safari... */
	mainPhoto.style.height = 'auto';

	if (parseInt(getElementStyle(mainPhoto.id, 'left')) > 40) {
	    mainPhoto.style.left = (parseInt(getElementStyle(mainPhoto.id, 'left')) -
		mainPhoto.origLeft/12) + 'px';
	}
	if (getElementStyle(mainPhoto.id, 'top') == 'auto') {
	    mainPhoto.style.top = '0';
	}
	if (parseInt(getElementStyle(mainPhoto.id, 'top')) < 40) {
	    mainPhoto.style.top = (parseInt(getElementStyle(mainPhoto.id, 'top')) + 5) + 'px';
	}
    } else {
	clearInterval(photoScalingInterval);
	addMessage('Image scaled to ' + mainPhoto.style.width + 'x' + mainPhoto.style.height);
	mainPhoto.state = 'downsized';
	insertimagedatablock();
    }
}

function upSizePhoto() {
    if (parseInt(mainPhoto.style.width) < mainPhoto.origWidth ||
	    parseInt(getElementStyle(mainPhoto.id, 'left')) < mainPhoto.origLeft) {
	if (parseInt(mainPhoto.style.width) < mainPhoto.origWidth-5) {
	    mainPhoto.style.width = (parseInt(mainPhoto.style.width) + SCALE_UP_FACTOR) + 'px';
	}
	if (parseInt(getElementStyle(mainPhoto.id, 'left')) < mainPhoto.origLeft) {
	    mainPhoto.style.left = (parseInt(getElementStyle(mainPhoto.id, 'left')) +
		mainPhoto.origLeft/10) + 'px';
	}
	if (parseInt(getElementStyle(mainPhoto.id, 'top')) > 0) {
	    mainPhoto.style.top = (parseInt(getElementStyle(mainPhoto.id, 'top')) - 5) + 'px';
	}
    } else {
	/* Clean up and reset */
	clearInterval(photoScalingInterval);
	mainPhoto.style.width = mainPhoto.origWidth + 'px';
	mainPhoto.style.height = 'auto';
	mainPhoto.style.left = mainPhoto.origLeft + 'px';
	mainPhoto.style.top = 'auto';
	addMessage('Image scaled back to ' + mainPhoto.width + 'x' + mainPhoto.height);
	mainPhoto.state = 'upsized';
	if (returnToPlaying) {
	    play();
	}
    }
}

function insertimagedatablock() {
    var dataBlock = document.createElement('div');
    dataBlock.id = 'imagedatablock';

    var dataBlockHeading = document.createElement('h3');
    dataBlockHeading.appendChild(document.createTextNode(PHOTO_DATA));
    dataBlock.appendChild(dataBlockHeading);

    if (thumbnails[currentImageIndex].className.match(/size:=([0-9]+)=/)) {
	var imageSize = Number(RegExp.$1) / 1024;
	var sizeInfo = document.createElement('p');
	sizeInfo.appendChild(
	    document.createTextNode(FILE_SIZE.replace(/%SIZE%/, Math.round(imageSize))));
	dataBlock.appendChild(sizeInfo);
    }
    if (thumbnails[currentImageIndex].className.match(/summary:=([^=]+)=/)) {
	var summaryInfo = document.createElement('p');
	summaryInfo.innerHTML = SUMMARY + RegExp.$1;
	dataBlock.appendChild(summaryInfo);
    }
    if (thumbnails[currentImageIndex].className.match(/description:=(.+)=/)) {
	var descriptionInfo = document.createElement('p');
	descriptionInfo.innerHTML = DESCRIPTION + RegExp.$1;
	dataBlock.appendChild(descriptionInfo);
    }

    var fullsizeLink = document.createElement('a');
    fullsizeLink.id = 'fullsizelink';
    fullsizeLink.href = thumbnails[currentImageIndex].parentNode.href
	+ (thumbnails[currentImageIndex].parentNode.href.indexOf('?') >= 0 ? '&' : '?')
	+ 'thumbIndex=' + currentImageIndex;
    /* Allow a user to rejoin the slideshow at this point */
    addEvent(fullsizeLink, 'click', saveCurrentImageIndex, false);

    fullsizeLink.appendChild(document.createTextNode(VIEW_IMAGE));
    var linkPara = document.createElement('p');
    linkPara.appendChild(fullsizeLink);
    dataBlock.appendChild(linkPara);

    /* In Gecko-based browsers, this'll fade the data block in smoothly. */
    dataBlock.style.MozOpacity = 0.2;
    showImageDataFadeTimeout = setTimeout('fadeInImageDataBlock()', 50);

    document.getElementById('main-image-container').appendChild(dataBlock);
}

function fadeInImageDataBlock() {
    clearTimeout(showImageDataFadeTimeout);
    var dataBlock = document.getElementById('imagedatablock');
    /* Don't bring -moz-opacity all the way to 1.0, as it causes flicker in Firefox 1.0 */
    if (dataBlock && getElementStyle(dataBlock.id, '-moz-opacity') < 0.92) {
	dataBlock.style.MozOpacity = parseFloat(getElementStyle(dataBlock.id, '-moz-opacity')) + 0.08;
	showImageDataFadeTimeout = setTimeout('fadeInImageDataBlock()', 50);
    }
}

function removeImageDataBlock() {
    var dataBlock = document.getElementById('imagedatablock');
    if (dataBlock) {
	dataBlock.parentNode.removeChild(dataBlock);
    }
}

/* Subalbums table */

function setupSubAlbums() {
    /* First check if there is a sub-album table */
    if (!document.getElementById('gsSubAlbumMatrix')) {
	return;
    }
    var subs = document.getElementById('gsSubAlbumMatrix').getElementsByTagName('td');
    for (var i = 0; i < subs.length; i++) {
	addEvent(subs[i], 'click', selectSubAlbum, false);
	var hasLink = subs[i].getElementsByTagName('a')[0];
	if (hasLink) {
	    addEvent(subs[i], 'mouseover', highlightSubAlbum, true);
	    addEvent(subs[i], 'mouseout', unHighlightSubAlbum, true);
	}
    }
}

function highlightSubAlbum(e) {
    /* Find the source element through varying levels of browser support */
    var targetElement = window.event ? window.event.srcElement : e ? e.target : null;
    if (!targetElement) {
	return;
    }
    while (targetElement.nodeName.toLowerCase() != 'td' &&
	   targetElement.nodeName.toLowerCase() != 'body') {
	targetElement = targetElement.parentNode;
    }
    if (targetElement.nodeName.toLowerCase() == 'body') {
	return;
    }
    targetElement.className += ' sahover';
}

function unHighlightSubAlbum(e) {
    /* Find the source element through varying levels of browser support */
    var targetElement = window.event ? window.event.srcElement : e ? e.target : null;
    if (!targetElement) {
	return;
    }
    while (targetElement.nodeName.toLowerCase() != 'td' &&
	   targetElement.nodeName.toLowerCase() != 'body') {
	targetElement = targetElement.parentNode;
    }
    if (targetElement.nodeName.toLowerCase() == 'body') {
	return;
    }
    targetElement.className = targetElement.className.replace(/\bsahover\b/, '');
}

function selectSubAlbum(e) {
    /* Find the source element through varying levels of browser support */
    var targetElement = window.event ? window.event.srcElement : e ? e.target : null;
    if (!targetElement) {
	return;
    }
    knackerEvent(e);

    while (targetElement.nodeName.toLowerCase() != 'td' &&
	   targetElement.nodeName.toLowerCase() != 'body') {
	targetElement = targetElement.parentNode;
    }
    if (targetElement.nodeName.toLowerCase() == 'body') {
	return;
    }
    eraseCookie('slideshowOffset');
    var firstlink = targetElement.getElementsByTagName('a')[0];
    if (firstlink) {
	window.location = firstlink.href;
    }
}

/* Photo page functions */

function setupBackToAlbumLink() {
    var pageLinks = document.getElementsByTagName('a');
    for (var i = 0; i < pageLinks.length; i++) {
	if (pageLinks[i].className.match(/\bbacktoalbum\b/)) {
	    addEvent(pageLinks[i], 'click', saveCurrentImageIndex, false);
	}
    }
}

function saveCurrentImageIndex() {
    if (window.location.toString().match(/thumbIndex=([0-9]+)/)) {
	createCookie('slideshowOffset', RegExp.$1);
    } else if (currentImageIndex) {
	createCookie('slideshowOffset', currentImageIndex);
    }
}
