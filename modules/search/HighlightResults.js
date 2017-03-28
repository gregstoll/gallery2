function search_HighlightResults(criteria) {
    criteria = criteria.replace(/([.*+?^${}()|[\]\/\\])/g, '\\$1')
    var regex = new RegExp("(" + criteria + ")", "ig");

    var spans = document.getElementsByTagName("span");
    for (var i = 0; i < spans.length; i++) {
	if (spans[i].className != "ResultData") {
	    continue;
	}
	for (j = 0; j < spans[i].childNodes.length; j++) {
	    if (spans[i].childNodes[j].nodeName == "#text") {
		node = spans[i].childNodes[j];
		result = node.nodeValue;
		/* Some browsers (Firefox) unescape the node value, so re-escape as necessary */
		result = result.replace(/</g, "&lt;");
		result = result.replace(/>/g, "&gt;");
		result = result.replace(/"/g, "&quot;");
		var spanEl = document.createElement("span");
		spanEl.innerHTML =
		    result.replace(regex, "<span class=\"giSearchHighlight\">$1</span>");
		node.parentNode.insertBefore(spanEl, node);
		node.parentNode.removeChild(node);
	    }
	}
    }
}
