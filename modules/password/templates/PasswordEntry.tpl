{*
 * $Revision: 17657 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<form action="{g->url}" method="post" id="passwordEntryForm">
<div>
    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="{$controller}"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    <input type="hidden" name="{g->formVar var="form[itemId]"}" value="{$form.itemId}"/>
    <input type="hidden" name="{g->formVar var="itemId"}" value="{$form.itemId}"/>
</div>

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Password Protected Album"} </h2>
</div>
<div class="gbBlock">
  {if isset($form.error.incorrectPassword)}
  <p class="giError">
    Password incorrect, please try again.
  </p>
  {/if}
  <p class="giDescription">
    <label for="Password1_tb">
      {g->text text="Enter the password!"}
    </label>
  </p>
  <input type="password" size="20" id="Password1_tb" name="{g->formVar var="form[password]"}"/>
  <script type="text/javascript">
    // <![CDATA[
    document.getElementById('Password1_tb').focus();
    // ]]>
  </script>
  {* We want submit the password action if enter is pressed *}
  {g->defaultButton name="form[action][password]"}
</div>

{g->callback type="core.LoadValidationPlugins"
	     level=$form.validationLevel key="password.PasswordEntry."|cat:$form.itemId}
{foreach from=$block.core.ValidationPlugins item=plugin}
  {include file="gallery:`$plugin.file`" l10Domain=$plugin.l10Domain}
{/foreach}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][password]"}" value="{g->text text="Submit"}"/>
</div>
</form>
