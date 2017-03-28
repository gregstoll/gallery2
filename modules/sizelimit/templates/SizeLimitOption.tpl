{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
  // <![CDATA[
  function SetSizeLimitOption_toggleXY(chk) {ldelim}
    var frm = document.getElementById('itemAdminForm');
    frm.elements["{g->formVar var="form[SizeLimitOption][dimensions][width]"}"].disabled = chk;
    frm.elements["{g->formVar var="form[SizeLimitOption][dimensions][height]"}"].disabled = chk;
    frm.elements["{g->formVar var="form[SizeLimitOption][keepOriginal]"}"].disabled =
      chk && frm.elements["{g->formVar var="form[SizeLimitOption][sizeChoice]"}"][0].checked;
  {rdelim}
  function SetSizeLimitOption_toggleSize(chk) {ldelim}
    var frm = document.getElementById('itemAdminForm');
    frm.elements["{g->formVar var="form[SizeLimitOption][filesize]"}"].disabled = chk;
    frm.elements["{g->formVar var="form[SizeLimitOption][keepOriginal]"}"].disabled =
      chk && frm.elements["{g->formVar var="form[SizeLimitOption][dimensionChoice]"}"][0].checked;
  {rdelim}
  // ]]>
</script>

<div class="gbBlock">
  <h3> {g->text text="Define picture size limit"} </h3>

  <div style="margin: 0.5em 0">
    <div style="font-weight: bold">
      {g->text text="Maximum dimensions of full sized images"}
    </div>
    <input type="radio" id="SizeLimit_DimNone" onclick="SetSizeLimitOption_toggleXY(1)"
	   name="{g->formVar var="form[SizeLimitOption][dimensionChoice]"}" value="unlimited"
     {if $SizeLimitOption.dimensionChoice == "unlimited"}checked="checked"{/if}/>
    <label for="SizeLimit_DimNone">
      {g->text text="No Limits"}
    </label>
    <br/>
    <input type="radio" onclick="SetSizeLimitOption_toggleXY(0)"
	   name="{g->formVar var="form[SizeLimitOption][dimensionChoice]"}" value="explicit"
     {if $SizeLimitOption.dimensionChoice == "explicit"}checked="checked"{/if}/>
    {g->dimensions formVar="form[SizeLimitOption][dimensions]"
		   width=$SizeLimitOption.width height=$SizeLimitOption.height}

    {if $SizeLimitOption.dimensionChoice == "unlimited"}
    <script type="text/javascript">
      var frm = document.getElementById('itemAdminForm');
      frm.elements["{g->formVar var="form[SizeLimitOption][dimensions][width]"}"].disabled = true;
      frm.elements["{g->formVar var="form[SizeLimitOption][dimensions][height]"}"].disabled = true;
    </script>
    {/if}

    {if !empty($form.error.SizeLimitOption.dim.missing)}
    <div class="giError">
      {g->text text="You must specify at least one of the dimensions"}
    </div>
    {/if}
  </div>

  <div style="margin: 0.5em 0">
    <div style="font-weight: bold">
      {g->text text="Maximum file size of full sized images in kilobytes"}
    </div>
    <input type="radio" id="SizeLimit_SizeNone" onclick="SetSizeLimitOption_toggleSize(1)"
	   name="{g->formVar var="form[SizeLimitOption][sizeChoice]"}" value="unlimited"
     {if $SizeLimitOption.sizeChoice == "unlimited"}checked="checked"{/if}/>
    <label for="SizeLimit_SizeNone">
      {g->text text="No Limits"}
    </label>
    <br/>
    <input type="radio" onclick="SetSizeLimitOption_toggleSize(0)"
	   name="{g->formVar var="form[SizeLimitOption][sizeChoice]"}" value="explicit"
     {if $SizeLimitOption.sizeChoice == "explicit"}checked="checked"{/if}/>
    <input type="text" size="7" maxlength="6"
	   name="{g->formVar var="form[SizeLimitOption][filesize]"}"
	   value="{$SizeLimitOption.filesize}"
     {if $SizeLimitOption.sizeChoice != "explicit"}disabled="disabled"{/if}/>

    {if !empty($form.error.SizeLimitOption.filesize.invalid)}
    <div class="giError">
      {g->text text="You must enter a number (greater than zero)"}
    </div>
    {/if}
  </div>

  <input type="checkbox" id="SizeLimit_KeepOriginal"
	 name="{g->formVar var="form[SizeLimitOption][keepOriginal]"}"
   {if $SizeLimitOption.keepOriginal} checked="checked" {/if}{if
    $SizeLimitOption.dimensionChoice == "unlimited" && $SizeLimitOption.sizeChoice == "unlimited"}
    disabled="disabled"{/if}/>
  <label for="SizeLimit_KeepOriginal">
    {g->text text="Keep original image?"}
  </label>
  <br/>
  <input type="checkbox" id="SizeLimit_ApplyToDescendents"
	 name="{g->formVar var="form[SizeLimitOption][applyToDescendents]"}"/>
  <label for="SizeLimit_ApplyToDescendents">
    {g->text text="Check here to apply size limits to the pictures in this album and all subalbums"}
  </label>
  <blockquote><p>
    {g->text text="Checking this option will rebuild pictures according to appropriate limits"}
  </p></blockquote>
  {g->changeInDescendents module="sizelimit" text="Use these size limits in all subalbums"}
  <blockquote><p>
    {g->text text="Checking this option will set same picture size limits in all subalbums"}
  </p></blockquote>
</div>
