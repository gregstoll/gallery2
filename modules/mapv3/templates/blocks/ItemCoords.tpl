{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{* Set defaults *}
{if !isset($item)}{assign var=item value=$theme.item}{/if}

{if !isset($coordStyle)}{assign var=coordStyle value=1}{/if}

{g->callback type="mapv3.ItemCoords" itemId=$item.id coordStyle=$coordStyle}

{if !empty($block.mapv3.ItemCoords) and !empty($block.mapv3.ItemCoords.lat) and !empty($block.mapv3.ItemCoords.lng)}
<div class="{$class}">
{g->text text="%s Coordinates:" arg1=$block.mapv3.ItemCoords.ItemType}
{if $coordStyle eq 1}
{$block.mapv3.ItemCoords.lat.deg}&deg;, {$block.mapv3.ItemCoords.lng.deg}&deg;
{elseif $coordStyle eq 2}
{* Use &#39; for apostrophe because &apos; doesn't work in IE *}
{$block.mapv3.ItemCoords.lat.deg}&deg;{$block.mapv3.ItemCoords.lat.min}&#39;&nbsp;{$block.mapv3.ItemCoords.lat.dir},
{$block.mapv3.ItemCoords.lng.deg}&deg;{$block.mapv3.ItemCoords.lng.min}&#39;&nbsp;{$block.mapv3.ItemCoords.lng.dir}
{elseif $coordStyle eq 3}
{$block.mapv3.ItemCoords.lat.deg}&deg;{$block.mapv3.ItemCoords.lat.min}&#39;{$block.mapv3.ItemCoords.lat.sec}&quot;&nbsp;{$block.mapv3.ItemCoords.lat.dir},
{$block.mapv3.ItemCoords.lng.deg}&deg;{$block.mapv3.ItemCoords.lng.min}&#39;{$block.mapv3.ItemCoords.lng.sec}&quot;&nbsp;{$block.mapv3.ItemCoords.lng.dir}
{/if}
</div>
    {else}
    <div>No coordinates</div>
{/if}