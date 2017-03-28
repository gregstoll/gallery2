{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Thumbnail Manager"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.add)}
    {g->text text="New image added successfully"}
  {/if}
  {if isset($status.addMime)}
    {g->text text="New setting added successfully"}
  {/if}
  {if isset($status.delete)}
    {g->text text="Image deleted successfully"}
  {/if}
  {if isset($status.deleteMime)}
    {g->text text="Setting deleted successfully"}
  {/if}
  {if isset($status.mime_duplicate)}
    <span class="giError">{g->text text="Mime type already assigned"}</span>
  {/if}
  {if isset($status.mime_error)}
    <span class="giError">{g->text text="Missing mime type"}</span>
  {/if}
  {if isset($status.file_error)}
    <span class="giError">{g->text text="Missing image file"}</span>
  {/if}
  {if isset($status.imagemime_error)}
    <span class="giError">{g->text text="Thumbnail image must be a JPEG"}</span>
  {/if}
</h2></div>
{/if}

{if !empty($form.badMime)}
<div class="gbBlock">
  <p class="giError">
    {g->text text="Warning: Other modules provide thumbnail support for some types.  Remove settings below for these mime types to ensure other modules are used:"}
    {foreach from=$form.badMime item=mime}{$mime} {/foreach}
  </p>
</div>
{/if}

{if !empty($form.list)}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="The thumbnail images shown below will be used for new items added to Gallery with the listed mime types."}
  </p>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Mime Types"} </th>
    <th> {g->text text="File"} </th>
    <th> {g->text text="Image"} </th>
    <th> {g->text text="Action"} </th>
  </tr>
  {foreach from=$form.list item=item}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {if count($item.itemMimeTypesList)>1}
	<table cellspacing="0">
	{foreach from=$item.itemMimeTypesList item=mime}
	  <tr class="{cycle values="gbEven,gbOdd"}">
	    <td>
	      {$mime}{if isset($form.mimeMap[$mime])} ({$form.mimeMap[$mime]}){/if}
	    </td><td>
	      <a href="{g->url arg1="controller=thumbnail.ThumbnailSiteAdmin"
	       arg2="form[action][delete]=1" arg3="form[delete][itemId]=`$item.id`"
	       arg4="form[delete][mimeType]=`$mime`"}">{g->text text="delete"}</a>
	    </td>
	  </tr>
	{/foreach}
	</table>
      {else}
	{$item.itemMimeTypesList.0}
      {/if}
    </td><td>
      {$item.fileName}
    </td><td>
      {g->image item=$item image=$item maxSize=150}
    </td><td>
      <a href="{g->url arg1="controller=thumbnail.ThumbnailSiteAdmin"
       arg2="form[action][delete]=1" arg3="form[delete][itemId]=`$item.id`"}"
       onclick="return confirm('{g->text text="Delete this image?"}\n{g->text text="(Will not remove thumbnails from existing items using this image, but those items will be unable to rebuild thumbs)"}')">
	{g->text text="delete"}
      </a>
    </td>
  </tr>
  {/foreach}
  {foreach from=$form.operationSupport key=module item=mimeTypes}
  <tr>
    <td>
      {foreach from=$mimeTypes item=mime}
	{$mime}{if isset($form.mimeMap[$mime])} ({$form.mimeMap[$mime]}){/if}<br/>
      {/foreach}
    </td><td colspan="3">
      {g->text text="Thumbnail support provided by module:"} {$module}
    </td>
  </tr>
  {/foreach}
  </table>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="New Setting"} </h3>

  <p class="giDescription" style="margin-bottom:10px">
    {g->text text="Images do not need to be thumbnail size; they will be resized as needed."}
  </p>

  {g->text text="Default thumbnail for mime type:"}
  <input type="text" size="30" name="{g->formVar var="form[mimeType]"}"/>
  <br/>

  <select name="{g->formVar var="form[blah]"}"
   onchange="this.form['{g->formVar var="form[mimeType]"}'].value=this.value;this.selectedIndex=0;this.blur()">
    <option value="">{g->text text="&laquo; Choose type or enter above &raquo;"}</option>
    {foreach from=$form.mimeMap key=mime item=extlist}
      <option value="{$mime}">{$mime} ({$extlist})</option>
    {/foreach}
  </select>
  <br/>

  <table><tr>
    <td style="padding-left:10px">
      <input type="{if !empty($form.list)}radio{else}hidden{/if}" id="rbNew"
       name="{g->formVar var="form[image]"}" value="new"/>
      <label for="rbNew">
	{g->text text="New jpeg file:"}
      </label>
    </td><td>
      <input type="file" size="45" name="{g->formVar var="form[1]"}"/>
    </td>
  </tr>
  {if !empty($form.list)}
  <tr>
    <td style="padding-left:10px">
      <input type="radio" id="rbOld" checked="checked"
       name="{g->formVar var="form[image]"}" value="old"/>
      <label for="rbOld">
	{g->text text="Existing image:"}
      </label>
    </td><td>
      <select name="{g->formVar var="form[oldimage]"}">
	{foreach from=$form.list item=item}
	  <option value="{$item.id}">{$item.fileName}</option>
	{/foreach}
      </select>
    </td>
  </tr>
  {/if}
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Save"}"/>
</div>
