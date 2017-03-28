{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2>
    {g->text one="%d member" many="%d members"
    	 count=$form.list.totalUserCount arg1=$form.list.totalUserCount}
  </h2>
</div>

<div class="gbBlock">
  {if ($form.list.maxPages > 1)}
    <div style="margin-bottom: 10px"><span class="gcBackground1" style="padding: 5px">
      <input type="hidden"
       name="{g->formVar var="form[list][page]"}" value="{$form.list.page}"/>
      <input type="hidden"
       name="{g->formVar var="form[list][maxPages]"}" value="{$form.list.maxPages}"/>

      {if ($form.list.page > 1)}
        <a href="{g->url arg1="view=members.MembersList" arg2="form[list][page]=1"}">
          {g->text text="&laquo; first"}
        </a>
        &nbsp;
        <a href="{g->url arg1="view=members.MembersList"
         arg2="form[list][page]=`$form.list.backPage`"}">
          {g->text text="&laquo; back"}
        </a>
      {else}
        {g->text text="&laquo; first"}
        &nbsp;
        {g->text text="&laquo; back"}
      {/if}

      &nbsp;
      {g->text text="Viewing page %d of %d" arg1=$form.list.page arg2=$form.list.maxPages}
      &nbsp;

      {if ($form.list.page < $form.list.maxPages)}
        <a href="{g->url arg1="view=members.MembersList"
         arg2="form[list][page]=`$form.list.nextPage`"}">
          {g->text text="next &raquo;"}
        </a>
        &nbsp;
        <a href="{g->url arg1="view=members.MembersList"
         arg2="form[list][page]=`$form.list.maxPages`"}">
          {g->text text="last &raquo;"}
        </a>
      {else}
        {g->text text="next &raquo;"}
        &nbsp;
        {g->text text="last &raquo;"}
      {/if}
    </span></div>
  {/if}

  <table class="gbDataTable"><tr>
    <th> {g->text text="#"} </th>
    <th> {g->text text="Username"} </th>
    <th> {g->text text="Full Name"} </th>
    <th> {g->text text="Member Since"} </th>
  </tr>

  {foreach from=$MembersList.users item=user name=MembersListLoop}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {$smarty.foreach.MembersListLoop.iteration+$form.list.startingUser}
    </td><td>
      <a href="{g->url arg1="view=members.MembersProfile" arg2="userId=`$user.id`"}">
       {$user.userName}
      </a>
    </td><td>
      {$user.fullName}
    </td><td>
      {g->date timestamp=$user.creationTimestamp}
    </td>
  </tr>
  {/foreach}
  </table>
</div>
