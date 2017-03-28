{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Permissions"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.changedOwner)}
    {g->text text="Owner changed successfully"}
  {/if}
  {if isset($status.addedGroupPermission)}
    {g->text text="Group permission added successfully"}
  {/if}
  {if isset($status.addedUserPermission)}
    {g->text text="User permission added successfully"}
  {/if}
  {if isset($status.deletedGroupPermission)}
    {g->text text="Group permission removed successfully"}
  {/if}
  {if isset($status.deletedUserPermission)}
    {g->text text="User permission removed successfully"}
  {/if}
</h2></div>
{/if}

{if $ItemPermissions.can.changePermissions}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Each item has its own independent set of permissions.  Changing the parent's permissions has no effect on the permissions of the child.  This allows you to restrict access to the parent of this item, but still grant full access to this item, or vice versa.  The most efficient way to use this permission system is to create groups and assign permissions to them.  Then if you want to grant permissions to a specific user, you can add (or remove) the user from the appropriate group."}
  </p>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Owner"} </h3>

  <p class="giDescription">
    {if empty($ItemPermissions.owner.fullName)}
      {g->text text="This item is owned by user: %s" arg1=$ItemPermissions.owner.userName}
    {else}
      {g->text text="This item is owned by user: %s (%s)"
	       arg1=$ItemPermissions.owner.userName arg2=$ItemPermissions.owner.fullName}
    {/if}
  </p>

  {if $ItemPermissions.can.changeOwner}
    <h4> {g->text text="New owner"} </h4>

    <input type="text" id="giFormUsername" autocomplete="off"
     name="{g->formVar var="form[owner][ownerName]"}" value="{$form.owner.ownerName}"/>
    {g->autoComplete element="giFormUsername"}
      {g->url arg1="view=core.SimpleCallback" arg2="command=lookupUsername"
	      arg3="prefix=__VALUE__" htmlEntities=false}
    {/g->autoComplete}

    <input type="hidden" name="{g->formVar var="form[serialNumber]"}" value="{$form.serialNumber}"/>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][changeOwner]"}" value="{g->text text="Change"}"/>

    {if $ItemPermissions.can.applyToSubItems}
    <p class="giDescription">
    <input type="checkbox" checked="checked"
      name="{g->formVar var="form[applyOwnerToSubItems]"}"
      value="{g->text text="Apply new owner to sub-items"}"/>
      {g->text text="Apply new owner to sub-items"}
    </p>
    {/if}

    {if !empty($form.error.owner.missingUser)}
    <div class="giError">
      {g->text text="You must enter a user name"}
    </div>
    {/if}
    {if !empty($form.error.owner.invalidUser)}
    <div class="giError">
      {g->text text="The user name you entered is invalid"}
    </div>
    {/if}
  {/if}
</div>

{if $ItemPermissions.can.changePermissions && $ItemPermissions.can.applyToSubItems}
<div class="gbBlock">
  <h3> {g->text text="Apply changes"} </h3>

  <p class="giDescription">
    {g->text text="This item has sub-items.  The changes you make here can be applied to just this item, or you can apply them to all sub-items.  Note that applying changes to sub-items will merge your change into the existing permissions of the sub-items and may be very time consuming if you have many sub-items.  It's more efficient to grant permissions to groups and then add and remove users from groups whenever possible. Changes are applied to sub-items by default."}
  </p>

  <input type="checkbox" checked="checked"
   name="{g->formVar var="form[applyToSubItems]"}" value="{g->text text="Apply to sub-items"}"/>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Group Permissions"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Group name"} </th>
    <th> {g->text text="Permission"} </th>
    {if $ItemPermissions.can.changePermissions}
    <th> {g->text text="Action"} </th>
    {/if}
  </tr>
  {if $ItemPermissions.groupPermissions}
    {section name=group loop=$ItemPermissions.groupPermissions}
    {assign var="entry" value=$ItemPermissions.groupPermissions[group]}
    {assign var="index" value=$smarty.section.group.iteration}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$entry.group.groupName}
      </td><td>
	{$entry.permission.description}
      </td>
      {if $ItemPermissions.can.changePermissions}
      <td>
	{if !empty($entry.deleteList)}
	  <select name="{g->formVar var="form[group][delete][$index]"}" size="1">
	  {foreach from=$entry.deleteList item=deleteEntry}
	    <option value="{$entry.group.id},{$deleteEntry.id}"{if ($deleteEntry.id == $entry.permission.id)} selected="selected"{/if}>{$deleteEntry.description}</option>
	  {/foreach}
	  </select>
	  <input type="submit" class="inputTypeSubmit"
	   name="{g->formVar var="form[action][deleteGroupPermission][$index]"}"
	   value="{g->text text="Remove"}"/>
	{else}
	  &nbsp;
	{/if}
      </td>
      {/if}
    </tr>
    {/section}
  {/if}
  </table>
