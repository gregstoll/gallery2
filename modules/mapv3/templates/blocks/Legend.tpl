{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

{if isset($mapv3.LegendPos) and isset($mapv3.LegendFeature) and $mapv3.LegendFeature neq '0' and ((isset($mapv3.AlbumLegend) and $mapv3.AlbumLegend) or (isset($mapv3.PhotoLegend) and $mapv3.PhotoLegend) or (isset($mapv3.regroupItems) and $mapv3.regroupItems))}
{if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}
<div class="{$class}">
{/if}
  <div id="legend" class="gcBackground1" style="{if $mapv3.LegendPos eq 'right'}right:5px;{if $mapv3.ThumbBarPos eq 'top'}position:relative;top:-{$mapv3.ThumbHeight}px;{/if}{/if}{if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}width:150px;{else}width:mywidth;{/if}border: 2px solid black;padding:.5em;">
    <center><h2>{g->text text="Legend(s)"}</h2></center><br/>
    {if $mapv3.AlbumLegend eq "1"}
    {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}
      <br/>
      <table style="border:1px solid; border-color: black !important;border-collapse:collapse;"><tr class="gcBackground2" onclick="togglealbumlegend();" style="cursor:pointer;background-color:#dfdfdf;border-bottom:1px solid black;"><td width=100%>{g->text text="Album Legend"}</td><td><img alt="none" name="albumarrow" src="{g->url href="modules/mapv3/images/down.png"}" height="16" width="15"/></td></tr>
      <tr><td colspan=3>
    {/if}
    <table {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}id="albumlegend" {if $mapv3.ExpandAlbumLegend}style="display:block;"{else}style="display:none;"{/if}{/if}>
    {if $mapv3.LegendPos eq 'top' or $mapv3.LegendPos eq 'bottom'}<tr><td>{g->text text="Album Legend"}</td>{/if}
    {foreach from=$mapv3.allmarkers key=num item=imagelist}
    {foreach from=$imagelist key=name item=images}
       {if $name eq $mapv3.useAlbumMarkerSet}
       {foreach from=$images item=image key=num}
       {if $mapv3.AlbumLegends.$num neq ""}
         {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}<tr>{/if}
         {assign var="colonpos" value=$image|strpos:":"}
         {assign var="newimage" value=$image|substr:$colonpos+2}
         <td><input onclick="togglemarkers('A{$num}')" value="1" type="checkbox" name="CA{$num}" checked="checked" /></td><td id="A{$num}">{$newimage}</td><td>{$mapv3.AlbumLegends.$num}</td>
         {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}</tr>{/if}
       {/if}
       {/foreach}
       {/if}
    {/foreach}
    {/foreach}
    </table>
    {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}</td></tr></table>{/if}
    {/if}
    {if $mapv3.PhotoLegend eq "1"}
    {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}
    <br/>
    <table style="border:1px solid; border-color: black !important;border-collapse:collapse;"><tr class="gcBackground2" onclick="togglephotolegend();" style="cursor:pointer;background-color:#dfdfdf;border-bottom:1px solid black;"><td width=100%>{g->text text="Photo Legend"}</td><td><img alt="none" name="photoarrow" src="{g->url href="modules/mapv3/images/down.png"}" height="16" width="15"/></td></tr>
    <tr><td colspan=3>
    {/if}
    <table {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}id="photolegend" {if $mapv3.ExpandAlbumLegend}style="display:block;"{else}style="display:none;"{/if}{/if}>
    {if $mapv3.LegendPos eq 'top' or $mapv3.LegendPos eq 'bottom'}<tr><td>{g->text text="Photo Legend"}</td>{/if}
    {foreach from=$mapv3.allmarkers key=num item=imagelist}
    {foreach from=$imagelist key=name item=images}
       {if $name eq $mapv3.useMarkerSet}
       {foreach from=$images item=image key=num}
       {if $mapv3.PhotoLegends.$num neq ""}
         {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}<tr>{/if}
         {assign var="colonpos" value=$image|strpos:":"}
         {assign var="newimage" value=$image|substr:$colonpos+2}
         <td><input onclick="togglemarkers('P{$num}')" value="1" type="checkbox" name="CP{$num}" checked="checked" /></td><td id="P{$num}">{$newimage}</td><td>{$mapv3.PhotoLegends.$num}</td>
         {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}</tr>{/if}
       {/if}
       {/foreach}
       {/if}
    {/foreach}
    {/foreach}
    </table>
    {if $mapv3.LegendPos eq 'right' or $mapv3.LegendPos eq 'left'}</td></tr></table>{/if}
    {/if}
    {if isset($mapv3.regroupItems) and $mapv3.regroupItems eq "1"}
    <br/><table style="border:1px solid;border-collapse:collapse;"><tr class="gcBackground2" style="cursor:pointer;background-color:#dfdfdf;border-bottom:1px solid;"><td colspan=2 width=100%>{g->text text="Grouping"}</td></tr>
    <tr><td>
    {if !isset($mapv3.isIE) or (isset($mapv3.isIE) and $mapv3.isIE eq false)}
    <img alt="none" src="{g->url href="modules/mapv3/images/multi/{$mapv3.regroupIcon}.png"}"/>
    {else}
    <img alt="none" src="{g->url href="modules/mapv3/images/blank.gif"}" style="filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src='{g->url href="modules/mapv3/images/multi/"}{$mapv3.regroupIcon}.png');"/>
    {/if}
    </td><td>{g->text text="When you click on this icon, it automatically zooms in to show the icons that are Regrouped together"}</td></tr>
    </table>
    {/if}
  </div>
</div>
{/if}
