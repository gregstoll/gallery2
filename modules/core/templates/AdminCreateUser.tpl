{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Create A New User"} </h2>
</div>

<div class="gbBlock">
  <div>
    <h4>
      {g->text text="Username"}
      <span class="giSubtitle"> {g->text text="(required)"} </span>
    </h4>

    <input type="text" size="32"
     name="{g->formVar var="form[userName]"}" value="{$form.userName}"/>
    <script type="text/javascript">
      document.getElementById('siteAdminForm')['{g->formVar var="form[userName]"}'].focus();
    </script>

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
  </div>

  <div>
    <h4> {g->text text="Full Name"} </h4>

    <input type="text" size="32"
     name="{g->formVar var="form[fullName]"}" value="{$form.fullName}"/>
  </div>

  <div>
    <h4>
      {g->text text="Email Address"}
      <span class="giSubtitle"> {g->text text="(required)"} </span>
    </h4>

    <input type="text" size="32"
     name="{g->formVar var="form[email]"}" value="{$form.email}"/>

    {if isset($form.error.email.missing)}
    <div class="giError">
      {g->text text="You must enter an email address"}
    </div>
    {/if}
  </div>

  <div>
    <h4> {g->text text="Language"} </h4>

    <select name="{g->formVar var="form[language]"}">
      {html_options options=$AdminCreateUser.languageList selected=$form.language}
    </select>
  </div>

  <div>
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
  </div>

  <div>
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
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][create]"}" value="{g->text text="Create User"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
