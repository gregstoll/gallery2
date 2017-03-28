{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2>
    {g->text text="Captcha Settings"}
  </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock">
  <h2 class="giSuccess">
    {g->text text="Settings saved successfully"}
  </h2>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="High Security - Always On"} </h3>
  <p class="giDescription">
    {g->text text="Always require the Captcha value to be entered before proceeding."}
  </p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Medium/Low Security - Failed Attempts"} </h3>
  <p class="giDescription">
    {g->text text="Users are not required to pass the Captcha test unless they have failed validation or user input at least this many times.  After that, they have to enter the Captcha value to log in, or perform certain other secured actions."}
  </p>

  {g->text text="Failed attempts:"}
  <select name="{g->formVar var="form[failedAttemptThreshold]"}">
    {html_options values=$CaptchaSiteAdmin.failedAttemptThresholdList selected=$form.failedAttemptThreshold output=$CaptchaSiteAdmin.failedAttemptThresholdList}
  </select>

  <p class="giDescription">
    {g->text text="Medium security counts failures by a key value such as the username used in a login attempt.  This means after one username has too many failures then anyone, even the real account owner, must perform captcha validation for their next login.  Low security counts failures in the session.  This provides some protection against casual password guessers, but no protection against automated attacks."}
  </p>
</div>

{if !empty($CaptchaSiteAdmin.options)}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Configure validation policy for modules using captcha:"}
  </p>
  <table class="gbDataTable">
    <tr><td></td><th>
      {g->text text="High"}
    </th><th>
      {g->text text="Medium"}
    </th><th>
      {g->text text="Low"}
    </th><th>
      {g->text text="Off"}
    </th></tr>
    {foreach from=$CaptchaSiteAdmin.options key=optionId item=option}
    <tr><td>
      {$option.title}
    </td><td>
      {if in_array('HIGH', $option.choices)}
	<input type="radio" name="{g->formVar var="form[level][`$optionId`]"}" value="HIGH"
	 {if $option.value=='HIGH'} checked="checked"{/if}/>
      {/if}
    </td><td>
      {if in_array('MEDIUM', $option.choices)}
	<input type="radio" name="{g->formVar var="form[level][`$optionId`]"}" value="MEDIUM"
	 {if $option.value=='MEDIUM'} checked="checked"{/if}/>
      {/if}
    </td><td>
      {if in_array('LOW', $option.choices)}
	<input type="radio" name="{g->formVar var="form[level][`$optionId`]"}" value="LOW"
	 {if $option.value=='LOW'} checked="checked"{/if}/>
      {/if}
    </td><td>
      {if in_array('OFF', $option.choices)}
	<input type="radio" name="{g->formVar var="form[level][`$optionId`]"}" value="OFF"
	 {if $option.value=='OFF'} checked="checked"{/if}/>
      {/if}
    </td></tr>
    {/foreach}
  </table>
</div>
{/if}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
