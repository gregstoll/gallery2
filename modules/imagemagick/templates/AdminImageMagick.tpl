{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="ImageMagick Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="ImageMagick is a graphics toolkit that can be used to process images that you upload to Gallery.  You must install the ImageMagick binaries on your server, then enter the path to them in the text box below.  If you're on a Unix machine, don't forget to make the binaries executable (<i>chmod 755 *</i> in the ImageMagick directory should do it)"}
  </p>

{if !$AdminImageMagick.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Directory to ImageMagick/GraphicsMagick binaries:"}
    </td><td>
      <input type="text" id='giFormPath' size="40" autocomplete="off"
       name="{g->formVar var="form[path]"}" value="{$form.path}"/>
      {g->autoComplete element="giFormPath"}
	{g->url arg1="view=core.SimpleCallback" arg2="command=lookupDirectories"
		arg3="prefix=__VALUE__" htmlEntities=false}
      {/g->autoComplete}

      {if isset($form.error.path.missing)}
      <div class="giError">
	{g->text text="You must enter a path to your ImageMagick binaries"}
      </div>
      {/if}
      {if isset($form.error.path.bad)}
      <div class="giError">
	{g->text text="The path you entered is not a valid directory or is not accessible."}
      </div>
      {/if}
      {if isset($form.error.path.testError)}
      <div class="giError">
	{g->text text="The path you entered doesn't contain valid ImageMagick binaries. Use the 'test' button to check where the error is."}
      </div>
      {/if}
      {if isset($form.error.path.badPath)}
      <div class="giError">
	{g->text text="The path you entered isn't a valid path."}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>
      {g->text text="JPEG Quality:"}
    </td><td>
      <select name="{g->formVar var="form[jpegQuality]"}">
	{html_options values=$AdminImageMagick.jpegQualityList
	 selected=$form.jpegQuality output=$AdminImageMagick.jpegQualityList}
      </select>
    </td>
  {if $form.cmykSupport!="none"}
  </tr><tr>
    <td colspan="2">
      {g->text text="ImageMagick can detect non-webviewable color spaces like CMYK and create a webviewable copy of such images. Only activate this option if you actually add CMYK based JPEG or TIFF images since the color space detection slows down the add item process a little bit."}
    </td>
  </tr><tr>
    <td>
      {g->text text="CMYK Support:"}
    </td><td>
      <input type="checkbox" {if $form.cmykSupport=="on"}checked="checked" {/if}
       onclick="document.getElementById('cmykSupport').value = this.checked ? 'on' : 'off'"/>
    </td>
  {/if}
  </tr></table>
  <input type="hidden" id="cmykSupport"
   name="{g->formVar var="form[cmykSupport]"}" value="{$form.cmykSupport}"/>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][test]"}" value="{g->text text="Test Settings"}"/>
{/if}
  {if $AdminImageMagick.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

{if !empty($AdminImageMagick.tests)}
<div class="gbBlock">
  <h3> {g->text text="ImageMagick binary test results"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Binary Name"} </th>
    <th> {g->text text="Pass/Fail"} </th>
  </tr>
  {foreach from=$AdminImageMagick.tests item=test}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
	{$test.name}
    </td><td>
	{if ($test.success)}
	  <div class="giSuccess">
	    {g->text text="Passed"}
	  </div>
	{else}
	  <div class="giError">
	    {g->text text="Failed"}
	  </div>
	  {if !empty($test.message)}
	    {g->text text="Error messages:"}
	    <br/>
	    <div class="giError">
	      {foreach from=$test.message item=line}
		<pre>{$line}</pre>
	      {/foreach}
	    </div>
	  {/if}
	{/if}
      </td>
    </tr>
  {/foreach}
  </table>
</div>
{/if}

{if $AdminImageMagick.mimeTypes || !empty($form.error.version.vulnerable)}
<div class="gbBlock">
  <h3> {g->text text="Version"} </h3>
  <p class="giDescription">
    {$AdminImageMagick.version.0} {$AdminImageMagick.version.1}
  </p>
  {if !empty($form.error.version.vulnerable)}
  <p class="giWarning">
    {g->text text="Warning: This version of %s has known vulnerabilities that could be exploited to execute arbitrary commands or cause a denial of service (references: %s1%s, %s2%s, %s3%s, %s4%s). You may wish to upgrade. This determination may be inaccurate for ImageMagick packages in Linux distributions." arg1=$AdminImageMagick.version.0
     arg2="<a href=\"http://nvd.nist.gov/nvd.cfm?cvename=CVE-2007-1797\">" arg3="</a>"
     arg4="<a href=\"http://nvd.nist.gov/nvd.cfm?cvename=CVE-2006-3744\">" arg5="</a>"
     arg6="<a href=\"http://nvd.nist.gov/nvd.cfm?cvename=CVE-2006-3743\">" arg7="</a>"
     arg8="<a href=\"http://nvd.nist.gov/nvd.cfm?cvename=CVE-2005-1739\">" arg9="</a>"}
  </p>
  <input type="checkbox" id="cbForceSave" name="{g->formVar var="form[forceSave]"}"/>
  <label for="cbForceSave">
    {g->text text="Use this version anyway"}
  </label>
  {/if}

  {if $AdminImageMagick.mimeTypes}
  <h4> {g->text text="Supported MIME Types"} </h4>
  <p class="giDescription">
    {g->text text="The ImageMagick module can support files with the following MIME types:"}
  </p>
  <p class="giDescription">
  {foreach from=$AdminImageMagick.mimeTypes item=mimeType}
    {$mimeType}<br />
  {/foreach}
  </p>
  {/if}
</div>
{/if}

{if ($AdminImageMagick.failCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text one="Debug output (%d failed test)" many="Debug output (%d failed tests)"
	     count=$AdminImageMagick.failCount arg1=$AdminImageMagick.failCount}
    <span id="AdminImageMagick_trace-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('AdminImageMagick_debugSnippet', 'AdminImageMagick_trace-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="We gathered this debug output while testing your ImageMagick binaries.  If you read through this carefully you may discover the reason why your ImageMagick binaries failed the tests."}
  </p>
  <pre id="AdminImageMagick_debugSnippet" class="gcBackground1 gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {$AdminImageMagick.debugSnippet}
  </pre>
</div>
{/if}
