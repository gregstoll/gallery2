{*
 * $Revision: 16321 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Square Thumbnails"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <table class="gbDataTable"><tr>
    <td>
      <input type="radio" id="rbCrop" name="{g->formVar var="form[mode]"}" value="crop"
       {if $form.mode=="crop"}checked="checked"{/if}/>
    </td>
    <td>
      <label for="rbCrop">
	{g->text text="Crop images to square"}
      </label>
    </td>
  </tr><tr>
    <td>
      <input type="radio" id="rbFit" name="{g->formVar var="form[mode]"}" value="fit"
       {if $form.mode=="fit"}checked="checked"{/if}
       {if !$form.allowFitToSquareMode} disabled="disabled"{/if}/>
    </td>
    <td>
      {if !$form.allowFitToSquareMode}
      <div class="giWarning">
	{g->text text="You need to enable a graphics toolkit module with PPM support to use the \"fit images to square\" mode."}
      </div>
      {/if}
      <label for="rbFit">
	{g->text text="Fit images to square, padding with background color:"}
      </label>
      <input type="text" size="6" name="{g->formVar var="form[color]"}" value="{$form.color}"
        {if !$form.allowFitToSquareMode} disabled="disabled"{/if}/>

      {if isset($form.error.color.missing)}
      <div class="giError">
	{g->text text="Color value missing"}
      </div>
      {/if}
      {if isset($form.error.color.invalid)}
      <span class="giError">
	{g->text text="Color value invalid"}
      </span>
      {/if}
    </td>
  </tr></table>

  <p class="giDescription">
    {g->text text="Background color is in RGB hex format; 000000 is black, FFFFFF is white."}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
