{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Rotate"} </h3>

  <p class="giDescription">
    {g->text text="You can only rotate the photo in 90 degree increments."}
  </p>

  {if $ItemEditRotateAndScalePhoto.editPhoto.can.rotate}
    <input type="hidden" name="{g->formVar var="mode"}" value="editPhoto"/>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][rotate][counterClockwise]"}"
     value="{g->text text="CC 90&deg;"}"/>
    &nbsp;
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][rotate][flip]"}" value="{g->text text="180&deg;"}"/>
    &nbsp;
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][rotate][clockwise]"}" value="{g->text text="C 90&deg;"}"/>
  {else}
  <b>
    {g->text text="There are no graphics toolkits enabled that support this type of photo, so we cannot rotate it."}
    {if $ItemEditRotateAndScalePhoto.isAdmin}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}">
	{g->text text="site admin"}
      </a>
    {/if}
  </b>
  {/if}
</div>

<div class="gbBlock">
  <h3> {g->text text="Scale"} </h3>

  <p class="giDescription">
    {g->text text="Shrink or enlarge the original photo.  When Gallery scales a photo, it maintains the same aspect ratio (height to width) of the original photo to avoid distortion.  Your photo will be scaled until it fits inside a bounding box with the size you enter here."}
  </p>

  {if $ItemEditRotateAndScalePhoto.editPhoto.can.resize}
    {g->dimensions formVar="form[resize]" width=$form.resize.width height=$form.resize.height}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][resize]"}" value="{g->text text="Scale"}"/>
  {else}
  <b>
    {g->text text="There are no graphics toolkits enabled that support this type of photo, so we cannot scale it."}
    {if $ItemEditRotateAndScalePhoto.isAdmin}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}">
	{g->text text="site admin"}
      </a>
    {/if}
  </b>
  {/if}

  {if !empty($form.error.resize.size.missing)}
  <div class="giError">
    {g->text text="You must enter a size"}
  </div>
  {/if}
  {if !empty($form.error.resize.size.invalid)}
  <div class="giError">
    {g->text text="You must enter a number (greater than zero)"}
  </div>
  {/if}
</div>

{* Include our extra ItemEditOptions *}
{foreach from=$ItemEdit.options item=option}
  {include file="gallery:`$option.file`" l10Domain=$option.l10Domain}
{/foreach}

{if $ItemEditRotateAndScalePhoto.editPhoto.can.rotate
 || $ItemEditRotateAndScalePhoto.editPhoto.can.resize}
<div class="gbBlock">
{if empty($ItemEditRotateAndScalePhoto.editPhoto.hasPreferredSource)}
  <h3> {g->text text="Preserve Original"} </h3>

  <p class="giDescription">
    {g->text text="Gallery does not modify your original photo when rotating and scaling. Instead, it duplicates your photo and works with copies.  This requires a little extra disk space but prevents your original from getting damaged.  Disabling this option will cause any actions (rotating, scaling, etc) to modify the original."}
  </p>

  {if $ItemEditRotateAndScalePhoto.editPhoto.isLinked}
  <b>
    {g->text text="This is a link to another photo, so you cannot change the original"}
  </b>
  {elseif $ItemEditRotateAndScalePhoto.editPhoto.isLinkedTo}
  <b>
    {g->text text="There are links to this photo, so you cannot change the original"}
  </b>
  {elseif $ItemEditRotateAndScalePhoto.editPhoto.noToolkitSupport}
  <b>
    {g->text text="There is no toolkit support to modify the original so operations may only be applied to the copies"}
  </b>
  {else}
    <input type="checkbox" id="cbPreserve" {if $form.preserveOriginal}checked="checked" {/if}
     name="{g->formVar var="form[preserveOriginal]"}"/>
    <label for="cbPreserve">
      {g->text text="Preserve Original Photo"}
    </label>
  {/if}
{else}
  <h3> {g->text text="Modified Photo"} </h3>

  <p class="giDescription">
    {g->text text="You are using a copy of the original photo that has been scaled or rotated.  The original photo is still available, but is no longer being used.  Any changes you make will be applied to the copy instead."}
  </p>

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][revertToOriginal]"}"
   value="{g->text text="Restore original"}"/>
{/if}
</div>
{/if}
