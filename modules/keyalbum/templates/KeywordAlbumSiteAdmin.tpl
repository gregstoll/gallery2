{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Keyword Album Settings"} </h2>
</div>

{if !empty($status) || !empty($form.error)}
<div class="gbBlock">
{if isset($status.saved)}
<h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2>
{/if}
{if !empty($form.error)}
<h2 class="giError">
  {g->text text="There was a problem processing your request."}
</h2>
{/if}
</div>
{/if}

<div class="gbBlock">
  <table class="gbDataTable"><tr>
    <td colspan="2"> {g->text text="Select where to include links in item summary info, usually shown next to thumbnails.  You can also add the Keyword Albums block in theme settings."} </td>
  </tr><tr>
    <td> {g->text text="Keyword Links"} </td>
    <td>
      <select name="{g->formVar var="form[summaryLinks]"}">
	{html_options options=$KeywordAlbumSiteAdmin.summaryList selected=$form.summaryLinks}
      </select>
    </td>
  </tr><tr>
    <td style="vertical-align:top;padding-top:6px"> {g->text text="Split keywords on"} </td>
    <td>
      <input type="checkbox" id="splitSemicolon" name="{g->formVar var="form[split][semicolon]"}"
       {if !empty($form.split.semicolon)} checked="checked"{/if}/>
      <label for="splitSemicolon">
	{g->text text="Semicolons"}
      </label>
      &nbsp;
      <input type="checkbox" id="splitComma" name="{g->formVar var="form[split][comma]"}"
       {if !empty($form.split.comma)} checked="checked"{/if}/>
      <label for="splitComma">
	{g->text text="Commas"}
      </label>
      &nbsp;
      <input type="checkbox" id="splitSpace" name="{g->formVar var="form[split][space]"}"
       {if !empty($form.split.space)} checked="checked"{/if}/>
      <label for="splitSpace">
	{g->text text="Spaces"}
      </label>
      {if isset($form.error.split)}
      <div class="giError">
	{g->text text="Select at least one separator"}
      </div>
      {/if}
      <p class="giDescription">
	{g->text text="Use any of the selected characters to separate multiple keywords in an item's keywords field."}
      </p>
    </td>
  </tr><tr>
    <td colspan="2"> {g->text text="Display of Keyword Albums"} </td>
  </tr><tr>
    <td> {g->text text="Sort order"} </td>
    <td>
      <select name="{g->formVar var="form[orderBy]"}" onchange="pickOrder()">
	{html_options options=$KeywordAlbumSiteAdmin.orderByList selected=$form.orderBy}
      </select>
      <select name="{g->formVar var="form[orderDirection]"}">
	{html_options options=$KeywordAlbumSiteAdmin.orderDirectionList
		      selected=$form.orderDirection}
      </select>
      {g->text text="with"}
      <select name="{g->formVar var="form[presort]"}">
	{html_options options=$KeywordAlbumSiteAdmin.presortList selected=$form.presort}
      </select>
      <script type="text/javascript">
	// <![CDATA[
	function pickOrder() {ldelim}
	  var list = '{g->formVar var="form[orderBy]"}';
	  var frm = document.getElementById('siteAdminForm');
	  var index = frm.elements[list].selectedIndex;
	  list = '{g->formVar var="form[orderDirection]"}';
	  frm.elements[list].disabled = (index <= 1) ?1:0;
	  list = '{g->formVar var="form[presort]"}';
	  frm.elements[list].disabled = (index <= 1) ?1:0;
	{rdelim}
	pickOrder();
	// ]]>
      </script>
    </td>
  </tr><tr>
    <td style="vertical-align: top; padding-top: 6px"> {g->text text="Description"} </td>
    <td>
      <textarea rows="4" cols="60"
       name="{g->formVar var="form[description]"}">{$form.description}</textarea>
      <p class="giDescription">
	{g->text text="Text for theme to display with all Keyword Albums."}
      </p>
    </td>
  </tr><tr>
    <td> {g->text text="Theme"} </td>
    <td>
      <select name="{g->formVar var="form[themeId]"}">
	{html_options options=$KeywordAlbumSiteAdmin.themeList selected=$form.themeId}
      </select>
    </td>
  </tr></table>
</div>

{capture assign="message"}{g->text
 text="Settings for %s theme in Keyword Albums" arg1=$ThemeSettingsForm.theme.name}{/capture}
{g->block type="core.ThemeSettingsForm" class="gbBlock" message=$message formId="siteAdminForm"}

<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="form[currentThemeId]"}" value="{$form.themeId}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
