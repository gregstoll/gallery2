{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Random Highlight"} </h3>

  <input type="checkbox" id="RandomHighlight_cb"
   name="{g->formVar var="form[RandomHighlightOption][isRandomHighlight]"}"
   {if $form.RandomHighlightOption.isRandomHighlight} checked="checked"{/if}/>
  <label for="RandomHighlight_cb">
    {g->text text="Activate random highlight for this album"}
  </label>
</div>
