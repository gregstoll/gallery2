{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Gallery Themes"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.activated)}
    {g->text text="Successfully activated theme %s" arg1=$status.activated}
  {/if}
  {if isset($status.deactivated)}
    {g->text text="Successfully deactivated theme %s" arg1=$status.deactivated}
  {/if}
  {if isset($status.installed)}
    {g->text text="Successfully installed theme %s" arg1=$status.installed}
  {/if}
  {if isset($status.uninstalled)}
    {g->text text="Successfully uninstalled theme %s" arg1=$status.uninstalled}
  {/if}
  {if isset($status.upgraded)}
    {g->text text="Successfully upgraded theme %s" arg1=$status.upgraded}
  {/if}
  {if isset($status.savedTheme)}
    {g->text text="Successfully saved theme settings"}
  {/if}
  {if isset($status.savedDefaults)}
    {g->text text="Successfully saved default album settings"}
  {/if}
  {if isset($status.restoredTheme)}
    {g->text text="Restored theme settings"}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Defaults"} </h3>

  <p class="giDescription">
    {g->text text="These are default display settings for albums in your gallery.  They can be overridden in each album."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Default sort order"}
    </td><td>
      <select name="{g->formVar var="form[default][orderBy]"}" onchange="pickOrder()">
	{html_options options=$AdminThemes.orderByList selected=$form.default.orderBy}
      </select>
      <select name="{g->formVar var="form[default][orderDirection]"}">
	{html_options options=$AdminThemes.orderDirectionList
		      selected=$form.default.orderDirection}
      </select>
      {g->text text="with"}
      <select name="{g->formVar var="form[default][presort]"}">
	{html_options options=$AdminThemes.presortList selected=$form.default.presort}
      </select>
      <script type="text/javascript">
	// <![CDATA[
	function pickOrder() {ldelim}
	  var list = '{g->formVar var="form[default][orderBy]"}';
	  var frm = document.getElementById('siteAdminForm');
	  var index = frm.elements[list].selectedIndex;
	  list = '{g->formVar var="form[default][orderDirection]"}';
	  frm.elements[list].disabled = (index == 0) ?1:0;
	  list = '{g->formVar var="form[default][presort]"}';
	  frm.elements[list].disabled = (index == 0) ?1:0;
	{rdelim}
	pickOrder();
	// ]]>
      </script>
    </td>
  </tr>
  <tr>
    <td>
      {g->text text="Default theme"}
    </td><td>
      <select name="{g->formVar var="form[default][theme]"}">
	{html_options options=$AdminThemes.themeList selected=$form.default.theme}
      </select>
      {if isset($form.error.themeUnavailable)}
      <div class="giError">
	{g->text text="The %s theme is incompatible with your Gallery version or no longer available.  Please upgrade the %s theme or pick another default theme." arg1=$AdminThemes.themeId arg2=$AdminThemes.themeId}
      </div>
      {/if}
     </td>
  </tr>
  <tr>
    <td>
      {g->text text="New albums"}
    </td><td>
      <select name="{g->formVar var="form[default][newAlbumsUseDefaults]"}">
	{html_options options=$AdminThemes.newAlbumsUseDefaultsList
		      selected=$form.default.newAlbumsUseDefaults}
      </select>
    </td>
  </tr></table>

  <p class="giDescription">
    {capture assign="pluginsLink"}<a href="{g->url arg1="view=core.SiteAdmin"
     arg2="subView=core.AdminPlugins"}">{/capture}
    {g->text text="To activate more themes visit the %sPlugins%s page."
	     arg1=$pluginsLink arg2="</a>"}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][saveDefaults]"}" value="{g->text text="Save Defaults"}"/>
</div>

<div class="gbTabBar">
  {foreach from=$AdminThemes.themes key=themeId item=theme}
  {if $theme.active}
    {if $AdminThemes.themeId == $themeId}
      <span class="giSelected o"><span>
	{g->text text=$theme.name l10Domain=$theme.l10Domain}
      </span></span>
    {else}
      <span class="o"><span>
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminThemes"
			 arg3="themeId=$themeId"}">{g->text text=$theme.name l10Domain=$theme.l10Domain}</a>
      </span></span>
    {/if}
  {/if}
  {/foreach}
</div>

