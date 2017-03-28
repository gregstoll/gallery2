{*
 * $Revision: 17075 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<form action="{g->url}" method="post" enctype="{$ItemAdmin.enctype}" id="itemAdminForm">
  <div>
    {g->hiddenFormVars}
    {if !empty($controller)}
    <input type="hidden" name="{g->formVar var="controller"}" value="{$controller}"/>
    {/if}
    {if !empty($form.formName)}
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    {/if}
    <input type="hidden" name="{g->formVar var="itemId"}" value="{$ItemAdmin.item.id}"/>
  </div>

  <table width="100%" cellspacing="0" cellpadding="0">
    <tr valign="top">
    <td id="gsSidebarCol"><div id="gsSidebar" class="gcBorder1">
      {if $ItemAdmin.item.parentId or !empty($ItemAdmin.thumbnail)}
      <div class="gbBlock">
	{if empty($ItemAdmin.thumbnail)}
	  {g->text text="No Thumbnail"}
	{else}
	  {g->image item=$ItemAdmin.item image=$ItemAdmin.thumbnail maxSize=130}
	{/if}
	<h3> {$ItemAdmin.item.title|markup} </h3>
      </div>
      {/if}

      <div class="gbBlock">
	<h2> {g->text text="Options"} </h2>
	<ul>
	  {foreach from=$ItemAdmin.subViewChoices key=choiceName item=choiceParams}
	    <li class="{g->linkId urlParams=$choiceParams}">
	    {if isset($choiceParams.active)}
	      {$choiceName}
	    {else}
	      {assign var=script value=$choiceParams.script|default:""}
	      {if isset($choiceParams.script)}
		{assign var=choiceParams
			value=$ItemAdmin.unsetCallback|@call_user_func:$choiceParams:"script"}
	      {/if}
	      <a href="{g->url params=$choiceParams}"{if !empty($script)} onclick="{$script}"{/if}> {$choiceName} </a>
	    {/if}
	    </li>
	  {/foreach}
	</ul>
      </div>
    </div></td>

    <td>
      <div id="gsContent" class="gcBorder1">
	{include file="gallery:`$ItemAdmin.viewBodyFile`" l10Domain=$ItemAdmin.viewL10Domain}
      </div>
    </td>
  </tr></table>
</form>
