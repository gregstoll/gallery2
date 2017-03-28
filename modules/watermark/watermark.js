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
 * $Revision: 17580 $
 */
var background_el;
var background_width;
var background_height;
var background_dd;
var background_x;
var background_y;
var floater_dd;

function initWatermarkFloater(floater_id, background_id, watermark_width,
			      watermark_height, xPercentage, yPercentage) {
    floater_el = document.getElementById(floater_id);
    background_el = document.getElementById(background_id);
    background_width = background_el.width || pxToNumber(background_el.style.width);
    background_height = background_el.height || pxToNumber(background_el.style.height);
    background_dd = new YAHOO.util.DDTarget(background_id);
    background_x = YAHOO.util.Dom.getX(background_id);
    background_y = YAHOO.util.Dom.getY(background_id);
    floater_dd = new YAHOO.util.DD(floater_id);

    var scale = 1.0;
    if (watermark_width > (0.85 * background_width)) {
	scale = (0.85 * background_width) / watermark_width;
    }
    if (watermark_height > (0.85 * background_height)) {
	scale = Math.min(scale, (0.85 * background_height) / watermark_height);
    }

    floater_width = Math.round(watermark_width * scale);
    floater_height = Math.round(watermark_height * scale);

    floater_el.width = floater_width;
    floater_el.style.width = floater_width + "px";

    floater_el.height = floater_height;
    floater_el.style.height = floater_height + "px";

    var floater_x = YAHOO.util.Dom.getX(floater_id);
    var floater_y = YAHOO.util.Dom.getY(floater_id);

    YAHOO.util.Dom.setXY(
	floater_id,
	[ background_x + Math.round(xPercentage / 100 * (background_width - floater_el.width)),
	  background_y + Math.round(yPercentage / 100 * (background_height - floater_el.height))]);

    var floater_x_offset = (YAHOO.util.Dom.getX(floater_id) - background_x);
    var floater_y_offset = (YAHOO.util.Dom.getY(floater_id) - background_y);
    floater_dd.setXConstraint(floater_x_offset - 1,
	                      background_width - floater_x_offset - floater_width, 1);
    floater_dd.setYConstraint(floater_y_offset - 1,
	                      background_height - floater_y_offset - floater_height, 1);
}

function pxToNumber(str) {
    str = str.replace("px", "");
    return Number(str);
}

function calculatePercentages(floater_id) {
    var floater_el = document.getElementById(floater_id);
    var floater_x = YAHOO.util.Dom.getX(floater_id);
    var floater_y = YAHOO.util.Dom.getY(floater_id);

    /* We name these elements inconsistently */
    x_el = document.getElementById("xPercentage") || document.getElementById("xPercent");
    y_el = document.getElementById("yPercentage") || document.getElementById("yPercent");

    x_el.value = 100.0 * (floater_x - background_x) / (background_width - floater_el.width);
    y_el.value = 100.0 * (floater_y - background_y) / (background_height - floater_el.height);
}

