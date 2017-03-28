{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Change Password"} </h2>
</div>

<div class="gbBlock">
  {g->text text="This page will allow you to reset the password on your account."}
</div>

<div class="gbBlock">
  {capture name="recoverUrl"}
  {g->url arg1="view=core.UserAdmin" arg2="subView=core.UserRecoverPassword"}
  {/capture}

  {if isset($form.error.request.missing)}
  <div class="giError">
    {g->text text="There is no request which matches the username and authorization provided. Request a new authorization from the %slost password page%s." arg1="<a href=\"`$smarty.capture.recoverUrl`\">" arg2="</a>"}
  </div>
  {/if}

  {if isset($form.error.request.tooOld)}
  <div class="giError">
    {g->text text="The request you are trying to access has expired.  Request a new authorization from the %slost password page%s." arg1="<a href=\"`$smarty.capture.recoverUrl`\">" arg2="</a>"}
  </div>
  {/if}

  {if isset($form.error.authString.missing)}
  <div class="giError">
    {g->text text="Authorization missing"}
  </div>
  {/if}

  {if isset($form.error.userName.missing)}
  <div class="giError">
    {g->text text="Username missing"}
  </div>
  {/if}

  <input type="hidden" id="giFormUsername" size="16"
   name="{g->formVar var="form[userName]"}" value="{$form.userName}"/>
  <input type="hidden" id="giFormUsername" size="16"
   name="{g->formVar var="form[authString]"}" value="{$form.authString}"/>

  <div>
    <h4>{g->text text="New Password"}</h4>

    <input type="password" name="{g->formVar var="form[password1]"}"/>
    {if isset($form.error.password.missing)}
    <div class="giError">
      {g->text text="You must enter a new password"}
    </div>
    {/if}
  </div>

  <div>
    <h4>{g->text text="Verify New Password"}</h4>

    <input type="password" name="{g->formVar var="form[password2]"}"/>
  </div>
  
  {if isset($form.error.password.mismatch)}
  <div class="giError">
    {g->text text="The passwords you entered did not match"}
  </div>
  {/if}

  <script type="text/javascript">
    document.getElementById('userAdminForm')['{g->formVar var="form[password1]"}'].focus();
  </script>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][submit]"}" value="{g->text text="Submit"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
