{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Account Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Account settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <div>
    <h4> {g->text text="Username"} </h4>
    <p class="giDescription">
      {$user.userName}
    </p>
  </div>

  <div>
    <h4> {g->text text="Full Name"} </h4>
    <input type="text" name="{g->formVar var="form[fullName]"}" value="{$form.fullName}"/>
  </div>

  <div>
    <h4>
      {g->text text="E-mail Address"}
      <span class="giSubtitle">
      {if !isset($UserAdmin.isSiteAdmin)}
	{g->text text="(required, password required for change)"}
      {else}
	{g->text text="(suggested, password required for change)"}
      {/if}
      </span>
    </h4>

    <input type="text" name="{g->formVar var="form[email]"}" value="{$form.email}"/>

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
  </div>

  {if $UserPreferences.translationsSupported}
  <div>
    <h4> {g->text text="Language"} </h4>

    <select name="{g->formVar var="form[language]"}">
      {html_options options=$UserPreferences.languageList selected=$form.language}
    </select>
  </div>
  {/if}

  <div>
    <h4>
      {g->text text="Current Password"}
      <span class="giSubtitle">
	{g->text text="(required to change the e-mail address)"}
      </span>
    </h4>

    <input type="password" name="{g->formVar var="form[currentPassword]"}"/>

    {if isset($form.error.currentPassword.missing)}
    <div class="giError">
      {g->text text="You must enter your current password to change the e-mail address"}
    </div>
    {/if}
    {if isset($form.error.currentPassword.incorrect)}
    <div class="giError">
      {g->text text="Incorrect password"}
    </div>
    {/if}
  </div>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][undo]"}" value="{g->text text="Reset"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
