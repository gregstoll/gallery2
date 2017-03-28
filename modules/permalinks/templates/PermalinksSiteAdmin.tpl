{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
  <h3> {g->text text="Permalinks"} </h3>
</div>

{if isset($status.PermalinksSiteAdmin.success)}
<div class="gbBlock">
<h2 class="giSuccess">{$status.PermalinksSiteAdmin.success}</h2>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Auto-permalink"} </h3>
  <p class="giDescription">
    {g->text text="If you use this feature, any time a new album is created, a permalink to it with the name of the album will be created, unless a permalink by the same name already exists. You will be able to remove the permalink if you wish."}
  </p>

  <p><input type="checkbox" name="{g->formVar var="form[PermalinksSiteAdmin][autoPermalink]"}"
    id="Permalinks_autoPermalink" {if $PermalinksSiteAdmin.autoPermalink } checked=true {/if}/>
    <label for="Permalinks_autoPermalink"> {g->text text="Enable auto-permalink"} </label></p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Global list of permalinks"} </h3>
  {if empty($PermalinksSiteAdmin.aliases)}
    <p> {g->text text="You have no permalinks"} </p>
  {else}

    <table class="gbDataTable">
    <tr>
      <th> {g->text text="Delete"} </th>
      <th> {g->text text="Permalink name"} </th>
    </tr>
    {foreach from=$PermalinksSiteAdmin.aliases item=name}
	  <tr class="{cycle values="gbEven,gbOdd"}">
	    <td><input type="checkbox" name="{g->formVar var="form[PermalinksSiteAdmin][delete][`$name`]"}"></td>
	    <td>{$name}</td>
	  </tr>
    {/foreach}
	</table>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][submit]"}" value="{g->text text="Save"}" />
</div>
