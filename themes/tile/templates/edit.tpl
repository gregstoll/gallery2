{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<p class="giDescription" style="margin-top: 1em">
  {g->text text="A tile display consists of a background image shown as a grid of tiles with thumbnails for other images placed in any tile position over the background.  Set the size and number of tiles, select the background image and assign thumbnail positions below.  Row 1, Column 1 is the upper left corner."}
</p>

<table class="gbDataTable"><tr>
  <td>
    {g->text text="Rows"}
  </td><td>
    <input type="text" size="4"
     name="{g->formVar var="form[rows]"}" value="{$theme.param.rows}"/>
  </td>
</tr><tr>
  <td>
    {g->text text="Columns"}
  </td><td>
    <input type="text" size="4"
     name="{g->formVar var="form[cols]"}" value="{$theme.param.cols}"/>
  </td>
</tr><tr>
  <td>
    {g->text text="Cell Width"}
  </td><td>
    <input type="text" size="4"
     name="{g->formVar var="form[cellWidth]"}" value="{$theme.param.cellWidth}"/>
  </td>
</tr><tr>
  <td>
    {g->text text="Cell Height"}
  </td><td>
    <input type="text" size="4"
     name="{g->formVar var="form[cellHeight]"}" value="{$theme.param.cellHeight}"/>
  </td>
</tr></table>

<table class="gbDataTable" style="margin-top: 1em"><tr>
  <th colspan="2" style="text-align:right"> {g->text text="Background"} </th>
  <th> {g->text text="Title"} </th>
  <th> {g->text text="Row"} </th>
  <th> {g->text text="Column"} </th>
</tr>
{foreach from=$theme.children key=i item=it}
{if isset($it.image) || isset($it.thumbnail)}
  <tr><td>
    {if isset($it.thumbnail)}
      {g->image item=$it image=$it.thumbnail maxSize=100 class="giThumbnail"}
    {else}
      {g->text text="no thumbnail"}
    {/if}
  </td><td>
  {if isset($it.image)}
    <input type="radio" {if $theme.param.backgroundId==$it.image.id}checked="checked" {/if}
     name="{g->formVar var="form[backgroundId]"}" value="{$it.image.id}"/>
  {/if}
  </td><td>
    <span class="giTitle">{$it.title|markup}</span>
  </td><td>
    {assign var="key" value="row_`$it.id`"}
    <input type="text" size="3"
     name="{g->formVar var="form[row_`$it.id`]"}" value="{$theme.param[$key]|default:''}"/>
  </td><td>
    {assign var="key" value="col_`$it.id`"}
    <input type="text" size="3"
     name="{g->formVar var="form[col_`$it.id`]"}" value="{$theme.param[$key]|default:''}"/>
  </td></tr>
  {/if}
{foreachelse}
  <tr><td colspan="5"><h4 class="giWarning">
    {g->text text="Add some photos!"}
  </h4></td></tr>
{/foreach}
</table>
