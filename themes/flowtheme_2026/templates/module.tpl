{* flowtheme_2026 - module view *}
<div class="ft-layout{if !empty($theme.params.sidebarBlocks)} has-side{/if}">
  {if !empty($theme.params.sidebarBlocks)}
  <aside class="ft-side">{g->theme include="sidebar.tpl"}</aside>
  {/if}
  <div class="ft-content">
    <div class="ft-panel">
      {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
    </div>
  </div>
</div>
