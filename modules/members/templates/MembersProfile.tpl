{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Member Profile for %s" arg1=$MembersProfile.user.userName} </h2>
</div>

<div class="gbBlock">
  <table class="gbDataTable">
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {g->text text="Name:"}
    </td><td>
      {$MembersProfile.user.fullName}
    </td>
  </tr>

  {if $MembersProfile.canViewProfileEmail}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {g->text text="E-mail:"}
    </td><td>
      {if sizeof($MembersProfile.user.email)}
        {mailto address=$MembersProfile.user.email encode="hex"}
      {else}
        {g->text text="None"}
      {/if}
    </td>
  </tr>
  {/if}

  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {g->text text="Member Since:"}
    </td><td>
      {g->date timestamp=$MembersProfile.user.creationTimestamp}
      {if $MembersProfile.daysSinceCreation > 0}
        {g->text one="(%d day)" many="(%d days)"
               count=$MembersProfile.daysSinceCreation arg1=$MembersProfile.daysSinceCreation}
      {elseif $MembersProfile.daysSinceCreation == 0}
        {g->text text="(today)"}
      {/if}
    </td>
  </tr>

  <tr class="{cycle values="gbEven,gbOdd"}">
    <td valign="top">
      {g->text text="Last Items:"}
    </td><td>
    {if count($MembersProfile.lastItems)}
      <table>
      {foreach from=$MembersProfile.lastItems item=item}
      <tr>
        <td>
        {if $item.canContainChildren}
          <img src="{g->url href="modules/members/data/directory.gif"}"
    	   alt="{g->text text="Album"}" width="16" height="16"/>
        {else}
          <img src="{g->url href="modules/members/data/file.gif"}"
    	   alt="{g->text text="Item"}" width="16" height="16"/>
        {/if}
        </td><td>
          <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$item.id`"}">
    	{$item.title|default:$item.pathComponent}
          </a>
        </td>
      </tr>
      {/foreach}
      <tr>
        <td colspan="2">
          <a href="{g->url arg1="view=members.MembersItems"
           arg2="userId=`$MembersProfile.user.id`"}">
    	{g->text text="List All &raquo;"}
          </a>
        </td>
      </tr></table>
    {else}
      {g->text text="None"}
    {/if}
    </td>
  </tr></table>
</div>
