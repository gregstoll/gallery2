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
function rateItem(itemId, rating, url) {
    url = url.replace('__AUTHTOKEN__', galleryAuthToken);
    YAHOO.util.Connect.asyncRequest(
	'GET', url, {success: handleRatingResponse, failure: null, scope: null}, null);
}

function handleRatingResponse(http) {
    var results = http.responseText.split("\n");
    updateItemRating(results);
}

function updateItemRating(results) {
    var itemId = results[0];
    var rating = results[1];
    var votes = results[2];
    var userRating = results[3];
    galleryAuthToken = results[4];

    updateElementDisplay('rating.rating.' + itemId, rating);
    updateElementDisplay('rating.votes.' + itemId, votes);
    updateElementDisplay('rating.userRating.' + itemId, userRating);
    updateAveragePercent(itemId, rating * 100 / 5);

    resetStarDisplay(itemId);
}

function resetStarDisplay(itemId) {
    var userRating = document.getElementById('rating.userRating.' + itemId).innerHTML;
    updateStarDisplay(itemId, userRating);
}

function updateAveragePercent(itemId, averagePercent) {
    var e = document.getElementById('rating.averagePercent.' + itemId);
    e.style.width = averagePercent + "%";
}

function updateStarDisplay(itemId, userRating) {
    var rating = document.getElementById('rating.rating.' + itemId).innerHTML;
    for (i=1; i<=5; i++) {
	var star = document.getElementById('rating.star.' + itemId + '.' + i);
	if (userRating != 'N/A') {
	    if ((rating >= i) && (userRating >= i)) {
		star.className='giRatingFullUserYes';
	    } else if ((rating >= i) && (userRating < i)) {
		star.className='giRatingFullUserNo';
	    } else if ((rating < i) && (userRating >= i)) {
		if (i - rating < 1) {
		    star.className='giRatingHalfUserYes';
		} else {
		    star.className='giRatingEmptyUserYes';
		}
	    } else if ((rating < i) && (userRating < i)) {
		if (i - rating < 1) {
		    star.className='giRatingHalfUserNo';
		} else {
		    star.className='giRatingEmpty';
		}
	    }
	} else {
	    if (rating >= i) {
		star.className='giRatingFullUserNo';
	    } else if (i - rating < 1) {
		star.className='giRatingHalfUserNo';
	    } else {
		star.className='giRatingEmpty';
	    }
	}
    }
}

function updateElementDisplay(id, str) {
    document.getElementById(id).replaceChild(
	    document.createTextNode(str),
	    document.getElementById(id).childNodes[0]);
}
