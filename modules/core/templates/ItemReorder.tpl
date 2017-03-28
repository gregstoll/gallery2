{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Reorder Album"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Order saved successfully"}
</h2></div>
{/if}

{if isset($ItemReorder.show.automaticOrderMessage)}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="This album has an automatic sort order specified, so you cannot change the order of items manually.  You must remove the automatic sort order to continue."}
    <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemEdit"
     arg3="itemEditPlugin=core.ItemEditAlbum" arg4="itemId=`$ItemAdmin.item.id`"}">
      {g->text text="change"}
    </a>
  </p>
</div>
{else}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Change the order of the items in this album."}
  </p>

  <h4> {g->text text="Move this item"} </h4>

  <select name="{g->formVar var="form[selectedId]"}">
    {foreach from=$ItemReorder.peers item=peer}
      <option value="{$peer.id}"> {$peer.title|default:$peer.pathComponent} </option>
    {/foreach}
  </select>

  <select name="{g->formVar var="form[placement]"}">
    <option value="before"> {g->text text="before"} </option>
    <option value="after"> {g->text text="after"} </option>
  </select>

  <select name="{g->formVar var="form[targetId]"}">
    {foreach from=$ItemReorder.peers item=peer}
      <option value="{$peer.id}"> {$peer.title|default:$peer.pathComponent} </option>
    {/foreach}
  </select>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reorder]"}" value="{g->text text="Reorder"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
{/if}
