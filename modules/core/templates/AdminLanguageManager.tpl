{*
 * $Revision: 17768 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Language Settings"} </h2>
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
    {if !empty($status.error.failedToDeleteLanguage)}
    {foreach from=$status.error.failedToDeleteLanguage item=language}
    {g->text text="Failed to delete the locale directory for %s." arg1=$language} <br/>
    {/foreach}
    {/if}
  </h2>
  {if !empty($status.error.failedToDownload)}
    {foreach name=pluginType from=$status.error.failedToDownload key=pluginType item=plugins}
      {foreach name=plugin from=$plugins key=pluginName item=failedFiles}
	<h2 class="giError"> {g->text text="Failed to download the following language packages for the %s plugin:" arg1=$pluginName}</h2>
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
  {foreach name=plugin from=$status.error.failedToInstall key=pluginName item=failedFiles}
    {foreach name=pluginType from=$status.error.failedToInstall key=pluginType item=plugins}
      {foreach name=plugin from=$plugins key=pluginName item=failedFiles}
	<h2 class="giError"> {g->text text="Failed to install language packages for %s plugin because the following files/directories could not be modified:" arg1=$pluginName} </h2>
	<ul>
	  {foreach from=$failedFiles item=file}
	    <li class="giError"> {$file} </li>
	  {/foreach}
	  </ul>
	{if !$smarty.foreach.pluginType.last}<br/>{/if}
      {/foreach}
    {/foreach}
  {/foreach}
  {/if}

</div>
{/if}

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.indexUpdated)}
    {g->text text="The repository index has been successfully refreshed."}
  {else}
    {if !empty($status.languageSettingsSaved)}
      {g->text text="The language settings have been saved."}
    {/if}
    {if !empty($status.languagePacksInstalled)}
     {g->text one="%d language pack upgraded or downloaded." many="%d language packs upgraded or downloaded."
	       count=$status.languagePacksInstalled arg1=$status.languagePacksInstalled}
    {/if}
    {if !empty($status.languagePacksDeleted)}
      {g->text one="%d language pack deleted." many="%d language packs deleted."
	       count=$status.languagePacksDeleted arg1=$status.languagePacksDeleted}
    {/if}
    {if !empty($status.allLanguagesCurrent)}
      <br />
      {g->text text="All the language packages for the selected languages are current" }
    {/if}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="General"} </h3>

  <p class="giDescription">
    {g->text text="Select language defaults for Gallery. Individual users can override this setting in their personal preferences or via the language selector block if available. Gallery will try to automatically detect the language preference of each user if the browser preference check is enabled."}
  </p>

  {if !empty($AdminLanguages.canTranslate)}
    <table class="gbDataTable"><tr>
      <td>
	{g->text text="Default language"}
      </td><td>
	<select name="{g->formVar var="form[default][language]"}">
	  {html_options options=$AdminLanguages.languageList selected=$form.default.language}
	</select>
      </td>
    </tr><tr>
      <td>
	{g->text text="Check Browser Preference"}
      </td><td>
	<input type="checkbox" {if $form.language.useBrowserPref}checked="checked" {/if}
	       name="{g->formVar var="form[language][useBrowserPref]"}"/>
      </td>
    </tr></table>
  {else}
    <div class="giWarning">
      {capture name="gettext"}
	<a href="http://php.net/gettext">{g->text text="gettext"}</a>
      {/capture}
      {g->text text="Your webserver does not support localization.  Please instruct your system administrator to reconfigure PHP with the %s option enabled." arg1=$smarty.capture.gettext}
    </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[save]"}" value="{g->text text="Save"}"/>
</div>

{if !$AdminLanguages.writeable.modules || !$AdminLanguages.writeable.themes}
<div class="gbBlock">
  <h3>{g->text text="Configure your Gallery"}</h3>
  {if $AdminLanguages.OS == 'unix'}
    <p class="giDescription">
      {g->text text="Before you can proceed, you have to change some permissions so that Gallery can install plugins for you.  It's easy.  Just execute the following in a shell or via your ftp client:"}
    </p>
    <p class="gcBackground1" style="border-width: 1px; border-style: dotted; padding: 4px">
      <b>
        cd {$AdminLanguages.basePath}<br/>
        {if !$AdminLanguages.writeable.modules}chmod -R 777 modules/*/po<br/>{/if}
        {if !$AdminLanguages.writeable.themes}chmod -R 777 themes/*/po<br/>{/if}
      </b>
    </p>
  {else}
    <p class="giDescription">
      {g->text text="Before you can proceed, please insure the following directories and sub-directories are writable, so that Gallery can install plugins for you:"}
    </p>
    <p class="gcBackground1" style="border-width: 1px; border-style: dotted; padding: 4px">
      <b>
        {if !$AdminLanguages.writeable.modules}{$AdminLanguages.basePath}modules<br/>{/if}
        {if !$AdminLanguages.writeable.themes}{$AdminLanguages.basePath}themes<br/>{/if}
      </b>
    </p>
  {/if}
  <p class="giDescription">
    {g->text text="If you have trouble changing permissions, ask your system administrator for assistance.  When you've fixed the permissions, click the Check Again button to proceed."}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input class="inputTypeSubmit" type="button" onclick="document.location='{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminLanguageManager"}'" value="{g->text text="Check Again"}" />
