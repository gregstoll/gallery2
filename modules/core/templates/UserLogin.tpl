{*
 * $Revision: 17578 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Login to your account"} </h2>
</div>

{capture name="recoverUrl"}{g->url arg1="view=core.UserAdmin" arg2="subView=core.UserRecoverPassword" arg3="return=1"}{/capture}
{if $user.isGuest || !empty($reauthenticate)}
<div class="gbBlock">
  {if isset($status.passwordRecovered)}
  <div class="gbBlock"><h2 class="giSuccess">
    {g->text text="Your password has been recovered, please login."}
  </h2></div>
  {/if}

  {if !empty($reauthenticate)}
    <div class="giWarning">
      {g->text text="The administration session has expired, please re-authenticate to access the administration area."}
    </div>
    <br/>
  {/if}
  <h4> {g->text text="Username"} </h4>

  <input type="text" id="giFormUsername" size="16"
   name="{g->formVar var="form[username]"}" value="{$form.username}"/>

  <script type="text/javascript">
    document.getElementById('userAdminForm')['{g->formVar var="form[username]"}'].focus();
  </script>

  {if isset($form.error.username.missing)}
  <div class="giError">
    {g->text text="You must enter a username"}
  </div>
  {/if}

  {if isset($form.error.username.disabled)}
  <div class="giError">
    {g->text text="Logins to this account are temporarily disabled due to multiple failed login attempts.  Wait for access to be restored, or use the %srecover password%s page to re-enable this account." arg1="<a href=\"`$smarty.capture.recoverUrl`\">" arg2="</a>"}
  </div>
  {/if}

  <h4> {g->text text="Password"} </h4>

  <input type="password" id="giFormPassword" size="16" name="{g->formVar var="form[password]"}"/>

  {if isset($form.error.password.missing)}
  <div class="giError">
    {g->text text="You must enter a password"}
  </div>
  {/if}
  {if isset($form.error.invalidPassword)}
  <div class="giError">
    {g->text text="Your login information is incorrect.  Please try again."}
  </div>
  {/if}
</div>

{* Include our ValidationPlugins *}
{g->callback type="core.LoadValidationPlugins" key="core.UserLogin."|cat:$form.username}
{foreach from=$block.core.ValidationPlugins item=plugin}
  {include file="gallery:`$plugin.file`" l10Domain=$plugin.l10Domain}
{/foreach}

<div class="gbBlock">
  {g->text text="Lost or forgotten passwords can be retrieved using the %srecover password%s page" arg1="<a href=\"`$smarty.capture.recoverUrl`\">" arg2="</a>"}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][login]"}" value="{g->text text="Login"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
{else} {* User is already logged in *}
<div class="gbBlock">
  <h4> {g->text text="Welcome, %s!" arg1=$user.fullName|default:$user.userName} </h4>
</div>
{/if}
