{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Rating Settings"} </h2>
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
    <td style="text-align:right">
      <input type="checkbox" id="allowAlbumRating"
	{if $form.allowAlbumRating} checked="checked" {/if}
	name="{g->formVar var="form[allowAlbumRating]"}"/>
    </td><td>
      <label for="allowAlbumRating">
	{g->text text="Allow users to rate entire albums, in addition to individual items."}
      </label>
    </td>
  </tr><tr>
    <td colspan="2">
      {capture assign=link}<a href="{g->url arg1="view=rating.RatingAlbum"
					    arg2="limit=3.5"}">{/capture}
      {g->text text="The settings below apply to the %sRating Album%s view, which shows highly rated items from across the Gallery." arg1=$link arg2="</a>"}
    </td>
  </tr><tr>
    <td> {g->text text="Query limit"} </td>
    <td>
      <input type="text" id="minLimit" size="8"
       name="{g->formVar var="form[minLimit]"}" value="{$form.minLimit}"/>
      {g->text text="Lowest value allowed for rating album threshold"} <br/>
      {if isset($form.error.minLimit)}
      <div class="giError">
	{g->text text="Invalid number"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td> {g->text text="Description"} </td>
    <td>
      <textarea rows="4" cols="60"
       name="{g->formVar var="form[description]"}">{$form.description}</textarea>
    </td>
  </tr><tr>
    <td> {g->text text="Sort order"} </td>
    <td>
      <select name="{g->formVar var="form[orderBy]"}" onchange="pickOrder()">
	{html_options options=$RatingSiteAdmin.orderByList selected=$form.orderBy}
      </select>
      <select name="{g->formVar var="form[orderDirection]"}">
	{html_options options=$RatingSiteAdmin.orderDirectionList
		      selected=$form.orderDirection}
      </select>
      {g->text text="with"}
      <select name="{g->formVar var="form[presort]"}">
	{html_options options=$RatingSiteAdmin.presortList selected=$form.presort}
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
    <td> {g->text text="Theme"} </td>
    <td>
      <select name="{g->formVar var="form[themeId]"}">
	{html_options options=$RatingSiteAdmin.themeList selected=$form.themeId}
      </select>
    </td>
  </tr></table>
</div>

{capture assign="message"}{g->text
 text="Settings for %s theme in Rating Album" arg1=$ThemeSettingsForm.theme.name}{/capture}
{g->block type="core.ThemeSettingsForm" class="gbBlock" message=$message formId="siteAdminForm"}

<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="form[currentThemeId]"}" value="{$form.themeId}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
