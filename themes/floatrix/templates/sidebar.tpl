{*
 * $Revision: 17075 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="gsSidebar" class="inner gcBorder1 gcBackground3" style="overflow: auto;">
  <a href="{g->url params=$theme.pageUrl arg1="jsWarning=true"}" id="hideSidebarTab"
      onclick="MM_changeProp('gsSidebarCol','','style.display','none','DIV');
            MM_changeProp('showSidebarTab','','style.display','block','DIV');
            return false;">
      <div style="width:20px; height:30px;"/></div></a>
  {* Show the sidebar blocks chosen for this theme *}
  {foreach from=$theme.params.sidebarBlocks item=block}
    {g->block type=$block.0 params=$block.1 class="gbBlock"}
  {/foreach}
</div>
<!--[if lte IE 6.5]><iframe></iframe><![endif]-->
