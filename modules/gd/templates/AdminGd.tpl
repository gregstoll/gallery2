{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Gd Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Gd is a graphics toolkit that can be used to process images that you upload to Gallery. The GD-library should be compiled in your PHP (--with-gd)."}
  </p>

  {g->text text="JPEG Quality:"}
  <select name="{g->formVar var="form[jpegQuality]"}">
    {html_options values=$AdminGd.jpegQualityList selected=$form.jpegQuality
     output=$AdminGd.jpegQualityList}
  </select>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  {if $AdminGd.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

<div class="gbBlock">
  <h3> {g->text text="GD library version"} </h3>

  {if $AdminGd.gdVersion}
    <table class="gbDataTable"><tr>
      <th> {g->text text="GD version"} </th>
      <th> {g->text text="Required"} </th>
      <th> {g->text text="Pass/fail"} </th>
    </tr>
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$AdminGd.gdVersion}
	{if $AdminGd.isGdBundled}
	  ({g->text text="bundled"})
	{/if}
      </td><td>
	{$AdminGd.minGdVersion}
      </td><td>
	{if ($AdminGd.gdVersionTooOld)}
	  <div class="giError">
	    {g->text text="Failed"}
	  </div>
	  <div class="giError">
	    {g->text text="This GD version is too old and is not supported by this module! Please upgrade your PHP installation to include the latest GD version."}
	  </div>
	{else}
	  <div class="giSuccess">
	    {g->text text="Passed"}
	  </div>
	{/if}
      </td>
    </tr></table>
  {else}
    <p class="giDescription">
      {g->text text="You don't seem to have the GD library available on this PHP installation."}
    </p>
  {/if}
</div>

{if $AdminGd.mimeTypes}
<div class="gbBlock">
  <h3> {g->text text="Supported MIME Types"} </h3>

  <p class="giDescription">
    {g->text text="The Gd module will support files with the following MIME types:"}
  </p>
  <p class="giDescription">
  {foreach from=$AdminGd.mimeTypes item=mimeType}
    {$mimeType}<br/>
  {/foreach}
  </p>
</div>
{/if}