<div class="gbBlock">
  <h3>
    {g->text text="%s Theme Settings" arg1=$AdminThemes.themes[$AdminThemes.themeId].name}
  </h3>

  <p class="giDescription">
    {g->text text="These are the global settings for the theme.  They can be overridden at the album level."}
  </p>

  {include file="gallery:modules/core/templates/JavaScriptWarning.tpl"}

  {if isset($AdminThemes.customTemplate)}
    {include file="gallery:`$AdminThemes.customTemplate`"
	     l10Domain=$AdminThemes.themes[$AdminThemes.themeId].l10Domain}
  {/if}

  {if !empty($AdminThemes.settings)}
    <table class="gbDataTable">
      {foreach from=$AdminThemes.settings item=setting}
	<tr class="{cycle values="gbEven,gbOdd"}">
	  <td>
	    {$setting.name}
	  </td>
	  <td>
	    {if ($setting.type == 'text-field')}
	      <input type="text" size="{$setting.typeParams.size|default:6}"
	       name="{g->formVar var="form[key][`$setting.key`]"}"
	       value="{$form.key[$setting.key]}"/>
	    {elseif ($setting.type == 'textarea')}
	      <textarea style="width:{$setting.typeParams.width|default:'400px'};height:{$setting.typeParams.height|default:'75px'};"
	       name="{g->formVar var="form[key][`$setting.key`]"}">{$form.key[$setting.key]}</textarea>
	    {elseif ($setting.type == 'single-select')}
	      <select name="{g->formVar var="form[key][`$setting.key`]"}">
		{html_options options=$setting.choices selected=$form.key[$setting.key]}
	      </select>
	    {elseif ($setting.type == 'checkbox')}
	      <input type="checkbox" {if !empty($setting.value)}checked="checked" {/if}
	       name="{g->formVar var="form[key][`$setting.key`]"}" />
	    {elseif ($setting.type == 'block-list')}
	      <table>
		<tr>
		  <td style="text-align: right;">
		    {g->text text="Available"}
		  </td>
		  <td>
		    <select id="blocksAvailableList_{$setting.key}"
			    onchange="bsw_selectToUse('{$setting.key}');">
		      <option value="">{g->text text="Choose a block"}</option>
		    </select>
		  </td>
		  <td class="bsw_BlockCommands">
		    <span id="bsw_AddButton_{$setting.key}"
			  onclick="bsw_addBlock('{$setting.key}');" class="bsw_ButtonDisabled">
		      {g->text text="Add"}
		    </span>
		  </td>
		</tr>

		<tr>
		  <td style="text-align: right; vertical-align: top;">
		    {g->text text="Selected"}
		  </td>
		  <td id="bsw_UsedBlockList_{$setting.key}">
		    <select id="blocksUsedList_{$setting.key}" size="10"
			    onchange="bsw_selectToChange('{$setting.key}');">
		      <option value=""></option> {* Dummy option so xhtml validates *}
		    </select>
		  </td>
		  <td class="bsw_BlockCommands">
		    <span style="display: block"
			  id="bsw_RemoveButton_{$setting.key}"
			  onclick="bsw_removeBlock('{$setting.key}');"
			  class="bsw_ButtonDisabled">
		      {g->text text="Remove"}
		    </span>

		    <span style="display: block"
			  id="bsw_MoveUpButton_{$setting.key}"
			  onclick="bsw_moveUp('{$setting.key}');"
			  class="bsw_ButtonDisabled">
		      {g->text text="Move Up"}
		    </span>

		    <span style="display: block"
			  id="bsw_MoveDownButton_{$setting.key}"
			  onclick="bsw_moveDown('{$setting.key}');"
			  class="bsw_ButtonDisabled">
		      {g->text text="Move Down"}
		    </span>
		  </td>
		</tr>
		<tr>
		  <td id="bsw_BlockOptions_{$setting.key}" colspan="3">
		  </td>
		</tr>
	      </table>
	      <input type="hidden"
		     id="albumBlockValue_{$setting.key}" size="60"
		     name="{g->formVar var="form[key][`$setting.key`]"}"
		     value="{$form.key[$setting.key]|replace:'"':'&quot;'}"/>

	      <script type="text/javascript">
		// <![CDATA[
		var block;
		var tmp;
		{foreach from=$AdminThemes.availableBlocks key=moduleId item=blocks}
		  {foreach from=$blocks key=blockName item=block}
		    block = bsw_addAvailableBlock("{$setting.key}", "{$moduleId}.{$blockName}",
			    "{g->text text=$block.description l10Domain="modules_$moduleId" forJavascript=true}");
		    {if !empty($block.vars)}
		      {foreach from=$block.vars key=varKey item=varInfo}
			tmp = new Array();
			{if ($varInfo.type == 'choice')}
			  {foreach from=$varInfo.choices key=choiceKey item=choiceValue}
			    tmp["{$choiceKey}"] = "{g->text text=$choiceValue
							    l10Domain="modules_$moduleId" forJavascript=true}";
			  {/foreach}
			{/if}
			block.addVariable("{$varKey}", "{$varInfo.default}",
			  "{g->text text=$varInfo.description l10Domain="modules_$moduleId" forJavascript=true}",
			  "{$varInfo.type}", tmp);
	                {if !empty($varInfo.overrides)}
	                {foreach from=$varInfo.overrides item=override}
	                block.addVariableOverride("{$varKey}", "{$override}");
                        {/foreach}
	                {/if}
		      {/foreach}
		    {/if}
		  {/foreach}
		{/foreach}
		{* Now initialize the form with the album block values *}
		bsw_initAdminForm("{$setting.key}", "{g->text text="Parameter" forJavascript=true}",
						    "{g->text text="Value" forJavascript=true}");
		// ]]>
	      </script>
	    {/if}
	  </td>
	</tr>

	{if isset($form.error.key[$setting.key].invalid)}
	<tr>
	  <td colspan="2" class="giError">
	    {$form.errorMessage[$setting.key]}
	  </td>
	</tr>
	{/if}
      {/foreach}
    </table>
  {elseif !isset($AdminThemes.customTemplate)}
    {g->text text="There are no settings for this theme"}
  {/if}
</div>

{if isset($AdminThemes.customTemplate) || !empty($AdminThemes.settings)}
<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="themeId"}" value="{$AdminThemes.themeId}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][saveTheme]"}" value="{g->text text="Save Theme Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][undoTheme]"}" value="{g->text text="Reset"}"/>
</div>
{/if}

