{*
 * $Revision: 16742 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
  //<![CDATA[
  var pluginData = {ldelim} "module" : {ldelim} {rdelim}, "theme" : {ldelim} {rdelim} {rdelim};
  {foreach name=names from=$AdminPlugins.plugins item=plugin}
  pluginData["{$plugin.type}"]["{$plugin.id}"] = {ldelim} "name" : "{$plugin.name|escape:"javascript"}", "deletable" : {$plugin.deletable}, "state" : "{$plugin.state}" {rdelim};
  {/foreach}

  var stateData = {ldelim}
    "inactive" : {ldelim}
      "class" : "icon-plugin-inactive",
      "text" : "{g->text text="Status: Inactive" forJavascript=true}",
      "actions" : {ldelim} "activate": 1, "uninstall" : 1, "delete" : 1 {rdelim},
      "callback": "copyVersionToInstalledVersion",
      "message" : {ldelim} "type" : "giSuccess", "text" : "{g->text text="__PLUGIN__ deactivated" forJavascript=true}" {rdelim}
    {rdelim},
    "active" : {ldelim}
      "class" : "icon-plugin-active",
      "text" : "{g->text text="Status: Active" forJavascript=true}",
      "actions" : {ldelim} "deactivate": 1, "uninstall" : 1, "delete" : 1  {rdelim},
      "callback": "copyVersionToInstalledVersion",
      "message" : {ldelim} "type" : "giSuccess", "text" : "{g->text text="__PLUGIN__ activated" forJavascript=true}" {rdelim}
    {rdelim},
    "uninstalled" : {ldelim}
      "class" : "icon-plugin-uninstall",
      "text" : "{g->text text="Status: Not Installed" forJavascript=true}",
      "actions" : {ldelim} "install": 1, "delete" : 1 {rdelim},
      "callback": "eraseInstalledVersion",
      "message" : {ldelim} "type" : "giSuccess", "text" : "{g->text text="__PLUGIN__ uninstalled" forJavascript=true}" {rdelim}
    {rdelim},
    "unupgraded" : {ldelim}
      "class" : "icon-plugin-upgrade",
      "text" : "{g->text text="Status: Upgrade Required (Inactive)" forJavascript=true}",
      "actions" : {ldelim} "upgrade": 1 {rdelim}
    {rdelim},
    "incompatible" : {ldelim}
      "class" : "icon-plugin-incompatible",
      "text" : "{g->text text="Status: Incompatible Plugin (Inactive)" forJavascript=true}",
      "actions" : {ldelim} {rdelim}
    {rdelim},
    "unconfigured" : {ldelim}
      "class" : "icon-plugin-inactive",
      "text" : "{g->text text="Status: Inactive (Configuration Required)" forJavascript=true}",
      "actions" : {ldelim} "configure" : 1, "uninstall" : 1, "delete" : 1 {rdelim},
      "callback": "copyVersionToInstalledVersion",
      "message" : {ldelim} "type" : "giWarning", "text" : "{g->text text="__PLUGIN__ needs configuration" forJavascript=true}" {rdelim}
    {rdelim},
    "deleted" : {ldelim}
      "message" : {ldelim} "type" : "giSuccess", "text" : "{g->text text="__PLUGIN__ deleted" forJavascript=true}" {rdelim}
    {rdelim}
  {rdelim};
  var errorPageUrl = '{g->url arg1="view=core.ErrorPage" htmlEntities=0}';
  var uninstallPrompt = {ldelim}
    "header" : '{g->text text="Warning!" forJavascript=true}',
    "body"   : '{g->text text="Do you really want to uninstall __PLUGIN__?" forJavascript=true}' +
	       '<br/>' +
               '<b>{g->text text="This will also remove any permissions and clean up any data created by this module." forJavascript=true}</b> ' +
               '{g->text text="This plugin will be uninstalled, but its files will be kept so that you can reinstall it." forJavascript=true}',
    "yes"    : '{g->text text="Yes" forJavascript=true}',
    "no"     : '{g->text text="No" forJavascript=true}'
  {rdelim};

  var uninstallAndDeletePrompt = {ldelim}
    "header" : '{g->text text="Warning!" forJavascript=true}',
    "body"   : '{g->text text="Do you really want to delete __PLUGIN__?" forJavascript=true}' +
	       '<br/>' +
               '<b>{g->text text="This will also remove any permissions and clean up any data created by this module." forJavascript=true}</b> ' +
               '{g->text text="This plugin will be uninstalled and its files will be deleted permanently." forJavascript=true}',
    "yes"    : '{g->text text="Yes" forJavascript=true}',
    "no"     : '{g->text text="No" forJavascript=true}'
  {rdelim};

  var deletePrompt = {ldelim}
    "header" : '{g->text text="Warning!" forJavascript=true}',
    "body"   : '{g->text text="Do you really want to delete __PLUGIN__?" forJavascript=true}' +
	       '<br/>' +
               '{g->text text="The files of this plugin will be deleted permanently." forJavascript=true}',
    "yes"    : '{g->text text="Yes" forJavascript=true}',
    "no"     : '{g->text text="No" forJavascript=true}'
  {rdelim};

  var legendStrings = {ldelim}
    "inactive"     : '{g->text text="disabled(__COUNT__)" forJavascript=true}',
    "active"       : '{g->text text="up to date(__COUNT__)" forJavascript=true}',
    "uninstalled"  : '{g->text text="not installed(__COUNT__)" forJavascript=true}',
    "unupgraded"   : '{g->text text="upgrade required(__COUNT__)" forJavascript=true}',
    "incompatible" : '{g->text text="incompatible(__COUNT__)" forJavascript=true}'
  {rdelim};

  var failedToDeleteMessage = '{g->text text="Failed to completely delete __PLUGIN__" forJavascript=true}';

  {literal}
  var contexts = {"module": {}, "theme": {}};
  var allActions = ["install", "configure", "upgrade", "activate", "deactivate", "uninstall", "delete"];

  YAHOO.util.Event.addListener(window, "scroll", updateStatusPosition, false);
  {/literal}
  //]]>
</script>

<div id="gbPluginStatusUpdates"></div>

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Gallery Plugins"} </h2>
</div>

<div class="gbTabBar">
  <span class="giSelected o"><span>
    {g->text text="Plugins"}
  </span></span>

  <span class="o"><span>
    {capture name=getMoreLink}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminRepository"}">{g->text text="Get More Plugins"}</a>
    {/capture}
    {$smarty.capture.getMoreLink}
  </span></span>
</div>

{if $AdminPlugins.showGetMorePluginsTip}
<div class="gbBlock">
  <p class="giDescription">
    <h2>
      {g->text text="Want more features?  New plugins are just a click away.  Just click the %s link to get started." arg1=$smarty.capture.getMoreLink}
    </h2>
  </p>
</div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Gallery features come as separate plugins.  You can download and install plugins to add more features to your Gallery, or you can disable features if you don't want to use them.  In order to use a feature, you must install, configure (if necessary) and activate it.  If you don't wish to use a feature, you can deactivate it."}
  </p>

  {include file="gallery:modules/core/templates/JavaScriptWarning.tpl"}

  {include file="gallery:modules/core/templates/AdminPluginsLegend.tpl" legendId="top"}
  <table class="gbDataTable">
    {assign var="group" value=""}
    {foreach from=$AdminPlugins.plugins item=plugin}
      {if $group != $plugin.group}
	{if !empty($group)}
	  <tr><td> &nbsp; </td></tr>
	{/if}
	<tr>
	  <th colspan="6"><h2>{$plugin.groupLabel}</h2></th>
	</tr><tr>
	  <th> &nbsp; </th>
	  <th> {g->text text="Plugin Name"} </th>
	  <th> {g->text text="Installed"} </th>
	  <th> {g->text text="Version"} </th>
	  <th> {g->text text="Description"} </th>
	  <th> {g->text text="Actions"} </th>
	</tr>
      {/if}
      {assign var="group" value=$plugin.group}

      <tr id="plugin-row-{$plugin.type}-{$plugin.id}" class="{cycle values="gbEven,gbOdd"}">
	<td style="position: relative;">
	  <div id="plugin-icon-{$plugin.type}-{$plugin.id}" style="height: 16px"></div>
	</td>

	<td id="plugin-{$plugin.type}-{$plugin.id}-name">
	 <div class="gbLink-Help" onclick="window.open('http://codex.gallery2.org/index.php/Gallery2:{$plugin.type|capitalize}s:{$plugin.id}', 'Gallery2_Help')"><a href="javascript:window.open('http://codex.gallery2.org/index.php/Gallery2:{$plugin.type|capitalize}s:{$plugin.id}', 'Gallery2_Help'); return false;">&nbsp;{g->text text="help"}&nbsp;</a></div>
	  {if empty($plugin.screenshot)}
	  {$plugin.name}
	  {else}
	  <span class="gTooltipTarget">{$plugin.name}</span>
	  {capture assign=screenshotLabel}{g->text text="Screenshot for %s" arg1=$plugin.name}{/capture}
	  <script type="text/javascript">
	    // <![CDATA[
	    new YAHOO.widget.Tooltip("gTooltip", {ldelim}
		context: "plugin-{$plugin.type}-{$plugin.id}-name",
		text: '<img src="{g->url href="`$plugin.screenshot`"}" alt="{$screenshotLabel|escape:"html":"UTF-8"|escape:"javascript":"UTF-8"}"/>',
		showDelay: 250 {rdelim});
            // ]]>
          </script>
	  {/if}
	</td>

	<td id="plugin-{$plugin.type}-{$plugin.id}-installedVersion" align="center">
	  {$plugin.installedVersion}
	</td>

	<td id="plugin-{$plugin.type}-{$plugin.id}-version" align="center">
	  {$plugin.version}
	</td>

	<td>
	  {$plugin.description}
	  {if $plugin.state == 'incompatible'}
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

	<td style="width: 150px">
          {if ($plugin.type == 'module' && $plugin.id == 'core') || $plugin.state == 'incompatible'}
	    &nbsp;
	  {else}
            <span id="action-install-{$plugin.type}-{$plugin.id}" style="display: none">
              <a style="cursor: pointer" onclick="performPluginAction('{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=install" useAuthToken=1}')">
                {g->text text="install"}
              </a>
            </span>
            <span id="action-upgrade-{$plugin.type}-{$plugin.id}" style="display: none">
              <a style="cursor: pointer" onclick="performPluginAction('{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=upgrade" useAuthToken=1}')">
                {g->text text="upgrade"}
              </a>
            </span>
            <span id="action-configure-{$plugin.type}-{$plugin.id}" style="display: none">
              <a style="cursor: pointer" onclick="performPluginAction('{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=configure" useAuthToken=1}')">
                {g->text text="configure"}
              </a> |
            </span>
            <span id="action-activate-{$plugin.type}-{$plugin.id}" style="display: none">
              <a style="cursor: pointer" onclick="performPluginAction('{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=activate"  useAuthToken=1}')">
                {g->text text="activate"}
              </a> |
            </span>
	    {* Omit some actions for default theme *}
	    {if !($plugin.type == 'theme' && $plugin.id == $AdminPlugins.defaultTheme && $plugin.state == 'active')}
            <span id="action-deactivate-{$plugin.type}-{$plugin.id}" style="display: none">
              <a style="cursor: pointer" onclick="performPluginAction('{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=deactivate" useAuthToken=1}')">
                {g->text text="deactivate"}
              </a> |
            </span>
            <span id="action-uninstall-{$plugin.type}-{$plugin.id}" style="display: none">
              <a style="cursor: pointer" onclick="verify(uninstallPrompt, '{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=uninstall" useAuthToken=1}')">
                {g->text text="uninstall"}
              </a>
            </span>
	    {if $AdminPlugins.canDeletePlugins}
            <span id="action-delete-{$plugin.type}-{$plugin.id}" style="display: none">
              | <a style="cursor: pointer" onclick="verify(pluginData['{$plugin.type}']['{$plugin.id}']['state'] == 'uninstalled' ? deletePrompt : uninstallAndDeletePrompt, '{$plugin.type}', '{$plugin.id}', '{g->url arg1="view=core.PluginCallback" arg2="pluginId=`$plugin.id`" arg3="pluginType=`$plugin.type`" arg4="command=delete" useAuthToken=1}')">
                {g->text text="delete"}
              </a>
            </span>
            {/if}
            {/if}
	  {/if}
          <script type="text/javascript"> updatePluginState('{$plugin.type}', '{$plugin.id}', '{$plugin.state}', false); </script>
	</td>
      </tr>
    {/foreach}
  </table>
  {include file="gallery:modules/core/templates/AdminPluginsLegend.tpl" legendId="bottom"}
  <script type="text/javascript"> updateStateCounts(); </script>
</div>
