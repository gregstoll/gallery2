{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Icon Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
{if empty($IconsSiteAdmin.iconpacks)}
  <p class="giDescription">
    {g->text text="No icon packs are available."}
  </p>
{else}
  <p class="giDescription">
    {g->text text="Select an icon pack to use for this Gallery."}
  </p><p>
    <select name="{g->formVar var="form[iconpack]"}">
      {html_options options=$IconsSiteAdmin.iconpacks selected=$form.iconpack}
    </select>
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
{/if}
</div>

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Icon Pack Browser"} </h2>
</div>

<div class="gbBlock">
  <table class="gbDataTable">
    <tr>
      <th>
	{g->text text="Link ID"}
      </th>
      {foreach from=$IconsSiteAdmin.packs key=dir item=pack}
      <th>
	{$pack.name}
      </th>
      {/foreach}
    </tr>
    {foreach from=$IconsSiteAdmin.classes key=class item=ignored}
    <tr>
      <td>
	{$class|replace:"_":"."|replace:"-":" "}
      </td>
      {foreach from=$IconsSiteAdmin.packs key=dir item=pack}
      <td>
	{if isset($pack.map[$class])}
	<img src="{g->url href="modules/icons/iconpacks/$dir/`$pack.map[$class]`"}"/>
	{/if}
      </td>
      {/foreach}
    </tr>
    {/foreach}
  </table>
</div>
