{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="User Management"} </h2>
</div>

<div class="gbBlock">
  {if !empty($status)}
  <h3 class="giSuccess">
    {if isset($status.deletedUser)}
      {g->text text="Removed user '%s'" arg1=$status.deletedUser}
    {/if}
    {if isset($status.createdUser)}
      {g->text text="Created user '%s'" arg1=$status.createdUser}
    {/if}
    {if isset($status.modifiedUser)}
      {g->text text="Modified user '%s'" arg1=$status.modifiedUser}
    {/if}
  </h3>
  {/if}

  <p class="giDescription">
    {g->text one="There is %d user in the system."
	     many="There are %d total users in the system."
	     count=$AdminUsers.totalUserCount arg1=$AdminUsers.totalUserCount}
  </p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Edit User"} </h3>

  <input type="text" id="giFormUsername" size="20" autocomplete="off"
   name="{g->formVar var="form[text][userName]"}" value="{$form.text.userName}"/>
  {g->autoComplete element="giFormUsername"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupUsername"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][editFromText]"}" value="{g->text text="Edit"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][deleteFromText]"}" value="{g->text text="Delete"}"/>

  {if isset($form.error.text.noSuchUser)}
  <div class="giError">
    {g->text text="User '%s' does not exist." arg1=$form.text.userName}
  </div>
  {/if}
  {if isset($form.error.text.noUserSpecified)}
  <div class="giError">
    {g->text text="You must enter a username"}
  </div>
  {/if}
  {if isset($form.error.text.cantDeleteSelf)}
  <div class="giError">
    {g->text text="You cannot delete yourself!"}
  </div>
  {/if}
  {if isset($form.error.text.cantDeleteAnonymous)}
  <div class="giError">
    {g->text text="You cannot delete the special guest user."}
  </div>
  {/if}
</div>

<div class="gbBlock">
  <h3> {g->text text="Edit User (by list)"} </h3>

  {if ($form.list.maxPages > 1)}
  <div style="margin-bottom: 10px"><span class="gcBackground1" style="padding: 5px">
    <input type="hidden"
     name="{g->formVar var="form[list][page]"}" value="{$form.list.page}"/>
    <input type="hidden"
     name="{g->formVar var="form[list][maxPages]"}" value="{$form.list.maxPages}"/>

    {if ($form.list.page > 1)}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminUsers"
     arg3="form[list][page]=1"}">{g->text text="&laquo; first"}</a>
    &nbsp;
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminUsers"
     arg3="form[list][page]=`$form.list.backPage`"}">{g->text text="&laquo; back"}</a>
    {/if}

    &nbsp;
    {g->text text="Viewing page %d of %d" arg1=$form.list.page arg2=$form.list.maxPages}
    &nbsp;

    {if ($form.list.page < $form.list.maxPages)}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminUsers"
     arg3="form[list][page]=`$form.list.nextPage`"}">{g->text text="next &raquo;"}</a>
    &nbsp;
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminUsers"
     arg3="form[list][page]=`$form.list.maxPages`"}">{g->text text="last &raquo;"}</a>
    {/if}
  </span></div>
  {/if}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Username"} </th>
      <th> {g->text text="Locked"} </th>
      <th> {g->text text="Failed Logins"} </th>
      <th> {g->text text="Action"} </th>
    </tr>

    {foreach from=$form.list.userNames key=userId item=user}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$user.userName}
      </td>
      <td>
	{if $user.locked}
        {g->text text="locked"}
        {else}
        &nbsp;
        {/if}
      </td>
      <td>
	{if $user.failedLogins}
        <span class="giWarning">{$user.failedLogins}</span>
        {else}
        &nbsp;
        {/if}
      </td>
      <td>
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminEditUser"
	 arg3="userId=$userId"}">{g->text text="edit"}</a>

	{if ($user.can.delete)}
	  &nbsp;
	  <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminDeleteUser"
	   arg3="userId=$userId"}">{g->text text="delete"}</a>
	{/if}
      </td>
    </tr>
    {/foreach}
  </table>

  {if !empty($form.list.filter) || $form.list.maxPages > 1}
    <input type="text"
     name="{g->formVar var="form[list][filter]"}" value="{$form.list.filter}"/>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][filterBySubstring]"}" value="{g->text text="Filter"}"/>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][filterClear]"}" value="{g->text text="Clear"}"/>
  {/if}

  {if (!empty($form.list.filter))}
    <span style="white-space: nowrap">
      &nbsp;
      {g->text one="%d user matches your filter" many="%d users match your filter"
	       count=$form.list.count arg1=$form.list.count}
    </span>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][create]"}" value="{g->text text="Create User"}"/>
</div>
