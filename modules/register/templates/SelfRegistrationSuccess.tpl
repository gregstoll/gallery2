{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Registration successful"} </h2>
</div>

<div class="gbBlock">
{if $SelfRegistrationSuccess.pending}
  <h2 class="giTitle">
    {g->text text="Your registration was successful."}
  </h2>
  <p class="giDescription">
    {if $SelfRegistrationSuccess.sentConfirmationEmail}
      {g->text text="You will shortly receive an email containing a link. You have to click this link to confirm and activate your account.  This procedure is necessary to prevent account abuse."}
    {else}
      {g->text text="Your registration will be processed and your account activated soon."}
    {/if}
  </p>
{else}
  <h2 class="giTitle">
      {g->text text="Your registration was successful and your account has been activated."}
  </h2>
  <p class="giDescription">
      {capture name=loginLink}
      <a href="{g->url arg1="view=core.UserAdmin" arg2="subView=core.UserLogin" 
        arg3="return=1" forceFullUrl=true}">
      {/capture}
      {capture name=loginLinkEnd}
      </a>
      {/capture}
      {g->text text="You can now %slogin%s to your account with your username and password."
	       arg1=$smarty.capture.loginLink arg2=$smarty.capture.loginLinkEnd}
  </p>
{/if}
</div>
