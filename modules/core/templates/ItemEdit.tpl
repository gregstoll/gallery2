{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Edit %s" arg1=$ItemEdit.itemTypeNames.0} </h2>
</div>

<input type="hidden" name="{g->formVar var="editPlugin"}" value="{$ItemEdit.editPlugin}"/>
<input type="hidden" name="{g->formVar var="form[serialNumber]"}" value="{$form.serialNumber}"/>

{if !empty($status) || !empty($form.error)}
<div class="gbBlock">
  {if !empty($status)}
  <h2 class="giSuccess">
    {if !empty($status.editMessage)}
      {$status.editMessage}
    {/if}
    {if !empty($status.warning)}
    <div class="giWarning">
      {foreach from=$status.warning item=warning}
	{$warning}
      {/foreach}
    </div>
    {/if}
  </h2>
  {/if}
  {if !empty($form.error)}
  <h2 class="giError">
    {g->text text="There was a problem processing your request."}
  </h2>
  {/if}
</div>
{/if}

<div class="gbTabBar">
  {foreach from=$ItemEdit.plugins item=plugin}
    {if $plugin.isSelected}
      <span class="giSelected o"><span>
	{$plugin.title}
      </span></span>
    {else}
      <span class="o"><span>
	<a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemEdit"
	 arg3="itemId=`$ItemAdmin.item.id`" arg4="editPlugin=`$plugin.id`"}">{$plugin.title}</a>
      </span></span>
    {/if}
  {/foreach}
</div>

{include file="gallery:`$ItemEdit.pluginFile`" l10Domain=$ItemEdit.pluginL10Domain}
