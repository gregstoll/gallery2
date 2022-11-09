{*
 * $Revision: 13671 $
 * If you want to customize this file, do not edit it directly since future upgrades
 * may overwrite it.  Instead, copy it into a new directory called "local" and edit that
 * version.  Gallery will look for that file first and use it if it exists.
 *}

{if !$ItemAddBulkExcelUpload.uploadsPermitted}
<div class="gbBlock giError">
  {g->text text="Your webserver is configured to disallow file uploads from your web browser at this time.  Please contact your system administrator for assistance."}
</div>
{else}

{if !empty($form.error)}
<div class="gbBlock giError">
  <h2>
    {g->text text="There was a problem processing your request, see below for details."}
  </h2>
  <div class="giWarning">
    {foreach from=$ItemAddBulkExcelUpload.status item=statusEntry}
      {$statusEntry.warnings.0}<br/>
    {/foreach}
  </div>
</div>
{/if}

{if !empty($form.error.upload)}
  <div class="gbBlock giError"><h2>
      {g->text text="There was a problem processing your request, see below for details."}
    </h2>
    <div class="giWarning">
      {foreach from=$ItemAddBulkExcelUpload.status item=statusEntry}
        {$statusEntry.warnings.0}<br/>
      {/foreach}
    </div>
  </div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Upload an XLSX with image descriptions and a ZIP file with the images."}
    <a onclick="document.getElementById('ItemAddBulk_instructions').classList.toggle('hidden')">{g->text text="[help]"}</a>
    <p>
    {if $ItemAddBulkExcelUpload.maxFileSize == 0}
      {g->text text="<b>Note:</b> You can upload up to %s at one time.  If you want to upload more than that, you must upload the files separately, use a different upload format, or ask your system administrator to allow larger uploads." arg1=$ItemAddBulkExcelUpload.totalUploadSize}
    {else}
      {g->text text="<b>Note:</b> You can upload up to %s at one time.  No individual file may be larger than %s. If you want to upload more than that, you must upload the files separately, use a different upload format, or ask your system administrator to allow larger uploads." arg1=$ItemAddBulkExcelUpload.totalUploadSize arg2=$ItemAddBulkExcelUpload.maxFileSize}
    {/if}
    </p>
  </p>
  <div id="ItemAddBulk_instructions" class="giDescription hidden">
    {capture assign=sampleDataFile}<a href="{g->url href="modules/bulkexcelupload/data/sample.zip"}">{/capture}
    {capture assign=sampleExcelSpreadsheet}<a href="{g->url href="modules/bulkexcelupload/data/sample.xlsx"}">{/capture}
    <p>{g->text text="Create an XSLX file containing the Number, Reference, Term, Media, Description, Place, Author and Date for each image.  Then create a ZIP with the image files, and enter the path to the XLSX and ZIP files in the box below.  For convenience, you can author the data file in Excel and then save it in the XLSX format.  Images you want to add must be on the ZIP file in the same order as they are on the XLSX file.  Here is a %ssample data file%s and a %ssample excel spreadsheet%s."
      arg1=$sampleDataFile arg2="</a>" arg3=$sampleExcelSpreadsheet arg4="</a>"}
    </p>
  </div>

  <div class="form-group">
    <label for="giExcelPath" class="control-label col-xs-2"><h4> {g->text text="Excel File"} </h4></label>
    <div class="col-xs-10">
      <input class="form-control" id='giExcelPath' type="file" size="120" name="{g->formVar var="form[excelPath]"}"/>
    </div>
    {if isset($form.error.excelPath.invalid)}
      <div class="giError">
        {g->text text="Invalid path for Excel file."}
      </div>
    {/if}
    {if isset($form.error.excelPath.missing)}
      <div class="giError">
        {g->text text="Missing path for Excel file."}
      </div>
    {/if}
  </div>
  <div class="form-group">
    <div class="col-xs-offset-2 col-xs-10 checkbox">
      <label for="checkbox_hasHeader">
        <input id="checkbox_hasHeader" type="checkbox" {if $form.hasHeader=="on"}checked="checked" {/if}
               onclick="document.getElementById('hasHeader').value = this.checked ? 'on' : 'off'"/>
        {g->text text="Read header"}
      </label>

    </div>
    <input type="hidden" id="hasHeader"
           name="{g->formVar var="form[hasHeader]"}" value="{$form.hasHeader}"/>
  </div>

  <div class="form-group">
  <label class="control-label col-xs-2" for="giZipPath"><h4> {g->text text="Zip File"} </h4></label>
    <div class="col-xs-10">
  <input class="form-control" id='giZipPath' type="file" size="120" name="{g->formVar var="form[zipPath]"}"/>
    </div>
  {if isset($form.error.zipPath.invalid)}
    <div class="giError">
    {g->text text="Invalid path for ZIP file."}
    </div>
  {/if}
  {if isset($form.error.zipPath.missing)}
    <div class="giError">
    {g->text text="Missing path for ZIP file."}
    </div>
  {/if}
  </div>
</div>

{* Include our extra ItemAddOptions *}
{foreach from=$ItemAdd.options item=option}
  {include file="gallery:`$option.file`" l10Domain=$option.l10Domain}
{/foreach}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add Items"}"/>
</div>

{/if}