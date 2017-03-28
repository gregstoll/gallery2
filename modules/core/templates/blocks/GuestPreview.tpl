{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if $user.isRegisteredUser}
<div class="{$class}">
  {capture name=guestPreviewMode}
  {if ($theme.guestPreviewMode)}
    <a href="{g->url arg1="controller=core.ShowItem" arg2="guestPreviewMode=0" arg3="return=1"}">{$user.userName}</a> | <span class="active"> {g->text text="guest"} </span>
  {else}
  <span class="active"> {$user.userName} </span> | <a href="{g->url arg1="controller=core.ShowItem" arg2="guestPreviewMode=1" arg3="return=1"}">{g->text text="guest"}</a>
  {/if}
  {/capture}
  {g->text text="display mode: %s" arg1=$smarty.capture.guestPreviewMode}
</div>
{/if}
