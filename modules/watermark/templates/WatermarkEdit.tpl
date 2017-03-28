{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<input type="hidden" name="{g->formVar var="watermarkId"}" value="{$watermark.id}"/>
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Edit A Watermark"} </h2>
</div>

{if !empty($form.error)}
<div class="gbBlock"><h2 class="giError">
  {g->text text="There was a problem processing your request."}
</h2></div>
{/if}

{if isset($status.add)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="New image added successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Watermark Name"} </h3>

  <p class="giDescription">
    {g->text text="Give this watermark a name so that you can identify it in a list."}
  </p>

  <input type="text" size="40"
   name="{g->formVar var="form[watermarkName]"}" value="{$form.watermarkName}"/>

 {if isset($form.error.watermarkName.missing)}
 <div class="giError">
   {g->text text="You must provide a name"}
 </div>
 {/if}
 {if isset($form.error.watermarkName.duplicate)}
 <div class="giError">
   {g->text text="Name already used by another watermark"}
 </div>
 {/if}
</div>

<div class="gbBlock">
  <h3> {g->text text="Placement"} </h3>

  <p class="giDescription">
    {g->text text="Place your watermark on the canvas below in the location where you'd like it to appear when you watermark newly uploaded photos.  You'll be able to edit individual photos to move the watermark later on, if you choose."}
  </p>

  {include file="gallery:modules/core/templates/JavaScriptWarning.tpl" l10Domain="modules_core"}

  <div id="watermark_background" class="gcBackground1"
   style="width: 400px; height: 300px; border-width: 1px; margin: 5px 0 10px 5px">
    <img id="watermark_floater"
     src="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$watermark.id`"}"
     width="{$watermark.width}" height="{$watermark.height}" alt="" style="position: absolute"/>
  </div>

  <input type="hidden" id="xPercentage"
   name="{g->formVar var="form[xPercentage]"}" value="{$form.xPercentage}"/>
  <input type="hidden" id="yPercentage"
   name="{g->formVar var="form[yPercentage]"}" value="{$form.yPercentage}"/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Choose which versions of the image you'd like to watermark"} </h3>

  {if isset($form.error.whichDerivative.missing)}
  <p class="giError">
    {g->text text="You must choose something to watermark!"}
  </p>
  {/if}

  <input type="checkbox" {if isset($form.whichDerivative.preferred)}checked="checked" {/if}
   name="{g->formVar var="form[whichDerivative][preferred]"}"/>
  {g->text text="Full size (won't damage the original file)"}
  <br/>

  <input type="checkbox" {if isset($form.whichDerivative.resizes)}checked="checked" {/if}
   name="{g->formVar var="form[whichDerivative][resizes]"}"/>
  {g->text text="Resizes"}
  <br/>

  <input type="checkbox" {if isset($form.whichDerivative.thumbnail)}checked="checked" {/if}
   name="{g->formVar var="form[whichDerivative][thumbnail]"}"/>
  {g->text text="Thumbnail"}
  <br/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Replace image"} </h3>

  <p class="giDescription">
    {g->text text="Uploading a new image file will reapply this watermark everywhere it is used in the Gallery.  The new image must match the mime type of the existing file."}
  </p>

  <input type="file" size="60" name="{g->formVar var="form[1]"}"/>

  {if isset($form.error.mimeType.mismatch)}
  <p class="giError">
    {g->text text="Mime type does not match existing file"}
  </p>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" onclick="calculatePercentages('watermark_floater'); return true"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>

{g->addToTrailer}
<script type="text/javascript">
// <![CDATA[
initWatermarkFloater("watermark_floater", "watermark_background",
	             {$watermark.width}, {$watermark.height},
	             {$form.xPercentage|default:0}, {$form.yPercentage|default:0});
// ]]>
</script>
{/g->addToTrailer}
