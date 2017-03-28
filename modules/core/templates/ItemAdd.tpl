{*
 * $Revision: 16433 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Add Items"} </h2>
</div>

{if !empty($form.error.itemsAddedDespiteFormErrors)}
<div class="gbBlock giError">
  {g->text text="Not all of the specified items have been added successfully."}
</div>
{/if}

{if (!$ItemAdd.hasToolkit)}
<div class="gbBlock giWarning">
  {g->text text="You don't have any Graphics Toolkit activated that can handle JPEG images.  If you add images, you will probably not have any thumbnails."}
  {capture name="url"}
    {g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}
  {/capture}
  {if $ItemAdd.isAdmin}
    {g->text text="Visit the %sModules%s page to activate a Graphics Toolkit." arg1="<a href=\"`$smarty.capture.url`\">" arg2="</a>"}
  {/if}
</div>
{/if}

<div class="gbTabBar">
  {foreach from=$ItemAdd.plugins item=plugin}
    {if $plugin.isSelected}
      <span class="giSelected o"><span>
	{$plugin.title}
      </span></span>
    {else}
      <span class="o"><span>
	<a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd"
	 arg3="itemId=`$ItemAdmin.item.id`" arg4="addPlugin=`$plugin.id`"}">{$plugin.title}</a>
      </span></span>
    {/if}
  {/foreach}
</div>

<input type="hidden" name="{g->formVar var="addPlugin"}" value="{$ItemAdd.addPlugin}"/>

{include file="gallery:`$ItemAdd.pluginFile`" l10Domain=$ItemAdd.pluginL10Domain}
