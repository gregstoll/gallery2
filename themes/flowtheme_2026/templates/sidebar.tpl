{* flowtheme_2026 - sidebar blocks *}
{foreach from=$theme.params.sidebarBlocks item=block}
  {g->block type=$block.0 params=$block.1 class="gbBlock"}
{/foreach}
