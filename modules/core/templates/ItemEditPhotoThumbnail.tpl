{*
 * $Revision: 17178 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <h3> {g->text text="Thumbnail"} </h3>

  <p class="giDescription">
    {g->text text="You can select which part of the photo will be used for the thumbnail.  This will have no effect on the resized or original versions of the image."}
  </p>

  {include file="gallery:modules/core/templates/JavaScriptWarning.tpl"}

  {if $ItemEditPhotoThumbnail.editThumbnail.can.crop}
  <div id="crop-canvas" style="width: 630px; height: 480px">
    <img id="crop-image" style="z-index: 1"/>
    <div id="crop-frame" style="z-index: 10">
      <div id="crop-marquee-top" class="horizontal-marquee"></div>
      <div id="crop-marquee-right" class="vertical-marquee"></div>
      <div id="crop-marquee-bottom" class="horizontal-marquee"></div>
      <div id="crop-marquee-left" class="vertical-marquee"></div>
      <div id="crop-handle"></div>
    </div>
  </div>

  {* 
   * A bug in YUI prevents us from using utilities.js in this case
   * https://sourceforge.net/tracker/?func=detail&atid=836476&aid=1814066&group_id=165715
   * After YUI upgrade consider moving this back to .inc and replacing
   * yahoo-dom-event.js and dragdrop-min.js with a single utilities.js file
   *}
  <script type="text/javascript" src="{g->url href="lib/yui/yahoo-dom-event.js"}"></script>
  <script type="text/javascript" src="{g->url href="lib/yui/dragdrop-min.js"}"></script>
  <script type="text/javascript" src="{g->url href="lib/javascript/Cropper.js"}"></script>
  <script type="text/javascript">
    // <![CDATA[
    var cropper;
    initCropper = function() {ldelim}
      cropper = new Cropper(
	  new CropImage("crop-canvas", "crop-image",
			"{$ItemEditPhotoThumbnail.editThumbnail.imageUrl}",
			{$ItemEditPhotoThumbnail.editThumbnail.imageWidth},
			{$ItemEditPhotoThumbnail.editThumbnail.imageHeight}),
	  new CropFrame("crop-image", "crop-frame"),
	  new CropHandle("crop-image", "crop-frame", "crop-handle"));

      cropper.setOrientation('{$ItemEditPhotoThumbnail.editThumbnail.selectedOrientation}');
      cropper.setFrameDimensions(
	    {$ItemEditPhotoThumbnail.editThumbnail.cropTop},
	    {$ItemEditPhotoThumbnail.editThumbnail.cropLeft} + {$ItemEditPhotoThumbnail.editThumbnail.cropWidth},
	    {$ItemEditPhotoThumbnail.editThumbnail.cropTop} + {$ItemEditPhotoThumbnail.editThumbnail.cropHeight},
	    {$ItemEditPhotoThumbnail.editThumbnail.cropLeft});

      if (document.getElementById("gallery").className == "opera") {ldelim}
	// Opera < 9.0 doesn't support opacity
	document.getElementById("crop-frame").style.background = "transparent";
      {rdelim}
    {rdelim}
    YAHOO.util.Event.addListener(window, "load", initCropper);

    function setAspectRatio(value) {ldelim}
      switch(value) {ldelim}
      {foreach from=$ItemEditPhotoThumbnail.editThumbnail.aspectRatioList key=index item=aspectRatio}
	case "{$index}":
	  cropper.setAspectRatio({$aspectRatio.width}, {$aspectRatio.height});
	  break;
      {/foreach}
      {rdelim}
    {rdelim}

    function setCropFields() {ldelim}
      var frm = document.getElementById('itemAdminForm');
      var region = cropper.getFrameDimensions();
      frm.crop_x.value = region.left;
      frm.crop_y.value = region.top;
      frm.crop_width.value = region.right - region.left;
      frm.crop_height.value = region.bottom - region.top;
    {rdelim}
    // ]]>
  </script>

  <h2> {g->text text="Aspect Ratio: "} </h2>

  <select onchange="setAspectRatio(this.value)">
    {foreach from=$ItemEditPhotoThumbnail.editThumbnail.aspectRatioList key=index item=aspect}
      <option label="{$aspect.label}" value="{$index}"
	{if $ItemEditPhotoThumbnail.editThumbnail.selectedAspect == $index}
	  selected="selected"
	{/if}
      > {$aspect.label} </option>
    {/foreach}
  </select>

  <select onchange="cropper.setOrientation(this.value)">
    {html_options options=$ItemEditPhotoThumbnail.editThumbnail.orientationList
		  selected=$ItemEditPhotoThumbnail.editThumbnail.selectedOrientation}
  </select>

  <input type="hidden" id="crop_x" name="{g->formVar var="form[crop][x]"}"/>
  <input type="hidden" id="crop_y" name="{g->formVar var="form[crop][y]"}"/>
  <input type="hidden" id="crop_width" name="{g->formVar var="form[crop][width]"}"/>
  <input type="hidden" id="crop_height" name="{g->formVar var="form[crop][height]"}"/>

  <input type="submit" class="inputTypeSubmit" onclick="setCropFields(); return true"
   name="{g->formVar var="form[action][crop]"}" value="{g->text text="Crop"}"/>
  <input type="button" class="inputTypeSubmit" onclick="cropper.resetFrame()"
   value="{g->text text="Undo Changes"}"/>
  <input type="submit" class="inputTypeSubmit" onclick="setCropFields(); return true"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset to default"}"/>

  {else}
  <b>
    {g->text text="There are no graphics toolkits enabled that support this type of photo, so we cannot crop the thumbnail."}
    {if $ItemEditPhotoThumbnail.isAdmin}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}">
	{g->text text="site admin"}
      </a>
    {/if}
  </b>
  {/if}
</div>
