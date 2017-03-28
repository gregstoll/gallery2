{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Delete Watermark"} </h2>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {$ConfirmDelete.item.name}
    <br/>
    {g->image item=$ConfirmDelete.item image=$ConfirmDelete.item maxSize=150}
  </p>
  <p class="giDescription">
    {g->text one="This watermark is used on one item." count=$ConfirmDelete.count
	     many="This watermark is used on %d items." arg1=$ConfirmDelete.count}
    {g->text text="The watermark will be removed from all items if the watermark image is deleted."}
  </p>

  <input type="hidden" name="{g->formVar var="form[watermarkId]"}" value="{$form.watermarkId}"/>
  {if !empty($form.fromAdmin)}
    <input type="hidden" name="{g->formVar var="form[fromAdmin]"}" value="{$form.fromAdmin}"/>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
