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

var DEBUG_MESSAGES = 8;
var msgnumber = 1;

/* Cross-browser event handling, by Scott Andrew */
function addEvent(element, eventType, lamdaFunction, useCapture) {
    if (element.addEventListener) {
	element.addEventListener(eventType, lamdaFunction, useCapture);
	return true;
    } else if (element.attachEvent) {
	var r = element.attachEvent('on' + eventType, lamdaFunction);
	return r;
    } else {
	return false;
    }
}

/* Kills an event's propagation and default action */
function knackerEvent(eventObject) {
    if (eventObject && eventObject.stopPropagation) {
	eventObject.stopPropagation();
    }
    if (window.event && window.event.cancelBubble) {
	window.event.cancelBubble = true;
    }

    if (eventObject && eventObject.preventDefault) {
	eventObject.preventDefault();
    }
    if (window.event) {
	window.event.returnValue = false;
    }
}

/*
 * Safari doesn't support canceling events in the standard way, so we must
 * hard-code an event return false for it to work.
 */
function cancelEventSafari() {
    return false;
}

/*
 * Cross-browser style extraction, from the JavaScript & DHTML Cookbook
 * <http://www.oreillynet.com/pub/a/javascript/excerpt/JSDHTMLCkbk_chap5/index5.html>
 */
function getElementStyle(elementID, CssStyleProperty) {
    var element = document.getElementById(elementID);
    if (element.currentStyle) {
	return element.currentStyle[toCamelCase(CssStyleProperty)];
    } else if (window.getComputedStyle) {
	var compStyle = window.getComputedStyle(element, '');
	return compStyle.getPropertyValue(CssStyleProperty);
    } else {
	return '';
    }
}

function createCookie(name, value, days) {
    var expires = '';
    if (days) {
	var date = new Date();
	date.setTime(date.getTime() + (days*24*60*60*1000));
	var expires = '; expires=' + date.toGMTString();
    }
    document.cookie = name + '=' + value + expires + '; path=/';
}

function readCookie(name) {
    var cookieCrumbs = document.cookie.split(';');
    var nameToFind = name + '=';
    for (var i = 0; i < cookieCrumbs.length; i++) {
	var crumb = cookieCrumbs[i];
	while (crumb.charAt(0) == ' ') {
	    crumb = crumb.substring(1, crumb.length); /* delete spaces */
	}
	if (crumb.indexOf(nameToFind) == 0) {
	    return crumb.substring(nameToFind.length, crumb.length);
	}
    }
    return null;
}

function eraseCookie(name) {
    createCookie(name, '', -1);
}

/*
 * CamelCases CSS property names.
 * From <http://dhtmlkitchen.com/learn/js/setstyle/index4.jsp>
 */
function toCamelCase(CssProperty) {
    var stringArray = CssProperty.toLowerCase().split('-');
    if (stringArray.length == 1) {
	return stringArray[0];
    }
    var ret = (CssProperty.indexOf("-") == 0)
	      ? stringArray[0].charAt(0).toUpperCase() + stringArray[0].substring(1)
	      : stringArray[0];
    for (var i = 1; i < stringArray.length; i++) {
	var s = stringArray[i];
	ret += s.charAt(0).toUpperCase() + s.substring(1);
    }
    return ret;
}

/* Adds a status message to the page; very useful for debugging without alerts */
function addMessage(msg) {
    if (DEBUG_AJAXIAN) {
	var msgarea = document.getElementById('msgarea');
	if (msgarea) {
	    msgarea.style.display = 'block';
	    if (msgarea.getElementsByTagName('h4').length < 1) {
		var debugHeading = document.createElement('h4');
		debugHeading.appendChild(document.createTextNode('Debug output'));
		msgarea.appendChild(debugHeading);
	    }
	    if (msgarea.getElementsByTagName('p').length > DEBUG_MESSAGES) {
		msgarea.removeChild(msgarea.getElementsByTagName('p')[0]);
	    }
	    var note = document.createElement('p');
	    note.appendChild(document.createTextNode(msgnumber + ': ' + msg));
	    msgnumber++;
	    msgarea.appendChild(note);
	}
    }
}
