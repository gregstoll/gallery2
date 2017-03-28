{*
 * $Revision: 17540 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
  // <![CDATA[
{if !empty($form.localServerFiles)}
  var symState = false;
  {* Generate indexes of items that we know, which will correspond to checkbox ids, below *}
  {strip}
  var knownTypeCheckboxIds = new Array(
  {assign var="first" value="1"}
  {foreach name=fileIndex from=$form.localServerFiles item=file}
    {if ( ($file.type == 'file' ||
	   ($file.type == 'directory' && $file.fileName != '..' && $file.fileName != 'CVS')
	  ) && empty($file.unknown))}
      {if !$first},{else}{assign var="first" value="0"}{/if}
      "{$smarty.foreach.fileIndex.iteration}"
    {/if}
  {/foreach}
  );
  {/strip}

  {literal}
  function toggleSelections() {
    for (i = 0; i < knownTypeCheckboxIds.length; i++) {
      var cb = document.getElementById('cb_' + knownTypeCheckboxIds[i]);
      cb.checked = !cb.checked;
  {/literal}
      {if $ItemAddFromServer.showSymlink}toggleSymlinkEnabled(knownTypeCheckboxIds[i]);{/if}
  {literal}
    }
  }

  function toggleSymlinkEnabled(a) {
    var cbSymlink = document.getElementById('symlink_' + a );
    var cbSelected = document.getElementById('cb_' + a );
    if (cbSymlink) {
      cbSymlink.disabled = !cbSelected.checked;
    }
  }

  function invertSymlinkSelection() {
    symState = !symState;
    for (i = 0; i < knownTypeCheckboxIds.length; i++) {
      var cb = document.getElementById('cb_' + knownTypeCheckboxIds[i]);
      var cbSymlink = document.getElementById('symlink_' + knownTypeCheckboxIds[i]);
      if (cb.checked == true) {
	if (symState == false) {
	  cbSymlink.checked = false;
	} else {
	  cbSymlink.checked = true;
	}
      }
    }
  }
  {/literal}
{/if}

  function findFiles(path) {ldelim}
    var url = '{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd"
	arg3="addPlugin=ItemAddFromServer" arg4="form[localServerPath]=__PATH__"
	arg5="itemId=`$ItemAdmin.item.id`" arg6="form[action][findFilesFromLocalServer]=1"
	arg7="form[formName]=ItemAddFromServer" forceFullUrl=true htmlEntities=false}';
    document.location.href = url.replace('__PATH__', escape(path));
  {rdelim}

  function getSelectedPath() {ldelim}
    return document.getElementById('itemAdminForm').elements['{g->formVar
      var="form[localServerPath]"}'].value;
  {rdelim}

  function selectPath(path) {ldelim}
    document.getElementById('itemAdminForm').elements['{g->formVar
      var="form[localServerPath]"}'].value = path;
  {rdelim}
  // ]]>