</div>
{else}
  <script type="text/javascript">
    // <![CDATA[
      {literal}
	function toggleAll(thisAllButton, otherAllButton, checkBoxGroup, languageCount) {
	  var selectAllButtons = YAHOO.util.Dom.getElementsByClassName("selectAll");
	  for( var i in selectAllButtons) {
	    selectAllButtons[i].checked = '';
	  }
	  var currentButton = document.getElementById(thisAllButton);
	  var element = document.getElementById(otherAllButton);
	  currentButton.checked = element.checked = 'checked';
	  for (var current = 1; current <= languageCount; current++) {
	    var element = document.getElementById(checkBoxGroup + current);
	    if (!element.disabled) {
	      element.checked = currentButton.checked;
	    }
	  }
	}
      {/literal}
    // ]]>
  </script>
  {assign var="languageCount" value=$AdminLanguages.languageUpgradeInfo|@count}

  <div class="gbBlock">
    <h3> {g->text text="Add or Remove Languages"} </h3>
  {if !empty($languageCount) }
     <p class="giDescription">
      {g->text text="Please click on the link below to go to 'Download Plugin List' and choose 'Update Plugin List' to get the latest language package information."}<br />
          <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminRepository"}">{g->text text="Administer Repositories"}</a>
     </p>
      <table class="gbDataTable">
	<tr>
	  <th> {g->text text="Language"} </th>
	  <th> {g->text text="Upgrade"} </th>
	  <th> {g->text text="Remove"} </th>
	  <th> {g->text text="Download"} </th>
	  <th> {g->text text="No Change"} </th>
	</tr>

	<tr class="gbEven">
	  <td>
	    {g->text text="Select All"}
	  </td>
	  <td style="text-align: center">
	    <input type="radio" id=selectAllUpgrade1 onclick="toggleAll('selectAllUpgrade1', 'selectAllUpgrade2', 'upgrade', {$languageCount})" 
		{if empty($AdminLanguages.enableSelectAll.upgrade)}disabled="disabled" {/if}
	        class="selectAll"/>
	  </td>
	  <td style="text-align: center">
	    <input type="radio" id=selectAllRemove1 onclick="toggleAll('selectAllRemove1', 'selectAllRemove2', 'remove', {$languageCount})" 
		{if empty($AdminLanguages.enableSelectAll.remove)}disabled="disabled" {/if}
		class="selectAll"
	    />
	  </td>
	  <td style="text-align: center">
	    <input type="radio" id=selectAllDownload1 onclick="toggleAll('selectAllDownload1', 'selectAllDownload2', 'download', {$languageCount})" 
		{if empty($AdminLanguages.enableSelectAll.download)}disabled="disabled" {/if}
		class="selectAll"
	    />
	  </td>
	  <td style="text-align: center">
	    &nbsp;
	  </td>
	</tr>
	{foreach from=$AdminLanguages.languageUpgradeInfo key=language item=languageData name=languages}
	  {assign var=i value=$smarty.foreach.languages.iteration}
	  <tr class="{cycle values="gbOdd, gbEven"}">
	    <td>
	      {$languageData.description}
	    </td>
	    <td style="text-align: center">
	      <input type="radio" id=upgrade{$i} name="{g->formVar var="form[languageAction][$language]"}" value='upgrade'
		{if empty($languageData.upgrade)}disabled="disabled" {/if}
	      />
	    </td>
	    <td style="text-align: center">
	      <input type="radio" id=remove{$i} name="{g->formVar var="form[languageAction][$language]"}" value='remove'
		{if empty($languageData.installed)}disabled="disabled" {/if}
	      />
	    </td>
	    <td style="text-align: center">
	      <input type="radio" id=download{$i} name="{g->formVar var="form[languageAction][$language]"}" value='download'
		{if !empty($languageData.installed)}disabled="disabled" {/if}
	      />
	    </td>
	    <td style="text-align: center">
	      <input type="radio" id=reset{$i} name="{g->formVar var="form[languageAction][$language]"}" value='reset' checked='checked' />
	    </td>
	  </tr>
	{/foreach}
	<tr class="{cycle values="gbOdd, gbEven"}">
	  <td>
	    {g->text text="Select All"}
	  </td>
	  <td style="text-align: center">
	    <input type="radio" id=selectAllUpgrade2 onclick="toggleAll('selectAllUpgrade2', 'selectAllUpgrade1', 'upgrade', {$languageCount})" 
		{if empty($AdminLanguages.enableSelectAll.upgrade)}disabled="disabled" {/if}
		class="selectAll"
	    />
	  </td>
	  <td style="text-align: center">
	    <input type="radio" id=selectAllRemove2 onclick="toggleAll('selectAllRemove2', 'selectAllRemove1', 'remove', {$languageCount})" 
		{if empty($AdminLanguages.enableSelectAll.remove)}disabled="disabled" {/if}
		class="selectAll"
	    />
	  </td>
	  <td style="text-align: center">
	    <input type="radio" id=selectAllDownload2 onclick="toggleAll('selectAllDownload2', 'selectAllDownload1', 'download', {$languageCount})" 
		{if empty($AdminLanguages.enableSelectAll.download)}disabled="disabled" {/if}
		class="selectAll"
	    />
	  </td>
	  <td style="text-align: center">
	    &nbsp;
	  </td>
	</tr>
      </table>
    </div>
    <div class="gbBlock gcBackground1">
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[save]"}" value="{g->text text="Save"}"/>
    </div>
  {else}
    <div class="giWarning">
      {g->text text='There are no local repository indices from which to determine language packages. Please click on the link below to go to "Download Plugin List" and choose "Update Plugin List" to get the latest language package information.'}<br />
          <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminRepository"}">{g->text text="Administer Repositories"}</a>
    </div>
  {/if} {* !empty($languageCount) *}
  </div>
{/if} {* modules/themes are writeable *}