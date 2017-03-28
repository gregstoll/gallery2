{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
	<h2>{g->text text="Send eCard"}</h2>
</div>

{if isset($form.action.preview)}
<div class="gbBlock">
	<h3>{g->text text="eCard Preview"}</h3>

	<table class="gbDataTable">
	<tr>
		<td class="gbEven"><strong>{g->text text="From"}:</strong></td>
		<td class="gbOdd">{$SendEcard.emailFrom}</td>
	</tr>
	<tr>
		<td class="gbEven"><strong>{g->text text="To"}:</strong></td>
		<td class="gbOdd">{$form.toName} &lt;{$form.to}&gt;</td>
	</tr>
	<tr>
		<td class="gbEven"><strong>{g->text text="Subject"}:</strong></td>
		<td class="gbOdd">{$SendEcard.subject}</td>
	</tr>
	<tr>
		<td colspan="2">
			<p>{$SendEcard.header}</p>

			<p>{$form.text}</p>

			{g->image item=$SendEcard.item image=$SendEcard.resize}

			<hr />
			<small>{$SendEcard.footer}</small>
		</td>
	</tr>
	</table>
</div>
{/if}

<form action="{g->url}" method="post" enctype="application/x-www-form-urlencoded" id="sendEcardForm">
<div>
	{g->hiddenFormVars}
	<input type="hidden" name="{g->formVar var="controller"}" value="ecard.SendEcard" />
	<input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}" />
	<input type="hidden" name="{g->formVar var="itemId"}" value="{$SendEcard.itemId}"/>
</div>

<div class="gbBlock">
	<h4>{g->text text="Your name"}</h4>
	<input type="text" id="fromName" size="60" class="gcBackground1"
		name="{g->formVar var="form[fromName]"}" value="{$form.fromName}"
		onfocus="this.className=''" onblur="this.className='gcBackground1'" />

	<h4>{g->text text="Your e-mail address"}
		<span class="giSubtitle"> {g->text text="(required)"}</span>
	</h4>
	<input type="text" id="from" size="60" class="gcBackground1"
		name="{g->formVar var="form[from]"}" value="{$form.from}"
		onfocus="this.className=''" onblur="this.className='gcBackground1'" />
	{if isset($form.error.from.missing)}
	<div class="giError">
		{g->text text="You must enter an e-mail address!"}
	</div>
	{/if}
	{if isset($form.error.from.invalid)}
	<div class="giError">
		{g->text text="You must enter a valid e-mail address!"}
	</div>
	{/if}

	<h4>{g->text text="Recipient's name"}</h4>
	<input type="text" id="toName" size="60" class="gcBackground1"
		name="{g->formVar var="form[toName]"}" value="{$form.toName}"
		onfocus="this.className=''" onblur="this.className='gcBackground1'" />

	<h4>{g->text text="Recipient's e-mail address"}
		<span class="giSubtitle"> {g->text text="(required)"}</span>
	</h4>
	<input type="text" id="to" size="60" class="gcBackground1"
		name="{g->formVar var="form[to]"}" value="{$form.to}"
		onfocus="this.className=''" onblur="this.className='gcBackground1'" />
	{if isset($form.error.to.missing)}
	<div class="giError">
		{g->text text="You must enter an e-mail address!"}
	</div>
	{/if}
	{if isset($form.error.to.invalid)}
	<div class="giError">
		{g->text text="You must enter a valid e-mail address!"}
	</div>
	{/if}

	<h4>{g->text text="Text"}
		<span class="giSubtitle"> {g->text text="(required)"}</span>
	</h4>
	<textarea rows="15" cols="60" id="text" class="gcBackground1"
		name="{g->formVar var="form[text]"}"
		onfocus="this.className=''" onblur="this.className='gcBackground1'">{$form.text}</textarea>
	{if isset($form.error.text.missing)}
	<div class="giError">
		{g->text text="You must enter text!"}
	</div>
	{/if}
</div>

  {* Include validation plugins *}
  {foreach from=$SendEcard.plugins item=plugin}
     {include file="gallery:`$plugin.file`" l10Domain=$plugin.l10Domain}
  {/foreach}


<div class="gbBlock gcBackground1">
	<input type="submit" class="inputTypeSubmit"
	  name="{g->formVar var="form[action][preview]"}" value="{g->text text="Preview"}"/>
	<input type="submit" class="inputTypeSubmit"
	  name="{g->formVar var="form[action][send]"}" value="{g->text text="Send"}"/>
	<input type="submit" class="inputTypeSubmit"
	  name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>

</form>
