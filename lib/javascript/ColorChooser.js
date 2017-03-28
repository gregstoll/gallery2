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
/*
 * Author: Felix Rabinovich
 *         Based in large parts on yui/examples/slider/rgb2.html
 */
var hue;
var picker;
var dd;

function init() {
	hue = YAHOO.widget.Slider.getVertSlider("Markup_hueBg", "Markup_hueThumb", 0, 180);
	hue.onChange = function() { hueUpdate(); };

	picker = YAHOO.widget.Slider.getSliderRegion("Markup_pickerDiv", "Markup_selector", 
			0, 180, 0, 180);
	picker.onChange = function() { pickerUpdate(); };
	hueUpdate();

	dd = new YAHOO.util.DD("Markup_colorChooser");
	dd.setHandleElId("Markup_colorHandle");
	dd.endDrag = function(e) {	};
    // yuck. correctly handle PNG transparency in Win IE
    // also, the color it will be below SELECT elements (date pieces)
    // see http://www.codetoad.com/forum/20_22736.asp
    var isIE = !window.opera && navigator.userAgent.indexOf('MSIE') != -1;
    if (isIE) {
        var imgID = "Markup_pickerbg";
        var img = document.getElementById(imgID);
        var imgName = img.src.toUpperCase();
        var imgTitle = (img.title) ? "title='" + img.title + "' " : "title='" + img.alt + "' ";
        var strNewHTML = "<span id='" + imgID + "'" + imgTitle
            + " style=\"width:192px; height:192px;display:inline-block;"
            + "filter:progid:DXImageTransform.Microsoft.AlphaImageLoader"
            + "(src=\'" + img.src + "\', sizingMethod='scale');\"></span>"; 
        img.outerHTML = strNewHTML;
    }
}

// hue, int[0,359]
var getH = function() {
	var h = (180 - hue.getValue()) / 180;
	h = Math.round(h*360);
	return (h == 360) ? 0 : h;
}

// saturation, int[0,1], left to right
var getS = function() {
	return picker.getXValue() / 180;
}

// value, int[0,1], top to bottom
var getV = function() {
	return (180 - picker.getYValue()) / 180;
}

function pickerUpdate() {
	swatchUpdate();
}

function hueUpdate() {
	var rgb = YAHOO.util.Color.hsv2rgb( getH(), 1, 1);
	document.getElementById("Markup_pickerDiv").style.backgroundColor = 
		"rgb(" + rgb.join(",") + ")";

	swatchUpdate();
}

function swatchUpdate() {
    var h=getH(), s=getS(), v=getV();
	document.getElementById("Markup_hval").value = h;
	document.getElementById("Markup_sval").value = Math.round(s * 100);
	document.getElementById("Markup_vval").value = Math.round(v * 100);

	var rgb = YAHOO.util.Color.hsv2rgb( h, s, v );

	document.getElementById("Markup_swatch").style.backgroundColor = 
		"rgb(" + rgb.join(",") + ")";

	document.getElementById("Markup_rval").value = rgb[0];
	document.getElementById("Markup_gval").value = rgb[1];
	document.getElementById("Markup_bval").value = rgb[2];
	document.getElementById("Markup_hexval").value = 
		YAHOO.util.Color.rgb2hex(rgb[0], rgb[1], rgb[2]);
}

function userUpdate() {
	var colorChooser = document.getElementById("Markup_colorChooser");
    var element = document.getElementById(colorChooser.g2ElementId);
	var color = document.getElementById("Markup_hexval").value;
	element.value = element.value + '[color=#' + color + ']';
	colorChooser.style.display = 'none';
	if (typeof(element.selectionStart) != "undefined") {
	element.selectionStart = element.selectionEnd = element.value.length;
	}
	element.focus();
}

