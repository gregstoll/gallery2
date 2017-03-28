{*
 * $Revision: 17440 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Repository"} </h2>
</div>

{if !empty($status.error)}
<div class="gbBlock">
  <h2 class="giError">
    {if !empty($status.error.download)}
    {foreach from=$status.error.download item=error}
    {$error}<br/>
    {/foreach}
    {g->text text="Please make sure that your internet connection is set up properly or try again later."}<br/>
    {/if}
    {if !empty($status.error.scanPlugin)}
    {foreach from=$status.error.scanPlugin item=pluginId}
    {g->text text="Failed to scan status from plugin: %s." arg1=$pluginId}<br/>
    {/foreach}
    {/if}
    {if !empty($status.error.repositoryInitErrorCount)}
    {g->text text="Your local copy of the repository was broken and has been fixed.  Please download the plugin list again."}
    {/if}
  </h2>
  {if !empty($status.error.failedToDownload)}
    {foreach name=pluginType from=$status.error.failedToDownload key=pluginType item=plugins}
      {foreach name=plugin from=$plugins key=pluginName item=failedFiles}
	<h2 class="giError"> {g->text text="Failed to download the following packages for the %s plugin:" arg1=$pluginName}</h2>
	<ul>
	  {foreach from=$failedFiles item=file}
	    <li class="giError"> {$file} </li>
	  {/foreach}
	</ul>
	{if !$smarty.foreach.pluginType.last}<br/>{/if}
      {/foreach}
    {/foreach}
  {/if}

  {if !empty($status.error.failedToInstall)}
    {foreach name=pluginType from=$status.error.failedToInstall key=pluginType item=plugins}
      {foreach name=plugin from=$plugins key=pluginName item=failedFiles}
	<h2 class="giError"> {g->text text="Failed to install the %s plugin because the following files/directories could not be modified:" arg1=$pluginName} </h2>
	<ul>
	  {foreach from=$failedFiles item=file}
	    <li class="giError"> {$file} </li>
	  {/foreach}
	  </ul>
	{if !$smarty.foreach.pluginType.last}<br/>{/if}
      {/foreach}
    {/foreach}
  {/if}

  {if !empty($status.error.cantUpgradeInUse)}
    {capture assign="pluginsLink"}<a href="{g->url arg1="view=core.SiteAdmin"
     arg2="subView=core.AdminPlugins"}">{/capture}
    <h2 class="giError"> {g->text text="Some plugins could not be automatically upgraded because they are in use.  Please return to the %sPlugins%s page and click the upgrade link for each of the following plugins:" arg1=$pluginsLink arg2="</a>"} </h2>
    <ul>
      {foreach from=$status.error.cantUpgradeInUse item=pluginName}
      <li class="giError"> {$pluginName} </li>
      {/foreach}
    </ul>
  {/if}
</div>
{/if}

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.indexUpdated)}
    {g->text text="The repository index has been successfully updated."}
  {elseif isset($status.noUpgradeAvailable)}
    {g->text text="All plugins are already up-to-date."}
  {elseif !empty($status.updated)}
    {foreach from=$status.updated item=item}
    <p>
      {g->text text="%s plugin updated." arg1=$item}
    </p>
    {/foreach}
  {/if}
</h2></div>
{/if}

