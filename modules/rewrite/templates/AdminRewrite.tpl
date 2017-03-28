{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="URL Rewrite Administration"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Successfully saved settings"}
  {/if}
</h2></div>
{/if}

{if !empty($form.error)}
<div class="gbBlock">
  <h2 class="giError"> {g->text text="An error occured while trying to save your settings"} </h2>

  <div class="giError">
  {if isset($AdminRewrite.errors)}
    {foreach from=$AdminRewrite.errors item=errstr}
      {$errstr}<br/>
    {/foreach}
  {/if}

  {if isset($form.error.dupe)}
    {g->text text="Duplicate URL patterns."}
  {/if}

  {if isset($form.error.empty)}
    {g->text text="Empty URL pattern."}
  {/if}
  </div>
</div>
{/if}

<div class="gbTabBar">
  {if ($AdminRewrite.mode == 'rules')}
    <span class="giSelected o"><span>
      {g->text text="Rules"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite"
       arg3="mode=rules"}">{g->text text="Rules"}</a>
    </span></span>
  {/if}

  {if ($AdminRewrite.mode == 'setup')}
    <span class="giSelected o"><span>
      {g->text text="Setup"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite"
       arg3="mode=setup"}">{g->text text="Setup"}</a>
    </span></span>
  {/if}

  {if ($AdminRewrite.mode == 'test')}
    <span class="giSelected o"><span>
      {g->text text="Test"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite"
       arg3="mode=test"}">{g->text text="Test"}</a>
    </span></span>
  {/if}
</div>

{* BEGIN Rules Tab *}
{if $AdminRewrite.mode == 'rules'}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Short URLs are compiled out of predefined keywords. Modules may provide additional keywords. Keywords are escaped with % (eg: %itemId%)."}
  </p>

  {if $AdminRewrite.parserId == 'pathinfo'}
  <p class="giDescription giWarning">
    {g->text text="It is recomended that you don't activate the 'Download Item' URL since it will slow down Gallery."}
  </p>
  {elseif $AdminRewrite.parserId == 'isapirewrite'}
  <p class="giDescription giWarning">
    {g->text text="A pattern may not begin with a keyword."}
  </p>
  {/if}

  <table class="gbDataTable">
  {assign var="group" value=""}
  {foreach from=$form.rules item=rules key=moduleId}
  {if !empty($group)}
    <tr><td> &nbsp; </td></tr>
  {/if}
  {assign var="group" value=$moduleId}
  <tr><th colspan="6"><h2>{$AdminRewrite.modules.$moduleId}</h2></th></tr>
  <tr>
    <th colspan="2" style="text-align: center;"> {g->text text="Active"} </th>
    <th> {g->text text="Help"} </th>
    <th> {g->text text="View"} </th>
    <th> {g->text text="URL Pattern"} </th>
    <th> {g->text text="Additional Keywords"} </th>
  </tr>

  {foreach from=$rules item=rule key=ruleId}
  {cycle values="gbEven,gbOdd" assign="rowClass"}
  <tr class="{$rowClass}">
    <td>
      {assign var="match" value=$AdminRewrite.info.$moduleId.$ruleId.match}
      {if isset($form.error.conditions.dupe.$moduleId.$ruleId)
	  || isset($form.error.pattern.dupe.$moduleId.$ruleId)
	  || isset($form.error.pattern.empty.$moduleId.$ruleId)
	  || isset($form.error.1.$moduleId.$ruleId)
	  || isset($form.error.3.$match)
	  || isset($form.error.4.$moduleId.$ruleId)}
	<div class="icon-plugin-incompatible" style="height:16px"
	     title="{g->text text="Status: Error"}"></div>
      {elseif isset($rule.active)}
	<div class="icon-plugin-active" style="height:16px"
	     title="{g->text text="Status: Active"}"></div>
      {else}
	<div class="icon-plugin-uninstall" style="height:16px"
	     title="{g->text text="Status: Not Active"}"></div>
      {/if}
    </td>
    <td>
      <input type="checkbox" name="{g->formVar var="form[rules][$moduleId][$ruleId][active]"}" 
	{if isset($rule.active)} checked="checked"{/if}/>
    </td>
    <td style="text-align: center;">
      <span id="rules-{$moduleId}-{$ruleId}-toggle"
	    class="giBlockToggle gcBackground1 gcBorder2"
	    style="border-width: 1px;"
	    onclick="BlockToggle('rules-{$moduleId}-{$ruleId}-help', 'rules-{$moduleId}-{$ruleId}-toggle', 'table-row')">+</span>
    </td>
    <td>
      {$AdminRewrite.info.$moduleId.$ruleId.comment}
    </td>
    <td>
      {if !isset($rule.pattern)}
	{g->text text="No URL Pattern"}
      {else}
      {if isset($AdminRewrite.info.$moduleId.$ruleId.locked)}
        <input type="hidden" name="{g->formVar var="form[rules][$moduleId][$ruleId][pattern]"}" value="{$rule.pattern}"/>
        <input type="text" size="40" name="dummy" value="{$rule.pattern}" disabled="disabled"/>
      {else}
        <input type="text" size="40" name="{g->formVar var="form[rules][$moduleId][$ruleId][pattern]"}" value="{$rule.pattern}"/>
      {/if}
      {/if}
    </td>
    <td>
      {foreach from=$AdminRewrite.info.$moduleId.$ruleId.keywords key=keyword item=tmp}
	%{$keyword}%
      {/foreach}
    </td>
  </tr>
  <tr class="{$rowClass}" style="display: none;" id="rules-{$moduleId}-{$ruleId}-help">
    <td colspan="2">
      &nbsp;
    </td>
    <td colspan="4">
      <b>{g->text text="Help"}</b><br/>
      {if isset($AdminRewrite.info.$moduleId.$ruleId.help)}
	{$AdminRewrite.info.$moduleId.$ruleId.help}
      {else}
	<i>{g->text text="No help available"}</i>
      {/if}<br/><br/>

      <b>{g->text text="Keywords"}</b><br/>
      {assign var="hasKeywordHelp" value=false}
      {foreach from=$AdminRewrite.info.$moduleId.$ruleId.keywords key=keyword item=info}
	{if isset($info.help)}
	  %{$keyword}% : {$info.help}<br/>
	  {assign var="hasKeywordHelp" value=true}
	{/if}
      {/foreach}
      {if !$hasKeywordHelp}
	<i>{g->text text="No keyword help available"}</i>
      {/if}
    </td>
  </tr>
  {/foreach} {* END Rule *}
  {/foreach} {* END Module *}
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][rules]"}" value="{g->text text="Save"}"/>
</div>
{/if}
{* END Rules Tab *}

