{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Add Item Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Enable"} </h3>

  <table class="gbDataTable"><tr>
    <td> {g->text text="Add From Web"} </td>
    <td>
      <select name="{g->formVar var="form[fromweb]"}">
	{html_options options=$ItemAddSiteAdmin.optionList selected=$form.fromweb}
      </select>
    </td>
  </tr><tr>
    <td> {g->text text="Add From Server"} </td>
    <td>
      <select name="{g->formVar var="form[fromserver]"}">
	{html_options options=$ItemAddSiteAdmin.optionList selected=$form.fromserver}
      </select>
    </td>
  </tr></table>

  <h4 class="giWarning"> {g->text text="Security Warning"} </h4>
  <div>
    {g->text text='"Add From Web" can be abused to attack other websites in your name.  For the attacked party it would seem as if you, the administrator of this Gallery, deliberately attacked their website because your Gallery acts on behalf of your users.  Therefore it is recommended to enable "Add From Web" only for trusted users.'}
  </div>
</div>

<div class="gbBlock">
  <h3> {g->text text="Local Server Upload Paths"} </h3>

  <p class="giDescription">
    {g->text text="Specify the legal directories on the local server where a user can store files and then upload them into Gallery using the <i>Upload from Local Server</i> feature.  The paths you enter here and all the files and directories under those paths will be available to any Gallery user who has upload privileges, so you should limit this to directories that won't contain sensitive data (eg. /tmp or /usr/ftp/incoming)"}
  </p>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Path"} </th>
    <th> {g->text text="Action"} </th>
  </tr>

  {foreach from=$ItemAddSiteAdmin.localServerDirList key=index item=dir}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {$dir|escape}
    </td><td>
      <a href="{g->url arg1="controller=itemadd.ItemAddSiteAdmin"
		       arg2="form[action][removeUploadLocalServerDir]=1"
		       arg3="form[uploadLocalServer][selectedDir]=$index"}">
	{g->text text="remove"}
      </a>
    </td>
  </tr>
  {/foreach}

  <tr>
    <td>
      <input type="text" size="40" id="newDir" autocomplete="off"
       name="{g->formVar var="form[uploadLocalServer][newDir]"}"
       value="{$form.uploadLocalServer.newDir}"/>
      {g->autoComplete element="newDir"}
	{g->url arg1="view=core.SimpleCallback" arg2="command=lookupDirectories"
		arg3="prefix=__VALUE__" htmlEntities=false}
      {/g->autoComplete}
    </td><td>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][addUploadLocalServerDir]"}"
       value="{g->text text="Add"}"/>
    </td>
  </tr></table>

  {if isset($form.error.uploadLocalServer.newDir.missing)}
  <div class="giError">
    {g->text text="You must enter a directory to add."}
  </div>
  {/if}

  {if isset($form.error.uploadLocalServer.newDir.restrictedByOpenBaseDir)}
  <div class="giError">
    {capture name="open_basedir"}
      <a href="http://php.net/ini_set">{g->text text="open_basedir documentation"}</a>
    {/capture}
    {g->text text="Your webserver is configured to prevent you from accessing that directory.  Please refer to the %s and consult your webserver administrator." arg1=$smarty.capture.open_basedir}
  </div>
  {/if}

  {if isset($form.error.uploadLocalServer.newDir.notReadable)}
  <div class="giError">
    {g->text text="The webserver does not have permissions to read that directory."}
  </div>
  {/if}

  {if isset($form.error.uploadLocalServer.newDir.notADirectory)}
  <div class="giError">
    {g->text text="The path you specified is not a valid directory."}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
