{*
 * $Revision: 17456 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Download %s" arg1=$AdminRepositoryDownload.pluginName} </h2>
</div>

{if isset($form.error)}
<div class="gbBlock">
  <h2 class="giError">
    {if isset($form.error.nothingSelected)}
      {g->text text="No packages have been selected."}
    {/if}
  </h2>
</div>
{/if}

<script type="text/javascript">
// <![CDATA[
  var allSources = [];
{foreach from=$AdminRepositoryDownload.upgradeData item=item}
  allSources.push('{$item.repository}');
{/foreach}
// ]]>
</script>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Download a package in order to use this plugin.  You can upgrade by choosing a newer version of the package to download."}
  </p>
  <h2> {g->text text="Base Packages"} </h2>
  {foreach from=$AdminRepositoryDownload.upgradeData item=item}
  <p>
    {if $item.base.relation == "older"}
      <input type="radio" onchange="showLanguagePacks('{$item.repository}')" name="{g->formVar var="form[base]"}" value="{$item.repository}:{$item.base.newBuild}"/>
      {g->text text="%s: version %s (build %s)" arg1="<b>`$item.repositoryName`</b>" arg2=$item.base.newVersion arg3=$item.base.newBuild}
    {elseif $item.base.relation == "newer"}
      <input type="radio" value="false" disabled="disabled" />
      {g->text text="%s: version %s (build %s) %sdowngrading is not supported!%s" arg1="<b>`$item.repositoryName`</b>" arg2=$item.base.newVersion arg3=$item.base.newBuild arg4="<b>" arg5="</b>"}
    {else}
      <input type="radio" onchange="showLanguagePacks('{$item.repository}')" name="{g->formVar var="form[base]"}" value="{$item.repository}:{$item.base.newBuild}" checked="checked"/>
      {g->text text="%sCurrently Installed%s: version %s (build %s)" arg1="<b>" arg2="</b>" arg3=$item.base.newVersion arg4=$item.base.newBuild}
      {assign var="currentlyInstalled" value=$item.repository}
    {/if}
  </p>
  <div style="position: relative; left: 25px; ">
    <div class="languagePacks" id="{$item.repository}_languagePacks"
	 style="{if empty($currentlyInstalled) || $item.repository != $currentlyInstalled}display: none{/if}">
      <p> {g->text text="The following language packages are available for this plugin"} </p>
      <p id="{$item.repository}_languages">
        {if !empty($item.languages)}
        {foreach from=$item.languages key=code item=pack}
	{counter assign="langId"}
	{capture assign="label"}
	{assign var="repository" value=$item.repository}
	{if $pack.relation == "older" && $pack.currentBuild}
	  {g->text text="%s version %s (upgrading from %s)"
	      arg1="<b>`$pack.name`</b>" arg2=$pack.newBuild arg3=$pack.currentBuild}
	{elseif $pack.relation == "older"}
	  {g->text text="%s version %s" arg1="<b>`$pack.name`</b>" arg2=$pack.newBuild}
	{elseif $pack.relation == "newer"}
	  {g->text text="%s version %s (%snewer version %s is installed%s)"
	      arg1="<b>`$pack.name`</b>" arg2=$pack.newBuild arg3="<b>" arg4=$pack.currentBuild arg5="</b>"}
	{else}
	  {g->text text="%s version %s (currently installed)" arg1="<b>`$pack.name`</b>" arg2=$pack.newBuild}
	{/if}
	{/capture}
	{$label}<br/>
        {/foreach}
        {else} {* !empty($item.languages) *}
          <i>{g->text text="No compatible language packages available"}</i>
        {/if}
      </p>
    </div>
  </div>
  {/foreach}
</div>

<div class="gbBlock gcBackground1">
  <input class="inputTypeSubmit" type="submit" name="{g->formVar var="form[action][download]"}" value="{g->text text="Update"}"/>
  <input class="inputTypeSubmit" type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  <input type="hidden" name="{g->formVar var="form[pluginType]"}" value="{$AdminRepositoryDownload.pluginType}" />
  <input type="hidden" name="{g->formVar var="form[pluginId]"}" value="{$AdminRepositoryDownload.pluginId}" />
</div>
