{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
	<h2>{g->text text="Confirmation"}</h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.sent)}
    {g->text text="eCard sent successfully"}
  {/if}
</h2></div>
{/if}

<script type="text/javascript">
// <![CDATA[
var sendUrl = '{g->url arg1="view=ecard.SendEcard" arg2="itemId=`$Confirmation.itemId`"
		       htmlEntities=false}';
var doneUrl = '{g->url arg1="view=core.ShowItem" arg2="itemId=`$Confirmation.itemId`"
		       htmlEntities=false}';
// ]]>
</script>

<div class="gbBlock gcBackground1">
	<button onclick="window.location=sendUrl" type="submit" class="inputTypeSubmit"
	  name="{g->formVar var="form[action][new]"}">{g->text text="Send another eCard"}</button>
	<button onclick="window.location=doneUrl" type="submit" class="inputTypeSubmit"
	  name="{g->formVar var="form[action][cancel]"}">{g->text text="Done"}</button>
</div>
