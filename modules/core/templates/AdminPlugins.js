function updatePluginState(pluginType, pluginId, state, visualChanges) {
    var pluginKey = pluginType + "-" + pluginId;
    var icon = document.getElementById("plugin-icon-" + pluginKey);
    icon.className = stateData[state]['class'];
    icon.title = stateData[state]['text'];
    for (var i in allActions) {
	if (allActions[i] == "delete" && !pluginData[pluginType][pluginId]["deletable"]) {
	    continue;
	}
	var node = document.getElementById("action-" + allActions[i] + "-" + pluginKey);
	if (node) {
	    node.style.display = stateData[state]['actions'][allActions[i]] ? 'inline' : 'none';
	}
    }

    var links = document.getElementsByTagName("li");
    for (i = 0; i < links.length; i++) {
	if (links[i].className && links[i].className.indexOf('gbLink-' + pluginId + '_') != -1) {
	    links[i].style.display = (state == 'active') ? 'block' : 'none';
	}
    }

    pluginData[pluginType][pluginId]['state'] = state;

    if (visualChanges && stateData[state]['message']) {
	addMessage(pluginType, pluginId, stateData[state]['message']['text'],
		   stateData[state]['message']['type']);

	if (stateData[state]['callback']) {
	    callback = stateData[state]['callback'] + '("' + pluginType + '", "' + pluginId + '")';
	    eval(callback);
	}
	updateStateCounts();
    }
}

function updateStateCounts() {
    var counts = {}
    for (i in pluginData) {
	for (j in pluginData[i]) {
	    var state = pluginData[i][j]['state'];

	    /* We don't have a legend for unconfigured */
	    if (state == 'unconfigured') {
		state = 'inactive';
	    }

	    if (!counts[state]) {
		counts[state] = 0;
	    }
	    counts[state]++;
	}
    }

    var states = ['inactive', 'active', 'uninstalled', 'unupgraded', 'incompatible'];
    for (i in states) {
	var state = states[i];
	var msgTopEl = document.getElementById('AdminPlugins_legend_' + state + '_msg_top');
	var msgBottomEl = document.getElementById('AdminPlugins_legend_' + state + '_msg_bottom');
	if (!counts[state]) {
	    msgTopEl.style.display = msgBottomEl.style.display = 'none';
	} else {
	    msgTopEl.style.display = msgBottomEl.style.display = 'inline';
	    msgTopEl.innerHTML = msgBottomEl.innerHTML =
		legendStrings[state].replace('__COUNT__', counts[state]);
	}
    }

    return counts;
}

function deletePlugin(pluginType, pluginId) {
    var re = new RegExp().compile('gbLink-' + pluginId + '_');
    var links = document.getElementsByTagName("li");
    for (i in links) {
	if (re.test(links[i].className)) {
	    links[i].style.display = 'none';
	}
    }

    var row = document.getElementById("plugin-row-" + pluginType + "-" + pluginId);
    if (row) {
	row.style.display = 'none';
    }

    addMessage(pluginType, pluginId, stateData['deleted']['message']['text'],
	       stateData['deleted']['message']['type']);
    pluginData[pluginType][pluginId]['state'] = 'deleted';
    updateStateCounts();
}

function copyVersionToInstalledVersion(pluginType, pluginId) {
    versionEl = document.getElementById("plugin-" + pluginType + "-" + pluginId + "-version");
    installedVersionEl = document.getElementById(
	"plugin-" + pluginType + "-" + pluginId + "-installedVersion");
    installedVersionEl.innerHTML = versionEl.innerHTML;
}

function eraseInstalledVersion(pluginType, pluginId) {
    installedVersionEl = document.getElementById(
	"plugin-" + pluginType + "-" + pluginId + "-installedVersion");
    installedVersionEl.innerHTML = '';
}

function setPluginBusyStatus(pluginType, pluginId, makeBusy) {
    var pluginKey = pluginType + "-" + pluginId;
    var row = document.getElementById("plugin-row-" + pluginKey);
    if (makeBusy) {
	row.className = row.className + " gbBusy";
    } else {
	row.className = row.className.replace(" gbBusy", "");
    }
}

