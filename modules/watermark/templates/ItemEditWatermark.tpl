{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
  // <![CDATA[
  var watermarkUrlMap = new Array;
  {foreach from=$ItemEditWatermark.watermarks item=watermark}
  watermarkUrlMap[{$watermark.id}] = {ldelim}
  'url': '{g->url htmlEntities=false arg1="view=core.DownloadItem" arg2="itemId=`$watermark.id`"}',
  'width': {$watermark.width},
  'height': {$watermark.height},
  'xPercent': {if $watermark.id==$form.watermarkId}{$form.xPercent}{else}{$watermark.xPercentage}{/if},
  'yPercent': {if $watermark.id==$form.watermarkId}{$form.yPercent}{else}{$watermark.yPercentage}{/if}
  {rdelim};
  {/foreach}

  {literal}
  function chooseWatermark(id) {
    var newImage = watermarkUrlMap[id];
    document.getElementById('watermark_floater').src = newImage.url;
    initWatermarkFloater('watermark_floater', 'watermark_original',
	newImage.width, newImage.height, newImage.xPercent, newImage.yPercent);
  }
  {/literal}
  // ]]>
</script>

<div class="gbBlock">
  <p class="giDescription">
  {if $ItemEditWatermark.isAlbum}
    {g->text text="You can choose a watermark to apply to all images in this album.  Settings here will replace any existing watermarks."}
  {else}
    {g->text text="You can choose a watermark to apply to this image."}
  {/if}
  {g->text text="Watermarks do not modify the original images, so they can be applied to full size, resizes or thumbnail without affecting the original, and can be removed later."}
  </p>
</div>

{if empty($ItemEditWatermark.watermarks)}
<div class="gbBlock">
  <h3> {g->text text="You have no watermarks"} </h3>

  <p class="giDescription">
    {g->text text="You must first upload some watermark images so that you can apply them to your image."}
    <a href="{g->url arg1="view=core.UserAdmin" arg2="subView=watermark.UserWatermarks"}">
      {g->text text="Upload some watermarks now."}
    </a>
  </p>
</div>
{else}
<div class="gbBlock">
  <h3> {g->text text="Step 1.  Choose which watermark you want to use"} </h3>

  <select name="{g->formVar var="form[watermarkId]"}" onchange="chooseWatermark(this.value)"
   id="watermarkList">
    {foreach from=$ItemEditWatermark.watermarks item=watermark}
    <option value="{$watermark.id}"{if
     ($form.watermarkId == $watermark.id)} selected="selected"{/if}>{$watermark.name}</option>
    {/foreach}
  </select>
</div>

<div class="gbBlock">
  <h3> {g->text text="Step 2.  Placement for the watermark"} </h3>

  {include file="gallery:modules/core/templates/JavaScriptWarning.tpl" l10Domain="modules_core"}

  {if isset($ItemEditWatermark.image)}
    {g->image id="watermark_original" maxSize=400 style="display: block"
	      item=$ItemEditWatermark.item image=$ItemEditWatermark.image forceRawImage=true}
  {else}
    <div id="watermark_original" class="gcBackground1"
     style="width: 400px; height: 300px; border-width: 1px; margin: 5px 0 10px 5px"></div>
  {/if}
  <img id="watermark_floater"
   src="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$form.watermarkId`"}"
   width="{$ItemEditWatermark.watermarks[$form.watermarkId].width}"
   height="{$ItemEditWatermark.watermarks[$form.watermarkId].height}"
   alt="{g->text text="watermark"}" style="position: absolute"/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Step 3.  Choose which images to watermark"} </h3>

  {if isset($form.error.versions.missing)}
  <div class="giError">
    {g->text text="You must choose something to watermark!"}
  </div>
  {/if}

  <input type="checkbox" {if isset($form.whichDerivative.preferred)}checked="checked" {/if}
   name="{g->formVar var="form[whichDerivative][preferred]"}" id="cbFullSize"/>
  <label for="cbFullSize">
    {g->text text="Full size (won't damage the original file)"}
  </label>
  <br/>

  <input type="checkbox" {if isset($form.whichDerivative.resize)}checked="checked" {/if}
   name="{g->formVar var="form[whichDerivative][resize]"}" id="cbResizes"/>
  <label for="cbResizes">
    {g->text text="Resizes"}
  </label>
  <br/>

  <input type="checkbox" {if isset($form.whichDerivative.thumbnail)}checked="checked" {/if}
   name="{g->formVar var="form[whichDerivative][thumbnail]"}" id="cbThumbnail"/>
  <label for="cbThumbnail">
    {g->text text="Thumbnail"}
  </label>
  <br/>
</div>

{if $ItemEditWatermark.isAlbum}
<div class="gbBlock">
  <h3> {g->text text="Step 4.  Subalbums"} </h3>

  <input type="checkbox" {if isset($form.recursive)}checked="checked" {/if}
   name="{g->formVar var="form[recursive]"}" id="cbRecursive"/>
  <label for="cbRecursive">
    {g->text text="Also apply watermark change to all subalbums"}
  </label>
</div>
{/if}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   onclick="calculatePercentages('watermark_floater'); return true"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Apply Watermark"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][remove]"}" value="{g->text text="Remove Watermark"}"/>
</div>

<input type="hidden" id="xPercent"
 name="{g->formVar var="form[xPercent]"}" value="{$form.xPercent}"/>
<input type="hidden" id="yPercent"
 name="{g->formVar var="form[yPercent]"}" value="{$form.yPercent}"/>

{g->addToTrailer}
<script type="text/javascript">
// <![CDATA[
chooseWatermark(document.getElementById("watermarkList").value);
// ]]>
</script>
{/g->addToTrailer}
{/if}
