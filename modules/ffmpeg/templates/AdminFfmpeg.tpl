{*
 * $Revision: 17614 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="FFMPEG Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
{if isset($status.saved)}
  {g->text text="Settings saved successfully"} <br/>
{/if}
{if isset($status.added)}
  {g->text text="Watermark successfully added to movie thumbnails"}
{/if}
{if isset($status.removed)}
  {g->text text="Watermark successfully removed from movie thumbnails"}
{/if}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="FFMPEG is a graphics toolkit that can be used to process video files that you upload to Gallery.  You must install the FFMPEG binary on your server, then enter the path to it in the text box below.  If you're on a Unix machine, don't forget to make the binary executable (<i>chmod 755 ffmpeg</i> in the right directory should do it)"}
  </p>
{if !$AdminFfmpeg.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  {if $AdminFfmpeg.canWatermark}
    <img src="{g->url href="modules/ffmpeg/images/sample.jpg"}" width="100" height="75" alt=""
     style="float: right"/>
  {/if}

  {g->text text="Path to FFMPEG:"}
  <input type="text" size="40" name="{g->formVar var="form[path]"}" value="{$form.path}"
    id='giFormPath' autocomplete="off"/>
  {g->autoComplete element="giFormPath"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupFiles" arg3="prefix=__VALUE__"
	    htmlEntities=false}
  {/g->autoComplete}

  {if isset($form.error.path.missing)}
  <div class="giError">
    {g->text text="You must enter a path to your FFMPEG binary"}
  </div>
  {/if}
  {if isset($form.error.path.testError)}
  <div class="giError">
    {g->text text="The path you entered doesn't contain a valid FFMPEG binary. Use the 'test' button to check where the error is."}
  </div>
  {/if}
  {if isset($form.error.path.badPath)}
  <div class="giError">
    {g->text text="The path you entered isn't a valid path to a <b>ffmpeg</b> binary."}
  </div>
  {/if}
  {if isset($form.error.path.notExecutable)}
  <div class="giError">
    {g->text text="The <b>ffmpeg</b> binary is not executable.  To fix it, run <b>chmod 755 %s</b> in a shell." arg1=$form.path}
  </div>
  {/if}

  <p class="giDescription">
    {g->text text="This module can automatically watermark the thumbnails for movies to help distinguish them from photos."}
    {if $AdminFfmpeg.canWatermark} {g->text text="See sample at right."} {/if}
  </p>
  {if $AdminFfmpeg.canWatermark}
    <input type="checkbox" id="cbWatermark" {if $form.useWatermark}checked="checked"{/if}
     name="{g->formVar var="form[useWatermark]"}" style="vertical-align: -5px"/>
    <label for="cbWatermark">
      {g->text text="Watermark new movie thumbnails"}
    </label>
    <br/>
    <input type="checkbox" id="cbAddWatermark" name="{g->formVar var="form[addWatermark]"}"
     style="vertical-align: -5px" onclick="document.getElementById('cbRemoveWatermark').checked=0"/>
    {capture name="cbRemoveWatermark"}
    <input type="checkbox" id="cbRemoveWatermark" name="{g->formVar var="form[removeWatermark]"}"
     style="vertical-align: -5px" onclick="document.getElementById('cbAddWatermark').checked=0"/>
    {/capture}
    {g->text text="%sAdd%s or %sRemove%s watermark from all existing movie thumbnails."
     arg1="<label for=\"cbAddWatermark\">" arg2="</label>"
     arg3="`$smarty.capture.cbRemoveWatermark`<label for=\"cbRemoveWatermark\">" arg4="</label>"}
  {else}
    {g->text text="Activate another graphics toolkit with watermark support to make this function available."}
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][test]"}" value="{g->text text="Test Settings"}"/>
{/if}
  {if $AdminFfmpeg.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

{if !empty($AdminFfmpeg.tests)}
<div class="gbBlock">
  <h3> {g->text text="FFMPEG binary test results"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Binary Name"} </th>
    <th> {g->text text="Pass/Fail"} </th>
  </tr>
  {foreach from=$AdminFfmpeg.tests item=test}
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
	  {if ! empty($test.message)}
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
</div>

{if $AdminFfmpeg.mimeTypes || $AdminFfmpeg.mimeTypesEncoder}
<div class="gbBlock">
  <h3> {g->text text="Supported MIME Types"} </h3>

  {if $AdminFfmpeg.mimeTypes}
  <p class="giDescription">
    {g->text text="The FFMPEG module can decode files of the following MIME types"}
  </p>
  <p class="giDescription">
  {foreach from=$AdminFfmpeg.mimeTypes item=mimeType}
    {$mimeType}<br/>
  {/foreach}
  </p>
  {/if}

  {if $AdminFfmpeg.mimeTypesEncoder}
  <p class="giDescription">
    {g->text text="The FFMPEG module can encode files to the following MIME types"}
  </p>
  <p class="giDescription">
  {foreach from=$AdminFfmpeg.mimeTypesEncoder item=mimeType}
    {$mimeType}
    {if isset($AdminFfmpeg.encodeWarnings.$mimeType)}
      <div class="giWarning">
        {if isset($AdminFfmpeg.encodeWarnings.$mimeType.mute)}
          {g->text text="Missing required audio codec, encoded videos will not contain sound."}
        {/if}
      </div>
    {else}
      <br/>
    {/if}
  {/foreach}
  </p>
  {/if}

</div>
{/if}

{if $AdminFfmpeg.version}
<div class="gbBlock">
  <h3> {g->text text="Version Information"} </h3>
  <p class="giDescription">
  {foreach from=$AdminFfmpeg.version item=ver}
    {$ver}<br/>
  {/foreach}
  </p>
</div>
{/if}

{if ($AdminFfmpeg.failCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text one="Debug output (%d failed test)" many="Debug output (%d failed tests)"
	     count=$AdminFfmpeg.failCount arg1=$AdminFfmpeg.failCount}
    <span id="AdminFfmpeg_trace-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('AdminFfmpeg_debugSnippet', 'AdminFfmpeg_trace-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="We gathered this debug output while testing your Ffmpeg installation.  If you read through this carefully you may discover the reason why Ffmpeg failed the tests."}
  </p>
  <pre id="AdminFfmpeg_debugSnippet" class="gcBackground1 gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {$AdminFfmpeg.debugSnippet}
  </pre>
</div>
{/if}
{/if}
