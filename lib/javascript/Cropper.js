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

/**
 * ================================================================================
 * CropHandle
 * ================================================================================
 */

CropHandle = function(image_id, frame_id, handle_id) {
    this.init(frame_id);
    this.setHandleElId(handle_id);

    this.image_region = YAHOO.util.Dom.getRegion(image_id);
    this.image_region.width = this.image_region.right - this.image_region.left;
    this.image_region.height = this.image_region.bottom - this.image_region.top;

    this.handle_region = YAHOO.util.Dom.getRegion(handle_id);
    this.handle_region.width = this.handle_region.right - this.handle_region.left;
    this.handle_region.height = this.handle_region.bottom - this.handle_region.top;
}

CropHandle.prototype = new YAHOO.util.DragDrop();

CropHandle.prototype.setFrame = function(frame) {
    this.frame = frame;
}

CropHandle.prototype.onMouseDown = function(e) {
    var panel = this.getEl();
    this.startWidth = panel.offsetWidth;
    this.startHeight = panel.offsetHeight;
    this.startPos = [YAHOO.util.Event.getPageX(e), YAHOO.util.Event.getPageY(e)];
}

CropHandle.prototype.onDrag = function(e) {
    var dragTo = [YAHOO.util.Event.getPageX(e), YAHOO.util.Event.getPageY(e)];
    var deltaX = dragTo[0] - this.startPos[0];
    var deltaY = dragTo[1] - this.startPos[1];

    dims = this.frame.getDimensions();
    this.frame.reshapeTo(dims.top,
			 dims.left + this.startWidth + deltaX,
			 dims.top + this.startHeight + deltaY,
			 dims.left);
    this.frame.constrainLocation(true);
    this.frame.constrainShape(Math.abs(deltaX) > Math.abs(deltaY));
};

/**
 * ================================================================================
 * CropFrame
 * ================================================================================
 */

CropFrame = function(image_id, frame_id) {
    this.base = YAHOO.util.DD;
    this.base(frame_id);

    // Don't make the browser scroll if we drag off of the image to avoid flicker.
    this.scroll = false;

    this.frame_id = frame_id;
    this.aspectRatio = [];

    border = 0;
    raw_border = YAHOO.util.Dom.getStyle(frame_id, "border");
    if (raw_border) {
	if (result = raw_border.match(/(\d+)/)) {
	    border = result[0];
	}
    }
    this.border = border;
    YAHOO.util.Event.addListener(this.frame_id, "dblclick", this.maximize, this);
}

CropFrame.prototype = new YAHOO.util.DD();

CropFrame.prototype.reshapeTo = function(top, right, bottom, left) {
    YAHOO.util.Dom.setStyle(this.frame_id, "width", (right - left) + "px");
    YAHOO.util.Dom.setStyle(this.frame_id, "height", (bottom - top) + "px");
    YAHOO.util.Dom.setXY(this.frame_id, [left, top]);
}

CropFrame.prototype.maximize = function(e, frame) {
    var image = frame.image.getDimensions();
    var orig_dims = frame.getDimensions();

    // Start by taking up the whole frame
    frame.reshapeTo(image.top, image.right, image.bottom, image.left);
    frame.constrainShape(true);

    // Try to stay close to the original location
    var new_dims = frame.getDimensions();
    frame.reshapeTo(orig_dims.top, orig_dims.left + new_dims.width,
		    orig_dims.top + new_dims.height, orig_dims.left);
    frame.constrainLocation(false);
}

CropFrame.prototype.constrainLocation = function(adjustSize) {
    var dims = this.getDimensions();
    var image = this.image.getDimensions();

    if (dims.top < image.top) {
	var delta = image.top - dims.top;
	dims.top = dims.top + delta;
	dims.bottom = dims.bottom + delta;
    }

    if (dims.right > image.right) {
	if (adjustSize) {
	    dims.right = image.right;
	} else {
	    var delta = image.right - dims.right;
	    dims.left = dims.left + delta;
	    dims.right = dims.right + delta;
	}
    }

    if (dims.bottom > image.bottom) {
	if (adjustSize) {
	    dims.bottom = image.bottom;
	} else {
	    var delta = image.bottom - dims.bottom;
	    dims.top = dims.top + delta;
	    dims.bottom = dims.bottom + delta;
	}
    }

    if (dims.left < image.left) {
	var delta = image.left - dims.left;
	dims.left = dims.left + delta;
	dims.right = dims.right + delta;
    }

    this.reshapeTo(dims.top, dims.right, dims.bottom, dims.left);
}

CropFrame.prototype.constrainShape = function(adjustHeight) {
    var dims = this.getDimensions();
    var image = this.image.getDimensions();

    var currentAspect = dims.width / top;
    if (currentAspect != this.aspectRatio) {
	if (adjustHeight) {
	    dims.bottom = dims.top + dims.width / this.aspectRatio;
	} else {
	    dims.right = dims.left + dims.height * this.aspectRatio;
	}
    }

    if (dims.bottom + this.border * 2 > image.bottom) {
	dims.bottom = image.bottom - this.border * 2;
	dims.height = dims.bottom - dims.top;
	dims.right = dims.left + dims.height * this.aspectRatio;
    }

    if (dims.right + this.border * 2 > image.right) {
	dims.right = image.right - this.border * 2;
	dims.width = dims.right - dims.left;
	dims.bottom = dims.top + dims.width / this.aspectRatio;
    }

    this.reshapeTo(dims.top, dims.right, dims.bottom, dims.left);
}