</div>

{if $ItemPermissions.can.changePermissions}
<div class="gbBlock">
  <h3> {g->text text="New Group Permission"} </h3>

  <input type="text" id="giFormGroupname" autocomplete="off"
   name="{g->formVar var="form[group][groupName]"}" value="{$form.group.groupName}"/>
  {g->autoComplete element="giFormGroupname"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupGroupname"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  <select name="{g->formVar var="form[group][permission]"}" size="1">
    {html_options options=$ItemPermissions.allPermissions selected=$form.group.permission}
  </select>

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][addGroupPermission]"}"
   value="{g->text text="Add Permission"}"/>

  {if !empty($form.error.group.invalidPermission)}
  <div class="giError">
    {g->text text="The permission you chose is invalid"}
  </div>
  {/if}
  {if !empty($form.error.group.missingGroup)}
  <div class="giError">
    {g->text text="You must enter a group name"}
  </div>
  {/if}
  {if !empty($form.error.group.invalidGroup)}
  <div class="giError">
    {g->text text="The group name you entered is invalid"}
  </div>
  {/if}
  {if !empty($form.error.group.alreadyHadPermission)}
  <div class="giError">
    {g->text text="Group already has this permission (check sub-permissions)"}
  </div>
  {/if}
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="User Permissions"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="User name"} </th>
    <th> {g->text text="Permission"} </th>
    {if $ItemPermissions.can.changePermissions}
    <th> {g->text text="Action"} </th>
    {/if}
  </tr>
  {if $ItemPermissions.userPermissions}
    {section name=user loop=$ItemPermissions.userPermissions}
    {assign var="entry" value=$ItemPermissions.userPermissions[user]}
    {assign var="index" value=$smarty.section.user.iteration}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$entry.user.userName}
      </td><td>
	{$entry.permission.description}
      </td>
      {if $ItemPermissions.can.changePermissions}
      <td>
	{if !empty($entry.deleteList)}
	  <select name="{g->formVar var="form[user][delete][$index]"}" size="1">
	  {foreach from=$entry.deleteList item=deleteEntry}
	    <option value="{$entry.user.id},{$deleteEntry.id}"{if ($deleteEntry.id == $entry.permission.id)} selected="selected"{/if}>{$deleteEntry.description}</option>
	  {/foreach}
	  </select>
	  <input type="submit" class="inputTypeSubmit"
	   name="{g->formVar var="form[action][deleteUserPermission][$index]"}"
	   value="{g->text text="Remove"}"/>
	{else}
	  &nbsp;
	{/if}
      </td>
      {/if}
    </tr>
    {/section}
  {/if}
  </table>
</div>

{if $ItemPermissions.can.changePermissions}
<div class="gbBlock">
  <h3> {g->text text="New User Permission"} </h3>

  <input type="text" id="giFormUsername2" class="giFormUsername" autocomplete="off"
   name="{g->formVar var="form[user][userName]"}" value="{$form.user.userName}"/>
  {g->autoComplete element="giFormUsername2"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupUsername"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  <select name="{g->formVar var="form[user][permission]"}" size="1">
    {html_options options=$ItemPermissions.allPermissions selected=$form.user.permission}
  </select>

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][addUserPermission]"}"
   value="{g->text text="Add Permission"}"/>

  {if !empty($form.error.user.invalidPermission)}
  <div class="giError">
    {g->text text="The permission you chose is invalid"}
  </div>
  {/if}
  {if !empty($form.error.user.missingUser)}
  <div class="giError">
    {g->text text="You must enter a user name"}
  </div>
  {/if}
  {if !empty($form.error.user.invalidUser)}
  <div class="giError">
    {g->text text="The user name you entered is invalid"}
  </div>
  {/if}
  {if !empty($form.error.user.alreadyHadPermission)}
  <div class="giError">
    {g->text text="The user already has this permission (check sub-permissions)"}
  </div>
  {/if}
</div>
{/if}