function performPluginAction(pluginType, pluginId, url) {
    if (contexts[pluginId]) {
	return;
    }

    contexts[pluginType][pluginId] = Array();

    var handleSuccess = function(response) {
	eval("var result = " + response.responseText)
	if (result['status'] == 'success') {
	    for (var stateChangePluginType in result['states']) {
		for (var stateChangePluginId in result['states'][stateChangePluginType]) {
		    updatePluginState(
			stateChangePluginType, stateChangePluginId,
			result['states'][stateChangePluginType][stateChangePluginId], true);
		}
	    }

	    for (var deletedPluginType in result['deleted']) {
		for (var deletedPluginId in result['deleted'][deletedPluginType]) {
		    deletePlugin(deletedPluginType, deletedPluginId,
				 result['deleted'][deletedPluginType][deletedPluginId]);
		}
	    }
	} else if (result['status'] == 'redirect') {
	    document.location.href = result['redirect'];
	} else if (result['status'] == 'error') {
	    document.location.href = errorPageUrl;
	} else if (result['status'] == 'fail') {
	    addMessage(pluginType, pluginId, failedToDeleteMessage, 'giWarning');
	}

	setPluginBusyStatus(pluginType, pluginId, false);
	delete(contexts[pluginType][pluginId]);
    }

    var handleFailure = function(response) {
	// Ignore for now
    }

    var callback = {
	success: handleSuccess,
	failure: handleFailure,
	scope: this
    }

    url += '&rnd=' + Math.random();
    YAHOO.util.Connect.asyncRequest('GET', url, callback);
    setPluginBusyStatus(pluginType, pluginId, true);

    return false;
}

var STATUS_BOX_ID = "gbPluginStatusUpdates";
function addMessage(pluginType, pluginId, messageText, messageType) {
    var pluginStatus = document.createElement("div");
    var detailsId = "plugin-status-details-" + pluginType + "-" + pluginId;
    if (document.getElementById(detailsId)) {
	document.getElementById(STATUS_BOX_ID).removeChild(document.getElementById(detailsId));
    }

    pluginStatus.className = messageType;
    pluginStatus.id = detailsId;
    pluginStatus.style.whiteSpace = "nowrap";
    var text = messageText.replace('__PLUGIN__', pluginData[pluginType][pluginId]["name"]);

    pluginStatus.appendChild(document.createTextNode(text));

    var containerEl = document.getElementById(STATUS_BOX_ID);
    containerEl.appendChild(pluginStatus);
    containerEl.style.display = "block";

    var statusDimensions = YAHOO.util.Dom.getRegion(containerEl);
    var bodyDimensions = YAHOO.util.Dom.getRegion(document.body);
    // For IE6:
    if (!bodyDimensions.right) {
	bodyDimensions = YAHOO.util.Dom.getRegion(document.getElementById("gallery"));
    }
    containerEl.style.left = (((bodyDimensions.right - bodyDimensions.left) -
	                       (statusDimensions.right - statusDimensions.left)) / 2) + "px";

    updateStatusPosition();
    setTimeout("removeMessage('" + pluginStatus.id + "')", 3000);
}

function removeMessage(pluginStatusId) {
    var containerEl = document.getElementById(STATUS_BOX_ID);
    var statusMessage = document.getElementById(pluginStatusId);
    if (statusMessage != null) {
	containerEl.removeChild(statusMessage);
    }

    if (containerEl.childNodes.length == 0) {
	containerEl.style.display = "none";
    }
}

function updateStatusPosition() {
    var containerEl = document.getElementById(STATUS_BOX_ID);
    containerEl.style.top = document.getElementsByTagName("html")[0].scrollTop + "px";
}

function verify(prompts, pluginType, pluginId, uninstallUrl) {
    var dialog = new YAHOO.widget.SimpleDialog(
	"gDialog", { width: "20em",
		effect: { effect:YAHOO.widget.ContainerEffect.FADE, duration:0.25 },
		fixedcenter: true,
		modal: true,
		draggable: false });
    dialog.setHeader(prompts['header']);
    dialog.setBody(prompts['body'].replace('__PLUGIN__', pluginData[pluginType][pluginId]["name"]));
    dialog.cfg.setProperty("icon", YAHOO.widget.SimpleDialog.ICON_WARN);

    var handleYes = function() {
	this.hide();
	performPluginAction(pluginType, pluginId, uninstallUrl);
    }

    var handleNo = function() {
	this.hide();
    }

    var myButtons = [ { text: prompts['yes'], handler:handleYes },
    { text: prompts['no'], handler:handleNo, isDefault:true } ];
    dialog.cfg.queueProperty("buttons", myButtons);
    dialog.render(document.body);
}

