{*
 * $Revision: 16876 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3>
    {g->text text="Type the letters appearing in the picture."}
  </h3>
  <div>
    <img src="{g->url arg1="view=captcha.CaptchaImage"}" width="100" height="100" alt=""/>
  </div>
  <input type="text" size="12"
   name="{g->formVar var="form[CaptchaValidationPlugin][word]"}" value=""/>

  {if isset($form.error.CaptchaValidationPlugin)}
  <div class="giError">
    {if isset($form.error.CaptchaValidationPlugin.missing)}
      {g->text text="You must enter the letters appearing in the picture."}
    {/if}
    {if isset($form.error.CaptchaValidationPlugin.invalid)}
      {g->text text="Incorrect letters."}
    {/if}
  </div>
  {/if}
</div>
