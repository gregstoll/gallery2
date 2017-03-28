{*
 * $Revision: 17029 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Zip Download Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
{if !$ZipCartAdmin.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  <p class="giDescription">
    {g->text text="Enter full path to the zip executable."}
  </p>

  {g->text text="Zip Path:"}
  <input type="text" id="giFormPath" size="40" autocomplete="off"
   name="{g->formVar var="form[path]"}" value="{$form.path}"/>
  {g->autoComplete element="giFormPath"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupFiles"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  {if isset($form.error.path.missing)}
  <div class="giError">
    {g->text text="You must enter a path to your zip binary"}
  </div>
  {/if}
  {if isset($form.error.path.badPath)}
  <div class="giError">
    {g->text text="The path you entered isn't valid or isn't executable"}
  </div>
  {/if}
  {if isset($form.error.path.badZip)}
  <div class="giError">
    {g->text text="Unable to create a zip file from this binary"}
  </div>
  {/if}
  {if isset($form.test) && $form.test}
  <div class="giSuccess">
    {g->text text="The path you entered is valid"}
  </div>
  {/if}
</div>

{if isset($form.error.path.badZip)}
<div class="gbBlock">
  <h3>
    {g->text text="Debug output"}
    <span id="ZipCartAdmin_trace-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('ZipCartAdmin_debugSnippet', 'ZipCartAdmin_trace-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="We gathered this debug output while testing your zip installation.  If you read through this carefully you may discover the reason why it failed our tests."}
  </p>
  <pre id="ZipCartAdmin_debugSnippet" class="gcBackground1 gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {$ZipCartAdmin.debugSnippet}
  </pre>
</div>
{/if}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][test]"}" value="{g->text text="Test Settings"}"/>
{/if}
  {if $ZipCartAdmin.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>