<div class="gbTabBar">
  <span class="o"><span>
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}">
      {g->text text="Plugins"}
    </a>
  </span></span>

  <span class="giSelected o"><span>
    {g->text text="Get More Plugins"}
  </span></span>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {capture name="noPersonalInfoTransmitted"}<b>{g->text text="No personal information about you or your Gallery installation is sent to the Gallery server at any time."}</b>{/capture}
    {g->text text="Download and install new plugins quickly and easily.  If you decide you don't like a plugin, you can delete it from your Gallery.  There are multiple different plugin repositories, you can choose which ones you want to use (it's ok to use all of them at once).  Once you've chosen, you must periodically download the latest plugin list to find out about available updates. %s On slower connections the process might take a minute or two." arg1=$smarty.capture.noPersonalInfoTransmitted}
  </p>

  <div id="AdminRepository_Configure" style="height: 0px; opacity: 0; overflow: hidden;">
    <input id="repositories_released" type="checkbox" name="{g->formVar var="form[repositories][released]"}" {if !empty($form.repositories.released)}checked="checked"{/if}  value="released">
    <label for="repositories_released">
    {g->text text="Officially Released Gallery Project plugins (%srecommended%s)" arg1="<b>" arg2="</b>"}
    {if !empty($AdminRepository.indexMetaData.released.timestamp)}
    {capture assign="updateDate"}{g->date style="datetime" timestamp=$AdminRepository.indexMetaData.released.timestamp}{/capture}
    <i>{g->text text="last updated on %s" arg1=$updateDate}</i>
    {/if}
    </label>
    </input>
    <br/>

    <input id="repositories_experimental" type="checkbox" name="{g->formVar var="form[repositories][experimental]"}" {if !empty($form.repositories.experimental)}checked="checked"{/if}  value="experimental">
    <label for="repositories_experimental">
    {g->text text="Experimental Gallery Project plugins (%scool new features, might be buggy%s)" arg1="<b>" arg2="</b>"}
    {if !empty($AdminRepository.indexMetaData.experimental.timestamp)}
    {capture assign="updateDate"}{g->date style="datetime" timestamp=$AdminRepository.indexMetaData.experimental.timestamp}{/capture}
    <i>{g->text text="last updated on %s" arg1=$updateDate}</i>
    {/if}
    </label>
    </input>
    <br/>

    <input id="repositories_community" type="checkbox" name="{g->formVar var="form[repositories][community]"}" {if !empty($form.repositories.community)}checked="checked"{/if}  value="community">
    <label for="repositories_community">
    {g->text text="Community plugins (%suse at your own risk!%s)" arg1="<b>" arg2="</b>"}
    {if !empty($AdminRepository.indexMetaData.community.timestamp)}
    {capture assign="updateDate"}{g->date style="datetime" timestamp=$AdminRepository.indexMetaData.community.timestamp}{/capture}
    <i>{g->text text="last updated on %s" arg1=$updateDate}</i>
    {/if}
    </label>
    </input>
    <br/>

    <p>
      <input id="AdminRepository_saveRepositoryList" type="button" class="inputTypeSubmit" onclick="if (validateRepositoryList()) {ldelim} saveRepositoryList();  closeRepositoryList() {rdelim}" value="{g->text text="Save"}"/>
      <input id="AdminRepository_closeRepositoryList" type="reset" class="inputTypeSubmit" onclick="closeRepositoryList()" value="{g->text text="Cancel"}"/>
    </p>
  </div>
  <script type="text/javascript">
    // <![CDATA[
    var postUrl = '{g->url}';
    var commandArg = '{g->formVar var="command"}';
    var viewArg = '{g->formVar var="view"}';
    var authTokenArg = '{g->formVar var="authToken"}={$AdminRepository.authToken}';
    var repositoryArgs = [
      '{g->formVar var="form[repositories][released]"}',
      '{g->formVar var="form[repositories][experimental]"}',
      '{g->formVar var="form[repositories][community]"}' ];

    var repositoryMessageTitle = '{g->text text="Repository Error" forJavascript=true}';
    var repositoryMessageBody = '{g->text text="You must select at least one repository." forJavascript=true}';
    var repositoryMessageOkButton = '{g->text text="Ok" forJavascript=true}';
    var formElements = document.getElementById("siteAdminForm").elements;
    // ]]>
  </script>
</div>

