{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="User Album Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="An album will be created for each user.  The user will have full permissions on the album."}
  </p>

  <table class="gbDataTable"><tr>
    <td> {g->text text="Create albums"} </td>
    <td>
      <select name="{g->formVar var="form[create]"}">
	{html_options options=$UserAlbumSiteAdmin.createList selected=$form.create}
      </select>
    </td>
  </tr><tr>
    <td> {g->text text="Albums viewable by"} </td>
    <td>
      <select name="{g->formVar var="form[view]"}">
	{html_options options=$UserAlbumSiteAdmin.viewList selected=$form.view}
      </select>
    </td>
  </tr><tr>
    <td> {g->text text="Full size images viewable"} </td>
    <td>
      <select name="{g->formVar var="form[fullSize]"}">
	{html_options options=$UserAlbumSiteAdmin.sizeList selected=$form.fullSize}
      </select>
    </td>
  </tr><tr>
    <td> {g->text text="Location for new user albums"} </td>
    <td>
      <select name="{g->formVar var="form[targetLocation]"}">
	{foreach from=$UserAlbumSiteAdmin.targetLocation item=album}
	<option value="{$album.data.id}"{if $album.data.id==$form.targetLocation
	 } selected="selected"{/if}>
	  {"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"|repeat:$album.depth}--
	  {$album.data.title|markup:strip|default:$album.data.pathComponent}
	</option>
	{/foreach}
      </select>
    </td>
  </tr><tr>
    <td> {g->text text="Login page"} </td>
    <td> <input type="checkbox" id="cbLoginRedirect" name="{g->formVar var="form[loginRedirect]"}"
	  {if !empty($form.loginRedirect)} checked="checked"{/if}/>
	 <label for="cbLoginRedirect"> {g->text text="Jump to user album after login"} </label>
    </td>
      </tr><tr>
    <td> {g->text text="Link to user album"} </td>
    <td> <input type="checkbox" id="cbHomeLink" name="{g->formVar var="form[homeLink]"}"
	  {if !empty($form.homeLink)} checked="checked"{/if}/>
	 <label for="cbHomeLink"> {g->text text="Show link"} </label>
    </td>
  </tr></table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
