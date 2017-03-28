{*
 * $Revision: 17113 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
var prompts = {ldelim}
  "header" : '{g->text text="Warning!" forJavascript=true}',
  "body"   : '{g->text text="Do you really want to delete %s?" forJavascript=true}',
  "yes"    : '{g->text text="Yes" forJavascript=true}',
  "no"     : '{g->text text="No" forJavascript=true}',
  "more"   : '{g->text text="Delete more items..." forJavascript=true}'
{rdelim};

{literal}
function core_confirmDelete(url, moreUrl, title) {
  var pageItemId = this.pageItemId;
  var dialog = new YAHOO.widget.SimpleDialog("gDialog", { width: "20em",
    effect: { effect:YAHOO.widget.ContainerEffect.FADE, duration:0.25 },
    fixedcenter: true,
    modal: true,
    draggable: false });
    
  dialog.setHeader(prompts['header']);
  var bodyText = prompts['body'].replace('%s', title);
  if (moreUrl) {
    bodyText += '<br /><br /><a href="" onclick="document.location.href=\''
	     + moreUrl + '\';return false">' + prompts['more'] + '</a>';
  }
  dialog.setBody(bodyText);
  dialog.cfg.setProperty("icon", YAHOO.widget.SimpleDialog.ICON_WARN);
  
  var handleYes = function() {
    document.location.href = url;
  }
    
  var handleNo = function() {
    this.hide();
  }
    
  var myButtons = [ { text: prompts['yes'], handler:handleYes },
        { text: prompts['no'], handler:handleNo, isDefault:true } ];
  dialog.cfg.queueProperty("buttons", myButtons);
  dialog.render(document.body);
}
{/literal}

