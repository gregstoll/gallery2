{*
 * $Revision: 17477 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Comments"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if !empty($status.saved)}
    {g->text text="Settings saved successfully"}
  {else if !empty($status.checked)}
    {g->text one="Checked %d comment." many="Checked %d comments." count=$status.checked.total arg1=$status.checked.total}
    {g->text one="Found %d spam comment." many="Found %d spam comments." count=$status.checked.spamCount arg1=$status.checked.spamCount}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="General Settings"} </h3>
  <table class="gbDataTable"><tr>
    <td>
      <label for="cbLatest">
	{g->text text="Show link for Latest Comments:"}
      </label>
    </td><td>
      <input type="checkbox" id="cbLatest" {if $form.latest}checked="checked" {/if}
       name="{g->formVar var="form[latest]"}"/>
    </td>
  </tr><tr>
    <td>
      {g->text text="Number of comments on Latest Comments page:"}
    </td><td>
      <input type="text" size="5" name="{g->formVar var="form[show]"}" value="{$form.show}"/>

      {if isset($form.error.show)}
      <div class="giError">
	{g->text text="Invalid value"}
      </div>
      {/if}
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Moderation Settings"} </h3>
  <p class="giDescription">
    {g->text text="Require administrator approval prior to publishing comments."}
  </p>
  <table class="gbDataTable"><tr>
    <td>
      <label for="cbModerate">
	{g->text text="Moderate all comments:"}
      </label>
    </td><td>
      <input type="checkbox" id="cbModerate" {if $form.moderate} checked="checked"{/if}
       name="{g->formVar var="form[moderate]"}"/>
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Anti-Spam Settings"} </h3>
  <p class="giDescription">
    {g->text text="You can reduce the amount of spam comments that you receive by using a service called %sAkismet%s.  This service is free for personal use.  In order to use it, you will need to %sregister for an API key%s and enter that key in the box below." arg1="<a href=\"http://akismet.com\">" arg2="</a>" arg3="<a href=\"http://akismet.com/personal\">" arg4="</a>"}
  </p>
  <table class="gbDataTable"><tr>
    <td colspan="2">
      {if $CommentSiteAdmin.akismetActive}
      <div class="giSuccess">
        {g->text text="Akismet is active."}
      </div>
      {else}
      <div class="giWarning">
        {g->text text="Akismet is not active."}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>
      {g->text text="API key:"}
    </td><td>
      <input type="text" size="15" maxlength="12" name="{g->formVar var="form[apiKey]"}" value="{$form.apiKey}"/>
      {if isset($form.error.apiKey.invalid)}
      <div class="giError">
        {g->text text="Invalid API key."}
      </div>
      {/if}
    </td>
  </tr></table>
  {if $CommentSiteAdmin.akismetActive}
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][checkAllWithAkismet]"}" value="{g->text text="Check all comments now"}"/>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
