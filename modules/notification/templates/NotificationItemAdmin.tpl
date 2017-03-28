{*
 * $Revision: 17802 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<script type="text/javascript">
  // <![CDATA[
  {literal}
  function enableRecursive(iteration) {
      var oldChecked = document.getElementById('old' + iteration).value == '1';
      var newChecked = document.getElementById('new' + iteration).checked;
      var disabled = !(oldChecked ^ newChecked);
      document.getElementById('recursive' + iteration).disabled = disabled;
  }
  {/literal}
  // ]]>
</script>

<div class="gbBlock gcBackground1">
    <h2> {g->text text="Item Notification Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock">
    <h2 class="giSuccess"> {g->text text="Settings saved successfully"} </h2>
</div>
{/if}

<div class="gbBlock">
    <table class="gbDataTable">

{foreach from=$form.notifications item=notification name=notifications}
  {assign var=i value=$smarty.foreach.notifications.iteration}
    {if $i % 30 == 1}
	<tr>
	    <th> {g->text text="Event"} </th>
	    <th> {g->text text="Action"} </th>
	    <th> {g->text text="Subscribe"} </th>
	    <th style="text-align: center;width:60px"> {g->text text="Apply to sub items"} </th>
	</tr>
    {/if}
	<tr>
	    <td>
	      <input type="hidden" name="{g->formVar var="form[notifications][$i][name]"}" value="{$notification.name}" />
	      <input type="hidden" name="{g->formVar var="form[notifications][$i][description]"}" value="{$notification.description}" />
	      {$notification.description}
	    </td>
	    <td>
	      <input type="hidden" name="{g->formVar var="form[notifications][$i][handler]"}" value="{$notification.handler}" />
	      <input type="hidden" name="{g->formVar var="form[notifications][$i][handlerDescription]"}" value="{$notification.handlerDescription}" />
	      {$notification.handlerDescription}
	    </td>
	    <td style="text-align: center">
	      <input id="old{$i}" type="hidden" name="{g->formVar var="form[notifications][$i][oldSubscribed]"}" value="{$notification.subscribed}" />
	      <input id="new{$i}" type="checkbox" name="{g->formVar var="form[notifications][$i][subscribed]"}"
	      	onchange="enableRecursive({$i})"
		{if !empty($notification.subscribed)} checked="checked"{/if} />
	    </td>
	    <td style="text-align: center">
	      <input id="recursive{$i}" type="checkbox" name="{g->formVar var="form[notifications][$i][recursive]"}" checked="checked" 
		{if empty($notification.subscribed)} disabled="disabled" {/if} />
	    </td>
	     {if isset($form.error.notifications[$i].notAuthorized)}
	      <td>
		<span class="giError">
		  {g->text text="Not authorized to receive notifications"}
		</span>
	      </td>
	    {/if}
	</tr>
{/foreach}
    </table>
</div>

<div class="gbBlock gcBackground1">
    <input type="submit" class="inputTypeSubmit"
	name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}" />
    <input type="reset" class="inputTypeSubmit" value="{g->text text="Reset"}"/>
</div>

