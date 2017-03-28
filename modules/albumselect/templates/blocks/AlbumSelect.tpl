{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="albumselect.LoadAlbumData"
	     stripTitles=true truncateTitles="20" createTextTree=true}

{if isset($block.albumselect)}
{assign var="data" value=$block.albumselect.LoadAlbumData.albumSelect}
<div class="{$class}">
  <select onchange="if (this.value) {ldelim} var newLocation = '{$data.links.prefix}' + this.value; this.options[0].selected = true; location.href = newLocation; {rdelim}">
    <option value="">
      {g->text text="&laquo; Jump to Album &raquo;"}
    </option>
    {foreach from=$data.tree item=node}
      <option value="{$data.links[$node.id]}">
	{$data.titles[$node.id]}
      </option>
    {/foreach}
  </select>
</div>
{/if}
