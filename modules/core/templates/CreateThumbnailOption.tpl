{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Thumbnails"} </h3>

  <p class="giDescription">
    {g->text text="Gallery can create thumbnails at upload time, or create them the first time you want to see the thumbnail itself.  Either way, it will create the thumbnail once and save it, but if you create them at upload time it makes viewing albums for the first time go faster at the expense of a longer upload time."}
  </p>

  <input type="checkbox" id="CreateThumbnail_cb" checked="checked"
   name="{g->formVar var="form[CreateThumbnailOption][createThumbnail]"}"/>
  <label for="CreateThumbnail_cb">
    {g->text text="Create thumbnails now"}
  </label>
</div>
