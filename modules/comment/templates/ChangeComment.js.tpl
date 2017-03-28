{*
 * $Revision: 17127 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
//<![CDATA[
var changeUrlPattern = '{g->url arg1="view=comment.CommentCallback" arg2="command=__COMMAND__" arg3="commentId=__COMMENT_ID__" useAuthToken=true htmlEntities=false}';
var prompts = {ldelim}
    'delete' : {ldelim}
	ask: true,
	undo: null,
	title : '{g->text text="Delete this comment?" forJavascript=true}',
	body : '{g->text text="Are you sure you want to delete this comment?" forJavascript=true}',
	confirm: '{g->text text="This comment has been deleted." forJavascript=true}'
    {rdelim},

    'despam' : {ldelim}
	ask: false,
	undo: 'spam',
	confirm: '{g->text text="This comment has been marked as not spam. __UNDO__" forJavascript=true}'
    {rdelim},

    'spam' : {ldelim}
	ask: false,
	undo: 'despam',
	confirm: '{g->text text="This comment has been marked as spam. __UNDO__" forJavascript=true}'
    {rdelim},

    'yes' : '{g->text text="Yes" forJavascript=true}',
    'no' : '{g->text text="Cancel" forJavascript=true}',
    'undo' : '{g->text text="Undo" forJavascript=true}'
{rdelim};
var errorPageUrl = '{g->url arg1="view=core.ErrorPage" htmlEntities=false}';

{literal}
function handleSuccess(response) {
  eval("var result = " + response.responseText);
  if (result['status'] == 'error') {
      document.location.href = errorPageUrl;
  }
}

var handleFailure = function(response) {
}

function dimComment(id) {
  var anim = new YAHOO.util.Anim(
    'comment-' + id, { opacity: { to: 0.2 } }, 1,
    YAHOO.util.Easing.easeIn);
  anim.animate();
}

function brightenComment(id) {
  var anim = new YAHOO.util.Anim(
    'comment-' + id, { opacity: { to: 1.0 } }, 1,
    YAHOO.util.Easing.easeOut);
  anim.animate();
}

function undoChange(id, command) {
  YAHOO.util.Connect.asyncRequest(
    'GET', changeUrlPattern.replace('__COMMENT_ID__', id).replace('__COMMAND__', prompts[command]['undo']),
    { success: function(response) {
	handleSuccess(response);
	document.getElementById('gallery').removeChild(
	  document.getElementById('comment-confirm-' + id));
	brightenComment(id);
      },
      failure: handleFailure, scope: this });
}

function showConfirmation(id, command) {
  var commentEl = document.getElementById('comment-' + id);
  var region = YAHOO.util.Dom.getRegion('comment-' + id);
  var confirmEl = document.createElement('div');
  confirmEl.id = 'comment-confirm-' + id;
  confirmEl.innerHTML = ('<div class="gbBlock gcBackground2" ' +
    'style="position: absolute; top: ' + (region.top + 20) + 'px; ' +
    'left: ' + (region.left + 100) + 'px;"><h2> ' +
    prompts[command]['confirm'] + '</h2></div>').replace(
      '__UNDO__',
      '<a href="#" style="cursor: pointer" onclick="undoChange(' + id +
      ', \'' + command + '\'); return false;">' +
      prompts['undo'] + '</a>');
  document.getElementById("gallery").appendChild(confirmEl);
}

function changeComment(id, command) {
  var doChange = function() {
    dimComment(id);
    YAHOO.util.Connect.asyncRequest(
      'GET', changeUrlPattern.replace('__COMMENT_ID__', id).replace('__COMMAND__', command),
      { success: function(response) { handleSuccess(response); showConfirmation(id, command); },
	failure: handleFailure, scope: this });
  }

  if (prompts[command]['ask']) {
    confirmChangeComment(id, command, doChange);
  } else {
    doChange();
  }
}

function confirmChangeComment(id, command, callback) {
    var dialog = new YAHOO.widget.SimpleDialog(
	"gDialog", { width: "20em",
            effect: { effect: YAHOO.widget.ContainerEffect.FADE, duration: 0.25 },
	    fixedcenter: true,
	    modal: true,
	    draggable: false
         });
    dialog.setHeader(prompts[command]['title']);
    dialog.setBody(prompts[command]['body']);
    dialog.cfg.setProperty("icon", YAHOO.widget.SimpleDialog.ICON_WARN);

    var handleYes = function() {
	this.hide();
        callback();
    }

    var handleNo = function() {
	this.hide();
    }

    var myButtons = [
        { text: prompts['yes'], handler: handleYes },
        { text: prompts['no'], handler: handleNo, isDefault: true }
    ];
    dialog.cfg.queueProperty("buttons", myButtons);
    dialog.render(document.body);
}
//]]>
</script>
{/literal}
