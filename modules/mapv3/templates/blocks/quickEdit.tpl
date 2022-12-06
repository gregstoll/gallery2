{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{* Set defaults *}
{if !isset($item)}{assign var=item value=$theme.item}{/if}

{if !isset($coordStyle)}{assign var=coordStyle value=1}{/if}

{g->callback type="mapv3.ItemCoords" itemId=$item.id coordStyle=$coordStyle}

{* TODO: Si las coordenadas estan vacias, ponerlas a un valor por defecto *}

<div class="{$class}">
{if empty($block.mapv3.ItemCoords) or empty($block.mapv3.ItemCoords.lat) or empty($block.mapv3.ItemCoords.lng)}
	{g->text text="No coordinates defined"}
{else}
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
	<a accesskey="g"
	   style="border-top:2px solid #cecece; \
           border-left:2px solid #cecece; \
	   border-bottom:2px solid #4a4a4a;\
	   border-right:2px solid #4a4a4a;    \
           padding:.2em;    \
           padding-right:1em;    \
           padding-left:1em;    \
           text-decoration:none;    \
           background-color:#ebebeb;    \
           color:#000;    \
           font-weight:normal;    \
           font-size:12px;"
	   href="{g->url arg1="view=mapv3.ShowMap" arg2="itemId=`$item.id`" arg3="plugin=quickEdit" arg4="Mode=Pick"}">
	    {g->text text="<u>G</u>et via a Map"}
	</a>
{/if}
</div>
