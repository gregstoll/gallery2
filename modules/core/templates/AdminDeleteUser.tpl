{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Delete a User"} </h2>
</div>

{if isset($AdminDeleteUser.numberOfItems) && $AdminDeleteUser.numberOfItems > 0}
<div class="gbBlock">
  <h3>
    {g->text text="User %s is the owner of %s items." arg1=$AdminDeleteUser.user.userName
	     arg2=$AdminDeleteUser.numberOfItems}
  </h3>
  <p class="giDescription">
    {g->text text="Delete user <strong>%s</strong> and..." arg1=$AdminDeleteUser.user.userName}

    <table class="gbDataTable"><tr>
      <td>
	<input type="radio" id="rbAssignNewOwner" checked="checked"
	 name="{g->formVar var="form[deletionVariant]"}" value="assignNewOwner"/>
      </td><td>
	<label for="rbAssignNewOwner">
	  {g->text text="Assign a new owner for all items of %s"
		   arg1=$AdminDeleteUser.user.userName}
	</label>
      </td></tr><tr><td>
	<input type="radio" id="rbDeleteItems"
	 name="{g->formVar var="form[deletionVariant]"}" value="deleteItems"/>
      </td><td>
	<label for="rbDeleteItems"> {g->text
	  text="Delete all items of %s and assign a new owner for all remaining non empty albums. Items that %s doesn't have permission to delete will also be reassigned to a new owner."
	  arg1=$AdminDeleteUser.user.userName arg2=$AdminDeleteUser.user.userName}
	</label>
      </td>
    </tr><tr>
      <td></td>
      <td>
	<p> {g->text text="New owner (leaving blank means one of the Site Admins):"} </p>

	<input type="text" id="giFormUsername" size="20" autocomplete="off"
	 name="{g->formVar var="form[text][newOwner]"}" value="{$form.text.newOwner}"/>
	{g->autoComplete element="giFormUsername"}
	  {g->url arg1="view=core.SimpleCallback" arg2="command=lookupUsername"
		  arg3="prefix=__VALUE__" htmlEntities=false}
	{/g->autoComplete}

	{if isset($form.error.text.noSuchUser)}
	<div class="giError">
	  {g->text text="User '%s' does not exist! Cannot assign items to a nonexistent user."
		   arg1=$form.text.newOwner}
	</div>
	{/if}
	{if isset($form.error.text.newOwnerIsDeletedUser)}
	<div class="giError">
	  {g->text text="The new owner must be a different user than the user we are deleting!"}
	</div>
	{/if}
	{if isset($form.error.text.newOwnerIsGuest)}
	<div class="giError">
	  {g->text text="The new owner cannot be a Guest / Anonymous user!"}
	</div>
	{/if}
      </td>
    </tr></table>
  </p>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Are you sure?"} </h3>

  <p class="giDescription">
    {g->text text="This will completely remove <strong>%s</strong> from Gallery.  There is no undo!"
	     arg1=$AdminDeleteUser.user.userName}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="userId"}" value="{$AdminDeleteUser.user.id}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
