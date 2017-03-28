{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="{$class}">
  {g->text text="Page:"}
  {assign var="lastPage" value=0}
  {foreach name=jumpRange from=$theme.jumpRange item=page}
  {if ($page - $lastPage >= 2)}
  <span>
    {if ($page - $lastPage == 2)}
    <a href="{g->url params=$theme.pageUrl arg1="page=`$page-1`"}">{$page-1}</a>
    {else}
    ...
    {/if}
  </span>
  {/if}

  <span>
    {if ($theme.currentPage == $page)}
    {$page}
    {else}
    <a href="{g->url params=$theme.pageUrl arg1="page=$page"}">{$page}</a>
    {/if}
  </span>
  {assign var="lastPage" value=$page}
  {/foreach}
</div>
