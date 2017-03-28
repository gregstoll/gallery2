{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Dynamic Album Settings"} </h2>
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
  <p class="giDescription">
    {g->text text="Album sizes are total number of items in the dynamic album. Number of items shown per page is a theme settings. Enter 0 for total size to disable a view. Use URL rewrite module to create simpler URLs to these views."}
  </p>
  <table class="gbDataTable"><tr>
    <td colspan="2"><h4> {g->text text="Updates Album"} </h4></td>
  </tr><tr>
    <td>
      {g->text text="URL"}
    </td><td>
      <a href="{g->url arg1="view=dynamicalbum.UpdatesAlbum" forceSessionId=false}">
	{g->url arg1="view=dynamicalbum.UpdatesAlbum" forceFullUrl=true forceSessionId=false}
      </a>
    </td>
  </tr><tr>
    <td>
      {g->text text="Total Size"}
    </td><td>
      <input type="text" size="6"
       name="{g->formVar var="form[size_date]"}" value="{$form.size_date}"/>
      {if isset($form.error.size_date)}
	<div class="giError"> {g->text text="Enter a number"} </div>
      {/if}
    </td>
  </tr><tr>
    <td>
      {g->text text="Default"}
    </td><td>
      <select name="{g->formVar var="form[type_date]"}">
	{html_options options=$DynamicAlbumSiteAdmin.defaultList selected=$form.type_date}
      </select>
      {capture assign="key"}{g->formVar var="show"}{/capture}
      &nbsp; {g->text text="Override default with parameter %s=value where value is %s, %s or %s"
		 arg1=$key arg2="data" arg3="all" arg4="album"}
    </td>
  </tr><tr>
    <td style="vertical-align: top; padding-top: 6px">
      {g->text text="Description"}
    </td><td>
      <textarea rows="3" cols="60"
       name="{g->formVar var="form[description_date]"}">{$form.description_date}</textarea>
      <p class="giDescription">
	{g->text text="Text for theme to display with all album of recent items."}
      </p>
    </td>
  </tr><tr>
    <td>
      {g->text text="Link"}
    </td><td>
      <input type="checkbox" id="itemLinkDate" {if !empty($form.itemlink_date)
       } checked="checked" {/if} name="{g->formVar var="form[itemlink_date]"}"/>
      <label for="itemLinkDate">
	{g->text text="Show Latest Updates link in each album"}
      </label>
    </td>
  </tr><tr>
    <td colspan="2"><h4> {g->text text="Popular Album"} </h4></td>
  </tr><tr>
    <td>
      {g->text text="URL"}
    </td><td>
      <a href="{g->url arg1="view=dynamicalbum.PopularAlbum" forceSessionId=false}">
	{g->url arg1="view=dynamicalbum.PopularAlbum" forceFullUrl=true forceSessionId=false}
      </a>
    </td>
  </tr><tr>
    <td>
      {g->text text="Total Size"}
    </td><td>
      <input type="text" size="6"
       name="{g->formVar var="form[size_views]"}" value="{$form.size_views}"/>
      {if isset($form.error.size_views)}
	<div class="giError"> {g->text text="Enter a number"} </div>
      {/if}
    </td>
  </tr><tr>
    <td>
      {g->text text="Default"}
    </td><td>
      <select name="{g->formVar var="form[type_views]"}">
	{html_options options=$DynamicAlbumSiteAdmin.defaultList selected=$form.type_views}
      </select>
    </td>
  </tr><tr>
    <td style="vertical-align: top; padding-top: 6px">
      {g->text text="Description"}
    </td><td>
      <textarea rows="3" cols="60"
       name="{g->formVar var="form[description_views]"}">{$form.description_views}</textarea>
      <p class="giDescription">
	{g->text text="Text for theme to display with all album of most viewed items."}
      </p>
    </td>
  </tr><tr>
    <td>
      {g->text text="Link"}
    </td><td>
      <input type="checkbox" id="itemLinkViews" {if !empty($form.itemlink_views)
       } checked="checked" {/if} name="{g->formVar var="form[itemlink_views]"}"/>
      <label for="itemLinkViews">
	{g->text text="Show Popular Items link in each album"}
      </label>
    </td>
  </tr><tr>
    <td colspan="2"><h4> {g->text text="Random Album"} </h4></td>
  </tr><tr>
    <td>
      {g->text text="URL"}
    </td><td>
      <a href="{g->url arg1="view=dynamicalbum.RandomAlbum" forceSessionId=false}">
	{g->url arg1="view=dynamicalbum.RandomAlbum" forceFullUrl=true forceSessionId=false}
      </a>
    </td>
  </tr><tr>
    <td>
      {g->text text="Total Size"}
    </td><td>
      <input type="text" size="6"
       name="{g->formVar var="form[size_random]"}" value="{$form.size_random}"/>
      {if isset($form.error.size_random)}
	<div class="giError"> {g->text text="Enter a number"} </div>
      {/if}
    </td>
  </tr><tr>
    <td>
      {g->text text="Default"}
    </td><td>
      <select name="{g->formVar var="form[type_random]"}">
	{html_options options=$DynamicAlbumSiteAdmin.defaultList selected=$form.type_random}
      </select>
    </td>
  </tr><tr>
    <td style="vertical-align: top; padding-top: 6px">
      {g->text text="Description"}
    </td><td>
      <textarea rows="3" cols="60"
       name="{g->formVar var="form[description_random]"}">{$form.description_random}</textarea>
      <p class="giDescription">
	{g->text text="Text for theme to display with all album of random items."}
      </p>
    </td>
  </tr><tr>
    <td>
      {g->text text="Link"}
    </td><td>
      <input type="checkbox" id="itemLinkRandom" {if !empty($form.itemlink_random)
       } checked="checked" {/if} name="{g->formVar var="form[itemlink_random]"}"/>
      <label for="itemLinkRandom">
	{g->text text="Show Random Items link in each album"}
      </label>
    </td>
  </tr></table>
</div>

{capture assign="message"}
  {g->text text="Theme"} &nbsp; &nbsp; &nbsp;
  <select name="{g->formVar var="form[themeId]"}">
    {html_options options=$DynamicAlbumSiteAdmin.themeList selected=$form.themeId}
  </select>
  <br/><br/>
  {g->text text="Settings for %s theme in Dynamic Albums" arg1=$ThemeSettingsForm.theme.name}
{/capture}
{g->block type="core.ThemeSettingsForm" class="gbBlock" message=$message formId="siteAdminForm"}

<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="form[currentThemeId]"}" value="{$form.themeId}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