CropFrame.prototype.setAspectRatio = function(width, height) {
    this.aspectWidth = width;
    this.aspectHeight = height;

    this.aspectRatio = width / height;
    this.constrainShape(true);
    this.constrainLocation(true);
    this.constrainShape(false);
    this.constrainLocation(false);
}

CropFrame.prototype.setOrientation = function(orientation) {
    if (orientation != this.orientation) {
	if (this.orientation) {
	    var dims = this.getDimensions();
	    this.reshapeTo(dims.top, dims.left + dims.height, dims.top + dims.width, dims.left);
	    this.setAspectRatio(this.aspectHeight, this.aspectWidth);
	}
	this.orientation = orientation;
    }
}

CropFrame.prototype.setImage = function(image) {
    this.image = image;
}

CropFrame.prototype.getDimensions = function() {
    var dims = YAHOO.util.Dom.getRegion(this.frame_id);

    /* Account for the border */
    dims.right = dims.right - this.border * 2;
    dims.bottom = dims.bottom - this.border * 2;

    dims.width = dims.right - dims.left;
    dims.height = dims.bottom - dims.top;
    return dims;
}

CropFrame.prototype.onDrag = function(e) {
    this.constrainLocation(false);
}

/**
 * ================================================================================
 * CropImage
 * ================================================================================
 */

CropImage = function(canvas_id, image_id, image_url, image_width, image_height) {
    this.image_id = image_id;

    var canvas_dims = YAHOO.util.Dom.getRegion(canvas_id);
    canvas_dims.width = canvas_dims.right - canvas_dims.left;
    canvas_dims.height = canvas_dims.bottom - canvas_dims.top;

    var aspect = image_width / image_height;
    var frame = {'width': image_width, 'height': image_height};
    if (image_width > image_height) {
      if (image_width > canvas_dims.width) {
        frame.width = canvas_dims.width;
	frame.height = canvas_dims.width / aspect;
      }
    } else {
      if (image_height > canvas_dims.height) {
        frame.height = canvas_dims.height;
	frame.width = canvas_dims.height * aspect;
      }
    }
    frame.left = Math.round(canvas_dims.left + ((canvas_dims.width - frame.width) / 2));
    frame.top = Math.round(canvas_dims.top + ((canvas_dims.height - frame.height) / 2));

    this.scale = frame.width / image_width;

    var image_el = document.getElementById(image_id);
    YAHOO.util.Dom.setXY(image_id, [frame.left, frame.top]);
    YAHOO.util.Dom.setStyle(image_id, "width", frame.width + "px");
    YAHOO.util.Dom.setStyle(image_id, "height", frame.height + "px");
    image_el.src = image_url;
}

CropImage.prototype.getDimensions = function() {
    var dims = YAHOO.util.Dom.getRegion(this.image_id);
    dims.width = dims.right - dims.left;
    dims.height = dims.bottom - dims.top;
    return dims;
}

CropImage.prototype.getScale = function() {
    return this.scale;
}

/**
 * ================================================================================
 * Cropper
 * ================================================================================
 */

Cropper = function(image, frame, handle) {
    this.image = image;
    this.frame = frame;
    this.handle = handle;

    this.frame.setImage(image);
    this.handle.setFrame(this.frame);
    this.frame.addInvalidHandleId(this.handle.handleElId);
}

Cropper.prototype.getFrameDimensions = function() {
    var frame_dims = this.frame.getDimensions();
    var image_dims = this.image.getDimensions();
    var scale = this.image.getScale();

    dims = {
	'top': Math.round((frame_dims.top - image_dims.top) / scale),
	'right': Math.round((frame_dims.right - image_dims.left) / scale),
	'bottom': Math.round((frame_dims.bottom - image_dims.top) / scale),
	'left': Math.round((frame_dims.left - image_dims.left) / scale),
	'width': Math.round(frame_dims.width / scale),
	'height': Math.round(frame_dims.height / scale)
    };

    return dims;
}

Cropper.prototype.resetFrame = function() {
    this.setFrameDimensions(this.origDimensions[0], this.origDimensions[1],
			    this.origDimensions[2], this.origDimensions[3]);
}

Cropper.prototype.setFrameDimensions = function(top, right, bottom, left) {
    this.origDimensions = [top, right, bottom, left];

    var scale = this.image.getScale();
    var image_dims = this.image.getDimensions();
    var top = image_dims.top + top * scale;
    var right = image_dims.left + right * scale;
    var bottom = image_dims.top + bottom * scale;
    var left = image_dims.left + left * scale;
    this.frame.reshapeTo(top, right, bottom, left);
    this.frame.setAspectRatio(right - left, bottom - top);
}

Cropper.prototype.setAspectRatio = function(width, height) {
    this.frame.setAspectRatio(width, height);
}

Cropper.prototype.setOrientation = function(orientation) {
    this.frame.setOrientation(orientation);
}
