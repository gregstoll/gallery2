{*
 * $Revision: 16925 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if $user.isGuest}
<div class="{$class}">
  <form id="LoginForm" action="{g->url arg1="return=true"}" method="post">
    <div>
      {g->hiddenFormVars}
      <input type="hidden" name="{g->formVar var="controller"}" value="core.UserLogin" />
      <input type="hidden" name="{g->formVar var="form[formName]"}" value="UserLogin" />

      <input type="text" id="giFormUsername" size="10"
	     name="{g->formVar var="form[username]"}" value="{g->text text="Username"}"
	     onfocus="var f=document.getElementById('giFormUsername'); if (f.value == '{g->text text="Username" forJavascript=1}') {ldelim} f.value = '' {rdelim}"
	     onblur="var f=document.getElementById('giFormUsername'); if (f.value == '') {ldelim} f.value = '{g->text text="Username" forJavascript=1}' {rdelim}"/>
      <input type="password" id="giFormPassword" size="10"
	    name="{g->formVar var="form[password]"}"/>

      {* Include our ValidationPlugins *}
      {g->callback type="core.LoadValidationPlugins"}
      {foreach from=$block.core.ValidationPlugins item=plugin}
	{include file="gallery:`$plugin.file`" l10Domain=$plugin.l10Domain}
      {/foreach}

      <input type="submit" class="inputTypeSubmit"
	     name="{g->formVar var="form[action][login]"}" value="{g->text text="Login"}"/>
    </div>
  </form>
</div>
{/if}

