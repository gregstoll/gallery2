{*
 * $Revision: 17029 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Dcraw Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Dcraw is a graphics toolkit that can be used to process RAW images produced by high end digital cameras.   You must install the Dcraw binary on your server, then enter the path to it in the text box below.  If you're on a Unix machine, don't forget to make the binary executable (<i>chmod 755 dcraw</i> should do it)."}
  </p>

  <p class="giDescription">
    {g->text text="<b>Note</b>: Gallery supports Dcraw v5.40 and more recent."}
  </p>

{if !$AdminDcraw.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  <table class="gbDataTable">
    <tr>
      <td>
	{g->text text="Path to dcraw binary:"}
      </td>
      <td>
	<input type="text" id="giFormPath" size="40" autocomplete="off"
	 name="{g->formVar var="form[path]"}" value="{$form.path}"/>
	{g->autoComplete element="giFormPath"}
	  {g->url arg1="view=core.SimpleCallback" arg2="command=lookupFiles"
		  arg3="prefix=__VALUE__" htmlEntities=false}
	{/g->autoComplete}

	{if isset($form.error.path.missing)}
	<div class="giError">
	  {g->text text="You must enter the path to your Dcraw binary."}
	</div>
	{/if}
	{if isset($form.error.path.bad)}
	<div class="giError">
	  {g->text text="The path you entered is not a valid Dcraw binary or is not accessible."}
	</div>
	{/if}
	{if isset($form.error.path.testError)}
	<div class="giError">
	  {g->text text="The path you entered isn't a valid Dcraw binary.  Use the 'test' button to check where the error is."}
	</div>
	{/if}
	{if isset($form.error.path.badPath)}
	<div class="giError">
	  {g->text text="The path you entered isn't valid."}
	</div>
	{/if}
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
  {if $AdminDcraw.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

{if !empty($AdminDcraw.tests)}
<div class="gbBlock">
  <h3> {g->text text="Dcraw binary test results"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Binary Name"} </th>
    <th> {g->text text="Pass/Fail"} </th>
  </tr>
  {foreach from=$AdminDcraw.tests item=test}
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

{if ($AdminDcraw.failCount > 0)}
<div class="gbBlock">
  <h3>
    {g->text one="Debug output (%d failed test)" many="Debug output (%d failed tests)"
	     count=$AdminDcraw.failCount arg1=$AdminDcraw.failCount}
    <span id="AdminDcraw_trace-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('AdminDcraw_debugSnippet', 'AdminDcraw_trace-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="We gathered this debug output while testing your Dcraw binaries.  If you read through this carefully you may discover the reason why your Dcraw binaries failed the tests."}
  </p>
  <pre id="AdminDcraw_debugSnippet" class="gcBackground1 gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {$AdminDcraw.debugSnippet}
  </pre>
</div>
{/if}
{/if}
