{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Group Management"} </h2>
</div>

<div class="gbBlock">
  {if !empty($status)}
  <h3 class="giSuccess">
    {if isset($status.deletedGroup)}
      {g->text text="Removed group '%s'" arg1=$status.deletedGroup}
    {/if}
    {if isset($status.createdGroup)}
      {g->text text="Created group '%s'" arg1=$status.createdGroup}
    {/if}
    {if isset($status.modifiedGroup)}
      {g->text text="Modified group '%s'" arg1=$status.modifiedGroup}
    {/if}
  </h3>
  {/if}

  <p class="giDescription">
    {g->text one="There is %d group in the system."
	     many="There are %d total groups in the system."
	     count=$AdminGroups.totalGroupCount arg1=$AdminGroups.totalGroupCount}
  </p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Edit Group"} </h3>

  <input type="text" id="giFormGroupname" size="20" autocomplete="off"
   name="{g->formVar var="form[text][groupName]"}" value="{$form.text.groupName}"/>
  {g->autoComplete element="giFormGroupname"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupGroupname"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][editFromText]"}" value="{g->text text="Edit"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][deleteFromText]"}" value="{g->text text="Delete"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][addRemoveUsersFromText]"}"
   value="{g->text text="Add/Remove Users"}"/>

  {if isset($form.error.text.noSuchGroup)}
  <div class="giError">
    {g->text text="Group '%s' does not exist." arg1=$form.text.groupName}
  </div>
  {/if}
  {if isset($form.error.text.noGroupSpecified)}
  <div class="giError">
    {g->text text="You must enter a group name"}
  </div>
  {/if}
  {if isset($form.error.text.cantDeleteGroup)}
  <div class="giError">
    {g->text text="You cannot delete that group"}
  </div>
  {/if}
  {if isset($form.error.text.cantEditGroupUsers)}
  <div class="giError">
    {g->text text="You cannot edit that group's users"}
  </div>
  {/if}
</div>

<div class="gbBlock">
  <h3> {g->text text="Edit Group (by list)"} </h3>

  {if ($form.list.maxPages > 1)}
  <div style="margin-bottom: 10px"><span class="gcBackground1" style="padding: 5px">
    <input type="hidden" name="{g->formVar var="form[list][page]"}" value="{$form.list.page}"/>
    <input type="hidden"
     name="{g->formVar var="form[list][maxPages]"}" value="{$form.list.maxPages}"/>

    {if ($form.list.page > 1)}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminGroups"
       arg3="form[list][page]=1"}">{g->text text="&laquo; first"}</a>
      &nbsp;
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminGroups"
       arg3="form[list][page]=`$form.list.backPage`"}">{g->text text="&laquo; back"}</a>
    {/if}

    &nbsp;
    {g->text text="Viewing page %d of %d" arg1=$form.list.page arg2=$form.list.maxPages}
    &nbsp;

    {if ($form.list.page < $form.list.maxPages)}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminGroups"
       arg3="form[list][page]=`$form.list.nextPage`"}">{g->text text="next &raquo;"}</a>
      &nbsp;
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminGroups"
       arg3="form[list][page]=`$form.list.maxPages`"}">{g->text text="last &raquo;"}</a>
    {/if}
  </span></div>
  {/if}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Group Name"} </th>
      <th> {g->text text="Action"} </th>
    </tr>

    {foreach from=$form.list.groupNames key=groupId item=group}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$group.groupName}
      </td>
      <td>
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminEditGroup"
	 arg3="groupId=$groupId"}">{g->text text="edit"}</a>

	{if ($group.can.delete)}
	  &nbsp;
	  <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminDeleteGroup"
	   arg3="groupId=$groupId"}">{g->text text="delete"}</a>
	{/if}

	{if ($group.can.editUsers)}
	  &nbsp;
	  <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminEditGroupUsers"
	   arg3="groupId=$groupId"}">{g->text text="members"}</a>
	{/if}
      </td>
    </tr>
    {/foreach}
  </table>

  {if !empty($form.list.filter) || ($form.list.maxPages > 1)}
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
      {g->text one="%d group matches your filter" many="%d groups match your filter"
	       count=$form.list.count arg1=$form.list.count}
    </span>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][create]"}" value="{g->text text="Create Group"}"/>
</div>
