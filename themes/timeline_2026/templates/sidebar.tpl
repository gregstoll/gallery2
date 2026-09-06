{* timeline_2026 - blocks rendered inside the navigation rail *}
{foreach from=$theme.params.sidebarBlocks item=block}
  {g->block type=$block.0 params=$block.1 class="gbBlock"}
{/foreach}
