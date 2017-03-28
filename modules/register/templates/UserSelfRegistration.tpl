{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Register As New User"} </h2>
</div>

<div class="gbBlock">
  <h4>
    {g->text text="Username"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
  </h4>

  <input type="text" size="32" name="{g->formVar var="form[userName]"}" value="{$form.userName}"/>

  {if isset($form.error.userName.missing)}
  <div class="giError">
    {g->text text="You must enter a username"}
  </div>
  {/if}
  {if isset($form.error.userName.exists)}
  <div class="giError">
    {g->text text="Username '%s' already exists" arg1=$form.userName}
  </div>
  {/if}

  <h4>
    {g->text text="Full Name"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
  </h4>

  <input type="text" size="32" name="{g->formVar var="form[fullName]"}" value="{$form.fullName}"/>

  {if isset($form.error.fullName.missing)}
  <div class="giError">
    {g->text text="You must enter your full name"}
  </div>
  {/if}

  <h4>
    {g->text text="Email Address"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
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
</div>

{* Include our extra ItemAddOptions *}
{foreach from=$UserSelfRegistration.plugins item=plugin}
  {include file="gallery:`$plugin.file`" l10Domain=$plugin.l10Domain}
{/foreach}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][create]"}" value="{g->text text="Register"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
