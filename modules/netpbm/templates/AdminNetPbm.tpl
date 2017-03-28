{*
 * $Revision: 17029 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="NetPBM Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="NetPBM is a graphics toolkit that can be used to process images that you upload to Gallery.  You must install the NetPBM binaries on your server, then enter the path to them in the text box below.  If you're on a Unix machine, don't forget to make the binaries executable (<i>chmod 755 *</i> in the NetPBM directory should do it)"}
  </p>

{if !$AdminNetPbm.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  <table class="gbDataTable"><tr>
    <td>
      {g->text text="NetPBM Directory:"}
    </td><td>
      <input type="text" size="40" name="{g->formVar var="form[path]"}" value="{$form.path}"
        id='giNetPBMPath' autocomplete="off"/>
      {g->autoComplete element="giNetPBMPath"}
        {g->url arg1="view=core.SimpleCallback" arg2="command=lookupDirectories"
		arg3="prefix=__VALUE__" htmlEntities=false}
      {/g->autoComplete}

      {if isset($form.error.path.missing)}
      <div class="giError">
	{g->text text="You must enter a path to your NetPBM binaries"}
      </div>
      {/if}
      {if isset($form.error.path.testError)}
      <div class="giError">
	{g->text text="The path you entered doesn't contain valid NetPBM binaries. Use the 'test' button to check where the error is."}
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
	{html_options values=$AdminNetPbm.jpegQualityList selected=$form.jpegQuality
	 output=$AdminNetPbm.jpegQualityList}
      </select>
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="NetPBM will lose embedded EXIF data if you modify the original image, unless you have the optional <b>jhead</b> binary installed on your server.  If you have jhead installed, enter the path below.  Without it, NetPBM will still perform all of its normal functions, but you should always use the <i>preserve original image</i> option when rotating and scaling images"}
  </p>

  {g->text text="jhead Directory:"}
  <input type="text" size="40" name="{g->formVar var="form[jheadPath]"}" value="{$form.jheadPath}"
    id='giJheadPath' autocomplete="off"/>
  {g->autoComplete element="giJheadPath"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupDirectories"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  {if isset($form.error.jheadPath.badPath)}
  <div class="giError">
    {g->text text="The path you entered isn't a valid path."}
  </div>
  {/if}
  {if isset($form.error.jheadPath.missing)}
  <div class="giWarning">
    {g->text text="You should enter a path to the optional jhead binary"}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][test]"}" value="{g->text text="Test Settings"}"/>
{/if}
  {if $AdminNetPbm.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

{if !empty($AdminNetPbm.tests)}
<div class="gbBlock">
  <h3> {g->text text="NetPBM binary test results"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Binary Name"} </th>
    <th> {g->text text="Pass/Fail"} </th>
  </tr>
  {foreach from=$AdminNetPbm.tests item=test}
    <tr>
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
	    {foreach from=$test.message item=line}
	      <pre>{$line}</pre>
	    {/foreach}
	  {/if}
	{/if}
      </td>
    </tr>
  {/foreach}
  </table>

  {if ($AdminNetPbm.failCount > 0) && !empty($AdminNetPbm.mimeTypes)}
    <div class="giWarning">
      {g->text text="Although your NetPBM installation is not completely functional, you can still use it for the mime types listed below."}
    </div>
  {/if}
</div>

{if $AdminNetPbm.mimeTypes}
<div class="gbBlock">
  <h3> {g->text text="Supported MIME Types"} </h3>

  <p class="giDescription">
    {g->text text="The NetPBM module can support files with the following MIME types:"}
  </p>
  <p class="giDescription">
  {foreach from=$AdminNetPbm.mimeTypes item=mimeType}
    {$mimeType}<br/>
  {/foreach}
  </p>
</div>
{/if}

{if ($AdminNetPbm.failCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text one="Debug output (%d failed test)" many="Debug output (%d failed tests)"
	     count=$AdminNetPbm.failCount arg1=$AdminNetPbm.failCount}
    <span id="AdminNetPbm_trace-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('AdminNetPbm_debugSnippet', 'AdminNetPbm_trace-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="We gathered this debug output while testing your NetPBM binaries.  If you read through this carefully you may discover the reason why your NetPBM binaries failed the tests."}
  </p>
  <pre id="AdminNetPbm_debugSnippet" class="gcBackground1 gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {$AdminNetPbm.debugSnippet}
  </pre>
</div>
{/if}
{/if}