</script>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Transfer files that are already on your server into your Gallery.  The files must already have been uploaded to your server some other way (like FTP) and must be placed in an accessible directory.  If you're on Unix this means that the files and the directory the files are in should have modes of at least 755."}
  </p>

  {if empty($ItemAddFromServer.localServerDirList)}
  <div class="giWarning">
    {g->text text="For security purposes, you can't use this feature until the Gallery Site Administrator configures a set of legal upload directories."}
    {if $ItemAdd.isAdmin}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=itemadd.ItemAddSiteAdmin"}">
	{g->text text="site admin"}
      </a>
    {/if}
  </div>
  {else}

  {if empty($form.localServerFiles)}
    <h4> {g->text text="Server Path"} </h4>

    <input type="text" size="80"
     name="{g->formVar var="form[localServerPath]"}" value="{$form.localServerPath}"/>

    {if isset($form.error.pathComponent.missing)}
    <div class="giError">
      {g->text text="You must enter a directory."}
    </div>
    {/if}
    {if isset($form.error.pathComponent.invalid)}
    <div class="giError">
      {g->text text="The directory you entered is invalid.  Make sure that the directory is readable by all users."}
    </div>
    {/if}
    {if isset($form.error.pathComponent.illegal)}
    <div class="giError">
      {g->text text="The directory you entered is illegal.  It must be a sub directory of one of the directories listed below."}
    </div>
    {/if}
    {if isset($form.error.pathComponent.collision)}
    <div class="giError">
      {g->text text="An item with the same name already exists."}
    </div>
    {/if}

    <br/>
    {g->text text="Legal Directories"}

    {if $ItemAdd.isAdmin}
    <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=itemadd.ItemAddSiteAdmin" arg3="return=true"}">
      {g->text text="modify"}
    </a>
    {/if}

    <ul style="list-style-type: none; margin-bottom: 1em">
      {foreach from=$ItemAddFromServer.localServerDirList item=dir}
	{capture name="escapedDir"}{$dir|escape|replace:"\\":"\\\\"}{/capture}
	<li>
	  <a href="javascript:selectPath('{$smarty.capture.escapedDir}')"> {$dir|escape} </a>
	</li>
      {/foreach}
    </ul>

    {if !empty($ItemAddFromServer.recentPaths)}
      {g->text text="Recent Directories"}

      <ul style="list-style-type: none">
	{foreach from=$ItemAddFromServer.recentPaths item=dir}
	  {capture name="escapedDir"}{$dir|escape|replace:"\\":"\\\\"}{/capture}
	  <li>
	  <a href="javascript:selectPath('{$smarty.capture.escapedDir}')"> {$dir|escape} </a>
	  </li>
	{/foreach}
      </ul>
    {/if}

    {capture name="submitLinks"}
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][findFilesFromLocalServer]"}"
       value="{g->text text="Find Files"}"
       onclick="findFiles(getSelectedPath()); return false;"/>
    {/capture}
  {else} {* {if empty($form.localServerFiles)} *}

    {capture name="path"}{strip}
      {foreach name="pathElements" from=$ItemAddFromServer.pathElements key=idx item=element}
	{if $idx>1}{$ItemAddFromServer.pathSeparator}{/if}
	{if ($element.legal && !$smarty.foreach.pathElements.last)}
	  <a href="javascript:findFiles('{$element.path|escape:"javascript":"UTF-8"}')">{$element.name|escape}</a>
	{else}
	  {$element.name|escape}
	{/if}
      {/foreach}
    {/strip}{/capture}
    <strong>
      {g->text text="Directory: %s" arg1=$smarty.capture.path}
    </strong>

    <input type="hidden"
     name="{g->formVar var="form[localServerPath]"}" value="{$form.localServerPath|escape}"/>
    <br/>

    <table class="gbDataTable"><tr>
      <th> </th>
      <th> {g->text text="File name"} </th>
      <th> {g->text text="Type"} </th>
      <th> {g->text text="Size"} </th>
      {if $ItemAddFromServer.showSymlink}
	<th> {g->text text="Use Symlink"} </th>
      {/if}
    </tr>
    {foreach name=fileIndex from=$form.localServerFiles item=file}
      {assign var=key value=$file.fileKey|escape}
      <tr class="{cycle values="gbEven,gbOdd"}">
      {if ($file.type == 'file')}
	<td style="text-align: center">
	  <input type="checkbox" id="cb_{$smarty.foreach.fileIndex.iteration}"
	   {if $ItemAddFromServer.showSymlink}
	     onclick="toggleSymlinkEnabled('{$smarty.foreach.fileIndex.iteration}')"
	   {/if}
	   name="{g->formVar var="form[localServerFiles][$key][selected]"}"/>
	</td><td>
	  <label for="cb_{$smarty.foreach.fileIndex.iteration}">
	    {$file.fileName|escape}
	  </label>
	</td><td>
	  {$file.itemType}
	</td><td>
	  {g->text one="%d byte" many="%d bytes" count=$file.stat.size arg1=$file.stat.size}
	</td>
	{if $ItemAddFromServer.showSymlink}
	  <td align="center">
	    <input type="checkbox" disabled="true"
	     id="symlink_{$smarty.foreach.fileIndex.iteration}"
	     name="{g->formVar var="form[localServerFiles][$key][useSymlink]"}"/>
	  </td>
	{/if}
      {else}
	<td>
	  <input type="checkbox" id="cb_{$smarty.foreach.fileIndex.iteration}"
	   {if $ItemAddFromServer.showSymlink}
	     onclick="toggleSymlinkEnabled('{$smarty.foreach.fileIndex.iteration}')"
	   {/if}
	   name="{g->formVar var="form[localServerDirectories][$key][selected]"}"
	   {if !$ItemAddFromServer.canAddAlbum}
	     disabled="true"
	   {/if}
	   />
	</td><td>
	  {if $file.legal}{strip}
	    <a href="javascript:findFiles('{$file.filePath|escape:"javascript":"UTF-8"}')">
	      {if $file.fileName == ".."}
		&laquo; {g->text text="Parent Directory"} &raquo;
	      {else}
		{$file.fileName|escape}
	      {/if}
	    </a>
	  {/strip}{else}
	    <i>{$file.fileName|escape}</i>
	  {/if}
	</td><td>
	  {g->text text="Directory"}
	</td><td>
	  &nbsp;
	</td>
	{if $ItemAddFromServer.showSymlink}
	<td style="text-align: center">
	  <input type="checkbox" disabled="true"
	   id="symlink_{$smarty.foreach.fileIndex.iteration}"
	   name="{g->formVar var="form[localServerDirectories][$key][useSymlink]"}"/>
	</td>
	{/if}
      {/if}
      </tr>
    {/foreach}
    <tr>
      <th>
	<input type="checkbox" id="cbSelectionToggle" onclick="toggleSelections()"/>
      </th>
      <th colspan="{if $ItemAddFromServer.showSymlink}2{else}3{/if}">
	<label for="cbSelectionToggle">
	  {g->text text="(Un)check all known types"}
	</label>
      </th>
      {if $ItemAddFromServer.showSymlink}
      <th>
	<label for="cbSymlinkToggle">
	  {g->text text="(Un)check symlinks"}<br/>{g->text text="for selected items"}
	</label>
      </th>
      <th style="text-align: center">
	<input type="checkbox" id="cbSymlinkToggle" onclick="invertSymlinkSelection()"/>
      </th>
      {/if}
    </tr></table>
  </div>

  <div class="gbBlock">
    <p class="giDescription">
      {g->text text="Copy base filenames to:"}
      <br/>
      <input type="checkbox" id="cbTitle" {if $form.set.title} checked="checked" {/if}
       name="{g->formVar var="form[set][title]"}"/>
      <label for="cbTitle"> {g->text text="Title"} </label>
      &nbsp;

      <input type="checkbox" id="cbSummary" {if $form.set.summary} checked="checked" {/if}
       name="{g->formVar var="form[set][summary]"}"/>
      <label for="cbSummary"> {g->text text="Summary"} </label>
      &nbsp;

      <input type="checkbox" id="cbDescription" {if $form.set.description} checked="checked" {/if}
       name="{g->formVar var="form[set][description]"}"/>
      <label for="cbDescription"> {g->text text="Description"} </label>
    </p>

    {capture name="submitLinks"}
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][addFromLocalServer]"}"
       value="{g->text text="Add Files"}"/>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][startOver]"}" value="{g->text text="Start Over"}"
       onclick="document.location.href = '{g->url arg1="view=core.ItemAdmin"
	 arg2="subView=core.ItemAdd" arg3="addPlugin=ItemAddFromServer"
	 arg4="itemId=`$ItemAdmin.item.id`" arg5="form[formName]=ItemAddFromServer"
	 arg5="form[action][startOver]=1" htmlEntities=false}'; return false;"/>
    {/capture}
    {assign var="showOptions" value="true"}
  {/if} {* {if !empty($form.localServerFiles)} *}
  {/if} {* {if empty($ItemAddFromServer.localServerDirList)} *}
</div>

{if isset($showOptions)}
  {* Include our extra ItemAddOptions *}
  {foreach from=$ItemAdd.options item=option}
    {include file="gallery:`$option.file`" l10Domain=$option.l10Domain}
  {/foreach}
{/if}

{if !empty($smarty.capture.submitLinks)}
<div class="gbBlock gcBackground1">
  {$smarty.capture.submitLinks}
</div>
{/if}
