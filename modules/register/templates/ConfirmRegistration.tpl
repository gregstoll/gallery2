{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Account Activation"} </h2>
</div>

<div class="gbBlock">
{if isset($form.error.unknownUser)}
  <div class="giError">
    {g->text text="This user cannot be activated."}
  </div>
  <p class="giDescription"}>
    {g->text text="This can happen if the URL you entered is not correct or you already activated this account. Please check if your mail client broke the link into several lines and append them without spaces."}
  </p>
{else}

  <p class="giDescription"}>
    {g->text text="Your account has been activated."}
  </p>
  <p class="giDescription"}>
     {g->text text="You can now login to your account with your username and password."}
  </p>
{/if}
</div>
