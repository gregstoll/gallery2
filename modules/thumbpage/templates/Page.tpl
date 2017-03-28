{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Select the page number to use for this item's thumbnail."}
  </p>

  <label for="page">
    {g->text text="Page"}
  </label>
  <select id="page" name="{g->formVar var="form[page]"}">
    {html_options options=$ItemEditThumbPage.pageList selected=$form.page}
  </select>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
