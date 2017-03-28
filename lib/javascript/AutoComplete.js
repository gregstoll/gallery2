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
function autoCompleteAttach(element, url) {
    // YUI's autocomplete data source expects to receive the url in pieces
    // so split it up accordingly.
    var path_and_params = url.split('?');
    var path = path_and_params[0];
    var params = path_and_params[1].split('&');

    var ds = new YAHOO.widget.DS_XHR(path, ["\n", "\t"]);
    ds.queryMatchContains = true;
    ds.responseType = YAHOO.widget.DS_XHR.TYPE_FLAT;

    // Extract that param that has __VALUE__ in it and use that as
    // the data source scriptQueryParam
    new_params = [];
    for (i = 0; i < params.length; i++) {
        if (params[i].indexOf('__VALUE__') != -1) {
	    tmp = params[i].split('=');
	    ds.scriptQueryParam = tmp[0];
	} else {
	    new_params.push(params[i]);
	}
    }
    ds.scriptQueryAppend = new_params.join('&');

    var target = YAHOO.util.Dom.get(element);
    var target_region = YAHOO.util.Dom.getRegion(target);
    var target_width = target_region.right - target_region.left;

    var shadow = document.createElement('div');
    shadow.id = element + '_autoCompleteShadow';
    target.parentNode.appendChild(shadow, target);
    YAHOO.util.Dom.addClass(shadow, 'autoCompleteShadow');

    var container = document.createElement('div');
    container.id = element + '_autoCompleteContainer';
    shadow.appendChild(container);
    YAHOO.util.Dom.addClass(container, 'autoCompleteContainer');

    // These are here for backwards compatibility to the CSS that we used in 2.1.  Unfortunately,
    // it breaks drop shadows in the new version.  Remove these when GalleryTheme API gets to 3.x
    YAHOO.util.Dom.setStyle(shadow, 'position', 'absolute');
    YAHOO.util.Dom.setStyle(shadow, 'background', '#FFF');
    YAHOO.util.Dom.setStyle(container, 'position', 'relative');

    // Adjust for relative positioning of the container inside the shadow
    var pos = [target_region.left, target_region.bottom];
    pos[0] += Number(YAHOO.util.Dom.getStyle(container, "right").replace(/[^0-9]/g, ""));
    pos[1] += Number(YAHOO.util.Dom.getStyle(container, "bottom").replace(/[^0-9]/g, ""));

    YAHOO.util.Dom.setXY(shadow, pos);
    YAHOO.util.Dom.setStyle(shadow, "width", target_width + "px");
    oAutoComp = new YAHOO.widget.AutoComplete(element, element + '_autoCompleteContainer', ds);

    // Change the default behavior of YUI!'s AutoComplete. Copy the list item as text and not
    // its HTML source to the input field of the AutoComplete container.

    var getInnerText = function(obj) {
	return obj.innerText ? obj.innerText
			     : obj.textContent ? obj.textContent // Fallback for Firefox
					       : "";
    }

    // @param string htmlEntitizedString e.g. &lt;p&gt;Hello&lt;/p&gt;
    // @return string HTML-entity-decoded string, e.g. <p>Hello</p>
    var htmlEntityDecode = function(htmlEntitizedString) {
        var htmlNode = document.createElement("div");
        htmlNode.innerHTML = htmlEntitizedString;

	// In Safari, innerText only works if the element is visible DOM content
	var isSafari = navigator.userAgent.toLowerCase().indexOf("safari") != -1;
	if (isSafari) {
	    var container = document.getElementsByTagName('body')[0];
	    container.appendChild(htmlNode);
	}

        var content = getInnerText(htmlNode);

	if (isSafari) {
	    htmlNode.parentNode.removeChild(htmlNode);
	}

	return content;
    }

    // @see http://developer.yahoo.com/yui/autocomplete/#customevents
    var itemSelectHandler = function(sType, aArgs) {    
        var elListItem = aArgs[1]; //the <li> element selected in the suggestion container
        var decodedString = htmlEntityDecode(elListItem.innerHTML);
        if (!decodedString) decodedString = elListItem.innerHTML; // fallback
        YAHOO.util.Dom.get(element).value = decodedString;
    };

    oAutoComp.itemSelectEvent.subscribe(itemSelectHandler);
}
