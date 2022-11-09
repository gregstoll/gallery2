{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

{g->callback type="mapv3.LoadFilters"}
<style>
    {literal}
    .map_thumbs_top{
    }
    .map_filter_wrapper {
        position: relative;
    }
    .filter_select {
        width: 75%;
    }
    .filter_column {
        width: 40%;
        display: inline-block;
    }
    .filter_table {
        width: 100%;
        border: 0;
    }
  {/literal}
</style>
{if !empty($block.mapv3.LoadFilters)}
<div class="{$class}{if $mapv3.ShowFilters eq "right"} map_filter_wrapper{/if}{if $mapv3.ShowFilters eq "top"} map_thumbs_top{/if}">
  {if isset($mapv3.ShowFilters) and ($mapv3.ShowFilters eq "top" or $mapv3.ShowFilters eq "bottom")}
  <div class="filter_table">
      <div class="filter_column">
  {/if}
  <h3>{g->text text="Area to show on the Map:"}</h3>
  {if isset($mapv3.ShowFilters) and ($mapv3.ShowFilters eq "top" or $mapv3.ShowFilters eq "bottom")}
      </div>
      <div class="filter_column">{/if}
    <select class="filter_select" onchange="{literal}if (this.value) { newLocation = this.value; this.options[0].selected = true; location.href= newLocation; }{/literal}">
    {foreach item=option from=$block.mapv3.LoadFilters.filters}
       <option label="{$option.name}" {if $option.params neq ''}value="{g->url params=$option.params}"{/if} {if $option.name eq $block.mapv3.LoadFilters.filterOn}selected{/if}>{$option.name}</option>
    {/foreach}
  </select>
  {if isset($mapv3.ShowFilters) and ($mapv3.ShowFilters eq "top" or $mapv3.ShowFilters eq "bottom")}{strip}
      </div>
  </div>{/strip}{/if}
</div>
{/if}
