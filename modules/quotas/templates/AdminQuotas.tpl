{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
 <div class="gbBlock gcBackground1">
  <h2> {g->text text="Quotas Administration"} </h2>
</div>
    
<div class="gbBlock">
  {if !empty($status)}
  <h3 class="giSuccess">
    {if isset($status.deletedGroup)}
      {g->text text="Removed group quota for '%s'" arg1=$status.deletedGroup}
    {/if}
    {if isset($status.createdGroup)}
      {g->text text="Created group quota for '%s'" arg1=$status.createdGroup}
    {/if}
    {if isset($status.modifiedGroup)}
      {g->text text="Modified group quota for '%s'" arg1=$status.modifiedGroup}
    {/if}
    {if isset($status.deletedUser)}
      {g->text text="Removed user quota for '%s'" arg1=$status.deletedUser}
    {/if}
    {if isset($status.createdUser)}
      {g->text text="Created user quota for '%s'" arg1=$status.createdUser}
    {/if}
    {if isset($status.modifiedUser)}
      {g->text text="Modified user quota for '%s'" arg1=$status.modifiedUser}
    {/if}
  </h3>
  {/if}

  <p class="giDescription">
    {g->text text="This system will let you assign disk space quotas to users. By creating a group quota, any users in that group will be assigned that quota. If a user belongs to more than one group, the highest quota of all the groups will be assigned. Although, if you create a user quota that will override any group quotas."}
  </p>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text one="There is %d group quota in the system." 
             many="There are %d total group quotas in the system." 
             count=$AdminQuotas.totalGroupCount
             arg1=$AdminQuotas.totalGroupCount}
  </p>
</div>

<div class="gbBlock">
  <h3> 
    {g->text text="Group Quotas"}
  </h3>

  <input id="giFormGroupname" type="text" size="20" autocomplete="off"
         name="{g->formVar var="form[text][groupName]"}" 
                           value="{$form.text.groupName}"/>
  {g->autoComplete element="giFormGroupname"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupGroupname"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][group][createFromText]"}" 
	 value="{g->text text="Create Quota"}"/>
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][group][editFromText]"}" 
	 value="{g->text text="Edit Quota"}"/>
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][group][deleteFromText]"}" 
	 value="{g->text text="Delete Quota"}"/>

  {if isset($form.error.text.noSuchGroupQuota)}
  <div class="giError">
    {g->text text="Group quota for '%s' does not exist. You must create one first." 
	     arg1=$form.text.groupName}
  </div>
  {/if}

  {if isset($form.error.text.GroupQuotaExists)}
  <div class="giError">
    {g->text text="Group quota for '%s' already exists." arg1=$form.text.groupName}
  </div>
  {/if}

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

</div>
{if ($AdminQuotas.totalGroupCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text text="Edit Group Quotas (by list)"}
  </h3>
  
  {if ($form.list.group.maxPages > 1)}
  <div style="margin-bottom: 10px">
    <span class="gcBackground1" style="padding: 5px">
    <input type="hidden" name="{g->formVar var="form[list][group][page]"}" 
           value="{$form.list.page}"/>
    <input type="hidden" name="{g->formVar var="form[list][group][maxPages]"}" 
           value="{$form.list.maxPages}"/>
    
    {if ($form.list.group.page > 1)}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                       arg3="form[list][group][page]=1"}">
        {g->text text="&laquo; first"}
      </a>
      &nbsp;
      <a href="{g->url arg1="view=core.SiteAdmin" 
                       arg2="subView=quotas.AdminQuotas" 
                       arg3="form[list][group][page]=`$form.list.group.backPage`"}">
        {g->text text="&laquo; back"}
      </a>
    {/if}
    &nbsp;
    {g->text text="Viewing page %d of %d"
	     arg1=$form.list.group.page
	     arg2=$form.list.group.maxPages}

    {if ($form.list.group.page < $form.list.group.maxPages)}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                     arg3="form[list][group][page]=`$form.list.group.nextPage`"}">
      {g->text text="next &raquo;"}
    </a>
    &nbsp;
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                     arg3="form[list][group][page]=`$form.list.group.maxPages`"}">
      {g->text text="last &raquo;"}
    </a>
    {/if}
    </span>
  </div>
  {/if}
  <table class="gbDataTable">
    <tr>
      <th>
        {g->text text="Group Name"}
      </th>
      <th>
        {g->text text="Quota"}
      </th>
      <th>
        {g->text text="Action"}
      </th>
    </tr>

    {foreach from=$form.list.groupNames key=groupId item=group}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
        {$group.groupName}
      </td>
      <td>
        {$group.quotaSize} {$group.quotaUnit}
      </td>
      <td>
        <a href="{g->url arg1="view=core.SiteAdmin" 
                         arg2="subView=quotas.AdminEditGroupQuota" 
                         arg3="groupId=$groupId"}">
          {g->text text="edit"}
        </a>
        &nbsp;
        <a href="{g->url arg1="view=core.SiteAdmin" 
                         arg2="subView=quotas.AdminDeleteGroupQuota" 
                         arg3="groupId=$groupId"}">
          {g->text text="delete"}
        </a>
      </td>
    </tr>
    {/foreach}
  </table>
