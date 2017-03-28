{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Recover a lost or forgotten password"} </h2>
</div>

<div class="gbBlock">
  {g->text text="Recovering your password requires that your user account has an email address assigned, and that you have access to the listed email address.  A confirmation will be emailed to you containing a URL which you must visit to set a new password for your account.  To prevent abuse, password recovery requests can not be attempted more than once in a 20 minute period.  A recovery confirmation is valid for seven days.  If it is not used during that time, it will be purged from the system and a new request will have to be made."}
</div>

{if isset($status.requestSent)}
<div class="gbBlock">
  {capture name="adminResetUrl"}
  <a href='{g->url arg1="view=core.UserAdmin" arg2="subView=core.UserRecoverPasswordAdmin"}'>
  {/capture}
  <div class="gbBlock">
    <h2 class="giSuccess">
      {g->text text="Your recovery request has been sent!"}
    </h2>
    {g->text text="Note that if the account does not have an email address, you may not receive the email and you should contact your system administrator for help."}
    <br/><br/>
    {g->text text="Administrators can use the %sEmergency Password Recovery%s page to recover the admin account if they fail to receive recovery email due to server problems, or lack of a working email address." arg1=$smarty.capture.adminResetUrl arg2="</a>"}
  </div>
</div>
{else}
<div class="gbBlock">
  <h4>{g->text text="Username"}</h4>

  <input type="text" id="giFormUsername" size="16"
   name="{g->formVar var="form[userName]"}" value="{$form.userName}"/>

  <script type="text/javascript">
    document.getElementById('userAdminForm')['{g->formVar var="form[userName]"}'].focus();
  </script>

  {if isset($form.error.userName.missing)}
  <div class="giError">
    {g->text text="You must enter a username"}
  </div>
  {/if}
</div>

{* Include our extra ItemAddOptions *}
{foreach from=$UserRecoverPassword.plugins item=plugin}
  {include file="gallery:`$plugin.file`" l10Domain=$plugin.l10Domain}
{/foreach}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][recover]"}" value="{g->text text="Recover"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
{/if}