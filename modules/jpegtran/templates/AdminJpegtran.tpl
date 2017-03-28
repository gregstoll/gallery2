{*
 * $Revision: 17029 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Jpegtran Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Jpegtran is a tool that can be used to rotate and crop JPEG images without degrading image quality.  You must install the jpegtran binary (part of libjpeg) on your server, then enter the path to it in the text box below.  If you're on a Unix machine, don't forget to make the binary executable (%s should do it)."
     arg1="<i>chmod 755 jpegtran</i>"}
  </p>

  <p class="giDescription">
    {g->text text="<b>Note</b>: The jpegtran binary must be compatible with version %s." arg1="6b"}
  </p>

{if !$AdminJpegtran.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  <table class="gbDataTable">
    <tr>
      <td>
        {g->text text="Path to jpegtran binary:"}
      </td>
      <td>
        {if isset($form.error.path.missing)}
        <div class="giError">
          {g->text text="You must enter a path to your jpegtran binary"}
        </div>
        {/if}
        {if isset($form.error.path.testError)}
        <div class="giError">
          {g->text text="The path you entered doesn't contain a valid jpegtran binary. Use the 'test' button to check where the error is."}
        </div>
        {/if}
        {if isset($form.error.path.badPath)}
        <div class="giError">
          {g->text text="The path you entered isn't a valid path to a <b>jpegtran</b> binary."}
        </div>
        {/if}
        {if isset($form.error.path.notExecutable)}
        <div class="giError">
          {g->text text="The <b>jpegtran</b> binary is not executable.  To fix it, run <b>chmod 755 %s</b> in a shell." arg1=$form.path}
        </div>
        {/if}

        <input type="text" id="giFormPath" size="40" autocomplete="off"
               name="{g->formVar var="form[path]"}" value="{$form.path}"/>
        {g->autoComplete element="giFormPath"}
	    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupFiles"
                arg3="prefix=__VALUE__" htmlEntities=false}
        {/g->autoComplete}
      </td>
    </tr>
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][test]"}" value="{g->text text="Test Settings"}"/>
{/if}
  {if $AdminJpegtran.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

{if !empty($AdminJpegtran.tests)}
<div class="gbBlock">
  <h3> {g->text text="Jpegtran binary test results"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Binary Name"} </th>
    <th> {g->text text="Pass/Fail"} </th>
  </tr>
  {foreach from=$AdminJpegtran.tests item=test}
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

{if ($AdminJpegtran.failCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text one="Debug output (%d failed test)" many="Debug output (%d failed tests)"
	     count=$AdminJpegtran.failCount arg1=$AdminJpegtran.failCount}
    <span id="AdminJpegtran_trace-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('AdminJpegtran_debugSnippet', 'AdminJpegtran_trace-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="We gathered this debug output while testing your Jpegtran binaries.  If you read through this carefully you may discover the reason why your jpegtran binaries failed the tests."}
  </p>
  <pre id="AdminJpegtran_debugSnippet" class="gcBackground1 gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {$AdminJpegtran.debugSnippet}
  </pre>
</div>
{/if}
{/if}