</div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
  {g->text one="There is %d user quota in the system."
           many="There are %d total user quotas in the system."
           count=$AdminQuotas.totalUserCount
           arg1=$AdminQuotas.totalUserCount}
  </p>
</div>

<div class="gbBlock">
  <h3>
    {g->text text="User Quotas"}
  </h3>

  <input id="giFormUsername" type="text" size="20" autocomplete="off" 
         name="{g->formVar var="form[text][userName]"}" value="{$form.text.userName}"/>
  {g->autoComplete element="giFormUsername"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupUsername" 
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][user][createFromText]"}" 
         value="{g->text text="Create Quota"}"/>
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][user][editFromText]"}" 
         value="{g->text text="Edit Quota"}"/>
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][user][deleteFromText]"}" 
         value="{g->text text="Delete Quota"}"/>

  {if isset($form.error.text.noSuchUserQuota)}
  <div class="giError">
    {g->text text="User quota for '%s' does not exist. You must create one first." 
	     arg1=$form.text.userName}
  </div>
  {/if}

  {if isset($form.error.text.UserQuotaExists)}
  <div class="giError">
    {g->text text="User quota for '%s' already exists." arg1=$form.text.userName}
  </div>
  {/if}

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
</div>

{if ($AdminQuotas.totalUserCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text text="Edit User Quotas (by list)"}
  </h3>
  
  {if ($form.list.user.maxPages > 1)}
  <div style="margin-bottom: 10px">
    <span class="gcBackground1" style="padding: 5px">
    <input type="hidden" name="{g->formVar var="form[list][page]"}" 
           value="{$form.list.user.page}"/>
    <input type="hidden" name="{g->formVar var="form[list][maxPages]"}" 
           value="{$form.list.user.maxPages}"/>

    {if ($form.list.user.page > 1)}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                     arg3="form[list][user][page]=1"}">
      {g->text text="&laquo; first"}
    </a>
    &nbsp;
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                     arg3="form[list][user][page]=`$form.list.user.backPage`"}">
      {g->text text="&laquo; back"}
    </a>
    {/if}
    &nbsp;
    {g->text text="Viewing page %d of %d"
	     arg1=$form.list.user.page
	     arg2=$form.list.user.maxPages}
    &nbsp;

    {if ($form.list.page < $form.list.user.maxPages)}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                     arg3="form[list][user][page]=`$form.list.user.nextPage`"}">
      {g->text text="next &raquo;"}
    </a>
    &nbsp;
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminQuotas" 
                     arg3="form[list][user][page]=`$form.list.user.maxPages`"}">
      {g->text text="last &raquo;"}
    </a>
    {/if}
    </span>
  </div>
  {/if}
  
  <table class="gbDataTable">
    <tr>
      <th>
        {g->text text="Username"}
      </th>
      <th>
        {g->text text="Quota"}
      </th>
      <th>
        {g->text text="Action"}
      </th>
    </tr>

    {foreach from=$form.list.userNames key=userId item=user}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
        {$user.userName}
      </td>
      <td>
	{$user.quotaSize} {$user.quotaUnit}
      </td>
      <td>
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminEditUserQuota" 
	                 arg3="userId=$userId"}">
	  {g->text text="edit"}
	</a>
	&nbsp;
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=quotas.AdminDeleteUserQuota" 
	                 arg3="userId=$userId"}">
	  {g->text text="delete"}
	</a>
      </td>
    </tr>
    {/foreach}
  </table>
</div>
{/if}
