{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Enter a URL below to a webcam or other live image anywhere on the net."}
  </p>

  {if empty($form.imageUrl) || !empty($form.error)}
    <h3> {g->text text="URL"} </h3>

    <input type="text" size="60"
     name="{g->formVar var="form[imageUrl]"}" value="{$form.imageUrl}"/>

    {if isset($form.error.imageUrl.missing)}
    <div class="giError">
      {g->text text="You must enter a URL to an image"}
    </div>
    {/if}
    {if isset($form.error.imageUrl.invalid)}
    <div class="giError">
      {g->text text="The URL entered must begin with http:// or file://"}
    </div>
    {/if}
    {if isset($form.error.imageUrl.unavailable)}
    <div class="giError">
      {g->text text="The image URL you specified is unavailable"}
    </div>
    {/if}
    {if isset($form.error.imageUrl.notImage)}
    <div class="giError">
      {g->text text="The image URL you specified is not an image type"}
    </div>
    {/if}

    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][checkUrl]"}" value="{g->text text="Check URL"}"/>

  {else}

    <strong>
      {g->text text="URL: %s" arg1=$form.imageUrl}
      <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd"
	 arg3="itemId=`$ItemAdmin.item.id`" arg4="addPlugin=ItemAddWebCam"}">
	{g->text text="change"}
      </a>
    </strong><br/>

    <img src="{$form.imageUrl}" alt="{$form.imageUrl}"/>
    <br/>

    <input type="hidden" name="{g->formVar var="form[imageUrl]"}" value="{$form.imageUrl}"/>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][addWebCam]"}" value="{g->text text="Add Image"}"/>
  {/if}
</div>