{if !$AdminRepository.writeable.modules || !$AdminRepository.writeable.themes}
<div class="gbBlock">
  <h3>{g->text text="Configure your Gallery"}</h3>
  {if $AdminRepository.OS == 'unix'}
    <p class="giDescription">
      {g->text text="Before you can proceed, you have to change some permissions so that Gallery can install plugins for you.  It's easy.  Just execute the following in a shell or via your ftp client:"}
    </p>
    <p class="gcBackground1" style="border-width: 1px; border-style: dotted; padding: 4px">
      <b>
        cd {$AdminRepository.basePath}<br/>
        {if !$AdminRepository.writeable.modules}chmod -R 777 modules<br/>{/if}
        {if !$AdminRepository.writeable.themes}chmod -R 777 themes<br/>{/if}
      </b>
    </p>
  {else}
    <p class="giDescription">
      {g->text text="Before you can proceed, please insure the following directories and sub-directories are writable, so that Gallery can install plugins for you:"}
    </p>
    <p class="gcBackground1" style="border-width: 1px; border-style: dotted; padding: 4px">
      <b>
        {if !$AdminRepository.writeable.modules}{$AdminRepository.basePath}/modules<br/>{/if}
        {if !$AdminRepository.writeable.themes}{$AdminRepository.basePath}/themes<br/>{/if}
      </b>
    </p>
  {/if}
  <p class="giDescription">
    {g->text text="If you have trouble changing permissions, ask your system administrator for assistance.  When you've fixed the permissions, click the Continue button to proceed."}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input class="inputTypeSubmit" type="button" onclick="document.location='{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminRepository"}'" value="{g->text text="Continue"}" />
</div>
{else}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][update]"}" value="{if isset($AdminRepository.indexMetaData)}{g->text text="Update Plugin List"}{else}{g->text text="Download Plugin List"}{/if}"/>
  <input id="AdminRepository_showRepositoryList" type="button" class="inputTypeSubmit" onclick="showRepositoryList()" value="{g->text text="Show Repository List"}"/>
</div>

{if isset($AdminRepository.indexMetaData)}
  {if $AdminRepository.isCoreUpgradeAvailable}
<div class="gbBlock">
  <h3>{g->text text="Upgrade Gallery"}</h3>
  <p class="giDescription">
    {g->text text="A new version of Gallery is available, but it cannot be upgraded through this interface. Upgrading it might make some of your current plugins stop working, but others that rely on the features of the new version may become available. Here are the recommended steps for upgrading:"}
  </p>
  <p>
    <ol>
      <li>{g->text text="%sDownload%s the new Gallery package" arg1="<a href=\"http://codex.gallery2.org/Downloads\">" arg2="</a>"}</li>
      <li>{g->text text="Read the %supgrade instructions%s and perform the upgrade" arg1="<a href=\"http://codex.gallery2.org/Gallery2:Upgrading\">" arg2="</a>"}</li>
    </ol>
  </p>
</div>
  {/if}
{/if}

