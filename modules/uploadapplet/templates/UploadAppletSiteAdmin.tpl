{*
 * $Revision: 17730 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Upload Applet Settings"} </h2>
</div>

<input type="hidden" name="{g->formVar var="form[variable][type]"}" />

<table><tr valign="top"><td>

<div class="gbBlock">
  <h3>{g->text text="Defaults"}</h3>
  <p>{g->text text="These variables provide default values for applets users execute on your site. Users will be able to override these defaults by making changes in the user interface of the applets, or by changing their local defaults file."}</p>
{if empty($form.uploaddefaultVariables)}
  <p>{g->text text="You have no default variables"}</p>
{else}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Variable"} </th>
      <th> {g->text text="Action"} </th>
    </tr>
    {foreach from=$form.uploaddefaultVariables item=variable}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>{$variable}</td>
	<td><a href="{g->url arg1="controller=uploadapplet.UploadAppletSiteAdmin"
	   arg2="form[action][delete]=1"
	   arg3=$variable|regex_replace:"/^(.*?)=.*$/":"form[delete][variable]=\\1"
	   arg4="form[variable][type]=default" arg5="mode=variables"}">
	   {g->text text="Delete"}</a></td>
      </tr>
    {/foreach}
  </table>
{/if}
</div>

<div class="gbBlock">
  <h4> {g->text text="Add a new default variable"} </h4>

  {if isset($form.error.default)}
  <div class="giError">
    {g->text text="You must enter a variable name and value"}
  </div>
  {/if}

  {g->text text="New variable"}<br/>
  <input type="text" name="{g->formVar var="form[default][name]"}" /> =
  <input type="text" name="{g->formVar var="form[default][value]"}" />
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add variable"}"
   onclick="javascript:this.form['{g->formVar var="form[variable][type]"}'].value='default';this.form.submit();" />
</div>

<div class="gbBlock">
  <h3>{g->text text="Overrides"}</h3>
  <p>{g->text text="These variables override any other values for applets users execute on your site. Users will not be able to change these values."}</p>
{if empty($form.uploadoverrideVariables)}
  <p>{g->text text="You have no override variables"}</p>
{else}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Variable"} </th>
      <th> {g->text text="Action"} </th>
    </tr>
    {foreach from=$form.uploadoverrideVariables item=variable}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>{$variable}</td>
	<td><a href="{g->url arg1="controller=uploadapplet.UploadAppletSiteAdmin"
	   arg2="form[action][delete]=1"
	   arg3=$variable|regex_replace:"/^(.*?)=.*$/":"form[delete][variable]=\\1"
	   arg4="form[variable][type]=override" arg5="mode=variables"}">
	   {g->text text="Delete"}</a></td>
      </tr>
    {/foreach}
  </table>
{/if}
</div>

<div class="gbBlock">
  <h4> {g->text text="Add a new override variable"} </h4>

  {if isset($form.error.override)}
  <div class="giError">
    {g->text text="You must enter a variable name and value"}
  </div>
  {/if}

  {g->text text="New variable"}<br/>
  <input type="text" name="{g->formVar var="form[override][name]"}" /> =
  <input type="text" name="{g->formVar var="form[override][value]"}" />
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add variable"}"
   onclick="javascript:this.form['{g->formVar var="form[variable][type]"}'].value='override';this.form.submit();" />
</div>


</td><td>
<div class="gbBlock">
  <h3>{g->text text="Help"}</h3>
  <p>{g->text text="Here is a selection of variables that affect uploads."}</p>
  <table class="gbDataTable">
    <tr><th>{g->text text="variable"}</th><th>{g->text text="values"}</th><th>{g->text text="help"}</th></tr>
    <tr class="gbEven"><td>resizeBeforeUpload</td><td>true/false</td>
      <td>{g->text text="instructs the applet to resize pictures before uploading to the album; by default, resizes to the album's intermediate image size"}</td></tr>
    <tr class="gbOdd"><td>resizeTo1</td><td>600</td>
      <td>{g->text text="dimension the images will be resized to; this overrides album settings"}</td></tr>
    <tr class="gbEven"><td>setCaptionsNone</td><td>true/false</td>
      <td>{g->text text="no automatic captions"}</td></tr>
    <tr class="gbOdd"><td>setCaptionsWithFilenames</td><td>true/false</td>
      <td>{g->text text="use filenames for captions"}</td></tr>
    <tr class="gbEven"><td>captionStripExtension</td><td>true/false</td>
      <td>{g->text text="if using the filename for captions, strip the extension"}</td></tr>
    <tr class="gbOdd"><td>setCaptionsWithMetadataComment</td><td>true/false</td>
      <td>{g->text text="use EXIF extension for caption"}</td></tr>
    <tr class="gbOdd"><td>htmlEscapeCaptions</td><td>true/false</td>
      <td>{g->text text="if false, the upload applet will use UTF-8 to send image meta-data to Gallery, rather than escaping it with HTML entities"}</td></tr>
    <tr class="gbEven"><td>useJavaResize</td><td>true/false</td>
      <td>{g->text text="set to false if you want to avoid losing EXIF data when the image is resized and ImageMagick is not found"}</td></tr>
    <tr class="gbOdd"><td>suppressWarningIM</td><td>true/false</td>
      <td>{g->text text="if true, the applet will not complain if it can't find ImageMagick"}</td></tr>
    <tr class="gbEven"><td>suppressWarningJpegtran</td><td>true/false</td>
      <td>{g->text text="if true, the applet will not complain if it can't find Jpegtran"}</td></tr>
    <tr class="gbOdd"><td>im.jpegQuality</td><td>0-99</td>
      <td>{g->text text="quality of JPEG compression when resizing with ImageMagick"}</td></tr>
  </table>
  <p><a href="https://gallery.svn.sourceforge.net/svnroot/gallery/trunk/gallery_remote/defaults.properties" target="other">
  	{g->text text="Complete list of variables"}</a></p>
</div>
</td></tr></table>
