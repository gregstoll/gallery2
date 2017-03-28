function validateRepositoryList() {
    var count = 0;
    for (i in repositoryArgs) {
        if (formElements[repositoryArgs[i]].checked) {
	    count++;
        }
    }
    if (count > 0) {
        return 1;
    }

    var dialog = new YAHOO.widget.SimpleDialog(
	"gDialog", { width: "20em",
		effect: { effect:YAHOO.widget.ContainerEffect.FADE, duration:0.25 },
		fixedcenter: true,
		modal: true,
		draggable: false });
    dialog.setHeader(repositoryMessageTitle);
    dialog.setBody(repositoryMessageBody);
    dialog.cfg.setProperty("icon", YAHOO.widget.SimpleDialog.ICON_WARN);
    dialog.cfg.queueProperty(
	"buttons",
	[ { text: repositoryMessageOkButton, handler:function() { this.hide() } } ]);
    dialog.render(document.body);
}

function saveRepositoryList() {
    var postArgs = commandArg + '=saveRepositoryList';
    postArgs += '&' + viewArg + '=core.RepositoryCallback';
    postArgs += '&' + authTokenArg;
    for (i in repositoryArgs) {
        postArgs += '&' + repositoryArgs[i] + '=' +
	    (formElements[repositoryArgs[i]].checked ? 1 : 0);
    }
    YAHOO.util.Connect.asyncRequest('POST', postUrl, function() { }, postArgs);
}

function closeRepositoryList() {
    var anim1 = new YAHOO.util.Anim(
	'AdminRepository_Configure', { opacity: { to: 0.0 }, height: { to: '0' } }, 1,
	YAHOO.util.Easing.easeOut);
    anim1.animate();

    var anim2 = new YAHOO.util.Anim(
	'AdminRepository_showRepositoryList', { opacity: { to: 1.0 } }, 1,
	YAHOO.util.Easing.easeOut);
    anim2.animate();
}

function showRepositoryList() {
    document.getElementById('AdminRepository_Configure').style.height = '100%';

    /* IE leaves the dotted focus-box around when the button is transparent, so shift focus */
    document.getElementById('AdminRepository_showRepositoryList').blur();

    var anim1 = new YAHOO.util.Anim(
	'AdminRepository_Configure', { opacity: { to: 1.0 } }, 1,
	YAHOO.util.Easing.easeOut);
    anim1.animate();

    var anim2 = new YAHOO.util.Anim(
	'AdminRepository_showRepositoryList', { opacity: { to: 0.0 } }, 1,
	YAHOO.util.Easing.easeOut);
    anim2.animate();
}
