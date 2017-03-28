{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Panorama"} </h3>

  <p class="giDescription">
    {g->text text="Note that panorama view only applies to full size photo, not resizes."}
  </p>

  <input type="checkbox" id="Panorama_cb" {if $form.PanoramaOption.isPanorama}checked="checked" {/if}
   name="{g->formVar var="form[PanoramaOption][isPanorama]"}"/>
  <label for="Panorama_cb">
    {g->text text="Activate panorama viewer applet for this photo"}
  </label>
</div>
