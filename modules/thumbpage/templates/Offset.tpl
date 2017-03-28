{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Select the time offset to use for this item's thumbnail."}
  </p>

  <input type="text" id="offset" size="8"
   name="{g->formVar var="form[offset]"}" value="{$form.offset}"/>
  <label for="offset">
    {g->text text="Seconds (Max = %s)" arg1=$form.duration}
  </label>

  {if isset($form.error.offset.invalid)}
  <div class="giError">
    {g->text text="Enter a value between 0 and %s" arg1=$form.duration}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