{* BEGIN Setup Tab *}
{if $AdminRewrite.mode == 'setup'}
{if $AdminRewrite.parserType == 'preGallery'}
<div class="gbBlock">
  <h3> {g->text text="Approved referers"} </h3>

  <p class="giDescription">
    {g->text text="Some rules only apply if the referer (the site that linked to the item) is something other than Gallery itself. Hosts in the list below will be treated as friendly referers."}<br/>
  </p>

  <p class="giDesciption giWarning">
    {g->text text="Warning: If you don't allow empty referer users won't be able to download nor play movies."}
  </p>

  <p class="giDescription">
    <input type="checkbox" name="{g->formVar var="form[allowEmptyReferer]"}" 
	{if isset($form.allowEmptyReferer)} checked="checked"{/if}/>
    {g->text text="Allow empty referer?"}
  </p>

  <table class="gbDataTable"><tr>
    <td><input type="text" name="{g->formVar var="form[dummy]"}" size="60" value="{$AdminRewrite.serverName}" disabled="disabled"/></td>
  {counter start=0 assign="i"}
  {foreach from=$form.accessList item=host}
  </tr><tr>
    <td><input type="text" name="{g->formVar var="form[accessList][$i]"}" size="60" value="{$host}"/></td>
    {counter print=false}
  {/foreach}
  </tr><tr>
    <td><input type="text" name="{g->formVar var="form[accessList][$i]"}" size="60"/></td>
  </tr><tr>
    <td><input type="text" name="{g->formVar var="form[accessList][`$i+1`]"}" size="60"/></td>
  </tr><tr>
    <td><input type="text" name="{g->formVar var="form[accessList][`$i+2`]"}" size="60"/></td>
  </tr></table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][accessList]"}" value="{g->text text="Save"}"/>
</div>
{else}
<div class="gbBlock">
  <h3> {g->text text="Approved referers"} </h3>

  <p class="giDescription">
    {g->text text="The parser you have selected does not support a referer check."}<br/>
  </p>
</div>
{/if}

{if isset($AdminParser.template)}
  {include file="gallery:`$AdminParser.template`"}

  {if isset($AdminParser.action)}
    <div class="gbBlock gcBackground1">
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][adminParser]"}" value="{g->text text="Save"}"/>
    </div>
  {/if}
{/if}
{/if}
{* END Rules Tab *}

{* BEGIN Test Tab *}
{if $AdminRewrite.mode == 'test'}
<div class="gbBlock">
  <h3> {g->text text="Test the Rewrite Parser Configuration"} </h3>
</div>
{if isset($TestResults.template)}
  {include file="gallery:`$TestResults.template`"}

  {if isset($TestResults.action)}
    <div class="gbBlock gcBackground1">
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][testParser]"}" value="{g->text text="Save"}"/>
    </div>
  {/if}
  {if isset($TestResults.refresh)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][refresh]"}" value="{g->text text="Test again"}"/>
  {/if}
{else}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="The selected URL Rewrite Parser does not provide any tests."}<br/>
  </p>
</div>
{/if}
{/if}
{* END Test Tab*}
