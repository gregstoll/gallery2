{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Change Password"} </h2>
</div>

{if isset($status.changedPassword)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Password changed successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="You must enter your current password to prove that it's you, then enter your new password twice to make sure that you didn't make a mistake."}
  </p>

  <div>
    <h4>
      {g->text text="Current Password"}
      <span class="giSubtitle">
	{g->text text="(required)"}
      </span>
    </h4>

    <input type="password" name="{g->formVar var="form[currentPassword]"}"/>

    {if isset($form.error.currentPassword.missing)}
    <div class="giError">
      {g->text text="You must enter your current password"}
    </div>
    {/if}
    {if isset($form.error.currentPassword.incorrect)}
    <div class="giError">
      {g->text text="Incorrect password"}
    </div>
    {/if}
  </div>

  <div>
    <h4>
      {g->text text="New Password"}
      <span class="giSubtitle">
	{g->text text="(required)"}
      </span>
    </h4>

    <input type="password" name="{g->formVar var="form[password1]"}"/>

    {if isset($form.error.password1.missing)}
    <div class="giError">
      {g->text text="You must enter a new password"}
    </div>
    {/if}
  </div>

  <div>
    <h4>
      {g->text text="Verify New Password"}
      <span class="giSubtitle">
	{g->text text="(required)"}
      </span>
    </h4>

    <input type="password" name="{g->formVar var="form[password2]"}"/>

    {if isset($form.error.password2.missing)}
    <div class="giError">
      {g->text text="You must enter your new password again!"}
    </div>
    {/if}
    {if isset($form.error.password2.mismatch)}
    <div class="giError">
      {g->text text="The passwords you entered did not match"}
    </div>
    {/if}
  </div>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Change"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
