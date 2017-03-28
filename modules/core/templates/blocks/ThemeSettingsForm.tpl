{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="{$class}">
{if isset($message)}
  <p class="giDescription"> {$message} </p>
{/if}

{include file="gallery:modules/core/templates/JavaScriptWarning.tpl"}

{if isset($ThemeSettingsForm.customTemplate)}
  {include file="gallery:`$ThemeSettingsForm.customTemplate`" l10Domain=$ThemeSettingsForm.theme.l10Domain}
{/if}

{if !empty($ThemeSettingsForm.settings)}
  <table class="gbDataTable"><tr>
    <th> {g->text text="Setting"} </th>
    <th> {g->text text="Value"} </th>
    <th> {g->text text="Use Global"} </th>
  </tr>

  {foreach from=$ThemeSettingsForm.settings item=setting}
    {assign var="settingKey" value=$setting.key}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$setting.name}
      </td><td>
	{if ($setting.type == 'text-field')}
	  <input type="text" size="{$setting.typeParams.size|default:6}"
		 onchange="changeSetting('{$settingKey}')"
		 name="{g->formVar var="form[key][$settingKey]"}" value="{$form.key.$settingKey}"/>
	{elseif ($setting.type == 'textarea')}
	  <textarea style="width:{$setting.typeParams.width|default:'400px'};height:{$setting.typeParams.height|default:'75px'};"
	   name="{g->formVar var="form[key][$settingKey]"}">{$form.key[$settingKey]}</textarea>
	{elseif ($setting.type == 'single-select')}
	  <select name="{g->formVar var="form[key][$settingKey]"}"
		  onchange="changeSetting('{$settingKey}')">
	    {html_options options=$setting.choices selected=$form.key.$settingKey}
	  </select>
	{elseif ($setting.type == 'checkbox')}
	  <input type="checkbox" onclick="changeSetting('{$settingKey}')"
		 name="{g->formVar var="form[key][$settingKey]"}"
	   {if !empty($form.key.$settingKey)} checked="checked"{/if}/>
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
		  <span id="bsw_AddButton_{$setting.key}" onclick="bsw_addBlock('{$setting.key}');"
		    class="bsw_ButtonDisabled">
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
		   onchange="changeSetting('{$settingKey}'); bsw_reInitAdminForm('{$settingKey}');"
		   id="albumBlockValue_{$setting.key}" size="60"
		   name="{g->formVar var="form[key][$settingKey]"}"
		   value="{$form.key.$settingKey|replace:'"':'&quot;'}"/>

	    <script type="text/javascript">
	      // <![CDATA[
	      var block;
	      var tmp;
	      {foreach from=$ThemeSettingsForm.availableBlocks key=moduleId item=blocks}
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

      <td align="center">
	<input type="checkbox" onclick="toggleGlobal('{$settingKey}');"
	       name="{g->formVar var="form[useGlobal][$settingKey]"}"
	 {if (!isset($ThemeSettingsForm.globalParams.$settingKey))}
	   disabled="disabled"
	 {elseif (!empty($form.useGlobal.$settingKey))}
	   checked="checked"
	 {/if}/>
      </td>
    </tr>

    {if isset($form.error.key.$settingKey.invalid)}
    <tr>
      <td colspan="2" class="giError">
	{$form.errorMessage.$settingKey}
      </td>
    </tr>
    {/if}
  {/foreach}
  </table>
{elseif !isset($ThemeSettingsForm.customTemplate)}
  <b> {g->text text="There are no settings for this theme"} </b>
{/if}
</div>

<script type="text/javascript">
  // <![CDATA[
  var isSaved = new Array;
  var savedValues = new Array;
  var globalValues = new Array;

  {*
   * Convert " to &quot; so that Javascript can parse embedded quotes in the
   * the data.  Convert \ to \\ so that when we move this value around the
   * backslash is preserved (else \" turns into ").
   *}
  {foreach from=$ThemeSettingsForm.globalParams key=key item=value}
    globalValues['{$key}'] = "{$value|replace:'"':'&quot;'|replace:'\\':'\\\\'}";
  {/foreach}

  {* Undo the quotes coercion here in Javascript so that we have the correct data *}
  globalValues['albumBlocks'] = globalValues['albumBlocks'].replace(/&quot;/g, '"');
  globalValues['photoBlocks'] = globalValues['photoBlocks'].replace(/&quot;/g, '"');
  globalValues['sidebarBlocks'] = globalValues['sidebarBlocks'].replace(/&quot;/g, '"');

  function toggleGlobal(key) {ldelim}
    var frm = document.getElementById('{$formId}');
    inputWidget = frm.elements['{g->formVar var="form[key]["}' + key + ']'];
    toggleWidget = frm.elements['{g->formVar var="form[useGlobal]["}' + key + ']'];
    {literal}
    if (toggleWidget.checked) {
      savedValues[key] = inputWidget.value;
      isSaved[key] = true;
      if (inputWidget.type == 'checkbox') {
	if (globalValues[key] != 0) {
	  inputWidget.checked = 'checked';
	} else {
	  inputWidget.checked = null;
	}
      } else {
	inputWidget.value = globalValues[key];
      }
    } else {
      if (inputWidget.type == 'checkbox') {
	if (globalValues[key] == 0) {
	  inputWidget.checked = 'checked';
	} else {
	  inputWidget.checked = null;
	}
      } else if (isSaved[key]) {
	inputWidget.value = savedValues[key];
      }
    }
    if (inputWidget.type != 'checkbox') inputWidget.onchange();
    bsw_showBlockOptions(key);
  }

  function changeSetting(key) {
    {/literal}
    var frm = document.getElementById('{$formId}');
    inputWidget = frm.elements['{g->formVar var="form[key]["}' + key + ']'];
    toggleWidget = frm.elements['{g->formVar var="form[useGlobal]["}' + key + ']'];
    {literal}
    if (inputWidget.type == 'checkbox') {
      toggleWidget.checked = ((globalValues[key] == 0 && !inputWidget.checked) ||
			      (globalValues[key] == 1 && inputWidget.checked));
    } else {
      toggleWidget.checked = (inputWidget.value == globalValues[key]);
    }
  }
  {/literal}
  // ]]>
</script>
