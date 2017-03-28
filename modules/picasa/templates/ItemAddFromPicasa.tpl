{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !$ItemAddFromPicasa.uploadsPermitted}
<div class="gbBlock giError">
  {g->text text="Your webserver is configured to disallow file uploads from your web browser at this time.  Please contact your system administrator for assistance."}
</div>
{else}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Upload archived Picasa export files directly from your computer."}
    {g->text text="Enter the full path to the file and an optional caption in the boxes below."}
  </p>

  <p class="giDescription">
    {if $ItemAddFromPicasa.maxFileSize == 0}
      {g->text text="<b>Note:</b> You can upload up to %s at one time.  If you want to upload more than that, you must upload the files separately, use a different upload format, or ask your system administrator to allow larger uploads." arg1=$ItemAddFromPicasa.totalUploadSize}
    {else}
      {g->text text="<b>Note:</b> You can upload up to %s at one time.  No individual file may be larger than %s. If you want to upload more than that, you must upload the files separately, use a different upload format, or ask your system administrator to allow larger uploads." arg1=$ItemAddFromPicasa.totalUploadSize arg2=$ItemAddFromPicasa.maxFileSize}
    {/if}
  </p>

  <div>
    <h4> {g->text text="File"} </h4>
    <input type="file" size="60" name="{g->formVar var="form[picasaZipPath]"}"/>
  </div>
  {if isset($form.error.picasaZipPath.missing)}
    <div class="giError">
      {g->text text="You did not enter a path."}
    </div>
  {/if}
  {if isset($form.error.picasaZipPath.uploaderror)}
    <div class="giError">
      {g->text text="There was an error during the upload."}
    </div>
  {/if} 
  {if isset($form.error.picasaZipPath.notsupported)}
    <div class="giError">
      {g->text text="The filetype you entered is not supported. Maybe you did not activate the corresponding toolkit. (E.g. Archive Upload for ZIP files)"}
    </div>
  {/if}
  {if isset($form.error.picasaZipPath.invalid)}
    <div class="giError">
      {g->text text="The archive you uploaded is not a valid Picasa export."}
    </div>
  {/if}
</div>

<div class="gbBlock">
  {g->text text="Set item titles from:"}
  <select name="{g->formVar var="form[set][title]"}">
    {html_options options=$ItemAddFromPicasa.titleList selected=$form.set.title}
  </select>
  &nbsp;

  {g->text text="Assign Picasa caption to:"}
  <input type="checkbox" id="cbSummary" {if !empty($form.set.summary)}checked="checked" {/if}
   name="{g->formVar var="form[set][summary]"}"/>
  <label for="cbSummary"> {g->text text="Summary"} </label>
  &nbsp;

  <input type="checkbox" id="cbDesc" {if !empty($form.set.description)}checked="checked" {/if}
   name="{g->formVar var="form[set][description]"}"/>
  <label for="cbDesc"> {g->text text="Description"} </label>
</div>

{* Include our extra ItemAddOptions *}
{foreach from=$ItemAdd.options item=option}
  {include file="gallery:`$option.file`" l10Domain=$option.l10Domain}
{/foreach}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][addFromPicasa]"}" value="{g->text text="Add Items"}"/>
</div>
{/if}
