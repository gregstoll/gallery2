{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<a href="{g->url arg1="view=debug.ShowTree" arg2="entityId=$entityId"}">
  {g->text text="%d: (%s)" arg1=$entityId arg2=$ShowTree.entityTable.$entityId.entityType}
</a>

{if !empty($ShowTree.isItem.$entityId)}
  &nbsp;
  <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=$entityId"}">
    {g->text text="[browse]"}
  </a>
{/if}
