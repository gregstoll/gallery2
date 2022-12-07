{*
 * $Revision: 1907 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
    <h2>{g->text text="Share Settings"}</h2>
</div>

{if isset($status.saved)}
    <div class="gbBlock">
	<h2 class="giSuccess">{g->text text="Settings have been saved"}</h2>
    </div>
{elseif !empty($form.error)}
    <div class="gbBlock">
	<h2 class="giError">{g->text text="An error occured"}</h2>
    </div>
{/if}
<div class="gbBlock">
    <h3> {g->text text="General settings"} </h3>
    <p class="giDescription">
	{g->text text="These control the settings which apply globally to the Share link blocks on your Gallery"}
    </p>
    <div class="form-group">

    <input type="checkbox" {if $form.onlyWhenLoggedIn}checked="checked" {/if}
           name="{g->formVar var="form[onlyWhenLoggedIn]"}" id="Share_onlyWhenLoggedIn"/>
    <label for="Share_onlyWhenLoggedIn">
	{g->text text="Show Share links only when logged in (hide for guests)"}
    </label>
    </div>

    <div class="form-group">
    <label for="Share_addThisAccountId">
	{g->text text="addThis.com account ID"}
	<input type="text" class="form-control" size="40"
	    {assign var="addThisAccountId" value=$form.addThisAccountId}
	    {if $form.addThisAccountId}value="{$form.addThisAccountId}" {/if}
	       name="{g->formVar var="form[addThisAccountId]"}"
	       id="Share_addThisAccountId"/>
    </label>
    </div>
</div>

<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
    <input type="submit" name="{g->formVar var="form[action][reset]"}" value="{g->text text="Cancel"}"/>
</div>
