{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="members.LoadMembers"}

<div class="{$class}">
  <h3> {g->text text="Members"} </h3>
  <p>
  {if ($block.members.LoadMembers.canViewList)}
    <a class="{g->linkId view="members.MembersList"}" style="padding: 3px"
       href="{g->url arg1="view=members.MembersList"}">
      {g->text one="%s member" many="%s members"
               count=$block.members.LoadMembers.count
               arg1=$block.members.LoadMembers.count}
    </a>
  {else}
    {g->text one="%s member" many="%s members"
             count=$block.members.LoadMembers.count
             arg1=$block.members.LoadMembers.count}
  {/if}
  </p>
</div>