<div class="gbBlock">
  {if isset($AdminRepository.browseData)}
  <p class="giDescription">
    {if $AdminRepository.isCoreUpgradeAvailable}
      {if isset($AdminRepository.showIncompatible)}
        {g->text text="Incompatible plugins are marked with an exclamation icon."}
      {else}
	{capture name="listLink"}<a href="{g->url arg1="view=core.SiteAdmin"
	  arg2="subView=core.AdminRepository"
	  arg3="coreApi=`$AdminRepository.latestCoreApiVersion`" arg4="themeApi=`$AdminRepository.latestThemeApiVersion`"
	  arg5="moduleApi=`$AdminRepository.latestModuleApiVersion`" arg6="showIncompatible=true"}">{/capture}
	{g->text text="A new core module version is available. There may be plugins that are incompatible with the installed core module, which are not shown here. You can view a %scomplete list%s of plugins, including incompatible ones, which are marked with a red icon." arg1=$smarty.capture.listLink arg2="</a>"}
      {/if}
    {/if}
  </p>

  {capture name=legend}
  <div class="AdminPlugins_legend">
    <span class="icon-plugin-upgrade" style="margin-right: 10px; vertical-align: top">
      {g->text text="upgrade available"}
    </span>

    <span class="icon-plugin-locked" style="margin-right: 10px; vertical-align: top">
      {g->text text="locked"}
    </span>

    <span class="icon-plugin-incompatible" style="margin-right: 10px; vertical-align: top">
      {g->text text="incompatible"}
    </span>
  </div>
  {/capture}
  {$smarty.capture.legend}

  <table class="gbDataTable">
    {assign var="group" value=""}
    {foreach from=$AdminRepository.browseData key=pluginId item=plugin}
      {if $group != $plugin.groupLabel}
	{if !empty($group)}
	<tr><td> &nbsp; </td></tr>
	{/if}
	<tr>
	  <th colspan="6"><h2>{$plugin.groupLabel}</h2></th>
	</tr><tr>
	  <th> &nbsp; </th>
          {if $plugin.type == 'themes'}
	  <th> {g->text text="Theme Name"} </th>
          {else}
	  <th> {g->text text="Module Name"} </th>
          {/if}
	  <th> {g->text text="Installed"} </th>
	  <th> {g->text text="Latest"} </th>
	  <th> {g->text text="Description"} </th>
	  <th> {g->text text="Actions"} </th>
	</tr>
      {/if}
      {assign var="group" value=$plugin.groupLabel}

      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>
	  <div style="height: 16px" class="icon-plugin-{if
	   $plugin.locked}locked" title="{g->text text="Locked Plugin"}"
	  {elseif !$plugin.isCompatible || $plugin.isDowngraded}incompatible" title="{g->text text="Incompatible Plugin"}"
	  {elseif $plugin.isUpgradeable}upgrade" title="{g->text text="Upgrade Available"}"
	  {else}download"{/if}/>
	</td>

	<td>
	  {$plugin.name}
	</td>

	<td align="center">
	  {$plugin.localVersion}
	</td>

	<td align="center">
	  {$plugin.repositoryVersion}
	</td>

	<td>
	  {$plugin.description}
	  {if !$plugin.isCompatible}
	    <br/>
	    <span class="giError">
	      {g->text text="Incompatible plugin!"}
	      {if $plugin.api.required.core != $plugin.api.provided.core}
		<br/>
		{g->text text="Core API Required: %s (available: %s)"
			 arg1=$plugin.api.required.core arg2=$plugin.api.provided.core}
	      {/if}
	      {if $plugin.api.required.plugin != $plugin.api.provided.plugin}
		<br/>
		{g->text text="Plugin API Required: %s (available: %s)"
			 arg1=$plugin.api.required.plugin arg2=$plugin.api.provided.plugin}
	      {/if}
	    </span>
	  {/if}
	</td>

	<td>
          {if $plugin.locked || !$plugin.isCompatible || $plugin.isDowngraded}
            &nbsp;
          {else}{strip}
          <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminRepositoryDownload" arg3="pluginType=`$plugin.type`" arg4="pluginId=`$pluginId`"}">
	  {if $plugin.isUpgradeable}
	    {g->text text="upgrade"}
          {elseif !$plugin.localVersion}
	    {g->text text="download"}
          {else}
	    {g->text text="modify"}
          {/if}
	  </a>{/strip}
          {/if}
	</td>
      </tr>
    {/foreach}
  </table>
  {$smarty.capture.legend}
  {/if}

</div>

{if $AdminRepository.upgradesAvailable}
<div class="gbBlock">
  <h3>{g->text text="Upgrade All Plugins"}</h3>
  <p class="giDescription">
    {g->text text="Gallery can automatically upgrade your themes and modules to the latest available versions. No new plugins will be downloaded."}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][upgradeAll]"}" value="{g->text text="Upgrade All"}"/>
</div>
{/if}
{/if} {* modules/themes are writeable *}
