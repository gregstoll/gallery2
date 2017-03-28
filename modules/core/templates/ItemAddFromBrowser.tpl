{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !$ItemAddFromBrowser.uploadsPermitted}
<div class="gbBlock giError">
  {g->text text="Your webserver is configured to disallow file uploads from your web browser at this time.  Please contact your system administrator for assistance."}
</div>
{else}

{if !empty($form.error.upload)}
<div class="gbBlock giError"><h2>
  {g->text text="There was a problem processing your request, see below for details."}
  </h2>
  <div class="giWarning">
  {foreach from=$ItemAddFromBrowser.status item=statusEntry}
    {$statusEntry.warnings.0}<br/>
  {/foreach}
  </div>
</div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Upload files directly from your computer."}
    {g->text text="Enter the full path to the file and an optional caption in the boxes below."}
  </p>

  <p class="giDescription">
    {if $ItemAddFromBrowser.maxFileSize == 0}
      {g->text text="<b>Note:</b> You can upload up to %s at one time.  If you want to upload more than that, you must upload the files separately, use a different upload format, or ask your system administrator to allow larger uploads." arg1=$ItemAddFromBrowser.totalUploadSize}
    {else}
      {g->text text="<b>Note:</b> You can upload up to %s at one time.  No individual file may be larger than %s. If you want to upload more than that, you must upload the files separately, use a different upload format, or ask your system administrator to allow larger uploads." arg1=$ItemAddFromBrowser.totalUploadSize arg2=$ItemAddFromBrowser.maxFileSize}
    {/if}
  </p>

  {assign var="UPLOAD_BOX_COUNT" value = "20"}
  {assign var="VISIBLE_BOX_COUNT" value = "2"}
  {section name="uploadBoxes" loop=$UPLOAD_BOX_COUNT}
  {assign var=iteration value=$smarty.section.uploadBoxes.iteration}
  <div id="fileDiv_{$iteration}" {if $iteration > $VISIBLE_BOX_COUNT} style="display:none"{/if}>
    <h4> {g->text text="File"} </h4>
    <input type="file" size="60" name="{g->formVar var="form[$iteration]"}"/>

    <h4> {g->text text="Caption"} </h4>
    <textarea rows="2" cols="60" name="{g->formVar var="form[caption][$iteration]"}"></textarea>
  </div>
  {/section}
  {if $UPLOAD_BOX_COUNT > $VISIBLE_BOX_COUNT}
  <script type="text/javascript">
	// <![CDATA[
	document.write('<h4><a id="addOne" href="javascript:addOne()">{g->text text="More Upload Boxes..." forJavascript=true}</a></h4>');
	var fileIndex = {$VISIBLE_BOX_COUNT};
	{literal}
	function addOne() {
		var link = document.getElementById('addOne');
		link.blur();
		document.getElementById('fileDiv_' + ++fileIndex).style.display = 'block';
		if (fileIndex >= {/literal}{$UPLOAD_BOX_COUNT}{literal}) {
			link.style.display = 'none';
		}
	}
      // ]]>
    {/literal}</script>
  {/if}
</div>

<div class="gbBlock">
  {g->text text="Set item titles from:"}
  <select name="{g->formVar var="form[set][title]"}">
    {html_options options=$ItemAddFromBrowser.titleList selected=$form.set.title}
  </select>
  &nbsp;

  {g->text text="Assign caption to:"}
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
   name="{g->formVar var="form[action][addFromBrowser]"}" value="{g->text text="Add Items"}"/>
</div>
{/if}
