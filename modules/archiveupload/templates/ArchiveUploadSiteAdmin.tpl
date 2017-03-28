{*
 * $Revision: 17029 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Archive Upload Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="This module will enable extraction of individual files from a zip archive to add each item to Gallery.  You must locate or install an unzip binary on your server, then enter the path to it in the text box below.  If you're on a Unix machine, don't forget to make the binary executable (<i>chmod 755 unzip</i> in the right directory should do it)"}
  </p>

{if !$form.canExec}
  <p class="giWarning">
    {g->text text="The exec() function is disabled in your PHP by the <b>disabled_functions</b> parameter in php.ini.  This module cannot be used until that setting is changed."}
  </p>
{else}
  {g->text text="Path to unzip:"}
  <input type="text" size="40"
   name="{g->formVar var="form[unzipPath]"}" value="{$form.unzipPath}"
   id='giFormPath' autocomplete="off"/>
  {g->autoComplete element="giFormPath"}
    {g->url arg1="view=core.SimpleCallback" arg2="command=lookupFiles"
	    arg3="prefix=__VALUE__" htmlEntities=false}
  {/g->autoComplete}

  {if isset($form.error.unzipPath.missing)}
  <div class="giError">
    {g->text text="You must enter a path to your unzip binary"}
  </div>
  {/if}
  {if isset($form.error.unzipPath.exec)}
  <div class="giError">
    {g->text text="The path you entered doesn't contain a valid unzip binary."}
  </div>
  {/if}
  {if isset($form.error.unzipPath.badPath)}
  <div class="giError">
    {g->text text="The path you entered isn't a valid path to an unzip binary."}
  </div>
  {/if}
  {if isset($form.error.unzipPath.notExecutable)}
  <div class="giError">
    {g->text text="The unzip binary is not executable.  To fix it, run <b>chmod 755 %s</b>" arg1=$form.unzipPath}
  </div>
  {/if}
  <br/>
  <input type="checkbox" id="cbRemoveMeta"
   name="{g->formVar var="form[removeMeta]"}"{if $form.removeMeta} checked="checked"{/if}/>
  <label for="cbRemoveMeta">
    {g->text text="Discard archive contents recognized as folder metadata"}
    (Thumbs.db, .DS_Store, .Trashes, __MACOSX)
  </label>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][test]"}" value="{g->text text="Test Settings"}"/>
{/if}
  {if $form.isConfigure}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  {/if}
</div>

{if !empty($ArchiveUploadSiteAdmin.tests)}
<div class="gbBlock">
  <h3> {g->text text="unzip binary test results"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Binary Name"} </th>
    <th> {g->text text="Pass/Fail"} </th>
  </tr>
  {foreach from=$ArchiveUploadSiteAdmin.tests item=test}
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
	    <pre>{$test.message}</pre>
	  {/if}
	{/if}
      </td>
    </tr>
  {/foreach}
  </table>
</div>
{/if}
