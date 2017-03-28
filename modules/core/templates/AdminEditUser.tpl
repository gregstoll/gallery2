{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Edit a user"} </h2>
</div>

<div class="gbBlock">
  <h4>
    {g->text text="Username"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
  </h4>

  <input type="hidden" name="{g->formVar var="userId"}" value="{$AdminEditUser.user.id}"/>
  <input type="text" id="giFormUsername" size="30"
   name="{g->formVar var="form[userName]"}" value="{$form.userName}"/>

  {if isset($form.error.userName.duplicate)}
  <div class="giError">
    {g->text text="That username is already in use"}
  </div>
  {/if}
  {if isset($form.error.userName.missing)}
  <div class="giError">
    {g->text text="You must enter a new username"}
  </div>
  {/if}

  <h4> {g->text text="Full Name"} </h4>

  <input type="text" size="32"
   name="{g->formVar var="form[fullName]"}" value="{$form.fullName}"/>

  {if $AdminEditUser.show.email}
  <h4>
    {g->text text="E-mail Address"}
    <span class="giSubtitle"> {g->text text="(suggested)"} </span>
  </h4>

  <input type="text" size="32" name="{g->formVar var="form[email]"}" value="{$form.email}"/>

  {if isset($form.error.email.missing)}
  <div class="giError">
    {g->text text="You must enter an email address"}
  </div>
  {/if}
  {if isset($form.error.email.invalid)}
  <div class="giError">
    {g->text text="Invalid email address"}
  </div>
  {/if}
  {/if}

  {if $AdminEditUser.show.language}
  <h4> {g->text text="Language"} </h4>

  <select name="{g->formVar var="form[language]"}">
    {html_options options=$AdminEditUser.languageList selected=$form.language}
  </select>
  {/if}

  {if $AdminEditUser.show.password}
  <h4>
    {g->text text="Password"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
  </h4>

  <input type="password" size="32" name="{g->formVar var="form[password1]"}"/>

  {if isset($form.error.password1.missing)}
  <div class="giError">
    {g->text text="You must enter a password"}
  </div>
  {/if}

  <h4>
    {g->text text="Verify Password"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
  </h4>

  <input type="password" size="32" name="{g->formVar var="form[password2]"}"/>

  {if isset($form.error.password2.missing)}
  <div class="giError">
    {g->text text="You must enter the password a second time"}
  </div>
  {/if}
  {if isset($form.error.password2.mismatch)}
  <div class="giError">
    {g->text text="The passwords you entered did not match"}
  </div>
  {/if}
  {/if}

  {if $AdminEditUser.show.locked || $AdminEditUser.failedLoginCount}
  <h4> {g->text text="Options"} </h4>
  {if $AdminEditUser.show.locked}
  <p>
    <input id="AdminEditUser_lockUser" type="checkbox" name="{g->formVar var="form[locked]"}" {if $form.locked}checked="checked"{/if}>
    <label for="AdminEditUser_lockUser">
      <b>{g->text text="Lock user."}</b>
      <span class="giInfo">{g->text text="Locked users are unable to edit their own account information. (Password, Name, Email, etc.)"}</span>
    </label>
  </p>
  {/if}
  {if $AdminEditUser.failedLoginCount}
  <p>
    <input id="AdminEditUser_failedLoginAttempts" type="checkbox" name="{g->formVar var="form[action][resetFailedLogins]"}">
    <label for="AdminEditUser_failedLoginAttempts">
      <b>{g->text text="Reset failed login count."}</b>
      <span class="giWarning">
	{g->text one="%d failed login attempt since the last successful login."
		 many="%d failed login attempts since the last successful login."
		 count=$AdminEditUser.failedLoginCount
		 arg1=$AdminEditUser.failedLoginCount}
      </span>
    </label>
  </p>
  {/if}
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][undo]"}" value="{g->text text="Reset"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
